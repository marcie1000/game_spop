#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL.h>
#include "emulator.h"
#include "cpu.h"
#include "graphics.h"

void update_event(s_input *input)
{
    while(SDL_PollEvent(&input->event))
    {
        switch(input->event.type)
        {
            case(SDL_QUIT):
                input->quit = SDL_TRUE;
                break;
            case(SDL_KEYDOWN):
                input->key[input->event.key.keysym.scancode] = SDL_TRUE;
                break;
            case(SDL_KEYUP):
                input->key[input->event.key.keysym.scancode] = SDL_FALSE;
                break;
            case(SDL_MOUSEMOTION):
                input->x = input->event.motion.x;
                input->y = input->event.motion.y;
                input->xrel = input->event.motion.xrel;
                input->yrel = input->event.motion.yrel;
                break;
            case(SDL_MOUSEWHEEL):
                input->xwheel = input->event.wheel.x;
                input->ywheel = input->event.wheel.y;
                break;
            case(SDL_MOUSEBUTTONDOWN):
                input->mouse[input->event.button.button] = SDL_TRUE;
                break;
            case(SDL_MOUSEBUTTONUP):
                input->mouse[input->event.button.button] = SDL_FALSE;
                break;
            case(SDL_WINDOWEVENT):
                if(input->event.window.event == SDL_WINDOWEVENT_RESIZED)
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
    s_opt *opt = &emu->opt;
    
    if(0 != initialize_screen(emu))
        return EXIT_FAILURE;
    if(0 != initialize_cpu(&emu->cpu))
        return EXIT_FAILURE;
    initialize_length_table(emu);
    memset(&emu->in, 0, sizeof(s_input));
    if(0 != initialize_SDL())
        return EXIT_FAILURE;
    init_opcodes_pointers(emu->opcode_functions);
    init_cb_pointers(emu->cb_functions);
    init_mnemonic_index(emu);
    init_prefix_mnemonic_index(emu);
    
    if(0 != load_rom(emu))
        return EXIT_FAILURE;
    
    if(emu->opt.bootrom)
    {
        if(0 != load_boot_rom(&emu->cpu))
            return EXIT_FAILURE;
    }
    
    //gb doctor log file
    opt->gbdoc_log = fopen("gbdoc.log", "w");
    if(opt->gbdoc_log == NULL)
    {
        fprintf(stderr, "fopen gbdoc.log: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}

void init_prefix_mnemonic_index(s_emu *emu)
{
    snprintf(emu->prefixed_mnemonic_index[0x00], 15, "RLC B");
    snprintf(emu->prefixed_mnemonic_index[0x01], 15, "RLC C");
    snprintf(emu->prefixed_mnemonic_index[0x02], 15, "RLC D");
    snprintf(emu->prefixed_mnemonic_index[0x03], 15, "RLC E");
    snprintf(emu->prefixed_mnemonic_index[0x04], 15, "RLC H");
    snprintf(emu->prefixed_mnemonic_index[0x05], 15, "RLC L");
    snprintf(emu->prefixed_mnemonic_index[0x06], 15, "RLC (HL)");
    snprintf(emu->prefixed_mnemonic_index[0x07], 15, "RLC A");
    snprintf(emu->prefixed_mnemonic_index[0x08], 15, "RRC B");
    snprintf(emu->prefixed_mnemonic_index[0x09], 15, "RRC C");
    snprintf(emu->prefixed_mnemonic_index[0x0A], 15, "RRC D");
    snprintf(emu->prefixed_mnemonic_index[0x0B], 15, "RRC E");
    snprintf(emu->prefixed_mnemonic_index[0x0C], 15, "RRC H");
    snprintf(emu->prefixed_mnemonic_index[0x0D], 15, "RRC L");
    snprintf(emu->prefixed_mnemonic_index[0x0E], 15, "RRC (HL)");
    snprintf(emu->prefixed_mnemonic_index[0x0F], 15, "RRC A");
    snprintf(emu->prefixed_mnemonic_index[0x10], 15, "RL B");
    snprintf(emu->prefixed_mnemonic_index[0x11], 15, "RL C");
    snprintf(emu->prefixed_mnemonic_index[0x12], 15, "RL D");
    snprintf(emu->prefixed_mnemonic_index[0x13], 15, "RL E");
    snprintf(emu->prefixed_mnemonic_index[0x14], 15, "RL H");
    snprintf(emu->prefixed_mnemonic_index[0x15], 15, "RL L");
    snprintf(emu->prefixed_mnemonic_index[0x16], 15, "RL (HL)");
    snprintf(emu->prefixed_mnemonic_index[0x17], 15, "RL A");
    snprintf(emu->prefixed_mnemonic_index[0x18], 15, "RR B");
    snprintf(emu->prefixed_mnemonic_index[0x19], 15, "RR C");
    snprintf(emu->prefixed_mnemonic_index[0x1A], 15, "RR D");
    snprintf(emu->prefixed_mnemonic_index[0x1B], 15, "RR E");
    snprintf(emu->prefixed_mnemonic_index[0x1C], 15, "RR H");
    snprintf(emu->prefixed_mnemonic_index[0x1D], 15, "RR L");
    snprintf(emu->prefixed_mnemonic_index[0x1E], 15, "RR (HL)");
    snprintf(emu->prefixed_mnemonic_index[0x1F], 15, "RR A");
    snprintf(emu->prefixed_mnemonic_index[0x20], 15, "SLA B");
    snprintf(emu->prefixed_mnemonic_index[0x21], 15, "SLA C");
    snprintf(emu->prefixed_mnemonic_index[0x22], 15, "SLA D");
    snprintf(emu->prefixed_mnemonic_index[0x23], 15, "SLA E");
    snprintf(emu->prefixed_mnemonic_index[0x24], 15, "SLA H");
    snprintf(emu->prefixed_mnemonic_index[0x25], 15, "SLA L");
    snprintf(emu->prefixed_mnemonic_index[0x26], 15, "SLA (HL)");
    snprintf(emu->prefixed_mnemonic_index[0x27], 15, "SLA A");
    snprintf(emu->prefixed_mnemonic_index[0x28], 15, "SRA B");
    snprintf(emu->prefixed_mnemonic_index[0x29], 15, "SRA C");
    snprintf(emu->prefixed_mnemonic_index[0x2A], 15, "SRA D");
    snprintf(emu->prefixed_mnemonic_index[0x2B], 15, "SRA E");
    snprintf(emu->prefixed_mnemonic_index[0x2C], 15, "SRA H");
    snprintf(emu->prefixed_mnemonic_index[0x2D], 15, "SRA L");
    snprintf(emu->prefixed_mnemonic_index[0x2E], 15, "SRA (HL)");
    snprintf(emu->prefixed_mnemonic_index[0x2F], 15, "SRA A");
    snprintf(emu->prefixed_mnemonic_index[0x30], 15, "SWAP B");
    snprintf(emu->prefixed_mnemonic_index[0x31], 15, "SWAP C");
    snprintf(emu->prefixed_mnemonic_index[0x32], 15, "SWAP D");
    snprintf(emu->prefixed_mnemonic_index[0x33], 15, "SWAP E");
    snprintf(emu->prefixed_mnemonic_index[0x34], 15, "SWAP H");
    snprintf(emu->prefixed_mnemonic_index[0x35], 15, "SWAP L");
    snprintf(emu->prefixed_mnemonic_index[0x36], 15, "SWAP (HL)");
    snprintf(emu->prefixed_mnemonic_index[0x37], 15, "SWAP A");
    snprintf(emu->prefixed_mnemonic_index[0x38], 15, "SRL B");
    snprintf(emu->prefixed_mnemonic_index[0x39], 15, "SRL C");
    snprintf(emu->prefixed_mnemonic_index[0x3A], 15, "SRL D");
    snprintf(emu->prefixed_mnemonic_index[0x3B], 15, "SRL E");
    snprintf(emu->prefixed_mnemonic_index[0x3C], 15, "SRL H");
    snprintf(emu->prefixed_mnemonic_index[0x3D], 15, "SRL L");
    snprintf(emu->prefixed_mnemonic_index[0x3E], 15, "SRL (HL)");
    snprintf(emu->prefixed_mnemonic_index[0x3F], 15, "SRL A");
    snprintf(emu->prefixed_mnemonic_index[0x40], 15, "BIT 0,B");
    snprintf(emu->prefixed_mnemonic_index[0x41], 15, "BIT 0,C");
    snprintf(emu->prefixed_mnemonic_index[0x42], 15, "BIT 0,D");
    snprintf(emu->prefixed_mnemonic_index[0x43], 15, "BIT 0,E");
    snprintf(emu->prefixed_mnemonic_index[0x44], 15, "BIT 0,H");
    snprintf(emu->prefixed_mnemonic_index[0x45], 15, "BIT 0,L");
    snprintf(emu->prefixed_mnemonic_index[0x46], 15, "BIT 0,(HL)");
    snprintf(emu->prefixed_mnemonic_index[0x47], 15, "BIT 0,A");
    snprintf(emu->prefixed_mnemonic_index[0x48], 15, "BIT 1,B");
    snprintf(emu->prefixed_mnemonic_index[0x49], 15, "BIT 1,C");
    snprintf(emu->prefixed_mnemonic_index[0x4A], 15, "BIT 1,D");
    snprintf(emu->prefixed_mnemonic_index[0x4B], 15, "BIT 1,E");
    snprintf(emu->prefixed_mnemonic_index[0x4C], 15, "BIT 1,H");
    snprintf(emu->prefixed_mnemonic_index[0x4D], 15, "BIT 1,L");
    snprintf(emu->prefixed_mnemonic_index[0x4E], 15, "BIT 1,(HL)");
    snprintf(emu->prefixed_mnemonic_index[0x4F], 15, "BIT 1,A");
    snprintf(emu->prefixed_mnemonic_index[0x50], 15, "BIT 2,B");
    snprintf(emu->prefixed_mnemonic_index[0x51], 15, "BIT 2,C");
    snprintf(emu->prefixed_mnemonic_index[0x52], 15, "BIT 2,D");
    snprintf(emu->prefixed_mnemonic_index[0x53], 15, "BIT 2,E");
    snprintf(emu->prefixed_mnemonic_index[0x54], 15, "BIT 2,H");
    snprintf(emu->prefixed_mnemonic_index[0x55], 15, "BIT 2,L");
    snprintf(emu->prefixed_mnemonic_index[0x56], 15, "BIT 2,(HL)");
    snprintf(emu->prefixed_mnemonic_index[0x57], 15, "BIT 2,A");
    snprintf(emu->prefixed_mnemonic_index[0x58], 15, "BIT 3,B");
    snprintf(emu->prefixed_mnemonic_index[0x59], 15, "BIT 3,C");
    snprintf(emu->prefixed_mnemonic_index[0x5A], 15, "BIT 3,D");
    snprintf(emu->prefixed_mnemonic_index[0x5B], 15, "BIT 3,E");
    snprintf(emu->prefixed_mnemonic_index[0x5C], 15, "BIT 3,H");
    snprintf(emu->prefixed_mnemonic_index[0x5D], 15, "BIT 3,L");
    snprintf(emu->prefixed_mnemonic_index[0x5E], 15, "BIT 3,(HL)");
    snprintf(emu->prefixed_mnemonic_index[0x5F], 15, "BIT 3,A");
    snprintf(emu->prefixed_mnemonic_index[0x60], 15, "BIT 4,B");
    snprintf(emu->prefixed_mnemonic_index[0x61], 15, "BIT 4,C");
    snprintf(emu->prefixed_mnemonic_index[0x62], 15, "BIT 4,D");
    snprintf(emu->prefixed_mnemonic_index[0x63], 15, "BIT 4,E");
    snprintf(emu->prefixed_mnemonic_index[0x64], 15, "BIT 4,H");
    snprintf(emu->prefixed_mnemonic_index[0x65], 15, "BIT 4,L");
    snprintf(emu->prefixed_mnemonic_index[0x66], 15, "BIT 4,(HL)");
    snprintf(emu->prefixed_mnemonic_index[0x67], 15, "BIT 4,A");
    snprintf(emu->prefixed_mnemonic_index[0x68], 15, "BIT 5,B");
    snprintf(emu->prefixed_mnemonic_index[0x69], 15, "BIT 5,C");
    snprintf(emu->prefixed_mnemonic_index[0x6A], 15, "BIT 5,D");
    snprintf(emu->prefixed_mnemonic_index[0x6B], 15, "BIT 5,E");
    snprintf(emu->prefixed_mnemonic_index[0x6C], 15, "BIT 5,H");
    snprintf(emu->prefixed_mnemonic_index[0x6D], 15, "BIT 5,L");
    snprintf(emu->prefixed_mnemonic_index[0x6E], 15, "BIT 5,(HL)");
    snprintf(emu->prefixed_mnemonic_index[0x6F], 15, "BIT 5,A");
    snprintf(emu->prefixed_mnemonic_index[0x70], 15, "BIT 6,B");
    snprintf(emu->prefixed_mnemonic_index[0x71], 15, "BIT 6,C");
    snprintf(emu->prefixed_mnemonic_index[0x72], 15, "BIT 6,D");
    snprintf(emu->prefixed_mnemonic_index[0x73], 15, "BIT 6,E");
    snprintf(emu->prefixed_mnemonic_index[0x74], 15, "BIT 6,H");
    snprintf(emu->prefixed_mnemonic_index[0x75], 15, "BIT 6,L");
    snprintf(emu->prefixed_mnemonic_index[0x76], 15, "BIT 6,(HL)");
    snprintf(emu->prefixed_mnemonic_index[0x77], 15, "BIT 6,A");
    snprintf(emu->prefixed_mnemonic_index[0x78], 15, "BIT 7,B");
    snprintf(emu->prefixed_mnemonic_index[0x79], 15, "BIT 7,C");
    snprintf(emu->prefixed_mnemonic_index[0x7A], 15, "BIT 7,D");
    snprintf(emu->prefixed_mnemonic_index[0x7B], 15, "BIT 7,E");
    snprintf(emu->prefixed_mnemonic_index[0x7C], 15, "BIT 7,H");
    snprintf(emu->prefixed_mnemonic_index[0x7D], 15, "BIT 7,L");
    snprintf(emu->prefixed_mnemonic_index[0x7E], 15, "BIT 7,(HL)");
    snprintf(emu->prefixed_mnemonic_index[0x7F], 15, "BIT 7,A");
    snprintf(emu->prefixed_mnemonic_index[0x80], 15, "RES 0,B");
    snprintf(emu->prefixed_mnemonic_index[0x81], 15, "RES 0,C");
    snprintf(emu->prefixed_mnemonic_index[0x82], 15, "RES 0,D");
    snprintf(emu->prefixed_mnemonic_index[0x83], 15, "RES 0,E");
    snprintf(emu->prefixed_mnemonic_index[0x84], 15, "RES 0,H");
    snprintf(emu->prefixed_mnemonic_index[0x85], 15, "RES 0,L");
    snprintf(emu->prefixed_mnemonic_index[0x86], 15, "RES 0,(HL)");
    snprintf(emu->prefixed_mnemonic_index[0x87], 15, "RES 0,A");
    snprintf(emu->prefixed_mnemonic_index[0x88], 15, "RES 1,B");
    snprintf(emu->prefixed_mnemonic_index[0x89], 15, "RES 1,C");
    snprintf(emu->prefixed_mnemonic_index[0x8A], 15, "RES 1,D");
    snprintf(emu->prefixed_mnemonic_index[0x8B], 15, "RES 1,E");
    snprintf(emu->prefixed_mnemonic_index[0x8C], 15, "RES 1,H");
    snprintf(emu->prefixed_mnemonic_index[0x8D], 15, "RES 1,L");
    snprintf(emu->prefixed_mnemonic_index[0x8E], 15, "RES 1,(HL)");
    snprintf(emu->prefixed_mnemonic_index[0x8F], 15, "RES 1,A");
    snprintf(emu->prefixed_mnemonic_index[0x90], 15, "RES 2,B");
    snprintf(emu->prefixed_mnemonic_index[0x91], 15, "RES 2,C");
    snprintf(emu->prefixed_mnemonic_index[0x92], 15, "RES 2,D");
    snprintf(emu->prefixed_mnemonic_index[0x93], 15, "RES 2,E");
    snprintf(emu->prefixed_mnemonic_index[0x94], 15, "RES 2,H");
    snprintf(emu->prefixed_mnemonic_index[0x95], 15, "RES 2,L");
    snprintf(emu->prefixed_mnemonic_index[0x96], 15, "RES 2,(HL)");
    snprintf(emu->prefixed_mnemonic_index[0x97], 15, "RES 2,A");
    snprintf(emu->prefixed_mnemonic_index[0x98], 15, "RES 3,B");
    snprintf(emu->prefixed_mnemonic_index[0x99], 15, "RES 3,C");
    snprintf(emu->prefixed_mnemonic_index[0x9A], 15, "RES 3,D");
    snprintf(emu->prefixed_mnemonic_index[0x9B], 15, "RES 3,E");
    snprintf(emu->prefixed_mnemonic_index[0x9C], 15, "RES 3,H");
    snprintf(emu->prefixed_mnemonic_index[0x9D], 15, "RES 3,L");
    snprintf(emu->prefixed_mnemonic_index[0x9E], 15, "RES 3,(HL)");
    snprintf(emu->prefixed_mnemonic_index[0x9F], 15, "RES 3,A");
    snprintf(emu->prefixed_mnemonic_index[0xA0], 15, "RES 4,B");
    snprintf(emu->prefixed_mnemonic_index[0xA1], 15, "RES 4,C");
    snprintf(emu->prefixed_mnemonic_index[0xA2], 15, "RES 4,D");
    snprintf(emu->prefixed_mnemonic_index[0xA3], 15, "RES 4,E");
    snprintf(emu->prefixed_mnemonic_index[0xA4], 15, "RES 4,H");
    snprintf(emu->prefixed_mnemonic_index[0xA5], 15, "RES 4,L");
    snprintf(emu->prefixed_mnemonic_index[0xA6], 15, "RES 4,(HL)");
    snprintf(emu->prefixed_mnemonic_index[0xA7], 15, "RES 4,A");
    snprintf(emu->prefixed_mnemonic_index[0xA8], 15, "RES 5,B");
    snprintf(emu->prefixed_mnemonic_index[0xA9], 15, "RES 5,C");
    snprintf(emu->prefixed_mnemonic_index[0xAA], 15, "RES 5,D");
    snprintf(emu->prefixed_mnemonic_index[0xAB], 15, "RES 5,E");
    snprintf(emu->prefixed_mnemonic_index[0xAC], 15, "RES 5,H");
    snprintf(emu->prefixed_mnemonic_index[0xAD], 15, "RES 5,L");
    snprintf(emu->prefixed_mnemonic_index[0xAE], 15, "RES 5,(HL)");
    snprintf(emu->prefixed_mnemonic_index[0xAF], 15, "RES 5,A");
    snprintf(emu->prefixed_mnemonic_index[0xB0], 15, "RES 6,B");
    snprintf(emu->prefixed_mnemonic_index[0xB1], 15, "RES 6,C");
    snprintf(emu->prefixed_mnemonic_index[0xB2], 15, "RES 6,D");
    snprintf(emu->prefixed_mnemonic_index[0xB3], 15, "RES 6,E");
    snprintf(emu->prefixed_mnemonic_index[0xB4], 15, "RES 6,H");
    snprintf(emu->prefixed_mnemonic_index[0xB5], 15, "RES 6,L");
    snprintf(emu->prefixed_mnemonic_index[0xB6], 15, "RES 6,(HL)");
    snprintf(emu->prefixed_mnemonic_index[0xB7], 15, "RES 6,A");
    snprintf(emu->prefixed_mnemonic_index[0xB8], 15, "RES 7,B");
    snprintf(emu->prefixed_mnemonic_index[0xB9], 15, "RES 7,C");
    snprintf(emu->prefixed_mnemonic_index[0xBA], 15, "RES 7,D");
    snprintf(emu->prefixed_mnemonic_index[0xBB], 15, "RES 7,E");
    snprintf(emu->prefixed_mnemonic_index[0xBC], 15, "RES 7,H");
    snprintf(emu->prefixed_mnemonic_index[0xBD], 15, "RES 7,L");
    snprintf(emu->prefixed_mnemonic_index[0xBE], 15, "RES 7,(HL)");
    snprintf(emu->prefixed_mnemonic_index[0xBF], 15, "RES 7,A");
    snprintf(emu->prefixed_mnemonic_index[0xC0], 15, "SET 0,B");
    snprintf(emu->prefixed_mnemonic_index[0xC1], 15, "SET 0,C");
    snprintf(emu->prefixed_mnemonic_index[0xC2], 15, "SET 0,D");
    snprintf(emu->prefixed_mnemonic_index[0xC3], 15, "SET 0,E");
    snprintf(emu->prefixed_mnemonic_index[0xC4], 15, "SET 0,H");
    snprintf(emu->prefixed_mnemonic_index[0xC5], 15, "SET 0,L");
    snprintf(emu->prefixed_mnemonic_index[0xC6], 15, "SET 0,(HL)");
    snprintf(emu->prefixed_mnemonic_index[0xC7], 15, "SET 0,A");
    snprintf(emu->prefixed_mnemonic_index[0xC8], 15, "SET 1,B");
    snprintf(emu->prefixed_mnemonic_index[0xC9], 15, "SET 1,C");
    snprintf(emu->prefixed_mnemonic_index[0xCA], 15, "SET 1,D");
    snprintf(emu->prefixed_mnemonic_index[0xCB], 15, "SET 1,E");
    snprintf(emu->prefixed_mnemonic_index[0xCC], 15, "SET 1,H");
    snprintf(emu->prefixed_mnemonic_index[0xCD], 15, "SET 1,L");
    snprintf(emu->prefixed_mnemonic_index[0xCE], 15, "SET 1,(HL)");
    snprintf(emu->prefixed_mnemonic_index[0xCF], 15, "SET 1,A");
    snprintf(emu->prefixed_mnemonic_index[0xD0], 15, "SET 2,B");
    snprintf(emu->prefixed_mnemonic_index[0xD1], 15, "SET 2,C");
    snprintf(emu->prefixed_mnemonic_index[0xD2], 15, "SET 2,D");
    snprintf(emu->prefixed_mnemonic_index[0xD3], 15, "SET 2,E");
    snprintf(emu->prefixed_mnemonic_index[0xD4], 15, "SET 2,H");
    snprintf(emu->prefixed_mnemonic_index[0xD5], 15, "SET 2,L");
    snprintf(emu->prefixed_mnemonic_index[0xD6], 15, "SET 2,(HL)");
    snprintf(emu->prefixed_mnemonic_index[0xD7], 15, "SET 2,A");
    snprintf(emu->prefixed_mnemonic_index[0xD8], 15, "SET 3,B");
    snprintf(emu->prefixed_mnemonic_index[0xD9], 15, "SET 3,C");
    snprintf(emu->prefixed_mnemonic_index[0xDA], 15, "SET 3,D");
    snprintf(emu->prefixed_mnemonic_index[0xDB], 15, "SET 3,E");
    snprintf(emu->prefixed_mnemonic_index[0xDC], 15, "SET 3,H");
    snprintf(emu->prefixed_mnemonic_index[0xDD], 15, "SET 3,L");
    snprintf(emu->prefixed_mnemonic_index[0xDE], 15, "SET 3,(HL)");
    snprintf(emu->prefixed_mnemonic_index[0xDF], 15, "SET 3,A");
    snprintf(emu->prefixed_mnemonic_index[0xE0], 15, "SET 4,B");
    snprintf(emu->prefixed_mnemonic_index[0xE1], 15, "SET 4,C");
    snprintf(emu->prefixed_mnemonic_index[0xE2], 15, "SET 4,D");
    snprintf(emu->prefixed_mnemonic_index[0xE3], 15, "SET 4,E");
    snprintf(emu->prefixed_mnemonic_index[0xE4], 15, "SET 4,H");
    snprintf(emu->prefixed_mnemonic_index[0xE5], 15, "SET 4,L");
    snprintf(emu->prefixed_mnemonic_index[0xE6], 15, "SET 4,(HL)");
    snprintf(emu->prefixed_mnemonic_index[0xE7], 15, "SET 4,A");
    snprintf(emu->prefixed_mnemonic_index[0xE8], 15, "SET 5,B");
    snprintf(emu->prefixed_mnemonic_index[0xE9], 15, "SET 5,C");
    snprintf(emu->prefixed_mnemonic_index[0xEA], 15, "SET 5,D");
    snprintf(emu->prefixed_mnemonic_index[0xEB], 15, "SET 5,E");
    snprintf(emu->prefixed_mnemonic_index[0xEC], 15, "SET 5,H");
    snprintf(emu->prefixed_mnemonic_index[0xED], 15, "SET 5,L");
    snprintf(emu->prefixed_mnemonic_index[0xEE], 15, "SET 5,(HL)");
    snprintf(emu->prefixed_mnemonic_index[0xEF], 15, "SET 5,A");
    snprintf(emu->prefixed_mnemonic_index[0xF0], 15, "SET 6,B");
    snprintf(emu->prefixed_mnemonic_index[0xF1], 15, "SET 6,C");
    snprintf(emu->prefixed_mnemonic_index[0xF2], 15, "SET 6,D");
    snprintf(emu->prefixed_mnemonic_index[0xF3], 15, "SET 6,E");
    snprintf(emu->prefixed_mnemonic_index[0xF4], 15, "SET 6,H");
    snprintf(emu->prefixed_mnemonic_index[0xF5], 15, "SET 6,L");
    snprintf(emu->prefixed_mnemonic_index[0xF6], 15, "SET 6,(HL)");
    snprintf(emu->prefixed_mnemonic_index[0xF7], 15, "SET 6,A");
    snprintf(emu->prefixed_mnemonic_index[0xF8], 15, "SET 7,B");
    snprintf(emu->prefixed_mnemonic_index[0xF9], 15, "SET 7,C");
    snprintf(emu->prefixed_mnemonic_index[0xFA], 15, "SET 7,D");
    snprintf(emu->prefixed_mnemonic_index[0xFB], 15, "SET 7,E");
    snprintf(emu->prefixed_mnemonic_index[0xFC], 15, "SET 7,H");
    snprintf(emu->prefixed_mnemonic_index[0xFD], 15, "SET 7,L");
    snprintf(emu->prefixed_mnemonic_index[0xFE], 15, "SET 7,(HL)");
    snprintf(emu->prefixed_mnemonic_index[0xFF], 15, "SET 7,A");
}

void init_mnemonic_index(s_emu *emu)
{
    snprintf(emu->mnemonic_index[0x00], 15, "NOP");
    snprintf(emu->mnemonic_index[0x01], 15, "LD BC,d16");
    snprintf(emu->mnemonic_index[0x02], 15, "LD (BC),A");
    snprintf(emu->mnemonic_index[0x03], 15, "INC BC");
    snprintf(emu->mnemonic_index[0x04], 15, "INC B");
    snprintf(emu->mnemonic_index[0x05], 15, "DEC B");
    snprintf(emu->mnemonic_index[0x06], 15, "LD B,d8");
    snprintf(emu->mnemonic_index[0x07], 15, "RLCA");
    snprintf(emu->mnemonic_index[0x08], 15, "LD (a16),SP");
    snprintf(emu->mnemonic_index[0x09], 15, "ADD HL,BC");
    snprintf(emu->mnemonic_index[0x0A], 15, "LD A,(BC)");
    snprintf(emu->mnemonic_index[0x0B], 15, "DEC BC");
    snprintf(emu->mnemonic_index[0x0C], 15, "INC C");
    snprintf(emu->mnemonic_index[0x0D], 15, "DEC C");
    snprintf(emu->mnemonic_index[0x0E], 15, "LD C,d8");
    snprintf(emu->mnemonic_index[0x0F], 15, "RRCA");
    snprintf(emu->mnemonic_index[0x10], 15, "STOP 0");
    snprintf(emu->mnemonic_index[0x11], 15, "LD DE,d16");
    snprintf(emu->mnemonic_index[0x12], 15, "LD (DE),A");
    snprintf(emu->mnemonic_index[0x13], 15, "INC DE");
    snprintf(emu->mnemonic_index[0x14], 15, "INC D");
    snprintf(emu->mnemonic_index[0x15], 15, "DEC D");
    snprintf(emu->mnemonic_index[0x16], 15, "LD D,d8");
    snprintf(emu->mnemonic_index[0x17], 15, "RLA");
    snprintf(emu->mnemonic_index[0x18], 15, "JR r8");
    snprintf(emu->mnemonic_index[0x19], 15, "ADD HL,DE");
    snprintf(emu->mnemonic_index[0x1A], 15, "LD A,(DE)");
    snprintf(emu->mnemonic_index[0x1B], 15, "DEC DE");
    snprintf(emu->mnemonic_index[0x1C], 15, "INC E");
    snprintf(emu->mnemonic_index[0x1D], 15, "DEC E");
    snprintf(emu->mnemonic_index[0x1E], 15, "LD E,d8");
    snprintf(emu->mnemonic_index[0x1F], 15, "RRA");
    snprintf(emu->mnemonic_index[0x20], 15, "JR NZ,r8");
    snprintf(emu->mnemonic_index[0x21], 15, "LD HL,d16");
    snprintf(emu->mnemonic_index[0x22], 15, "LD (HL+),A");
    snprintf(emu->mnemonic_index[0x23], 15, "INC HL");
    snprintf(emu->mnemonic_index[0x24], 15, "INC H");
    snprintf(emu->mnemonic_index[0x25], 15, "DEC H");
    snprintf(emu->mnemonic_index[0x26], 15, "LD H,d8");
    snprintf(emu->mnemonic_index[0x27], 15, "DAA");
    snprintf(emu->mnemonic_index[0x28], 15, "JR Z,r8");
    snprintf(emu->mnemonic_index[0x29], 15, "ADD HL,HL");
    snprintf(emu->mnemonic_index[0x2A], 15, "LD A,(HL+)");
    snprintf(emu->mnemonic_index[0x2B], 15, "DEC HL");
    snprintf(emu->mnemonic_index[0x2C], 15, "INC L");
    snprintf(emu->mnemonic_index[0x2D], 15, "DEC L");
    snprintf(emu->mnemonic_index[0x2E], 15, "LD L,d8");
    snprintf(emu->mnemonic_index[0x2F], 15, "CPL");
    snprintf(emu->mnemonic_index[0x30], 15, "JR NC,r8");
    snprintf(emu->mnemonic_index[0x31], 15, "LD SP,d16");
    snprintf(emu->mnemonic_index[0x32], 15, "LD (HL-),A");
    snprintf(emu->mnemonic_index[0x33], 15, "INC SP");
    snprintf(emu->mnemonic_index[0x34], 15, "INC (HL)");
    snprintf(emu->mnemonic_index[0x35], 15, "DEC (HL)");
    snprintf(emu->mnemonic_index[0x36], 15, "LD (HL),d8");
    snprintf(emu->mnemonic_index[0x37], 15, "SCF");
    snprintf(emu->mnemonic_index[0x38], 15, "JR C,r8");
    snprintf(emu->mnemonic_index[0x39], 15, "ADD HL,SP");
    snprintf(emu->mnemonic_index[0x3A], 15, "LD A,(HL-)");
    snprintf(emu->mnemonic_index[0x3B], 15, "DEC SP");
    snprintf(emu->mnemonic_index[0x3C], 15, "INC A");
    snprintf(emu->mnemonic_index[0x3D], 15, "DEC A");
    snprintf(emu->mnemonic_index[0x3E], 15, "LD A,d8");
    snprintf(emu->mnemonic_index[0x3F], 15, "CCF");
    snprintf(emu->mnemonic_index[0x40], 15, "LD B,B");
    snprintf(emu->mnemonic_index[0x41], 15, "LD B,C");
    snprintf(emu->mnemonic_index[0x42], 15, "LD B,D");
    snprintf(emu->mnemonic_index[0x43], 15, "LD B,E");
    snprintf(emu->mnemonic_index[0x44], 15, "LD B,H");
    snprintf(emu->mnemonic_index[0x45], 15, "LD B,L");
    snprintf(emu->mnemonic_index[0x46], 15, "LD B,(HL)");
    snprintf(emu->mnemonic_index[0x47], 15, "LD B,A");
    snprintf(emu->mnemonic_index[0x48], 15, "LD C,B");
    snprintf(emu->mnemonic_index[0x49], 15, "LD C,C");
    snprintf(emu->mnemonic_index[0x4A], 15, "LD C,D");
    snprintf(emu->mnemonic_index[0x4B], 15, "LD C,E");
    snprintf(emu->mnemonic_index[0x4C], 15, "LD C,H");
    snprintf(emu->mnemonic_index[0x4D], 15, "LD C,L");
    snprintf(emu->mnemonic_index[0x4E], 15, "LD C,(HL)");
    snprintf(emu->mnemonic_index[0x4F], 15, "LD C,A");
    snprintf(emu->mnemonic_index[0x50], 15, "LD D,B");
    snprintf(emu->mnemonic_index[0x51], 15, "LD D,C");
    snprintf(emu->mnemonic_index[0x52], 15, "LD D,D");
    snprintf(emu->mnemonic_index[0x53], 15, "LD D,E");
    snprintf(emu->mnemonic_index[0x54], 15, "LD D,H");
    snprintf(emu->mnemonic_index[0x55], 15, "LD D,L");
    snprintf(emu->mnemonic_index[0x56], 15, "LD D,(HL)");
    snprintf(emu->mnemonic_index[0x57], 15, "LD D,A");
    snprintf(emu->mnemonic_index[0x58], 15, "LD E,B");
    snprintf(emu->mnemonic_index[0x59], 15, "LD E,C");
    snprintf(emu->mnemonic_index[0x5A], 15, "LD E,D");
    snprintf(emu->mnemonic_index[0x5B], 15, "LD E,E");
    snprintf(emu->mnemonic_index[0x5C], 15, "LD E,H");
    snprintf(emu->mnemonic_index[0x5D], 15, "LD E,L");
    snprintf(emu->mnemonic_index[0x5E], 15, "LD E,(HL)");
    snprintf(emu->mnemonic_index[0x5F], 15, "LD E,A");
    snprintf(emu->mnemonic_index[0x60], 15, "LD H,B");
    snprintf(emu->mnemonic_index[0x61], 15, "LD H,C");
    snprintf(emu->mnemonic_index[0x62], 15, "LD H,D");
    snprintf(emu->mnemonic_index[0x63], 15, "LD H,E");
    snprintf(emu->mnemonic_index[0x64], 15, "LD H,H");
    snprintf(emu->mnemonic_index[0x65], 15, "LD H,L");
    snprintf(emu->mnemonic_index[0x66], 15, "LD H,(HL)");
    snprintf(emu->mnemonic_index[0x67], 15, "LD H,A");
    snprintf(emu->mnemonic_index[0x68], 15, "LD L,B");
    snprintf(emu->mnemonic_index[0x69], 15, "LD L,C");
    snprintf(emu->mnemonic_index[0x6A], 15, "LD L,D");
    snprintf(emu->mnemonic_index[0x6B], 15, "LD L,E");
    snprintf(emu->mnemonic_index[0x6C], 15, "LD L,H");
    snprintf(emu->mnemonic_index[0x6D], 15, "LD L,L");
    snprintf(emu->mnemonic_index[0x6E], 15, "LD L,(HL)");
    snprintf(emu->mnemonic_index[0x6F], 15, "LD L,A");
    snprintf(emu->mnemonic_index[0x70], 15, "LD (HL),B");
    snprintf(emu->mnemonic_index[0x71], 15, "LD (HL),C");
    snprintf(emu->mnemonic_index[0x72], 15, "LD (HL),D");
    snprintf(emu->mnemonic_index[0x73], 15, "LD (HL),E");
    snprintf(emu->mnemonic_index[0x74], 15, "LD (HL),H");
    snprintf(emu->mnemonic_index[0x75], 15, "LD (HL),L");
    snprintf(emu->mnemonic_index[0x76], 15, "HALT");
    snprintf(emu->mnemonic_index[0x77], 15, "LD (HL),A");
    snprintf(emu->mnemonic_index[0x78], 15, "LD A,B");
    snprintf(emu->mnemonic_index[0x79], 15, "LD A,C");
    snprintf(emu->mnemonic_index[0x7A], 15, "LD A,D");
    snprintf(emu->mnemonic_index[0x7B], 15, "LD A,E");
    snprintf(emu->mnemonic_index[0x7C], 15, "LD A,H");
    snprintf(emu->mnemonic_index[0x7D], 15, "LD A,L");
    snprintf(emu->mnemonic_index[0x7E], 15, "LD A,(HL)");
    snprintf(emu->mnemonic_index[0x7F], 15, "LD A,A");
    snprintf(emu->mnemonic_index[0x80], 15, "ADD A,B");
    snprintf(emu->mnemonic_index[0x81], 15, "ADD A,C");
    snprintf(emu->mnemonic_index[0x82], 15, "ADD A,D");
    snprintf(emu->mnemonic_index[0x83], 15, "ADD A,E");
    snprintf(emu->mnemonic_index[0x84], 15, "ADD A,H");
    snprintf(emu->mnemonic_index[0x85], 15, "ADD A,L");
    snprintf(emu->mnemonic_index[0x86], 15, "ADD A,(HL)");
    snprintf(emu->mnemonic_index[0x87], 15, "ADD A,A");
    snprintf(emu->mnemonic_index[0x88], 15, "ADC A,B");
    snprintf(emu->mnemonic_index[0x89], 15, "ADC A,C");
    snprintf(emu->mnemonic_index[0x8A], 15, "ADC A,D");
    snprintf(emu->mnemonic_index[0x8B], 15, "ADC A,E");
    snprintf(emu->mnemonic_index[0x8C], 15, "ADC A,H");
    snprintf(emu->mnemonic_index[0x8D], 15, "ADC A,L");
    snprintf(emu->mnemonic_index[0x8E], 15, "ADC A,(HL)");
    snprintf(emu->mnemonic_index[0x8F], 15, "ADC A,A");
    snprintf(emu->mnemonic_index[0x90], 15, "SUB B");
    snprintf(emu->mnemonic_index[0x91], 15, "SUB C");
    snprintf(emu->mnemonic_index[0x92], 15, "SUB D");
    snprintf(emu->mnemonic_index[0x93], 15, "SUB E");
    snprintf(emu->mnemonic_index[0x94], 15, "SUB H");
    snprintf(emu->mnemonic_index[0x95], 15, "SUB L");
    snprintf(emu->mnemonic_index[0x96], 15, "SUB (HL)");
    snprintf(emu->mnemonic_index[0x97], 15, "SUB A");
    snprintf(emu->mnemonic_index[0x98], 15, "SBC A,B");
    snprintf(emu->mnemonic_index[0x99], 15, "SBC A,C");
    snprintf(emu->mnemonic_index[0x9A], 15, "SBC A,D");
    snprintf(emu->mnemonic_index[0x9B], 15, "SBC A,E");
    snprintf(emu->mnemonic_index[0x9C], 15, "SBC A,H");
    snprintf(emu->mnemonic_index[0x9D], 15, "SBC A,L");
    snprintf(emu->mnemonic_index[0x9E], 15, "SBC A,(HL)");
    snprintf(emu->mnemonic_index[0x9F], 15, "SBC A,A");
    snprintf(emu->mnemonic_index[0xA0], 15, "AND B");
    snprintf(emu->mnemonic_index[0xA1], 15, "AND C");
    snprintf(emu->mnemonic_index[0xA2], 15, "AND D");
    snprintf(emu->mnemonic_index[0xA3], 15, "AND E");
    snprintf(emu->mnemonic_index[0xA4], 15, "AND H");
    snprintf(emu->mnemonic_index[0xA5], 15, "AND L");
    snprintf(emu->mnemonic_index[0xA6], 15, "AND (HL)");
    snprintf(emu->mnemonic_index[0xA7], 15, "AND A");
    snprintf(emu->mnemonic_index[0xA8], 15, "XOR B");
    snprintf(emu->mnemonic_index[0xA9], 15, "XOR C");
    snprintf(emu->mnemonic_index[0xAA], 15, "XOR D");
    snprintf(emu->mnemonic_index[0xAB], 15, "XOR E");
    snprintf(emu->mnemonic_index[0xAC], 15, "XOR H");
    snprintf(emu->mnemonic_index[0xAD], 15, "XOR L");
    snprintf(emu->mnemonic_index[0xAE], 15, "XOR (HL)");
    snprintf(emu->mnemonic_index[0xAF], 15, "XOR A");
    snprintf(emu->mnemonic_index[0xB0], 15, "OR B");
    snprintf(emu->mnemonic_index[0xB1], 15, "OR C");
    snprintf(emu->mnemonic_index[0xB2], 15, "OR D");
    snprintf(emu->mnemonic_index[0xB3], 15, "OR E");
    snprintf(emu->mnemonic_index[0xB4], 15, "OR H");
    snprintf(emu->mnemonic_index[0xB5], 15, "OR L");
    snprintf(emu->mnemonic_index[0xB6], 15, "OR (HL)");
    snprintf(emu->mnemonic_index[0xB7], 15, "OR A");
    snprintf(emu->mnemonic_index[0xB8], 15, "CP B");
    snprintf(emu->mnemonic_index[0xB9], 15, "CP C");
    snprintf(emu->mnemonic_index[0xBA], 15, "CP D");
    snprintf(emu->mnemonic_index[0xBB], 15, "CP E");
    snprintf(emu->mnemonic_index[0xBC], 15, "CP H");
    snprintf(emu->mnemonic_index[0xBD], 15, "CP L");
    snprintf(emu->mnemonic_index[0xBE], 15, "CP (HL)");
    snprintf(emu->mnemonic_index[0xBF], 15, "CP A");
    snprintf(emu->mnemonic_index[0xC0], 15, "RET NZ");
    snprintf(emu->mnemonic_index[0xC1], 15, "POP BC");
    snprintf(emu->mnemonic_index[0xC2], 15, "JP NZ,a16");
    snprintf(emu->mnemonic_index[0xC3], 15, "JP a16");
    snprintf(emu->mnemonic_index[0xC4], 15, "CALL NZ,a16");
    snprintf(emu->mnemonic_index[0xC5], 15, "PUSH BC");
    snprintf(emu->mnemonic_index[0xC6], 15, "ADD A,d8");
    snprintf(emu->mnemonic_index[0xC7], 15, "RST 00H");
    snprintf(emu->mnemonic_index[0xC8], 15, "RET Z");
    snprintf(emu->mnemonic_index[0xC9], 15, "RET");
    snprintf(emu->mnemonic_index[0xCA], 15, "JP Z,a16");
    snprintf(emu->mnemonic_index[0xCB], 15, "PREFIX CB");
    snprintf(emu->mnemonic_index[0xCC], 15, "CALL Z,a16");
    snprintf(emu->mnemonic_index[0xCD], 15, "CALL a16");
    snprintf(emu->mnemonic_index[0xCE], 15, "ADC A,d8");
    snprintf(emu->mnemonic_index[0xCF], 15, "RST 08H");
    snprintf(emu->mnemonic_index[0xD0], 15, "RET NC");
    snprintf(emu->mnemonic_index[0xD1], 15, "POP DE");
    snprintf(emu->mnemonic_index[0xD2], 15, "JP NC,a16");
    snprintf(emu->mnemonic_index[0xD3], 15, " ");
    snprintf(emu->mnemonic_index[0xD4], 15, "CALL NC,a16");
    snprintf(emu->mnemonic_index[0xD5], 15, "PUSH DE");
    snprintf(emu->mnemonic_index[0xD6], 15, "SUB d8");
    snprintf(emu->mnemonic_index[0xD7], 15, "RST 10H");
    snprintf(emu->mnemonic_index[0xD8], 15, "RET C");
    snprintf(emu->mnemonic_index[0xD9], 15, "RETI");
    snprintf(emu->mnemonic_index[0xDA], 15, "JP C,a16");
    snprintf(emu->mnemonic_index[0xDB], 15, " ");
    snprintf(emu->mnemonic_index[0xDC], 15, "CALL C,a16");
    snprintf(emu->mnemonic_index[0xDD], 15, " ");
    snprintf(emu->mnemonic_index[0xDE], 15, "SBC A,d8");
    snprintf(emu->mnemonic_index[0xDF], 15, "RST 18H");
    snprintf(emu->mnemonic_index[0xE0], 15, "LDH (a8),A");
    snprintf(emu->mnemonic_index[0xE1], 15, "POP HL");
    snprintf(emu->mnemonic_index[0xE2], 15, "LD (C),A");
    snprintf(emu->mnemonic_index[0xE3], 15, " ");
    snprintf(emu->mnemonic_index[0xE4], 15, " ");
    snprintf(emu->mnemonic_index[0xE5], 15, "PUSH HL");
    snprintf(emu->mnemonic_index[0xE6], 15, "AND d8");
    snprintf(emu->mnemonic_index[0xE7], 15, "RST 20H");
    snprintf(emu->mnemonic_index[0xE8], 15, "ADD SP,r8");
    snprintf(emu->mnemonic_index[0xE9], 15, "JP (HL)");
    snprintf(emu->mnemonic_index[0xEA], 15, "LD (a16),A");
    snprintf(emu->mnemonic_index[0xEB], 15, " ");
    snprintf(emu->mnemonic_index[0xEC], 15, " ");
    snprintf(emu->mnemonic_index[0xED], 15, " ");
    snprintf(emu->mnemonic_index[0xEE], 15, "XOR d8");
    snprintf(emu->mnemonic_index[0xEF], 15, "RST 28H");
    snprintf(emu->mnemonic_index[0xF0], 15, "LDH A,(a8)");
    snprintf(emu->mnemonic_index[0xF1], 15, "POP AF");
    snprintf(emu->mnemonic_index[0xF2], 15, "LD A,(C)");
    snprintf(emu->mnemonic_index[0xF3], 15, "DI");
    snprintf(emu->mnemonic_index[0xF4], 15, " ");
    snprintf(emu->mnemonic_index[0xF5], 15, "PUSH AF");
    snprintf(emu->mnemonic_index[0xF6], 15, "OR d8");
    snprintf(emu->mnemonic_index[0xF7], 15, "RST 30H");
    snprintf(emu->mnemonic_index[0xF8], 15, "LD HL,SP+r8");
    snprintf(emu->mnemonic_index[0xF9], 15, "LD SP,HL");
    snprintf(emu->mnemonic_index[0xFA], 15, "LD A,(a16)");
    snprintf(emu->mnemonic_index[0xFB], 15, "EI");
    snprintf(emu->mnemonic_index[0xFC], 15, " ");
    snprintf(emu->mnemonic_index[0xFD], 15, " ");
    snprintf(emu->mnemonic_index[0xFE], 15, "CP d8");
    snprintf(emu->mnemonic_index[0xFF], 15, "RST 38H");
}

int load_boot_rom(s_cpu *cpu)
{
    FILE *bootrom = fopen("boot_rom/dmg_rom.bin", "rb");
    if(NULL == bootrom)
    {
        perror("ERROR: cannot open boot_rom/dmg_rom.bin: ");
        return EXIT_FAILURE;
    }
    
    fread(&cpu->ROM_Bank[0][0], sizeof(cpu->ROM_Bank[0][0]), 0x100, bootrom);
    fclose(bootrom);
    
    printf("Boot rom loaded.\n");
    
    return EXIT_SUCCESS;
}

int load_rom(s_emu *emu)
{
    s_cpu *cpu = &emu->cpu;
    if(!emu->opt.rom_argument)
    {
        memset(cpu->ROM_Bank, 0xFF, sizeof(cpu->ROM_Bank));
        return EXIT_SUCCESS;
    }
    
    FILE *rom = fopen(emu->opt.rom_filename, "rb");
    if(NULL == rom)
    {
        fprintf(stderr, "ERROR: cannot open '%s': %s\n", emu->opt.rom_filename, strerror(errno));
        return EXIT_FAILURE;
    }
    
    fread(&cpu->ROM_Bank[0][0], sizeof(cpu->ROM_Bank[0][0]), ROM_BANK_SIZE, rom);
    //keeps the rom bytes separatly during bootrom execution
    memcpy(cpu->ROM_Bank_0_tmp, cpu->ROM_Bank[0], sizeof(cpu->ROM_Bank[0]));
    fread(&cpu->ROM_Bank[1][0], sizeof(cpu->ROM_Bank[1][0]), ROM_BANK_SIZE, rom);
    fclose(rom);
    
    printf("Rom loaded.\n");
    
    return EXIT_SUCCESS;
}

void destroy_emulator(s_emu *emu, int status)
{
    destroy_screen(&emu->screen);
    destroy_SDL();
    fclose(emu->opt.gbdoc_log);
    exit(status);
}

void destroy_SDL(void)
{
    SDL_Quit();
}

void bypass_bootrom(s_emu *emu)
{
    s_cpu *cpu = &emu->cpu;
    
    cpu->pc = 0x100;
    cpu->regA = 0x01;
    cpu->regF = 0xB0;
    cpu->regC = 0x13;
    cpu->regE = 0xD8;
    cpu->regH = 0x01;
    cpu->regL = 0x4D;
    cpu->sp = 0xFFFE;
    
    cpu->io_reg.SCX = 0;
    cpu->io_reg.SCY = 0;
    cpu->io_reg.LCDC = 0x91;
    cpu->io_reg.NR11 = 0xbf;
    cpu->io_reg.NR14 = 0xbf;
    cpu->io_reg.NR52 = 0xf1;
    cpu->io_reg.NR12 = 0xf3;
    cpu->io_reg.NR13 = 0xff;
    cpu->io_reg.NR50 = 0x77;
    cpu->io_reg.NR51 = 0xf3;
    cpu->io_reg.BGP = 0xfc;    
}

void emulate(s_emu *emu)
{
    s_cpu *cpu = &emu->cpu;
    cpu->t_cycles = 0;
    emu->frame_timer = SDL_GetTicks64();
    
    if(!emu->opt.bootrom)
        bypass_bootrom(emu);
    
    while(!emu->in.quit)
    {
        update_event(&emu->in);
        if(emu->in.resize)
        {
            resize_screen(&emu->screen);
            emu->in.resize = SDL_FALSE;
        }
        if(emu->in.key[SDL_SCANCODE_P])
        {
            pause_menu(emu);
        }
        
        interpret(emu, emu->opcode_functions);
        interpret(emu, emu->opcode_functions);
        interpret(emu, emu->opcode_functions);
        interpret(emu, emu->opcode_functions);
        interpret(emu, emu->opcode_functions);
        interpret(emu, emu->opcode_functions);
        interpret(emu, emu->opcode_functions);
        interpret(emu, emu->opcode_functions);
        
        ppu_modes_and_scanlines(emu);
        
        interpret(emu, emu->opcode_functions);
        interpret(emu, emu->opcode_functions);
        interpret(emu, emu->opcode_functions);
        interpret(emu, emu->opcode_functions);
        interpret(emu, emu->opcode_functions);
        interpret(emu, emu->opcode_functions);
        interpret(emu, emu->opcode_functions);
        interpret(emu, emu->opcode_functions);

        ppu_modes_and_scanlines(emu);
        render_frame_and_vblank_if_needed(emu);        
        
    }
}

void pause_menu(s_emu *emu)
{
    //wait for P key release
    while(!emu->in.quit && emu->in.key[SDL_SCANCODE_P])
    {
        update_event(&emu->in);
        SDL_Delay(5);
    }
    
    printf(
        "Emulator paused.\n"
        "Press P to continue, O to options.\n"
    );
    
    while(!emu->in.quit)
    {
        update_event(&emu->in);
        if(emu->in.key[SDL_SCANCODE_P])
        {
            while(emu->in.key[SDL_SCANCODE_P])
                update_event(&emu->in);
            return;
        }
        if(emu->in.key[SDL_SCANCODE_O])
        {
            if(0 == parse_options_during_exec(&emu->opt))
                return;
        }
        SDL_Delay(5);
    }
}

/**
 * @brief Handle options at program launch or when O is pressed during pause.
 * @param opt
 * @param argc
 * @param argv
 * @param is_program_beginning: to distinguish both cases.
 * @returns 0 if user input OK, 1 if need to exit/re-ask user input.
 */
int parse_options(s_opt *opt, size_t argc, char *argv[], bool is_program_beginning)
{
    const char help_msg_beginning[] = 
    "Usage\n"
    "\n"
    "   ./game_spop <ROM file> [option]\n"
    "\n"
    "Options\n"
    "   --breakpoint         = enable debugging with breakpoints. The program will\n"
    "                          ask to enter a PC value breakpoint at start, and will\n"
    "                          ask for a new breakpoint when the previous one is\n"
    "                          reached.\n"
    "   --bypass-bootrom     = launch directly the ROM (only if a rom is passed\n"
    "                          in argument). This option can only be provided at\n"
    "                          launch.\n"
    "   --debug-info         = at every new instruction, prints the mnemonic, the\n"
    "                          3 bytes object code, and all registers, PC, SP and\n"
    "                          register F flags values in the console. Emulator is\n"
    "                          much slower when this option is enabled.\n"
    "   --gb-doctor          = log cpu state into a file to be used with the Gameboy\n"
    "                          doctor tool (https://github.com/robert/gameboy-doctor).\n"
    "                          Only at launch.\n."
    "   --step, -s           = enable step by step debugging. Emulator will stop\n"
    "                          at each new instruction and ask to continue or edit\n"
    "                          options.\n"
    "   --help, -h           = show this help message and exit.\n";
    
    const char help_msg_during_exec[] = 
    "Options\n"
    "   --breakpoint         = enable debugging with breakpoints. The program will\n"
    "                          ask to enter a PC value breakpoint at start, and will\n"
    "                          ask for a new breakpoint when the previous one is\n"
    "                          reached.\n"
    "   --debug-info         = at every new instruction, prints the mnemonic, the\n"
    "                          3 bytes object code, and all registers, PC, SP and\n"
    "                          register F flags values in the console. Emulator is\n"
    "                          much slower when this option is enabled.\n"
    "   --step, -s           = enable step by step debugging. Emulator will stop\n"
    "                          at each new instruction and ask to continue or edit\n"
    "                          options.\n"
    "   --help, -h           = show this help message and exit.\n";
    
    for(size_t i = 0 + is_program_beginning; i < argc; i++)
    {
        if((0 == strcmp(argv[i], "--bypass-bootrom")) && (is_program_beginning))
            opt->bootrom = false;
        else if(0 == strcmp(argv[i], "--debug-info"))
            opt->debug_info = !opt->debug_info;
        else if(0 == strcmp(argv[i], "--help") || (0 == strcmp(argv[i], "-h")))
        {
            if(is_program_beginning)
                printf("%s", help_msg_beginning);
            else
                printf("%s", help_msg_during_exec);
            return EXIT_FAILURE;
        }
        else if(0 == strcmp(argv[i], "--breakpoint"))
        {
            opt->breakpoints = !opt->breakpoints;
        }
        else if(0 == strcmp(argv[i], "--gb-doctor") && (is_program_beginning))
        {
            opt->gb_doctor = !opt->gb_doctor;
        }
        else if(0 == strcmp(argv[i], "--step") || (0 == strcmp(argv[i], "-s")))
        {
            opt->step_by_step = !opt->step_by_step;
        }
        else if(0 == strncmp(argv[i], "--", 2))
        {
            if(is_program_beginning)
                fprintf(stderr, "Unknown argument '%s', abort.\n\n%s", argv[i], help_msg_beginning);
            else
                fprintf(stderr, "Unknown argument '%s', abort.\n\n%s", argv[i], help_msg_during_exec);
            return EXIT_FAILURE;
        }
        else if(is_program_beginning)
        {
            opt->rom_argument = true;
            snprintf(opt->rom_filename, FILENAME_MAX, "%s", argv[i]);
        }
    }

    ask_breakpoint(opt);
    
    return EXIT_SUCCESS;
}

/**
 * @brief Handle the options menu during pause.
 */
int parse_options_during_exec(s_opt *opt)
{
    bool quit = false;
    char entry[FILENAME_MAX] = "";
    char *sub;
    size_t argc = 0;
    //count number of args
    while(!quit)
    {
        printf("Active options:\n");
        if(opt->breakpoints)
            printf("  --breakpoint\n");
        if(opt->debug_info)
            printf("  --debug-info\n");
        if(opt->step_by_step)
            printf("  --step\n");
        if(!opt->breakpoints && !opt->debug_info && !opt->step_by_step)
            printf("  none.\n");
        
        printf(
            "\nEnter an option to toggle or press ENTER to continue without changes.\n"
            "See --help to have a list of available options.\n"
        );
        if(NULL == fgets(entry, FILENAME_MAX, stdin))
            continue;
        if(entry[0] == '\0')
            continue;
        if(entry[0] == '\n')
            return EXIT_SUCCESS;
        //argc count
        sub = strtok(entry, " \n");
        while(NULL != sub)
        {
            argc++;
            sub = strtok(NULL, " \n");
        }
        char argv[argc][30];
        char *ptr[30];
        //copy to argv
        sub = strtok(entry, " \n");
        snprintf(argv[0], 30, "%s", sub);
        ptr[0] = argv[0];
        for(size_t i = 1; i < argc; i++)
        {
            sub = strtok(NULL, " \n");
            snprintf(argv[i], 30, "%s", sub);
            ptr[i] = argv[i];
        }
        if(0 == parse_options(opt, argc, ptr, false))
            quit = true;
    }
    return EXIT_SUCCESS;
}

/**
 * @brief Handle the command lines arguments gived to the program
 * when lauching. 
 */
int parse_start_options(s_opt *opt, int argc, char *argv[])
{
    opt->bootrom = true;
    opt->rom_argument = false;
    opt->debug_info = false;
    opt->breakpoints = false;
    opt->step_by_step = false;
    opt->gb_doctor = false;
    if(argc <= 1)
        return EXIT_SUCCESS;
    
    if(0 != parse_options(opt, (size_t) argc, argv, true))
        return EXIT_FAILURE;
    
    if(!opt->rom_argument && !opt->bootrom)
    {
        printf("No ROM provided: boot rom will be executed.\n");
        opt->bootrom = true;
    }
    
    return EXIT_SUCCESS;
}

void ask_breakpoint(s_opt *opt)
{
    if(!opt->breakpoints && !opt->step_by_step)
        return;
        
    bool quit = false;
    char bp[10] = "";
    while(!quit)
    {
        printf("Enter a breakpoint value OR press ENTER to start/continue program normally OR\n"
               "enter 'O' to see options menu:\n");
        if(NULL == fgets(bp, 10, stdin))
            continue;
        if(bp[0] == 'O' || bp[0] == 'o')
        {
            while(0 != parse_options_during_exec(opt));
            return;
        }
        if(bp[0] == '\n')
        {
            opt->breakpoints = false;
            printf("Starting/continuing execution with no breakpoint.\n");
            break;
        }
        errno = 0;
        char *endptr;
        long val = strtol(bp, &endptr, 0);
        if(errno != 0)
        {
            fprintf(stderr, "strtol: %s\n", strerror(errno));
            continue;
        }
        if(val == 0 && endptr == bp)
        {
            fprintf(stderr, "Error: no digits were found.\n");
            continue;
        }
        if(val > 0xFFFF)
        {
            fprintf(stderr, "Error: breakpoint value must not exceed 0xFFFF!\n");
            continue;
        }
        if(val < 0)
        {
            fprintf(stderr, "Error: breakpoint value cannot be negative\n");
            continue;
        }
        opt->breakpoint_value = val;
        printf("Breakpoint value set to 0x%04X.\n");
        quit = true;            
    }
}

void gbdoctor(s_emu *emu)
{
    s_cpu *cpu = &emu->cpu;
    s_opt *opt = &emu->opt;
    
    cpu->inst_counter++;
    
    if(!opt->gb_doctor)
        return;
    
    uint8_t pc0, pc1, pc2, pc3;
    read_memory(emu, cpu->pc, &pc0);
    read_memory(emu, cpu->pc + 1, &pc1);
    read_memory(emu, cpu->pc + 2, &pc2);
    read_memory(emu, cpu->pc + 3, &pc3);
    if(0 > fprintf(
        opt->gbdoc_log, 
        "A:%02X F:%02X B:%02X C:%02X D:%02X E:%02X H:%02X L:%02X "
        "SP:%04X PC:%04X PCMEM:%02X,%02X,%02X,%02X\n",
        cpu->regA, cpu->regF, cpu->regB, cpu->regC, cpu->regD, cpu->regE,
        cpu->regH, cpu->regL, cpu->sp, cpu->pc, pc0, pc1, pc2, pc3
    ))
    {
        perror("gbdoctor fprintf: ");
        destroy_emulator(emu, EXIT_FAILURE);
    }
}
