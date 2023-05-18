#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <SDL.h>

#include "emulator.h"

int main(int argc, char *argv[])
{
    s_emu emu;
    bool rom_argument = false;
    bool bootrom = true;
    char filename[255] = "roms/cpu_instrs.gb";
    
    if(argc == 2)
    {
        if(NULL == strstr(argv[1], "--"))
        {
            snprintf(filename, 255, "%s", argv[1]);
            rom_argument = true;
        }
    }
    else if(argc > 2)
    {
        if(NULL == strstr(argv[1], "--"))
        {
            rom_argument = true;
            snprintf(filename, 255, "%s", argv[1]);
            if(NULL != strstr(argv[2], "--bypass-bootrom"))
                bootrom = false;
        }
        else if(NULL != strstr(argv[1], "--bypass-bootrom"))
        {
            if(NULL == strstr(argv[2], "--"))
            {
                snprintf(filename, 255, "%s", argv[2]);
                rom_argument = true;
                bootrom = false;
            }
        }
    }
    
    if(0 != initialize_emulator(&emu, rom_argument, filename, bootrom))
        destroy_emulator(&emu, EXIT_FAILURE);
    
    emulate(&emu, bootrom);
    destroy_emulator(&emu, EXIT_SUCCESS);
    
    return 0;
}

