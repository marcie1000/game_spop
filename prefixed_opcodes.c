#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>

#include "cpu.h"
#include "emulator.h"
#include "opcodes.h"
#include "prefixed_opcodes.h"

void cb_opcode_unimplemented(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    fprintf(stderr, "WARNING: Prefixed instruction %s (0x%02X) unimplemented!\n",
            emu->prefixed_mnemonic_index[op], op);
    SDL_Delay(2000);
    destroy_emulator(emu, EXIT_FAILURE);
}

//void prefixed_RLC_B(void *arg, uint8_t op)
//void prefixed_RLC_C(void *arg, uint8_t op)
//void prefixed_RLC_D(void *arg, uint8_t op)
//void prefixed_RLC_E(void *arg, uint8_t op)
//void prefixed_RLC_H(void *arg, uint8_t op)
//void prefixed_RLC_L(void *arg, uint8_t op)
//void prefixed_RLC_derefHL(void *arg, uint8_t op)
//void prefixed_RLC_A(void *arg, uint8_t op)
//void prefixed_RRC_B(void *arg, uint8_t op)
//void prefixed_RRC_C(void *arg, uint8_t op)
//void prefixed_RRC_D(void *arg, uint8_t op)
//void prefixed_RRC_E(void *arg, uint8_t op)
//void prefixed_RRC_H(void *arg, uint8_t op)
//void prefixed_RRC_L(void *arg, uint8_t op)
//void prefixed_RRC_derefHL(void *arg, uint8_t op)
//void prefixed_RRC_A(void *arg, uint8_t op)
//void prefixed_RL_B(void *arg, uint8_t op)
void prefixed_RL_C(void *arg, UNUSED uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint8_t newC = cpu->regC;
    newC <<= 1;
    flag_assign(cpu->regF & CARRY_FMASK, &newC, 0x01);
    flag_assign(cpu->regC & 0x80, &cpu->regF, CARRY_FMASK);
    flag_assign(newC == 0, &cpu->regF, ZERO_FMASK);
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK + HALF_CARRY_FMASK);
    cpu->regC = newC;
    cpu->t_cycles += 8;
}
//void prefixed_RL_D(void *arg, uint8_t op)
//void prefixed_RL_E(void *arg, uint8_t op)
//void prefixed_RL_H(void *arg, uint8_t op)
//void prefixed_RL_L(void *arg, uint8_t op)
//void prefixed_RL_derefHL(void *arg, uint8_t op)
//void prefixed_RL_A(void *arg, uint8_t op)
//void prefixed_RR_B(void *arg, uint8_t op)
//void prefixed_RR_C(void *arg, uint8_t op)
//void prefixed_RR_D(void *arg, uint8_t op)
//void prefixed_RR_E(void *arg, uint8_t op)
//void prefixed_RR_H(void *arg, uint8_t op)
//void prefixed_RR_L(void *arg, uint8_t op)
//void prefixed_RR_derefHL(void *arg, uint8_t op)
//void prefixed_RR_A(void *arg, uint8_t op)
//void prefixed_SLA_B(void *arg, uint8_t op)
//void prefixed_SLA_C(void *arg, uint8_t op)
//void prefixed_SLA_D(void *arg, uint8_t op)
//void prefixed_SLA_E(void *arg, uint8_t op)
//void prefixed_SLA_H(void *arg, uint8_t op)
//void prefixed_SLA_L(void *arg, uint8_t op)
//void prefixed_SLA_derefHL(void *arg, uint8_t op)
//void prefixed_SLA_A(void *arg, uint8_t op)
//void prefixed_SRA_B(void *arg, uint8_t op)
//void prefixed_SRA_C(void *arg, uint8_t op)
//void prefixed_SRA_D(void *arg, uint8_t op)
//void prefixed_SRA_E(void *arg, uint8_t op)
//void prefixed_SRA_H(void *arg, uint8_t op)
//void prefixed_SRA_L(void *arg, uint8_t op)
//void prefixed_SRA_derefHL(void *arg, uint8_t op)
//void prefixed_SRA_A(void *arg, uint8_t op)
//void prefixed_SWAP_B(void *arg, uint8_t op)
//void prefixed_SWAP_C(void *arg, uint8_t op)
//void prefixed_SWAP_D(void *arg, uint8_t op)
//void prefixed_SWAP_E(void *arg, uint8_t op)
//void prefixed_SWAP_H(void *arg, uint8_t op)
//void prefixed_SWAP_L(void *arg, uint8_t op)
//void prefixed_SWAP_derefHL(void *arg, uint8_t op)
void prefixed_SWAP_A(void *arg, UNUSED uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regA = ((cpu->regA & 0xF0) >> 4) | ((cpu->regA & 0x0F) << 4);
    
    flag_assign(cpu->regA == 0, &cpu->regF, ZERO_FMASK);
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK | HALF_CARRY_FMASK | CARRY_FMASK);
    
    cpu->t_cycles += 8;
}
//void prefixed_SRL_B(void *arg, uint8_t op)
//void prefixed_SRL_C(void *arg, uint8_t op)
//void prefixed_SRL_D(void *arg, uint8_t op)
//void prefixed_SRL_E(void *arg, uint8_t op)
//void prefixed_SRL_H(void *arg, uint8_t op)
//void prefixed_SRL_L(void *arg, uint8_t op)
//void prefixed_SRL_derefHL(void *arg, uint8_t op)
void prefixed_SRL_A(void *arg, UNUSED uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    flag_assign(cpu->regA & 0x01, &cpu->regF, CARRY_FMASK);
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK | HALF_CARRY_FMASK);
    cpu->regA >>= 1;
    flag_assign(cpu->regA == 0, &cpu->regF, ZERO_FMASK);
    
    cpu->t_cycles += 8;
}
//void prefixed_BIT_0_B(void *arg, uint8_t op)
//void prefixed_BIT_0_C(void *arg, uint8_t op)
//void prefixed_BIT_0_D(void *arg, uint8_t op)
//void prefixed_BIT_0_E(void *arg, uint8_t op)
//void prefixed_BIT_0_H(void *arg, uint8_t op)
//void prefixed_BIT_0_L(void *arg, uint8_t op)
//void prefixed_BIT_0_derefHL(void *arg, uint8_t op)
//void prefixed_BIT_0_A(void *arg, uint8_t op)
//void prefixed_BIT_1_B(void *arg, uint8_t op)
//void prefixed_BIT_1_C(void *arg, uint8_t op)
//void prefixed_BIT_1_D(void *arg, uint8_t op)
//void prefixed_BIT_1_E(void *arg, uint8_t op)
//void prefixed_BIT_1_H(void *arg, uint8_t op)
//void prefixed_BIT_1_L(void *arg, uint8_t op)
//void prefixed_BIT_1_derefHL(void *arg, uint8_t op)
//void prefixed_BIT_1_A(void *arg, uint8_t op)
//void prefixed_BIT_2_B(void *arg, uint8_t op)
//void prefixed_BIT_2_C(void *arg, uint8_t op)
//void prefixed_BIT_2_D(void *arg, uint8_t op)
//void prefixed_BIT_2_E(void *arg, uint8_t op)
//void prefixed_BIT_2_H(void *arg, uint8_t op)
//void prefixed_BIT_2_L(void *arg, uint8_t op)
//void prefixed_BIT_2_derefHL(void *arg, uint8_t op)
//void prefixed_BIT_2_A(void *arg, uint8_t op)
//void prefixed_BIT_3_B(void *arg, uint8_t op)
//void prefixed_BIT_3_C(void *arg, uint8_t op)
//void prefixed_BIT_3_D(void *arg, uint8_t op)
//void prefixed_BIT_3_E(void *arg, uint8_t op)
//void prefixed_BIT_3_H(void *arg, uint8_t op)
//void prefixed_BIT_3_L(void *arg, uint8_t op)
//void prefixed_BIT_3_derefHL(void *arg, uint8_t op)
//void prefixed_BIT_3_A(void *arg, uint8_t op)
//void prefixed_BIT_4_B(void *arg, uint8_t op)
//void prefixed_BIT_4_C(void *arg, uint8_t op)
//void prefixed_BIT_4_D(void *arg, uint8_t op)
//void prefixed_BIT_4_E(void *arg, uint8_t op)
//void prefixed_BIT_4_H(void *arg, uint8_t op)
//void prefixed_BIT_4_L(void *arg, uint8_t op)
//void prefixed_BIT_4_derefHL(void *arg, uint8_t op)
//void prefixed_BIT_4_A(void *arg, uint8_t op)
//void prefixed_BIT_5_B(void *arg, uint8_t op)
//void prefixed_BIT_5_C(void *arg, uint8_t op)
//void prefixed_BIT_5_D(void *arg, uint8_t op)
//void prefixed_BIT_5_E(void *arg, uint8_t op)
//void prefixed_BIT_5_H(void *arg, uint8_t op)
//void prefixed_BIT_5_L(void *arg, uint8_t op)
//void prefixed_BIT_5_derefHL(void *arg, uint8_t op)
//void prefixed_BIT_5_A(void *arg, uint8_t op)
//void prefixed_BIT_6_B(void *arg, uint8_t op)
//void prefixed_BIT_6_C(void *arg, uint8_t op)
//void prefixed_BIT_6_D(void *arg, uint8_t op)
//void prefixed_BIT_6_E(void *arg, uint8_t op)
//void prefixed_BIT_6_H(void *arg, uint8_t op)
//void prefixed_BIT_6_L(void *arg, uint8_t op)
//void prefixed_BIT_6_derefHL(void *arg, uint8_t op)
//void prefixed_BIT_6_A(void *arg, uint8_t op)
//void prefixed_BIT_7_B(void *arg, uint8_t op)
//void prefixed_BIT_7_C(void *arg, uint8_t op)
//void prefixed_BIT_7_D(void *arg, uint8_t op)
//void prefixed_BIT_7_E(void *arg, uint8_t op)

