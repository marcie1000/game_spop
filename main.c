#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <SDL.h>

#include "emulator.h"

int main(int argc, char *argv[])
{
    printf("%d, %s\n", argc, argv[0]);
    
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

