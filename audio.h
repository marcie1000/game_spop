#ifndef AUDIO_H
#define AUDIO_H

#include "emulator.h"

int init_audio(s_emu *emu);
void destroy_audio(s_emu *emu);
void audio_update(s_emu *emu);
void div_apu_update(s_audio *au);
void wavelength_sweep(s_audio *au, s_io *io);
void volume_sweep(s_audio *au, int *volume, int ch);
void update_square_channel_state(s_audio *au, s_io *io, int ch);
void update_ch3_state(s_audio *au, s_io *io);
void update_ch4_state(s_audio *au, s_io *io);
void fill_square_channel_stream(s_emu *emu, int ch);
void fill_noise_channel_stream(s_emu *emu);
void fill_wave_channel_stream(s_emu *emu);

#endif //AUDIO_H

