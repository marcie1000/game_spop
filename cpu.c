#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL.h>
#include "cpu.h"
#include "emulator.h"
#include "opcodes.h"
#include "prefixed_opcodes.h"

int write_io_registers(s_emu *emu, uint16_t adress, uint8_t data)
{
    s_cpu *cpu = &emu->cpu;
    
    switch(adress)
    {
        case 0xFF11:
            cpu->io_reg.NR11 = data;
            break;
        case 0xFF12:
            cpu->io_reg.NR12 = data;
            break;
        case 0xFF13:
            cpu->io_reg.NR13 = data;
            break;
        case 0xFF14:
            cpu->io_reg.NR14 = data & 0xC7;
            break;
        case 0xFF24:
            cpu->io_reg.NR50 = data;
            break;
        case 0xFF25:
            cpu->io_reg.NR51 = data;
            break;
        case 0xFF26:
            flag_assign(data, &cpu->io_reg.NR52, 0x80);
            break;
        case 0xFF40:
            cpu->io_reg.LCDC = data;
            break;
        case 0xFF42:
            cpu->io_reg.SCY = data;
            break;
        case 0xFF44:
            fprintf(stderr, "ERROR: attempt to write in I/O register LY (0xFF44), read only!\n");
            return EXIT_FAILURE;
            break;
        case 0xFF47:
            cpu->io_reg.BGP = data;
            break;
        case 0xFF50:
            cpu->io_reg.BANK = data;
            memcpy(cpu->ROM_Bank[0], cpu->ROM_Bank_0_tmp, sizeof(cpu->ROM_Bank[0]));
            break;
        default:
            fprintf(stderr, "WARNING: attempt to write I/O register at adress 0x%04X (unimplemented!)\n", adress);
            SDL_Delay(3000);
            return EXIT_FAILURE;
            break;
    }
    
    return EXIT_SUCCESS;
}

int read_io_registers(s_emu *emu, uint16_t adress, uint8_t *data)
{
    s_cpu *cpu = &emu->cpu;
    
    switch(adress)
    {
        case 0xFF11:
            *data = cpu->io_reg.NR11 & 0xC0;
            break;
        case 0xFF12:
            *data = cpu->io_reg.NR12;
            break;
        case 0xFF13:
            fprintf(stderr, "ERROR: attempt to read at adress FF13, NR13 I/O register (write only)\n");
            return EXIT_FAILURE;
            break;
        case 0xFF14:
            *data = 0x40 & cpu->io_reg.NR14;
            break;
        case 0xFF24:
            *data = cpu->io_reg.NR50;
            break;
        case 0xFF25:
            *data = cpu->io_reg.NR51;
            break;
        case 0xFF26:
            *data = cpu->io_reg.NR52;
            break;
        case 0xFF40:
            *data = cpu->io_reg.LCDC;
            break;
        case 0xFF42:
            *data = cpu->io_reg.SCY;
            break;
        case 0xFF44:
            *data = cpu->io_reg.LY;
            break;
        case 0xFF47:
            *data = cpu->io_reg.BGP;
            break;
        default:
            fprintf(stderr, "WARNING: attempt to read I/O register at adress 0x%04X (unimplemented!)\n", adress);
            SDL_Delay(3000);
            return EXIT_FAILURE;
            break;
    }
    
    return EXIT_SUCCESS;
}

