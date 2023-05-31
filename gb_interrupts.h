#ifndef GB_INTERRUPTS_H
#define GB_INTERRUPTS_H

#include "emulator.h"

extern void interrupt_handler(s_emu *emu);
extern void LCD_STAT_interrupt_flags(s_emu *emu);
extern void timer_handle(s_emu *emu);

#endif //GB_INTERRUPTS_H

