#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "emulator.h"

extern int initialize_screen(s_screen *screen);
extern void destroy_screen(s_screen *screen);
extern void resize_screen(s_screen *screen);

#endif //GRAPHICS_H

