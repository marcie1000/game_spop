#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <SDL.h>

#include "emulator.h"

int main(int argc, char *argv[])
{
    s_emu emu;    
    if(0 != parse_start_options(&emu.opt, argc, argv))
        exit(EXIT_FAILURE);
    
    if(0 != initialize_emulator(&emu))
        destroy_emulator(&emu, EXIT_FAILURE);
    
    emulate(&emu);
    destroy_emulator(&emu, EXIT_SUCCESS);
    
    return 0;
}

