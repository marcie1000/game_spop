#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>

#include "cpu.h"
#include "emulator.h"
#include "opcodes.h"

void opcode_unimplemented(s_emu *emu, UNUSED uint32_t op)
{
    printf("WARNING: instruction %x unimplemented!\n", op);
    emu->in.quit = SDL_TRUE;
}

