#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>
#include "emulator.h"
#include "cpu.h"

void update_event(s_input *input)
{
    SDL_Event event;
    while(SDL_PollEvent(&event))
    {
        switch(event.type)
        {
            case(SDL_QUIT):
                input->quit = SDL_TRUE;
                break;
            case(SDL_KEYDOWN):
                input->key[event.key.keysym.scancode] = SDL_TRUE;
                break;
            case(SDL_KEYUP):
                input->key[event.key.keysym.scancode] = SDL_FALSE;
                break;
            case(SDL_MOUSEMOTION):
                input->x = event.motion.x;
                input->y = event.motion.y;
                input->xrel = event.motion.xrel;
                input->yrel = event.motion.yrel;
                break;
            case(SDL_MOUSEWHEEL):
                input->xwheel = event.wheel.x;
                input->ywheel = event.wheel.y;
                break;
            case(SDL_MOUSEBUTTONDOWN):
                input->mouse[event.button.button] = SDL_TRUE;
                break;
            case(SDL_MOUSEBUTTONUP):
                input->mouse[event.button.button] = SDL_FALSE;
                break;
            case(SDL_WINDOWEVENT):
                if(event.window.event == SDL_WINDOWEVENT_RESIZED)
                    input->resize = SDL_TRUE;
                break;
        }
    }
}

int initialize_SDL(void)
{
    if(0 != SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO))
    {
        fprintf(stderr, "Error SDL_Init: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}

int initialize_emulator(s_emu *emu)
{
    if(0 != initialize_cpu(&emu->cpu))
        return EXIT_FAILURE;
    initialize_length_table(emu);
    memset(&emu->in, 0, sizeof(s_input));
    if(0 != initialize_SDL())
        return EXIT_FAILURE;
    init_opcodes_pointers(emu->opcode_functions);
    init_cb_pointers(emu->cb_functions);
    
    if(0 != load_boot_rom(&emu->cpu))
        return EXIT_FAILURE;
    
    return EXIT_SUCCESS;
}

int load_boot_rom(s_cpu *cpu)
{
    FILE *bootrom = fopen("boot_rom/dmg_rom.bin", "rb");
    if(NULL == bootrom)
    {
        perror("ERROR: cannot open boot_rom/dmg_rom.bin: ");
        return EXIT_FAILURE;
    }
    
    fread(&cpu->mem[0], sizeof(cpu->mem[0]), 0x100, bootrom);
    fclose(bootrom);
    
    printf("Boot rom loaded.\n");
    
    return EXIT_SUCCESS;
}

void destroy_emulator(void)
{
    destroy_SDL();
}

void destroy_SDL(void)
{
    SDL_Quit();
}

void emulate(s_emu *emu)
{
    emu->cpu.cycles = 70224;
    
    while(!emu->in.quit)
    {
        emu->cpu.cycles -= 70224;
        
        update_event(&emu->in);
        
        //one frame takes 70224 instructions
        while(!emu->in.quit && emu->cpu.cycles < 70224)
            interpret(emu, emu->opcode_functions);
        
        SDL_Delay(16);
        
    }
}

