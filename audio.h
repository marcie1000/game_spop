#ifndef AUDIO_H
#define AUDIO_H

#include "emulator.h"

int init_audio(s_emu *emu);
void destroy_audio(s_emu *emu);
void audio_update(s_emu *emu);

#endif //AUDIO_H

