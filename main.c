#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <SDL.h>

#include "emulator.h"

int main(int argc, char *argv[])
{
    s_emu emu;
    bool rom_argument = true;
    char filename[255] = "roms/Tetris (JUE) (V1.0) [!].gb";
    
    if(argc > 1)
    {
        rom_argument = true;
        snprintf(filename, 255, "%s", argv[1]);
    }
    
    if(0 != initialize_emulator(&emu, rom_argument, filename))
        destroy_emulator(&emu, EXIT_FAILURE);
    
    emulate(&emu);
    destroy_emulator(&emu, EXIT_SUCCESS);
    
    return 0;
}

