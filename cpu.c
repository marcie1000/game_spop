#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cpu.h"
#include "emulator.h"
#include "opcodes.h"

void initialize_cpu(s_cpu *cpu)
{
    memset(cpu, 0, sizeof(s_cpu));
    cpu->pc = START_ADRESS;
    initialize_length_table(&cpu->jump_table);
    load_boot_rom(cpu);
}

int load_boot_rom(s_cpu *cpu)
{
    FILE *bootrom = fopen("boot_rom/dmg_rom.bin", "rb");
    if(NULL == bootrom)
    {
        perror("Error fopen: ");
        return EXIT_FAILURE;
    }
    
    fread(&cpu->ram[0], sizeof(cpu->ram[0]), 0x100, bootrom);
    fclose(bootrom);
    
    printf("Boot rom loaded.\n");
    
    return EXIT_SUCCESS;
}

uint32_t get_opcode(s_cpu *cpu)
{
    uint32_t op = 0;
    op = (cpu->ram[cpu->pc] << 16);
    if(cpu->pc < RAM_SIZE)
        op += (cpu->ram[cpu->pc + 1] << 8);
    if(cpu->pc < RAM_SIZE - 1)
        op += (cpu->ram[cpu->pc + 2]);
    return op;
}

uint8_t get_action(uint32_t opcode)
{
    return (opcode & 0xFF000000) >> 24;
}

void interpret(s_emu *emu, void (*opcode_functions[OPCODE_NB])(s_emu *, uint32_t))
{
    uint32_t opcode = get_opcode(&emu->cpu);
    uint8_t action = get_action(opcode);
    (*opcode_functions[action])(emu, opcode);
    emu->cpu.pc += emu->cpu.jump_table.length[action];
}

void initialize_length_table(s_jump *table)
{
    memset(&table->length, 1U, OPCODE_NB);
    
    table->length[0x1] = 3;
    table->length[0x6] = 2;
    table->length[0x8] = 3;
    table->length[0xE] = 2;
    table->length[0x11] = 3;
    table->length[0x15] = 1;
    table->length[0x16] = 2;
    table->length[0x18] = 2;
    table->length[0x1E] = 2;
    table->length[0x20] = 2;
    table->length[0x21] = 3;
    table->length[0x26] = 2;
    table->length[0x28] = 2;
    table->length[0x2E] = 2;
    table->length[0x30] = 2;
    table->length[0x31] = 3;
    table->length[0x36] = 2;
    table->length[0x38] = 2;
    table->length[0x3e] = 2;
    table->length[0xc2] = 3;
    table->length[0xc3] = 3;
    table->length[0xc4] = 3;
    table->length[0xc6] = 2;
    table->length[0xca] = 3;
    table->length[0xcc] = 3;
    table->length[0xcd] = 3;
    table->length[0xce] = 2;
    table->length[0xd2] = 3;
    table->length[0xd3] = 0;
    table->length[0xd4] = 3;
    table->length[0xd6] = 2;
    table->length[0xda] = 3;
    table->length[0xdb] = 0;
    table->length[0xdc] = 3;
    table->length[0xdd] = 0;
    table->length[0xde] = 2;
    table->length[0xe0] = 2;
    table->length[0xe3] = 0;
    table->length[0xe4] = 0;
    table->length[0xe6] = 2;
    table->length[0xe8] = 2;
    table->length[0xea] = 3;
    table->length[0xeb] = 0;
    table->length[0xec] = 0;
    table->length[0xed] = 0;
    table->length[0xee] = 2;
    table->length[0xf0] = 2;
    table->length[0xf4] = 0;
    table->length[0xf6] = 2;
    table->length[0xf8] = 2;
    table->length[0xfa] = 3;
    table->length[0xfc] = 0;
    table->length[0xfd] = 0;
    table->length[0xfe] = 2;
}

void init_opcodes_pointers(void (*opcode_functions[OPCODE_NB])(s_emu *, uint32_t))
{
    for(size_t i = 0; i < OPCODE_NB; i++)
    {
        opcode_functions[i] = &opcode_unimplemented;
    }
}

//void initialize_jump_table(s_jump *table)
//{
//    table->mask[0x0] = 0xFF; table->id[0x0] = 0x00;             //NOP - 0x00
//    table->mask[0x1] = 0xFF0000; table->id[0x1] = 0x010000;     //LD BC ,u16 - 0x01
//    table->mask[0x2] = 0xFF; table->id[0x2] = 0x02;             //LD (BC), A - 0x02
//    table->mask[0x3] = 0xFF; table->id[0x3] = 0x03;             //INC BC - 0x03
//    table->mask[0x4] = 0xFF; table->id[0x4] = 0x04;             //INC B - 0x04
//    table->mask[0x5] = 0xFF; table->id[0x5] = 0x05;             //DEC B - 0x05
//    table->mask[0x6] = 0xFF00; table->id[0x6] = 0x0600;         //LD B,u8 - 0x06
//    table->mask[0x7] = 0xFF; table->id[0x7] = 0x07;             //RLCA - 0x07
//    table->mask[0x8] = 0xFF0000; table->id[0x8] = 0x080000;     //LD (u16),SP - 0x08
//    table->mask[0x9] = 0xFF; table->id[0x9] = 0x09;             //ADD HL,BC - 0x09
//    table->mask[0xA] = 0xFF; table->id[0xA] = 0x0A;             //LD A,(BC) - 0x0A
//    table->mask[0xB] = 0xFF; table->id[0xB] = 0x0B;             //DEC BC - 0x0B
//    table->mask[0xC] = 0xFF; table->id[0xC] = 0x0C;             //INC C - 0x0C
//    table->mask[0xD] = 0xFF; table->id[0xD] = 0x0D;             //DEC C - 0x0D
//    table->mask[0xE] = 0xFF00; table->id[0xE] = 0x0E00;         //LD C,u8 - 0x0E
//    table->mask[0xF] = 0xFF; table->id[0xF] = 0x0F;             //RRCA - 0x0F
//    table->mask[0x10] = 0xFF; table->id[0x10] = 0x10;
//    table->mask[0x11] = 0xFF; table->id[0x11] = 0x11;
//    table->mask[0x12] = 0xFF; table->id[0x12] = 0x12;
//    table->mask[0x13] = 0xFF; table->id[0x13] = 0x13;
//    table->mask[0x14] = 0xFF; table->id[0x14] = 0x14;
//    table->mask[0x15] = 0xFF; table->id[0x15] = 0x15;
//    table->mask[0x16] = 0xFF; table->id[0x16] = 0x16;
//    table->mask[0x17] = 0xFF; table->id[0x17] = 0x17;
//    table->mask[0x18] = 0xFF; table->id[0x18] = 0x18;
//    table->mask[0x19] = 0xFF; table->id[0x19] = 0x19;
//    table->mask[0x1A] = 0xFF; table->id[0x1A] = 0x1A;
//    table->mask[0x1B] = 0xFF; table->id[0x1B] = 0x1B;
//}



