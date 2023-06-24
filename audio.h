#ifndef AUDIO_H
#define AUDIO_H

#include "emulator.h"

int init_audio(s_emu *emu);
void destroy_audio(s_emu *emu);
void audio_update(s_emu *emu);
void update_square_channel_state(s_audio *au, s_io *io, int ch);
void fill_square_ch_stream(s_emu *emu, int ch);
void div_apu_update(s_audio *au);
void wavelength_sweep(s_audio *au, s_io *io);
void volume_sweep(s_audio *au, int *volume, int ch);
void update_ch3_state(s_audio *au, s_io *io);

#endif //AUDIO_H

