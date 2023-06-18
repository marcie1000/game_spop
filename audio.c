#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>
#include <assert.h>
#include <math.h>
#include "emulator.h"
#include "audio.h"

void div_apu_update(s_audio *au)
{
    if(au->DIV_APU == 0)
        return;
        
    if(au->DIV_APU % 2 == 1)
    {
        if(au->ch_len_timer[0] < 64)
            au->ch_len_timer[0]++;
        if(au->ch_len_timer[1] < 64)
            au->ch_len_timer[1]++;
    }
    if(au->DIV_APU % 4 == 3)
    {
        au->ch1_wl_sweep_timer++;
    }
    if(au->DIV_APU >= 7)
    {
        au->ch_vol_sweep_timer[0]++;
        au->ch_vol_sweep_timer[1]++;
        au->DIV_APU = 0;
    }
}

void wavelength_sweep(s_audio *au, s_io *io)
{
    if(au->ch1_wl_sweep_pace == 0)
        return;
    
    if(au->ch1_wl_sweep_timer != au->ch1_wl_sweep_pace)
        return;
        
    au->ch1_wl_sweep_timer = 0;
    
    if(!au->ch1_wl_sweep_dir)
    {
        au->ch_wavelen[0] += au->ch_wavelen[0] / (pow(2, au->ch1_wl_sweep_slope_ctr));
        if(au->ch_wavelen[0] > 0x07FF)
            au->ch_enable[0] = false;
    }
    else
    {
        au->ch_wavelen[0] -= au->ch_wavelen[0] / (pow(2, au->ch1_wl_sweep_slope_ctr));
        if(au->ch_wavelen[0] > 0x07FF)
            au->ch_wavelen[0] = 0;
    }
    
    io->NR13 = au->ch_wavelen[0] & 0x00FF;
    io->NR14 &= ~0x07;
    io->NR14 |= (au->ch_wavelen[0] & 0x0700) >> 8;
    
    if(au->ch_wavelen[0] <= 2042)
        au->ch_freq[0] = 131072 / (2048 - au->ch_wavelen[0]);
    else
        au->ch_freq[0] = 21845;
}

void volume_sweep(s_audio *au, int *volume, int ch)
{
    if(au->ch_vol_sweep_pace[ch] == 0)
    {
        *volume = au->ch_init_volume[ch];
        return;
    }
    //volume sweep
    if(au->ch_vol_sweep_timer[ch] % au->ch_vol_sweep_pace[ch] == 0)
    {
        au->ch_vol_sweep_timer[ch] = 1;
        if(au->ch_vol_sweep_counter[ch] < 15)
            au->ch_vol_sweep_counter[ch]++;
    }
    *volume = au->ch_init_volume[ch];
    if(!au->ch_envl_dir[ch])
        *volume -= au->ch_vol_sweep_counter[ch];
    else
        *volume += au->ch_vol_sweep_counter[ch];
        
    if(*volume < 0) *volume = 0;
    if(*volume > 15) *volume = 15;
}

