#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <SDL.h>

#include "emulator.h"

int main(void)
{
    s_emu emu;
    void (*opcode_functions[OPCODE_NB]) (s_emu*, uint32_t);
    
    if(0 != initialize_emulator(&emu, opcode_functions))
    {
        destroy_emulator();
        return EXIT_FAILURE;
    }
    
    emulate(&emu, opcode_functions);
    destroy_emulator();
    
    return 0;
}

