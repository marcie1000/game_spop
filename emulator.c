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
    init_mnemonic_index(emu);
    
    if(0 != load_boot_rom(&emu->cpu))
        return EXIT_FAILURE;
    
    return EXIT_SUCCESS;
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