void fill_stream(s_emu *emu, int ch)
{
    s_audio *au = &emu->audio;
//    static uint8_t old_volume = 0;
    
    wavelength_sweep(au, &emu->cpu.io_reg);
    
    //if no sound to be played (because length timer is ended or channel not enabled)
    if((au->ch_sound_len_enable[ch] && (au->ch_len_timer[ch] == 64)) || !au->ch_enable[ch])
    {
        //disable channel
        emu->cpu.io_reg.NR52 &= ~(0x01 << ch);
        return;
    }
    
    //Variables used to seemlessly continue the sound when samples_played is reset to 0
    //after a SDL_QueueAudio (new buffer beginning)
    static bool period_ended[2] = {true,  true};
    //if the last period of the previous buffer is unfinished.
    static bool must_finish_period[2] = {false, false};
    //the samples_played value considered as the beginning of the first period of the
    //current buffer, after the last period of the previous buffer is ended.
    static uint16_t start_shift[2] = {0, 0};
    if((au->samples_played == 0) && !period_ended[ch])
        must_finish_period[ch] = true;
    
    assert(au->ch_duty_ratio[ch] < 4);
    
    int volume;
    volume_sweep(au, &volume, ch);
    
    uint64_t duty_change, duty_reset;
    //after how many samples a complete period is elapsed
    duty_reset = ((float)AUDIO_SAMPLE_RATE / au->ch_freq[ch]) * 2;
    //after how many samples the signal should turn from low to high, depending on duty ratio
    duty_change = au->duty_ratios[au->ch_duty_ratio[ch]] * duty_reset;
    
    
//    static uint16_t old_period_counter = 0;
    
    //how many periods elapsed since the last samples_played reset, 
    //with period_counter value reset to 0 when the last period of
    //the previous buffer is done.
    uint16_t period_counter = (au->samples_played + 
                               AUDIO_SAMPLES_PER_QUEUES * must_finish_period[ch] - 
                               start_shift[ch]) / duty_reset;
//    if(period_counter == old_period_counter && au->DIV_APU != 0)
//        volume = old_volume;
    
    //a samples counter that is reset at the beginning of each new period
    uint64_t new_period_samples_played = au->samples_played                     + 
                                         AUDIO_SAMPLES_PER_QUEUES * must_finish_period[ch] - 
                                         start_shift[ch]                        - 
                                         period_counter * duty_reset            ;
    //the actual state of the signal (low or high)
    bool duty = new_period_samples_played >= duty_change;
    
    assert(au->samples_played + 1 < AUDIO_SAMPLES_PER_QUEUES);
    if(duty)
    {
        //fills the data (left and right)
        au->fstream[au->samples_played    ] += (float)volume * 1/15 * au->ch_l[ch] * 
                                               1/8 * (au->l_output_vol + 1)        ;
                                               
        au->fstream[au->samples_played + 1] += (float)volume * 1/15 * au->ch_r[ch] * 
                                               1/8 * (au->r_output_vol + 1)        ;
    }
    else
    {
        au->fstream[au->samples_played    ] -= (float)volume * 1/15 * au->ch_l[ch] * 
                                               1/8 * (au->l_output_vol + 1)        ;
                                               
        au->fstream[au->samples_played + 1] -= (float)volume * 1/15 * au->ch_r[ch] * 
                                               1/8 * (au->r_output_vol + 1)        ;
    }
    
    
//    au->fstream[au->samples_played] = 
//    if(au->fstream[au->samples_played] != 0)
//    {
//        fprintf(emu->opt.logfile, "%f;%u;%u;%u;%u;%lu;%u;%lu\n",
//        au->fstream[au->samples_played], volume, au->ch_vol_sweep_counter[ch], au->ch_vol_sweep_timer[ch], period_counter,
//        new_period_samples_played, duty, au->samples_played);
    //}
//    old_volume = volume;
//    old_period_counter = period_counter;


    if(!must_finish_period[ch])
        period_ended[ch] = (au->samples_played % duty_reset == 0);
    else
        period_ended[ch] = ((AUDIO_SAMPLES_PER_QUEUES + au->samples_played - start_shift[ch]) % 
                           duty_reset == 0);
        
    if(must_finish_period[ch] && period_ended[ch])
    {
        must_finish_period[ch] = false;
        start_shift[ch] = au->samples_played;
    }
}

void update_channel_state(s_audio *au, s_io *io, int ch)
{
    if(ch == 0)
        au->ch_wavelen[ch] = io->NR13 + ((io->NR14 & 0x07) << 8);
    else if(ch == 1)
        au->ch_wavelen[ch] = io->NR23 + ((io->NR24 & 0x07) << 8);
        
    if(au->ch_wavelen[ch] <= 2042)
        au->ch_freq[ch] = 131072 / (2048 - au->ch_wavelen[ch]);
    else
        au->ch_freq[ch] = 21845;
    
    //if channel 1 triggered
    if(au->ch_trigger[ch])
    {
        io->NR52 |= 0x01 << ch;
        au->ch_vol_sweep_timer[ch] = 1;
        au->ch_len_timer[ch] = au->ch_init_len_timer[ch];
//        au->DIV_APU               = 0;
        au->samples_timer         = 0;
        au->samples_played        = 0;
        au->DIV_APU               = 0;
        au->ch_vol_sweep_counter[ch] = 0;
        
        if(ch == 0)
        {
            au->ch1_wl_sweep_timer     = 0;
            au->ch1_wl_sweep_counter   = 0;
            au->ch1_wl_sweep_pace      = (io->NR10 & 0x70) >> 4;
            au->ch1_wl_sweep_dir       = (io->NR10 & 0x08)     ;
            au->ch1_wl_sweep_slope_ctr = (io->NR10 & 0x07)     ;
        }
        au->ch_trigger[ch] = false;
    }

    au->ch_enable[ch] = io->NR52 & (0x01 << ch);
}

