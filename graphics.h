#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "emulator.h"
#include <stdint.h>

extern int initialize_screen(s_emu *emu);
extern void destroy_screen(s_screen *scr);
extern void resize_screen(s_screen *scr);
extern void scan_OAM(s_emu *emu);
//extern int load_tile(s_emu *emu, uint8_t x_pos, uint8_t y_pos, uint8_t index, UNUSED uint8_t attributes);
extern int draw_scanline(s_emu *emu);
extern int lockscreen(s_screen *scr);
extern void ppu_modes_and_scanlines(s_emu *emu);
extern void render_frame_and_vblank_if_needed(s_emu *emu);
extern int draw_window(s_emu *emu, int i, uint8_t *pixel, uint8_t *color);
extern int draw_background(s_emu *emu, int i, uint8_t *pixel, uint8_t *color);
extern int draw_OBJ(s_emu *emu, int i, uint8_t *pixel, uint8_t *color);
extern int draw_OBJ_tile(s_emu *emu, int i, uint8_t *pixel, uint8_t *color, uint8_t sptd);
extern int DMA_transfer(s_emu *emu);
extern int lockscreen_plot(s_plot *plot);
extern int initialize_plot_win(s_emu *emu);
extern void destroy_plot_win(s_plot *plot);
extern void draw_plot(s_emu *emu);
extern int render_plot(s_emu *emu);

#endif //GRAPHICS_H

