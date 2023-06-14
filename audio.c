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
        if(au->ch1_len_timer < 64)
            au->ch1_len_timer++;
    }
    if(au->DIV_APU % 4 == 3)
    {
        au->ch1_wl_sweep_timer++;
    }
    if(au->DIV_APU >= 7)
    {
        au->ch1_vol_sweep_timer++;
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
        au->ch1_wavelen += au->ch1_wavelen / (pow(2, au->ch1_wl_sweep_slope_ctr));
        if(au->ch1_wavelen > 0x07FF)
            au->ch1_enable = false;
    }
    else
    {
        au->ch1_wavelen -= au->ch1_wavelen / (pow(2, au->ch1_wl_sweep_slope_ctr));
        if(au->ch1_wavelen > 0x07FF)
            au->ch1_wavelen = 0;
    }
    
    io->NR13 = au->ch1_wavelen & 0x00FF;
    io->NR14 &= ~0x07;
    io->NR14 |= (au->ch1_wavelen & 0x0700) >> 8;
    
    au->ch1_freq = 131072 / (2048 - au->ch1_wavelen);
}

void volume_sweep(s_audio *au, int *volume)
{
    if(au->ch1_vol_sweep_pace == 0)
    {
        *volume = au->ch1_init_volume;
        return;
    }
    //volume sweep
    if(au->ch1_vol_sweep_timer % au->ch1_vol_sweep_pace == 0)
    {
        au->ch1_vol_sweep_timer = 1;
        if(au->ch1_vol_sweep_counter < 15)
            au->ch1_vol_sweep_counter++;
    }
    *volume = au->ch1_init_volume;
    if(!au->ch1_envl_dir)
        *volume -= au->ch1_vol_sweep_counter;
    else
        *volume += au->ch1_vol_sweep_counter;
        
    if(*volume < 0) *volume = 0;
    if(*volume > 15) *volume = 15;
}