int write_memory(s_emu *emu, uint16_t adress, uint8_t data)
{
    s_cpu *cpu = &emu->cpu;
    
    if(adress < 0x3FFF)
    {
        fprintf(stderr, "ERROR: attempt to write in 16 KiB ROM bank 00 at adress 0x%04X\n", adress);
        return EXIT_FAILURE;
    }
    else if(adress <= 0x7FFF)
    {
        fprintf(stderr, "ERROR: attempt to write in 16 KiB switchable ROM bank at adress 0x%04X\n", adress);
        return EXIT_FAILURE;
    }
    //VRAM
    else if(adress >= 0x8000 && adress <= 0x9FFF)
    {
//        if(adress <= 0x8fff && data != 0)
//        {
//            
//        }
        cpu->VRAM[adress - 0x8000] = data;
    }
    //8 KiB External RAM 
    else if(adress >= 0xA000 && adress <= 0xBFFF)
    {
        if(cpu->cartridge)
            cpu->external_RAM[adress - 0xA000] = data;
    }
    //WRAM
    else if(adress >= 0xC000 && adress <= 0xDFFF)
    {
        cpu->WRAM[adress - 0xC000] = data;
    }
    //ECHO RAM
    else if(adress >= 0xE000 && adress <= 0xFDFF)
    {
        fprintf(stderr, "ERROR: attempt to write in ECHO RAM at adress 0x%04X (prohibited)\n", adress);
        return EXIT_FAILURE;
    }
    //sprite attribute table (OAM)
    else if(adress >= 0xFE00 && adress <= 0xFE9F)
    {
        cpu->OAM[adress - 0xFE00] = data;
    }
    else if(adress >= 0xFEA0 && adress <= 0xFEFF)
    {
        fprintf(stderr, "ERROR: attempt to write at adress 0x%04X (prohibited)\n", adress);
        return EXIT_FAILURE;
    }
    else if(adress >= 0xFF00 && adress <= 0xFF7F)
    {
        if(0 != write_io_registers(emu, adress, data))
            return EXIT_FAILURE;
    }    
    //HRAM
    else if(adress >= 0xFF80 && adress <= 0xFFFE)
    {
        cpu->HRAM[adress - 0xFF80] = data;
    }
    else if(adress == 0xFFFF)
    {
        fprintf(stderr, "WARNING: attempt to write in Interrupt Enable register (IE) at adress 0x%04X (unimplemented)\n", adress);
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}

int read_memory(s_emu *emu, uint16_t adress, uint8_t *data)
{
    s_cpu *cpu = &emu->cpu;
    
    if(adress <= 0x3FFF)
    {
        *data = cpu->ROM_Bank[0][adress];
    }
    else if(adress >= 0x4000 && adress <= 0x7FFF)
    {
        *data = cpu->ROM_Bank[1][adress - 0x4000];
    }
    //VRAM
    else if(adress >= 0x8000 && adress <= 0x9FFF)
    {
        *data = cpu->VRAM[adress - 0x8000];
    }
    //8 KiB External RAM 
    else if(adress >= 0xA000 && adress <= 0xBFFF)
    {
        *data = cpu->external_RAM[adress - 0xA000];
    }
    //WRAM
    else if(adress >= 0xC000 && adress <= 0xDFFF)
    {
        *data = cpu->WRAM[adress - 0xC000];
    }
    //ECHO RAM
    else if(adress >= 0xE000 && adress <= 0xFDFF)
    {
        fprintf(stderr, "ERROR: attempt to read in ECHO RAM at adress 0x%04X (prohibited)\n", adress);
        return EXIT_FAILURE;
    }
    //sprite attribute table (OAM)
    else if(adress >= 0xFE00 && adress <= 0xFE9F)
    {
        *data = cpu->OAM[adress - 0xFE00];
    }
    else if(adress >= 0xFEA0 && adress <= 0xFEFF)
    {
        fprintf(stderr, "ERROR: attempt to read at adress 0x%04X (prohibited)\n", adress);
        return EXIT_FAILURE;
    }
    else if(adress >= 0xFF00 && adress <= 0xFF7F)
    {
        if(0 != read_io_registers(emu, adress, data))
            return EXIT_FAILURE;
    }    
    //HRAM
    else if(adress >= 0xFF80 && adress <= 0xFFFE)
    {
        *data = cpu->HRAM[adress - 0xFF80];
    }
    else if(adress == 0xFFFF)
    {
        fprintf(stderr, "WARNING: attempt to read in Interrupt Enable register (IE) at adress 0x%04X (unimplemented)\n", adress);
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}

int initialize_cpu(s_cpu *cpu)
{
    memset(cpu, 0, sizeof(s_cpu));
    cpu->pc = START_ADRESS;
    return EXIT_SUCCESS;
}

uint32_t get_opcode(s_emu *emu)
{
    s_cpu *cpu = &emu->cpu;
    uint32_t op;
    uint8_t tmp;
    
    if(0 != read_memory(emu, cpu->pc, &tmp))
        return 0;
    op = tmp << 16;
    if(cpu->pc < MEM_SIZE)
    {
        if(0 != read_memory(emu, cpu->pc + 1, &tmp))
            return 0;
        op += tmp << 8;
    }
    if(cpu->pc < MEM_SIZE - 1)
    {
        if(0 != read_memory(emu, cpu->pc + 2, &tmp))
            return 0;
        op += tmp;
    }
    return op;
}

uint8_t get_action(uint32_t opcode)
{
    return (opcode & 0xFF0000) >> 16;
}

uint8_t get_cb_opcode(uint32_t op32)
{
    return (op32 & 0x0000FF00) >> 8;
}

void interpret(s_emu *emu, void (*opcode_functions[OPCODE_NB])(void *, uint32_t))
{
    s_cpu *cpu = &emu->cpu;
    uint32_t opcode = get_opcode(emu);
    uint8_t action = get_action(opcode);
//    if(action == 0xCB)
//        printf("Opcode 0x%06X      mnemonic %-10s %s    pc = 0x%04X, sp = 0x%02X\n", 
//               opcode, emu->mnemonic_index[action], emu->prefixed_mnemonic_index[(opcode & 0x0000FF00) >> 8], cpu->pc, cpu->sp);
//    else
//        printf("Opcode 0x%06X      mnemonic %-15s      pc = 0x%04X, sp = 0x%02X\n", 
//               opcode, emu->mnemonic_index[action], cpu->pc, cpu->sp);
    (*opcode_functions[action])(emu, opcode);
//    printf("A=0x%02X, B=0x%02X, C=0x%02X, D=0x%02X, E=0x%02X, F=0x%02X, H=0x%02X, L=0x%02X\n",
//           cpu->regA, cpu->regB, cpu->regC, cpu->regD, cpu->regE, cpu->regF, cpu->regH, cpu->regL);
//    printf("Flags: zero = %u, neg = %u, half-carry = %u, carry = %u\n\n",
//           (cpu->regF & 0x80) >> 7, (cpu->regF & 0x40) >> 6, (cpu->regF & 0x20) >> 5, (cpu->regF & 0x10) >> 4);
////    printf("LY = %u\n\n", cpu->io_reg.LY);
//    if(cpu->pc == 0x00e0)
//    {
//        printf("");
//    }
    cpu->pc += emu->length_table[action];
}

void initialize_length_table(s_emu *emu)
{
    memset(&emu->length_table, 1U, OPCODE_NB);
    
    emu->length_table[0x01] = 3;
    emu->length_table[0x06] = 2;
    emu->length_table[0x08] = 3;
    emu->length_table[0x0E] = 2;
    emu->length_table[0x11] = 3;
    emu->length_table[0x15] = 1;
    emu->length_table[0x16] = 2;
    emu->length_table[0x18] = 2;
    emu->length_table[0x1E] = 2;
    emu->length_table[0x20] = 2;
    emu->length_table[0x21] = 3;
    emu->length_table[0x26] = 2;
    emu->length_table[0x28] = 2;
    emu->length_table[0x2E] = 2;
    emu->length_table[0x30] = 2;
    emu->length_table[0x31] = 3;
    emu->length_table[0x36] = 2;
    emu->length_table[0x38] = 2;
    emu->length_table[0x3e] = 2;
    emu->length_table[0xc2] = 3;
    emu->length_table[0xc3] = 3;
    emu->length_table[0xc4] = 3;
    emu->length_table[0xc6] = 2;
    emu->length_table[0xca] = 3;
    emu->length_table[0xcb] = 2;
    emu->length_table[0xcc] = 3;
    emu->length_table[0xcd] = 3;
    emu->length_table[0xce] = 2;
    emu->length_table[0xd2] = 3;
    emu->length_table[0xd3] = 0;
    emu->length_table[0xd4] = 3;
    emu->length_table[0xd6] = 2;
    emu->length_table[0xda] = 3;
    emu->length_table[0xdb] = 0;
    emu->length_table[0xdc] = 3;
    emu->length_table[0xdd] = 0;
    emu->length_table[0xde] = 2;
    emu->length_table[0xe0] = 2;
    emu->length_table[0xe3] = 0;
    emu->length_table[0xe4] = 0;
    emu->length_table[0xe6] = 2;
    emu->length_table[0xe8] = 2;
    emu->length_table[0xea] = 3;
    emu->length_table[0xeb] = 0;
    emu->length_table[0xec] = 0;
    emu->length_table[0xed] = 0;
    emu->length_table[0xee] = 2;
    emu->length_table[0xf0] = 2;
    emu->length_table[0xf4] = 0;
    emu->length_table[0xf6] = 2;
    emu->length_table[0xf8] = 2;
    emu->length_table[0xfa] = 3;
    emu->length_table[0xfc] = 0;
    emu->length_table[0xfd] = 0;
    emu->length_table[0xfe] = 2;
}

void init_opcodes_pointers(void (*opcode_functions[OPCODE_NB])(void *, uint32_t))
{
    for(size_t i = 0; i < OPCODE_NB; i++)
    {
        opcode_functions[i] = &opcode_unimplemented;
    }
    
    opcode_functions[0x00] = &NOP;
    //opcode_functions[0x01] = &LD_BC_d16;
    //opcode_functions[0x02] = &LD_derefBC_A;
    opcode_functions[0x03] = &INC_BC;
    opcode_functions[0x04] = &INC_B;
    opcode_functions[0x05] = &DEC_B;
    opcode_functions[0x06] = &LD_B_d8;
    opcode_functions[0x07] = &RLCA;
    //opcode_functions[0x08] = &LD_derefa16_SP;
    //opcode_functions[0x09] = &ADD_HL_BC;
    //opcode_functions[0x0A] = &LD_A_derefBC;
    opcode_functions[0x0B] = &DEC_BC;
    opcode_functions[0x0C] = &INC_C;
    opcode_functions[0x0D] = &DEC_C;
    opcode_functions[0x0E] = &LD_C_d8;
    //opcode_functions[0x0F] = &RRCA;
    //opcode_functions[0x10] = &STOP_0;
    opcode_functions[0x11] = &LD_DE_d16;
    //opcode_functions[0x12] = &LD_derefDE_A;
    opcode_functions[0x13] = &INC_DE;
    //opcode_functions[0x14] = &INC_D;
    opcode_functions[0x15] = &DEC_D;
    opcode_functions[0x16] = &LD_D_d8;
    opcode_functions[0x17] = &RLA;
    opcode_functions[0x18] = &JR_r8;
    //opcode_functions[0x19] = &ADD_HL_DE;
    opcode_functions[0x1A] = &LD_A_derefDE;
    //opcode_functions[0x1B] = &DEC_DE;
    //opcode_functions[0x1C] = &INC_E;
    opcode_functions[0x1D] = &DEC_E;
    opcode_functions[0x1E] = &LD_E_d8;
    //opcode_functions[0x1F] = &RRA;
    opcode_functions[0x20] = &JR_NZ_r8;
    opcode_functions[0x21] = &LD_HL_d16;
    opcode_functions[0x22] = &LD_derefHLplus_A;
    opcode_functions[0x23] = &INC_HL;
    opcode_functions[0x24] = &INC_H;
    //opcode_functions[0x25] = &DEC_H;
    //opcode_functions[0x26] = &LD_H_d8;
    //opcode_functions[0x27] = &DAA;
    opcode_functions[0x28] = &JR_Z_r8;
    //opcode_functions[0x29] = &ADD_HL_HL;
    //opcode_functions[0x2A] = &LD_A_derefHLplus;
    opcode_functions[0x2B] = &DEC_HL;
    //opcode_functions[0x2C] = &INC_L;
    //opcode_functions[0x2D] = &DEC_L;
    opcode_functions[0x2E] = &LD_L_d8;
    //opcode_functions[0x2F] = &CPL;
    //opcode_functions[0x30] = &JR_NC_r8;
    opcode_functions[0x31] = &LD_SP_d16;
    opcode_functions[0x32] = &LD_derefHLminus_A;
    //opcode_functions[0x33] = &INC_SP;
    opcode_functions[0x34] = &INC_derefHL;
    //opcode_functions[0x35] = &DEC_derefHL;
    //opcode_functions[0x36] = &LD_derefHL_d8;
    //opcode_functions[0x37] = &SCF;
    //opcode_functions[0x38] = &JR_C_r8;
    //opcode_functions[0x39] = &ADD_HL_SP;
    //opcode_functions[0x3A] = &LD_A_derefHLminus;
    //opcode_functions[0x3B] = &DEC_SP;
    //opcode_functions[0x3C] = &INC_A;
    opcode_functions[0x3D] = &DEC_A;
    opcode_functions[0x3E] = &LD_A_d8;
    //opcode_functions[0x3F] = &CCF;
    //opcode_functions[0x40] = &LD_B_B;
    //opcode_functions[0x41] = &LD_B_C;
    //opcode_functions[0x42] = &LD_B_D;
    //opcode_functions[0x43] = &LD_B_E;
    //opcode_functions[0x44] = &LD_B_H;
    //opcode_functions[0x45] = &LD_B_L;
    //opcode_functions[0x46] = &LD_B_derefHL;
    //opcode_functions[0x47] = &LD_B_A;
    //opcode_functions[0x48] = &LD_C_B;
    //opcode_functions[0x49] = &LD_C_C;
    //opcode_functions[0x4A] = &LD_C_D;
    //opcode_functions[0x4B] = &LD_C_E;
    //opcode_functions[0x4C] = &LD_C_H;
    //opcode_functions[0x4D] = &LD_C_L;
    //opcode_functions[0x4E] = &LD_C_derefHL;
    opcode_functions[0x4F] = &LD_C_A;
    //opcode_functions[0x50] = &LD_D_B;
    //opcode_functions[0x51] = &LD_D_C;
    //opcode_functions[0x52] = &LD_D_D;
    //opcode_functions[0x53] = &LD_D_E;
    //opcode_functions[0x54] = &LD_D_H;
    //opcode_functions[0x55] = &LD_D_L;
    //opcode_functions[0x56] = &LD_D_derefHL;
    opcode_functions[0x57] = &LD_D_A;
    //opcode_functions[0x58] = &LD_E_B;
    //opcode_functions[0x59] = &LD_E_C;
    //opcode_functions[0x5A] = &LD_E_D;
    //opcode_functions[0x5B] = &LD_E_E;
    //opcode_functions[0x5C] = &LD_E_H;
    //opcode_functions[0x5D] = &LD_E_L;
    //opcode_functions[0x5E] = &LD_E_derefHL;
    //opcode_functions[0x5F] = &LD_E_A;
    //opcode_functions[0x60] = &LD_H_B;
    //opcode_functions[0x61] = &LD_H_C;
    //opcode_functions[0x62] = &LD_H_D;
    //opcode_functions[0x63] = &LD_H_E;
    //opcode_functions[0x64] = &LD_H_H;
    //opcode_functions[0x65] = &LD_H_L;
    //opcode_functions[0x66] = &LD_H_derefHL;
    opcode_functions[0x67] = &LD_H_A;
    //opcode_functions[0x68] = &LD_L_B;
    //opcode_functions[0x69] = &LD_L_C;
    //opcode_functions[0x6A] = &LD_L_D;
    //opcode_functions[0x6B] = &LD_L_E;
    //opcode_functions[0x6C] = &LD_L_H;
    //opcode_functions[0x6D] = &LD_L_L;
    //opcode_functions[0x6E] = &LD_L_derefHL;
    //opcode_functions[0x6F] = &LD_L_A;
    //opcode_functions[0x70] = &LD_derefHL_B;
    //opcode_functions[0x71] = &LD_derefHL_C;
    //opcode_functions[0x72] = &LD_derefHL_D;
    //opcode_functions[0x73] = &LD_derefHL_E;
    //opcode_functions[0x74] = &LD_derefHL_H;
    //opcode_functions[0x75] = &LD_derefHL_L;
    //opcode_functions[0x76] = &HALT;
    opcode_functions[0x77] = &LD_derefHL_A;
    opcode_functions[0x78] = &LD_A_B;
    //opcode_functions[0x79] = &LD_A_C;
    //opcode_functions[0x7A] = &LD_A_D;
    opcode_functions[0x7B] = &LD_A_E;
    opcode_functions[0x7C] = &LD_A_H;
    opcode_functions[0x7D] = &LD_A_L;
    //opcode_functions[0x7E] = &LD_A_derefHL;
    //opcode_functions[0x7F] = &LD_A_A;
    opcode_functions[0x80] = &ADD_A_B;
    //opcode_functions[0x81] = &ADD_A_C;
    //opcode_functions[0x82] = &ADD_A_D;
    //opcode_functions[0x83] = &ADD_A_E;
    //opcode_functions[0x84] = &ADD_A_H;
    //opcode_functions[0x85] = &ADD_A_L;
    opcode_functions[0x86] = &ADD_A_derefHL;
    //opcode_functions[0x87] = &ADD_A_A;
    //opcode_functions[0x88] = &ADC_A_B;
    //opcode_functions[0x89] = &ADC_A_C;
    //opcode_functions[0x8A] = &ADC_A_D;
    //opcode_functions[0x8B] = &ADC_A_E;
    //opcode_functions[0x8C] = &ADC_A_H;
    //opcode_functions[0x8D] = &ADC_A_L;
    //opcode_functions[0x8E] = &ADC_A_derefHL;
    //opcode_functions[0x8F] = &ADC_A_A;
    opcode_functions[0x90] = &SUB_B;
    //opcode_functions[0x91] = &SUB_C;
    //opcode_functions[0x92] = &SUB_D;
    //opcode_functions[0x93] = &SUB_E;
    //opcode_functions[0x94] = &SUB_H;
    //opcode_functions[0x95] = &SUB_L;
    //opcode_functions[0x96] = &SUB_derefHL;
    //opcode_functions[0x97] = &SUB_A;
    //opcode_functions[0x98] = &SBC_A_B;
    //opcode_functions[0x99] = &SBC_A_C;
    //opcode_functions[0x9A] = &SBC_A_D;
    //opcode_functions[0x9B] = &SBC_A_E;
    //opcode_functions[0x9C] = &SBC_A_H;
    //opcode_functions[0x9D] = &SBC_A_L;
    //opcode_functions[0x9E] = &SBC_A_derefHL;
    //opcode_functions[0x9F] = &SBC_A_A;
    //opcode_functions[0xA0] = &AND_B;
    //opcode_functions[0xA1] = &AND_C;
    //opcode_functions[0xA2] = &AND_D;
    //opcode_functions[0xA3] = &AND_E;
    //opcode_functions[0xA4] = &AND_H;
    //opcode_functions[0xA5] = &AND_L;
    //opcode_functions[0xA6] = &AND_derefHL;
    //opcode_functions[0xA7] = &AND_A;
    //opcode_functions[0xA8] = &XOR_B;
    //opcode_functions[0xA9] = &XOR_C;
    //opcode_functions[0xAA] = &XOR_D;
    //opcode_functions[0xAB] = &XOR_E;
    //opcode_functions[0xAC] = &XOR_H;
    //opcode_functions[0xAD] = &XOR_L;
    //opcode_functions[0xAE] = &XOR_derefHL;
    opcode_functions[0xAF] = &XOR_A;
    //opcode_functions[0xB0] = &OR_B;
    //opcode_functions[0xB1] = &OR_C;
    //opcode_functions[0xB2] = &OR_D;
    //opcode_functions[0xB3] = &OR_E;
    //opcode_functions[0xB4] = &OR_H;
    //opcode_functions[0xB5] = &OR_L;
    //opcode_functions[0xB6] = &OR_derefHL;
    //opcode_functions[0xB7] = &OR_A;
    //opcode_functions[0xB8] = &CP_B;
    //opcode_functions[0xB9] = &CP_C;
    //opcode_functions[0xBA] = &CP_D;
    //opcode_functions[0xBB] = &CP_E;
    //opcode_functions[0xBC] = &CP_H;
    //opcode_functions[0xBD] = &CP_L;
    opcode_functions[0xBE] = &CP_derefHL;
    //opcode_functions[0xBF] = &CP_A;
    //opcode_functions[0xC0] = &RET_NZ;
    opcode_functions[0xC1] = &POP_BC;
    //opcode_functions[0xC2] = &JP_NZ_a16;
    opcode_functions[0xC3] = &JP_a16;
    //opcode_functions[0xC4] = &CALL_NZ_a16;
    opcode_functions[0xC5] = &PUSH_BC;
    //opcode_functions[0xC6] = &ADD_A_d8;
    //opcode_functions[0xC7] = &RST_00H;
    //opcode_functions[0xC8] = &RET_Z;
    opcode_functions[0xC9] = &RET;
    //opcode_functions[0xCA] = &JP_Z_a16;
    opcode_functions[0xCB] = &PREFIX_CB;
    //opcode_functions[0xCC] = &CALL_Z_a16;
    opcode_functions[0xCD] = &CALL_a16;
    //opcode_functions[0xCE] = &ADC_A_d8;
    //opcode_functions[0xCF] = &RST_08H;
    //opcode_functions[0xD0] = &RET_NC;
    //opcode_functions[0xD1] = &POP_DE;
    //opcode_functions[0xD2] = &JP_NC_a16;
    opcode_functions[0xD3] = &opcode_non_existant;
    //opcode_functions[0xD4] = &CALL_NC_a16;
    //opcode_functions[0xD5] = &PUSH_DE;
    //opcode_functions[0xD6] = &SUB_d8;
    //opcode_functions[0xD7] = &RST_10H;
    //opcode_functions[0xD8] = &RET_C;
    //opcode_functions[0xD9] = &RETI;
    //opcode_functions[0xDA] = &JP_C_a16;
    opcode_functions[0xDB] = &opcode_non_existant;
    //opcode_functions[0xDC] = &CALL_C_a16;
    opcode_functions[0xDD] = &opcode_non_existant;
    //opcode_functions[0xDE] = &SBC_A_d8;
    //opcode_functions[0xDF] = &RST_18H;
    opcode_functions[0xE0] = &LDH_derefa8_A;
    //opcode_functions[0xE1] = &POP_HL;
    opcode_functions[0xE2] = &LD_derefC_A;
    opcode_functions[0xE3] = &opcode_non_existant;
    opcode_functions[0xE4] = &opcode_non_existant;
    //opcode_functions[0xE5] = &PUSH_HL;
    //opcode_functions[0xE6] = &AND_d8;
    //opcode_functions[0xE7] = &RST_20H;
    //opcode_functions[0xE8] = &ADD_SP_r8;
    //opcode_functions[0xE9] = &JP_derefHL;
    opcode_functions[0xEA] = &LD_derefa16_A;
    opcode_functions[0xEB] = &opcode_non_existant;
    opcode_functions[0xEC] = &opcode_non_existant;
    opcode_functions[0xED] = &opcode_non_existant;
    //opcode_functions[0xEE] = &XOR_d8;
    //opcode_functions[0xEF] = &RST_28H;
    opcode_functions[0xF0] = &LDH_A_derefa8;
    //opcode_functions[0xF1] = &POP_AF;
    //opcode_functions[0xF2] = &LD_A_derefC;
    opcode_functions[0xF3] = &DI;
    opcode_functions[0xF4] = &opcode_non_existant;
    //opcode_functions[0xF5] = &PUSH_AF;
    //opcode_functions[0xF6] = &OR_d8;
    //opcode_functions[0xF7] = &RST_30H;
    //opcode_functions[0xF8] = &LD_HL_SPplusr8;
    //opcode_functions[0xF9] = &LD_SP_HL;
    //opcode_functions[0xFA] = &LD_A_derefa16;
    //opcode_functions[0xFB] = &EI;
    opcode_functions[0xFC] = &opcode_non_existant;
    opcode_functions[0xFD] = &opcode_non_existant;
    opcode_functions[0xFE] = &CP_d8;
    //opcode_functions[0xFF] = &RST_38H;


}

void init_cb_pointers(void (*cb_functions[CB_NB]) (void*, uint8_t))
{
    for(size_t i = 0; i < CB_NB; i++)
    {
        cb_functions[i] = &cb_opcode_unimplemented;
    }
    
    //cb_functions[0x00] = &prefixed_RLC_B;
    //cb_functions[0x01] = &prefixed_RLC_C;
    //cb_functions[0x02] = &prefixed_RLC_D;
    //cb_functions[0x03] = &prefixed_RLC_E;
    //cb_functions[0x04] = &prefixed_RLC_H;
    //cb_functions[0x05] = &prefixed_RLC_L;
    //cb_functions[0x06] = &prefixed_RLC_derefHL;
    //cb_functions[0x07] = &prefixed_RLC_A;
    //cb_functions[0x08] = &prefixed_RRC_B;
    //cb_functions[0x09] = &prefixed_RRC_C;
    //cb_functions[0x0A] = &prefixed_RRC_D;
    //cb_functions[0x0B] = &prefixed_RRC_E;
    //cb_functions[0x0C] = &prefixed_RRC_H;
    //cb_functions[0x0D] = &prefixed_RRC_L;
    //cb_functions[0x0E] = &prefixed_RRC_derefHL;
    //cb_functions[0x0F] = &prefixed_RRC_A;
    //cb_functions[0x10] = &prefixed_RL_B;
    cb_functions[0x11] = &prefixed_RL_C;
    //cb_functions[0x12] = &prefixed_RL_D;
    //cb_functions[0x13] = &prefixed_RL_E;
    //cb_functions[0x14] = &prefixed_RL_H;
    //cb_functions[0x15] = &prefixed_RL_L;
    //cb_functions[0x16] = &prefixed_RL_derefHL;
    //cb_functions[0x17] = &prefixed_RL_A;
    //cb_functions[0x18] = &prefixed_RR_B;
    //cb_functions[0x19] = &prefixed_RR_C;
    //cb_functions[0x1A] = &prefixed_RR_D;
    //cb_functions[0x1B] = &prefixed_RR_E;
    //cb_functions[0x1C] = &prefixed_RR_H;
    //cb_functions[0x1D] = &prefixed_RR_L;
    //cb_functions[0x1E] = &prefixed_RR_derefHL;
    //cb_functions[0x1F] = &prefixed_RR_A;
    //cb_functions[0x20] = &prefixed_SLA_B;
    //cb_functions[0x21] = &prefixed_SLA_C;
    //cb_functions[0x22] = &prefixed_SLA_D;
    //cb_functions[0x23] = &prefixed_SLA_E;
    //cb_functions[0x24] = &prefixed_SLA_H;
    //cb_functions[0x25] = &prefixed_SLA_L;
    //cb_functions[0x26] = &prefixed_SLA_derefHL;
    //cb_functions[0x27] = &prefixed_SLA_A;
    //cb_functions[0x28] = &prefixed_SRA_B;
    //cb_functions[0x29] = &prefixed_SRA_C;
    //cb_functions[0x2A] = &prefixed_SRA_D;
    //cb_functions[0x2B] = &prefixed_SRA_E;
    //cb_functions[0x2C] = &prefixed_SRA_H;
    //cb_functions[0x2D] = &prefixed_SRA_L;
    //cb_functions[0x2E] = &prefixed_SRA_derefHL;
    //cb_functions[0x2F] = &prefixed_SRA_A;
    //cb_functions[0x30] = &prefixed_SWAP_B;
    //cb_functions[0x31] = &prefixed_SWAP_C;
    //cb_functions[0x32] = &prefixed_SWAP_D;
    //cb_functions[0x33] = &prefixed_SWAP_E;
    //cb_functions[0x34] = &prefixed_SWAP_H;
    //cb_functions[0x35] = &prefixed_SWAP_L;
    //cb_functions[0x36] = &prefixed_SWAP_derefHL;
    //cb_functions[0x37] = &prefixed_SWAP_A;
    //cb_functions[0x38] = &prefixed_SRL_B;
    //cb_functions[0x39] = &prefixed_SRL_C;
    //cb_functions[0x3A] = &prefixed_SRL_D;
    //cb_functions[0x3B] = &prefixed_SRL_E;
    //cb_functions[0x3C] = &prefixed_SRL_H;
    //cb_functions[0x3D] = &prefixed_SRL_L;
    //cb_functions[0x3E] = &prefixed_SRL_derefHL;
    //cb_functions[0x3F] = &prefixed_SRL_A;
    //cb_functions[0x40] = &prefixed_BIT_0_B;
    //cb_functions[0x41] = &prefixed_BIT_0_C;
    //cb_functions[0x42] = &prefixed_BIT_0_D;
    //cb_functions[0x43] = &prefixed_BIT_0_E;
    //cb_functions[0x44] = &prefixed_BIT_0_H;
    //cb_functions[0x45] = &prefixed_BIT_0_L;
    //cb_functions[0x46] = &prefixed_BIT_0_derefHL;
    //cb_functions[0x47] = &prefixed_BIT_0_A;
    //cb_functions[0x48] = &prefixed_BIT_1_B;
    //cb_functions[0x49] = &prefixed_BIT_1_C;
    //cb_functions[0x4A] = &prefixed_BIT_1_D;
    //cb_functions[0x4B] = &prefixed_BIT_1_E;
    //cb_functions[0x4C] = &prefixed_BIT_1_H;
    //cb_functions[0x4D] = &prefixed_BIT_1_L;
    //cb_functions[0x4E] = &prefixed_BIT_1_derefHL;
    //cb_functions[0x4F] = &prefixed_BIT_1_A;
    //cb_functions[0x50] = &prefixed_BIT_2_B;
    //cb_functions[0x51] = &prefixed_BIT_2_C;
    //cb_functions[0x52] = &prefixed_BIT_2_D;
    //cb_functions[0x53] = &prefixed_BIT_2_E;
    //cb_functions[0x54] = &prefixed_BIT_2_H;
    //cb_functions[0x55] = &prefixed_BIT_2_L;
    //cb_functions[0x56] = &prefixed_BIT_2_derefHL;
    //cb_functions[0x57] = &prefixed_BIT_2_A;
    //cb_functions[0x58] = &prefixed_BIT_3_B;
    //cb_functions[0x59] = &prefixed_BIT_3_C;
    //cb_functions[0x5A] = &prefixed_BIT_3_D;
    //cb_functions[0x5B] = &prefixed_BIT_3_E;
    //cb_functions[0x5C] = &prefixed_BIT_3_H;
    //cb_functions[0x5D] = &prefixed_BIT_3_L;
    //cb_functions[0x5E] = &prefixed_BIT_3_derefHL;
    //cb_functions[0x5F] = &prefixed_BIT_3_A;
    //cb_functions[0x60] = &prefixed_BIT_4_B;
    //cb_functions[0x61] = &prefixed_BIT_4_C;
    //cb_functions[0x62] = &prefixed_BIT_4_D;
    //cb_functions[0x63] = &prefixed_BIT_4_E;
    //cb_functions[0x64] = &prefixed_BIT_4_H;
    //cb_functions[0x65] = &prefixed_BIT_4_L;
    //cb_functions[0x66] = &prefixed_BIT_4_derefHL;
    //cb_functions[0x67] = &prefixed_BIT_4_A;
    //cb_functions[0x68] = &prefixed_BIT_5_B;
    //cb_functions[0x69] = &prefixed_BIT_5_C;
    //cb_functions[0x6A] = &prefixed_BIT_5_D;
    //cb_functions[0x6B] = &prefixed_BIT_5_E;
    //cb_functions[0x6C] = &prefixed_BIT_5_H;
    //cb_functions[0x6D] = &prefixed_BIT_5_L;
    //cb_functions[0x6E] = &prefixed_BIT_5_derefHL;
    //cb_functions[0x6F] = &prefixed_BIT_5_A;
    //cb_functions[0x70] = &prefixed_BIT_6_B;
    //cb_functions[0x71] = &prefixed_BIT_6_C;
    //cb_functions[0x72] = &prefixed_BIT_6_D;
    //cb_functions[0x73] = &prefixed_BIT_6_E;
    //cb_functions[0x74] = &prefixed_BIT_6_H;
    //cb_functions[0x75] = &prefixed_BIT_6_L;
    //cb_functions[0x76] = &prefixed_BIT_6_derefHL;
    //cb_functions[0x77] = &prefixed_BIT_6_A;
    //cb_functions[0x78] = &prefixed_BIT_7_B;
    //cb_functions[0x79] = &prefixed_BIT_7_C;
    //cb_functions[0x7A] = &prefixed_BIT_7_D;
    //cb_functions[0x7B] = &prefixed_BIT_7_E;
    cb_functions[0x7C] = &prefixed_BIT_7_H;
    //cb_functions[0x7D] = &prefixed_BIT_7_L;
    //cb_functions[0x7E] = &prefixed_BIT_7_derefHL;
    //cb_functions[0x7F] = &prefixed_BIT_7_A;
    //cb_functions[0x80] = &prefixed_RES_0_B;
    //cb_functions[0x81] = &prefixed_RES_0_C;
    //cb_functions[0x82] = &prefixed_RES_0_D;
    //cb_functions[0x83] = &prefixed_RES_0_E;
    //cb_functions[0x84] = &prefixed_RES_0_H;
    //cb_functions[0x85] = &prefixed_RES_0_L;
    //cb_functions[0x86] = &prefixed_RES_0_derefHL;
    //cb_functions[0x87] = &prefixed_RES_0_A;
    //cb_functions[0x88] = &prefixed_RES_1_B;
    //cb_functions[0x89] = &prefixed_RES_1_C;
    //cb_functions[0x8A] = &prefixed_RES_1_D;
    //cb_functions[0x8B] = &prefixed_RES_1_E;
    //cb_functions[0x8C] = &prefixed_RES_1_H;
    //cb_functions[0x8D] = &prefixed_RES_1_L;
    //cb_functions[0x8E] = &prefixed_RES_1_derefHL;
    //cb_functions[0x8F] = &prefixed_RES_1_A;
    //cb_functions[0x90] = &prefixed_RES_2_B;
    //cb_functions[0x91] = &prefixed_RES_2_C;
    //cb_functions[0x92] = &prefixed_RES_2_D;
    //cb_functions[0x93] = &prefixed_RES_2_E;
    //cb_functions[0x94] = &prefixed_RES_2_H;
    //cb_functions[0x95] = &prefixed_RES_2_L;
    //cb_functions[0x96] = &prefixed_RES_2_derefHL;
    //cb_functions[0x97] = &prefixed_RES_2_A;
    //cb_functions[0x98] = &prefixed_RES_3_B;
    //cb_functions[0x99] = &prefixed_RES_3_C;
    //cb_functions[0x9A] = &prefixed_RES_3_D;
    //cb_functions[0x9B] = &prefixed_RES_3_E;
    //cb_functions[0x9C] = &prefixed_RES_3_H;
    //cb_functions[0x9D] = &prefixed_RES_3_L;
    //cb_functions[0x9E] = &prefixed_RES_3_derefHL;
    //cb_functions[0x9F] = &prefixed_RES_3_A;
    //cb_functions[0xA0] = &prefixed_RES_4_B;
    //cb_functions[0xA1] = &prefixed_RES_4_C;
    //cb_functions[0xA2] = &prefixed_RES_4_D;
    //cb_functions[0xA3] = &prefixed_RES_4_E;
    //cb_functions[0xA4] = &prefixed_RES_4_H;
    //cb_functions[0xA5] = &prefixed_RES_4_L;
    //cb_functions[0xA6] = &prefixed_RES_4_derefHL;
    //cb_functions[0xA7] = &prefixed_RES_4_A;
    //cb_functions[0xA8] = &prefixed_RES_5_B;
    //cb_functions[0xA9] = &prefixed_RES_5_C;
    //cb_functions[0xAA] = &prefixed_RES_5_D;
    //cb_functions[0xAB] = &prefixed_RES_5_E;
    //cb_functions[0xAC] = &prefixed_RES_5_H;
    //cb_functions[0xAD] = &prefixed_RES_5_L;
    //cb_functions[0xAE] = &prefixed_RES_5_derefHL;
    //cb_functions[0xAF] = &prefixed_RES_5_A;
    //cb_functions[0xB0] = &prefixed_RES_6_B;
    //cb_functions[0xB1] = &prefixed_RES_6_C;
    //cb_functions[0xB2] = &prefixed_RES_6_D;
    //cb_functions[0xB3] = &prefixed_RES_6_E;
    //cb_functions[0xB4] = &prefixed_RES_6_H;
    //cb_functions[0xB5] = &prefixed_RES_6_L;
    //cb_functions[0xB6] = &prefixed_RES_6_derefHL;
    //cb_functions[0xB7] = &prefixed_RES_6_A;
    //cb_functions[0xB8] = &prefixed_RES_7_B;
    //cb_functions[0xB9] = &prefixed_RES_7_C;
    //cb_functions[0xBA] = &prefixed_RES_7_D;
    //cb_functions[0xBB] = &prefixed_RES_7_E;
    //cb_functions[0xBC] = &prefixed_RES_7_H;
    //cb_functions[0xBD] = &prefixed_RES_7_L;
    //cb_functions[0xBE] = &prefixed_RES_7_derefHL;
    //cb_functions[0xBF] = &prefixed_RES_7_A;
    //cb_functions[0xC0] = &prefixed_SET_0_B;
    //cb_functions[0xC1] = &prefixed_SET_0_C;
    //cb_functions[0xC2] = &prefixed_SET_0_D;
    //cb_functions[0xC3] = &prefixed_SET_0_E;
    //cb_functions[0xC4] = &prefixed_SET_0_H;
    //cb_functions[0xC5] = &prefixed_SET_0_L;
    //cb_functions[0xC6] = &prefixed_SET_0_derefHL;
    //cb_functions[0xC7] = &prefixed_SET_0_A;
    //cb_functions[0xC8] = &prefixed_SET_1_B;
    //cb_functions[0xC9] = &prefixed_SET_1_C;
    //cb_functions[0xCA] = &prefixed_SET_1_D;
    //cb_functions[0xCB] = &prefixed_SET_1_E;
    //cb_functions[0xCC] = &prefixed_SET_1_H;
    //cb_functions[0xCD] = &prefixed_SET_1_L;
    //cb_functions[0xCE] = &prefixed_SET_1_derefHL;
    //cb_functions[0xCF] = &prefixed_SET_1_A;
    //cb_functions[0xD0] = &prefixed_SET_2_B;
    //cb_functions[0xD1] = &prefixed_SET_2_C;
    //cb_functions[0xD2] = &prefixed_SET_2_D;
    //cb_functions[0xD3] = &prefixed_SET_2_E;
    //cb_functions[0xD4] = &prefixed_SET_2_H;
    //cb_functions[0xD5] = &prefixed_SET_2_L;
    //cb_functions[0xD6] = &prefixed_SET_2_derefHL;
    //cb_functions[0xD7] = &prefixed_SET_2_A;
    //cb_functions[0xD8] = &prefixed_SET_3_B;
    //cb_functions[0xD9] = &prefixed_SET_3_C;
    //cb_functions[0xDA] = &prefixed_SET_3_D;
    //cb_functions[0xDB] = &prefixed_SET_3_E;
    //cb_functions[0xDC] = &prefixed_SET_3_H;
    //cb_functions[0xDD] = &prefixed_SET_3_L;
    //cb_functions[0xDE] = &prefixed_SET_3_derefHL;
    //cb_functions[0xDF] = &prefixed_SET_3_A;
    //cb_functions[0xE0] = &prefixed_SET_4_B;
    //cb_functions[0xE1] = &prefixed_SET_4_C;
    //cb_functions[0xE2] = &prefixed_SET_4_D;
    //cb_functions[0xE3] = &prefixed_SET_4_E;
    //cb_functions[0xE4] = &prefixed_SET_4_H;
    //cb_functions[0xE5] = &prefixed_SET_4_L;
    //cb_functions[0xE6] = &prefixed_SET_4_derefHL;
    //cb_functions[0xE7] = &prefixed_SET_4_A;
    //cb_functions[0xE8] = &prefixed_SET_5_B;
    //cb_functions[0xE9] = &prefixed_SET_5_C;
    //cb_functions[0xEA] = &prefixed_SET_5_D;
    //cb_functions[0xEB] = &prefixed_SET_5_E;
    //cb_functions[0xEC] = &prefixed_SET_5_H;
    //cb_functions[0xED] = &prefixed_SET_5_L;
    //cb_functions[0xEE] = &prefixed_SET_5_derefHL;
    //cb_functions[0xEF] = &prefixed_SET_5_A;
    //cb_functions[0xF0] = &prefixed_SET_6_B;
    //cb_functions[0xF1] = &prefixed_SET_6_C;
    //cb_functions[0xF2] = &prefixed_SET_6_D;
    //cb_functions[0xF3] = &prefixed_SET_6_E;
    //cb_functions[0xF4] = &prefixed_SET_6_H;
    //cb_functions[0xF5] = &prefixed_SET_6_L;
    //cb_functions[0xF6] = &prefixed_SET_6_derefHL;
    //cb_functions[0xF7] = &prefixed_SET_6_A;
    //cb_functions[0xF8] = &prefixed_SET_7_B;
    //cb_functions[0xF9] = &prefixed_SET_7_C;
    //cb_functions[0xFA] = &prefixed_SET_7_D;
    //cb_functions[0xFB] = &prefixed_SET_7_E;
    //cb_functions[0xFC] = &prefixed_SET_7_H;
    //cb_functions[0xFD] = &prefixed_SET_7_L;
    //cb_functions[0xFE] = &prefixed_SET_7_derefHL;
    //cb_functions[0xFF] = &prefixed_SET_7_A;

}