void audio_update(s_emu *emu)
{
    s_audio *au = &emu->audio;
    s_io *io = &emu->cpu.io_reg;
    s_opt *opt = &emu->opt;
    
    if(!opt->audio)
        return;

    //sound on/off
    if(!(io->NR52 & 0x80))
        return;
        
    if(au->queues_since_last_frame >= QUEUES_PER_FRAME)
        return;
        
    update_channel_state(au, io, 0);
    update_channel_state(au, io, 1);
    
    div_apu_update(au);
    
    if(au->samples_timer >= CPU_FREQ / AUDIO_SAMPLE_RATE)
    {
        au->samples_timer -= CPU_FREQ / AUDIO_SAMPLE_RATE;
        fill_stream(emu, 0);
        fill_stream(emu, 1);
        au->samples_played += 2;
    }
    
    //does not fill a complete buffer in the queue when fast forward is toggled
    if(emu->opt.fast_forward)
    {
        float tmp[au->samples_played];
        memcpy(tmp, au->fstream, sizeof(float[au->samples_played]));
        au->samples_played = 0;
        if(0 != SDL_QueueAudio(au->dev, tmp, (int)(sizeof(tmp))))
        {
            fprintf(stderr, "Error SDL_QueueAudio: %s\n", SDL_GetError());
            destroy_emulator(emu, EXIT_FAILURE);
        }
        au->queues_since_last_frame++;
        memset(au->fstream, 0, sizeof(au->fstream));
        return;
    }
    
    //otherwise, when a buffer is complet, send it to the queue
    if(au->samples_played >= AUDIO_SAMPLES_PER_QUEUES)
    {
        au->samples_played = 0;
        if(0 != SDL_QueueAudio(au->dev, au->fstream, (int)(sizeof(au->fstream))))
        {
            fprintf(stderr, "Error SDL_QueueAudio: %s\n", SDL_GetError());
            destroy_emulator(emu, EXIT_FAILURE);
        }
        au->queues_since_last_frame++;
        memset(au->fstream, 0, sizeof(au->fstream));
    }
}

int init_audio(s_emu *emu)
{    
    if(!emu->opt.audio)
        return EXIT_SUCCESS;
        
    s_audio *au = &emu->audio;
 
    memset(au, 0, sizeof(s_audio));
    
    au->spec_want.freq     = AUDIO_SAMPLE_RATE;
    au->spec_want.format   = AUDIO_F32;
    au->spec_want.channels = 2;
    au->spec_want.samples  = AUDIO_SAMPLES_DRIVER;
    au->spec_want.callback = NULL;
    au->spec_want.userdata = (void*)&au->samples_played;
    
    au->dev = SDL_OpenAudioDevice(
        NULL, 0, 
        &au->spec_want, &au->spec_have,
        SDL_AUDIO_ALLOW_FORMAT_CHANGE
    );
    if(!au->dev)
    {
        fprintf(stderr, "Error creating SDL audio device: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }
    
    if(au->spec_want.samples != au->spec_have.samples)
    {
        fprintf(stderr, "Error: samples want = %u, samples have = %u.\n",
                au->spec_want.samples, au->spec_have.samples);
        return EXIT_FAILURE;
    }
    
//    au->duty_ratios[0] = 0x01;
//    au->duty_ratios[1] = 0x81;
//    au->duty_ratios[2] = 0x87;
//    au->duty_ratios[3] = 0x7E;

    au->duty_ratios[0] = 0.125;
    au->duty_ratios[1] = 0.25;
    au->duty_ratios[2] = 0.50;
    au->duty_ratios[3] = 0.75;
    
    return EXIT_SUCCESS;
}

void destroy_audio(s_emu *emu)
{
    if(!emu->opt.audio)
        return;
    if(0 != emu->audio.dev)
        SDL_CloseAudioDevice(emu->audio.dev);
}