void fill_stream(s_emu *emu)
{
    s_audio *au = &emu->audio;
//    static uint8_t old_volume = 0;
    
    wavelength_sweep(au, &emu->cpu.io_reg);
    
    //if no sound to be played (because length timer is ended or channel not enabled)
    if((au->ch1_sound_len_enable && (au->ch1_len_timer == 64)) || !au->ch1_enable)
    {
        au->fstream[au->samples_played] = 0;
        au->fstream[au->samples_played + 1] = 0;
        //disable channel
        emu->cpu.io_reg.NR52 &= ~0x01;
        return;
    }
    
    //Variables used to seemlessly continue the sound when samples_played is reset to 0
    //after a SDL_QueueAudio
    bool new_buffer = (au->samples_played == 0);
    static bool period_ended = true;
    //if the last period of the previous buffer is unfinished.
    static bool must_finish_period = false;
    //the samples_played value considered as the beginning of the first period of the
    //current buffer, after the last period of the previous buffer is ended.
    static uint16_t start_shift = 0;
    if(new_buffer && !period_ended)
        must_finish_period = true;
    
    assert(au->ch1_duty_ratio < 4);
    
    int volume;
    volume_sweep(au, &volume);
    
    uint64_t duty_change, duty_reset;
    //after how many samples a complete period is elapsed
    duty_reset = ((float)AUDIO_SAMPLE_RATE / au->ch1_freq);
    //after how many samples the signal should turn from low to high, depending on duty ratio
    duty_change = au->duty_ratios[au->ch1_duty_ratio] * duty_reset;
    
    
//    static uint16_t old_period_counter = 0;
    
    //how many periods elapsed since the last samples_played reset, 
    //with period_counter value reset to 0 when the last period of
    //the previous buffer is done.
    uint16_t period_counter = (au->samples_played + AUDIO_SAMPLES * must_finish_period - start_shift) / duty_reset;
//    if(period_counter == old_period_counter && au->DIV_APU != 0)
//        volume = old_volume;
    
    //a samples counter that is reset at the beginning of each new period
    uint64_t new_period_samples_played = au->samples_played + AUDIO_SAMPLES * must_finish_period - start_shift - period_counter * duty_reset;
    //the actual state of the signal (low or high)
    bool duty = new_period_samples_played >= duty_change;
    
    assert(au->samples_played + 1 < AUDIO_SAMPLES);
    if(duty)
    {
        //fills the data (left and right)
        au->fstream[au->samples_played] = (float)volume * 1/15 * au->ch1_l * 1/8 * (au->l_output_vol + 1);
        au->fstream[au->samples_played + 1] = (float)volume * 1/15 * au->ch1_r * 1/8 * (au->r_output_vol + 1);
    }
    else
    {
        au->fstream[au->samples_played] = -(float)volume * 1/15 * au->ch1_l * 1/8 * (au->l_output_vol + 1);
        au->fstream[au->samples_played + 1] = -(float)volume * 1/15 * au->ch1_r * 1/8 * (au->r_output_vol + 1);
    }
    
    
//    au->fstream[au->samples_played] = 
//    if(au->fstream[au->samples_played] != 0)
//    {
        fprintf(emu->opt.logfile, "%f;%u;%u;%u;%u;%lu;%u;%lu\n",
        au->fstream[au->samples_played], volume, au->ch1_vol_sweep_counter, au->ch1_vol_sweep_timer, period_counter,
        new_period_samples_played, duty, au->samples_played);
    //}
//    old_volume = volume;
//    old_period_counter = period_counter;


    if(!must_finish_period)
        period_ended = (au->samples_played % duty_reset == 0);
    else
        period_ended = ((AUDIO_SAMPLES + au->samples_played - start_shift) % duty_reset == 0);
        
    if(must_finish_period && period_ended)
    {
        must_finish_period = false;
        start_shift = au->samples_played;
    }
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
        
    if(au->queues_since_last_frame >= 1)
        return;
        
    au->ch1_wavelen = io->NR13 + ((io->NR14 & 0x07) << 8);
    au->ch1_freq = 131072 / (2048 - au->ch1_wavelen);
    
    //if channel 1 triggered
    if(au->ch1_trigger)
    {
        io->NR52 |= 0x01;
        au->ch1_trigger = true;
        au->ch1_vol_sweep_timer = 1;
        au->ch1_len_timer = au->ch1_init_len_timer;
        au->DIV_APU               = 0;
        au->samples_timer         = 0;
        au->samples_played        = 0;
        au->DIV_APU               = 0;
        au->ch1_vol_sweep_counter = 0;
        au->ch1_wl_sweep_timer    = 0;
        au->ch1_wl_sweep_counter  = 0;
        
        au->ch1_wl_sweep_pace      = (io->NR10 & 0x70) >> 4;
        au->ch1_wl_sweep_dir       = (io->NR10 & 0x08);
        au->ch1_wl_sweep_slope_ctr = (io->NR10 & 0x07);
        au->ch1_trigger = false;
    }

    au->ch1_enable = io->NR52 & 0x01;
    
    div_apu_update(au);
    
    if(au->samples_timer >= CPU_FREQ / AUDIO_SAMPLE_RATE)
    {
        au->samples_timer -= CPU_FREQ / AUDIO_SAMPLE_RATE;
        fill_stream(emu);
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
        return;
    }
    
    //otherwise, when a buffer is complet, send it to the queue
    if(au->samples_played >= AUDIO_SAMPLES)
    {
        au->samples_played = 0;
        if(0 != SDL_QueueAudio(au->dev, au->fstream, (int)(sizeof(au->fstream))))
        {
            fprintf(stderr, "Error SDL_QueueAudio: %s\n", SDL_GetError());
            destroy_emulator(emu, EXIT_FAILURE);
        }
        au->queues_since_last_frame++;
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
    au->spec_want.samples  = 400;
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

