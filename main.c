#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <SDL.h>

#include "emulator.h"

int main(void)
{
    s_emu emu;
    
    if(0 != initialize_emulator(&emu))
    {
        destroy_emulator();
        return EXIT_FAILURE;
    }
    
    emulate(&emu);
    destroy_emulator();
    
    return 0;
}

