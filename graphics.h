#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "emulator.h"

extern int initialize_screen(s_emu *emu);
extern void destroy_screen(s_screen *screen);
extern void resize_screen(s_screen *screen);
//extern int scan_OAM(s_emu *emu);
//extern int load_tile(s_emu *emu, uint8_t x_pos, uint8_t y_pos, uint8_t index, UNUSED uint8_t attributes);
extern int draw_scanline(s_emu *emu);
extern int lockscreen(s_screen *screen);
extern void ppu_modes_and_scanlines(s_emu *emu);
extern void render_frame_and_vblank_if_needed(s_emu *emu);
extern int draw_window(s_emu *emu, UNUSED size_t i);
extern int draw_background(s_emu *emu, size_t i, uint8_t *pixel);
extern int draw_OBJ(s_emu *emu, UNUSED size_t i);

#endif //GRAPHICS_H

