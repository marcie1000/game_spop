#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "emulator.h"

extern int initialize_screen(s_screen *screen);
extern void destroy_screen(s_screen *screen);
extern void resize_screen(s_screen *screen);
extern int scan_OAM(s_emu *emu);
extern int load_tile(s_emu *emu, uint8_t x_pos, uint8_t y_pos, uint8_t index, UNUSED uint8_t attributes);
extern int draw_scanline(s_emu *emu);
extern int lockscreen(s_screen *screen);

#endif //GRAPHICS_H