void prefixed_BIT_7_H(void *arg, UNUSED uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    flag_assign(!(cpu->regH & 0x80), &cpu->regF, ZERO_FMASK);
    
    //half carry
    flag_assign(true, &cpu->regF, HALF_CARRY_FMASK);
    
    //negative flag
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK);
    
    emu->cpu.t_cycles += 8;
}

//void prefixed_BIT_7_L(void *arg, uint8_t op)
//void prefixed_BIT_7_derefHL(void *arg, uint8_t op)
//void prefixed_BIT_7_A(void *arg, uint8_t op)
//void prefixed_RES_0_B(void *arg, uint8_t op)
//void prefixed_RES_0_C(void *arg, uint8_t op)
//void prefixed_RES_0_D(void *arg, uint8_t op)
//void prefixed_RES_0_E(void *arg, uint8_t op)
//void prefixed_RES_0_H(void *arg, uint8_t op)
//void prefixed_RES_0_L(void *arg, uint8_t op)
//void prefixed_RES_0_derefHL(void *arg, uint8_t op)
//void prefixed_RES_0_A(void *arg, uint8_t op)
//void prefixed_RES_1_B(void *arg, uint8_t op)
//void prefixed_RES_1_C(void *arg, uint8_t op)
//void prefixed_RES_1_D(void *arg, uint8_t op)
//void prefixed_RES_1_E(void *arg, uint8_t op)
//void prefixed_RES_1_H(void *arg, uint8_t op)
//void prefixed_RES_1_L(void *arg, uint8_t op)
//void prefixed_RES_1_derefHL(void *arg, uint8_t op)
//void prefixed_RES_1_A(void *arg, uint8_t op)
//void prefixed_RES_2_B(void *arg, uint8_t op)
//void prefixed_RES_2_C(void *arg, uint8_t op)
//void prefixed_RES_2_D(void *arg, uint8_t op)
//void prefixed_RES_2_E(void *arg, uint8_t op)
//void prefixed_RES_2_H(void *arg, uint8_t op)
//void prefixed_RES_2_L(void *arg, uint8_t op)
//void prefixed_RES_2_derefHL(void *arg, uint8_t op)
//void prefixed_RES_2_A(void *arg, uint8_t op)
//void prefixed_RES_3_B(void *arg, uint8_t op)
//void prefixed_RES_3_C(void *arg, uint8_t op)
//void prefixed_RES_3_D(void *arg, uint8_t op)
//void prefixed_RES_3_E(void *arg, uint8_t op)
//void prefixed_RES_3_H(void *arg, uint8_t op)
//void prefixed_RES_3_L(void *arg, uint8_t op)
//void prefixed_RES_3_derefHL(void *arg, uint8_t op)
//void prefixed_RES_3_A(void *arg, uint8_t op)
//void prefixed_RES_4_B(void *arg, uint8_t op)
//void prefixed_RES_4_C(void *arg, uint8_t op)
//void prefixed_RES_4_D(void *arg, uint8_t op)
//void prefixed_RES_4_E(void *arg, uint8_t op)
//void prefixed_RES_4_H(void *arg, uint8_t op)
//void prefixed_RES_4_L(void *arg, uint8_t op)
//void prefixed_RES_4_derefHL(void *arg, uint8_t op)
//void prefixed_RES_4_A(void *arg, uint8_t op)
//void prefixed_RES_5_B(void *arg, uint8_t op)
//void prefixed_RES_5_C(void *arg, uint8_t op)
//void prefixed_RES_5_D(void *arg, uint8_t op)
//void prefixed_RES_5_E(void *arg, uint8_t op)
//void prefixed_RES_5_H(void *arg, uint8_t op)
//void prefixed_RES_5_L(void *arg, uint8_t op)
//void prefixed_RES_5_derefHL(void *arg, uint8_t op)
//void prefixed_RES_5_A(void *arg, uint8_t op)
//void prefixed_RES_6_B(void *arg, uint8_t op)
//void prefixed_RES_6_C(void *arg, uint8_t op)
//void prefixed_RES_6_D(void *arg, uint8_t op)
//void prefixed_RES_6_E(void *arg, uint8_t op)
//void prefixed_RES_6_H(void *arg, uint8_t op)
//void prefixed_RES_6_L(void *arg, uint8_t op)
//void prefixed_RES_6_derefHL(void *arg, uint8_t op)
//void prefixed_RES_6_A(void *arg, uint8_t op)
//void prefixed_RES_7_B(void *arg, uint8_t op)
//void prefixed_RES_7_C(void *arg, uint8_t op)
//void prefixed_RES_7_D(void *arg, uint8_t op)
//void prefixed_RES_7_E(void *arg, uint8_t op)
//void prefixed_RES_7_H(void *arg, uint8_t op)
//void prefixed_RES_7_L(void *arg, uint8_t op)
//void prefixed_RES_7_derefHL(void *arg, uint8_t op)
//void prefixed_RES_7_A(void *arg, uint8_t op)
//void prefixed_SET_0_B(void *arg, uint8_t op)
//void prefixed_SET_0_C(void *arg, uint8_t op)
//void prefixed_SET_0_D(void *arg, uint8_t op)
//void prefixed_SET_0_E(void *arg, uint8_t op)
//void prefixed_SET_0_H(void *arg, uint8_t op)
//void prefixed_SET_0_L(void *arg, uint8_t op)
//void prefixed_SET_0_derefHL(void *arg, uint8_t op)
//void prefixed_SET_0_A(void *arg, uint8_t op)
//void prefixed_SET_1_B(void *arg, uint8_t op)
//void prefixed_SET_1_C(void *arg, uint8_t op)
//void prefixed_SET_1_D(void *arg, uint8_t op)
//void prefixed_SET_1_E(void *arg, uint8_t op)
//void prefixed_SET_1_H(void *arg, uint8_t op)
//void prefixed_SET_1_L(void *arg, uint8_t op)
//void prefixed_SET_1_derefHL(void *arg, uint8_t op)
//void prefixed_SET_1_A(void *arg, uint8_t op)
//void prefixed_SET_2_B(void *arg, uint8_t op)
//void prefixed_SET_2_C(void *arg, uint8_t op)
//void prefixed_SET_2_D(void *arg, uint8_t op)
//void prefixed_SET_2_E(void *arg, uint8_t op)
//void prefixed_SET_2_H(void *arg, uint8_t op)
//void prefixed_SET_2_L(void *arg, uint8_t op)
//void prefixed_SET_2_derefHL(void *arg, uint8_t op)
//void prefixed_SET_2_A(void *arg, uint8_t op)
//void prefixed_SET_3_B(void *arg, uint8_t op)
//void prefixed_SET_3_C(void *arg, uint8_t op)
//void prefixed_SET_3_D(void *arg, uint8_t op)
//void prefixed_SET_3_E(void *arg, uint8_t op)
//void prefixed_SET_3_H(void *arg, uint8_t op)
//void prefixed_SET_3_L(void *arg, uint8_t op)
//void prefixed_SET_3_derefHL(void *arg, uint8_t op)
//void prefixed_SET_3_A(void *arg, uint8_t op)
//void prefixed_SET_4_B(void *arg, uint8_t op)
//void prefixed_SET_4_C(void *arg, uint8_t op)
//void prefixed_SET_4_D(void *arg, uint8_t op)
//void prefixed_SET_4_E(void *arg, uint8_t op)
//void prefixed_SET_4_H(void *arg, uint8_t op)
//void prefixed_SET_4_L(void *arg, uint8_t op)
//void prefixed_SET_4_derefHL(void *arg, uint8_t op)
//void prefixed_SET_4_A(void *arg, uint8_t op)
//void prefixed_SET_5_B(void *arg, uint8_t op)
//void prefixed_SET_5_C(void *arg, uint8_t op)
//void prefixed_SET_5_D(void *arg, uint8_t op)
//void prefixed_SET_5_E(void *arg, uint8_t op)
//void prefixed_SET_5_H(void *arg, uint8_t op)
//void prefixed_SET_5_L(void *arg, uint8_t op)
//void prefixed_SET_5_derefHL(void *arg, uint8_t op)
//void prefixed_SET_5_A(void *arg, uint8_t op)
//void prefixed_SET_6_B(void *arg, uint8_t op)
//void prefixed_SET_6_C(void *arg, uint8_t op)
//void prefixed_SET_6_D(void *arg, uint8_t op)
//void prefixed_SET_6_E(void *arg, uint8_t op)
//void prefixed_SET_6_H(void *arg, uint8_t op)
//void prefixed_SET_6_L(void *arg, uint8_t op)
//void prefixed_SET_6_derefHL(void *arg, uint8_t op)
//void prefixed_SET_6_A(void *arg, uint8_t op)
//void prefixed_SET_7_B(void *arg, uint8_t op)
//void prefixed_SET_7_C(void *arg, uint8_t op)
//void prefixed_SET_7_D(void *arg, uint8_t op)
//void prefixed_SET_7_E(void *arg, uint8_t op)
//void prefixed_SET_7_H(void *arg, uint8_t op)
//void prefixed_SET_7_L(void *arg, uint8_t op)
//void prefixed_SET_7_derefHL(void *arg, uint8_t op)
//void prefixed_SET_7_A(void *arg, uint8_t op)

