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

void prefixed_RLC_B(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint8_t newB = cpu->regB;
    newB <<= 1;
    //copy previous bit 7 to new bit 0
    flag_assign((cpu->regB & 0x80), &newB, 0x01);
    //copy previous bit 7 to carry flag
    flag_assign((cpu->regB & 0x80), &cpu->regF, CARRY_FMASK);
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK | HALF_CARRY_FMASK);
    cpu->regB = newB;
    flag_assign(cpu->regB == 0, &cpu->regF, ZERO_FMASK);
    
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_RLC_C(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint8_t newC = cpu->regC;
    newC <<= 1;
    //copy previous bit 7 to new bit 0
    flag_assign((cpu->regC & 0x80), &newC, 0x01);
    //copy previous bit 7 to carry flag
    flag_assign((cpu->regC & 0x80), &cpu->regF, CARRY_FMASK);
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK | HALF_CARRY_FMASK);
    cpu->regC = newC;
    flag_assign(cpu->regC == 0, &cpu->regF, ZERO_FMASK);
    
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_RLC_D(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint8_t newD = cpu->regD;
    newD <<= 1;
    //copy previous bit 7 to new bit 0
    flag_assign((cpu->regD & 0x80), &newD, 0x01);
    //copy previous bit 7 to carry flag
    flag_assign((cpu->regD & 0x80), &cpu->regF, CARRY_FMASK);
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK | HALF_CARRY_FMASK);
    cpu->regD = newD;
    flag_assign(cpu->regD == 0, &cpu->regF, ZERO_FMASK);
    
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_RLC_E(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint8_t newE = cpu->regE;
    newE <<= 1;
    //copy previous bit 7 to new bit 0
    flag_assign((cpu->regE & 0x80), &newE, 0x01);
    //copy previous bit 7 to carry flag
    flag_assign((cpu->regE & 0x80), &cpu->regF, CARRY_FMASK);
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK | HALF_CARRY_FMASK);
    cpu->regE = newE;
    flag_assign(cpu->regE == 0, &cpu->regF, ZERO_FMASK);
    
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_RLC_H(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint8_t newH = cpu->regH;
    newH <<= 1;
    //copy previous bit 7 to new bit 0
    flag_assign((cpu->regH & 0x80), &newH, 0x01);
    //copy previous bit 7 to carry flag
    flag_assign((cpu->regH & 0x80), &cpu->regF, CARRY_FMASK);
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK | HALF_CARRY_FMASK);
    cpu->regH = newH;
    flag_assign(cpu->regH == 0, &cpu->regF, ZERO_FMASK);
    
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_RLC_L(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint8_t newL = cpu->regL;
    newL <<= 1;
    //copy previous bit 7 to new bit 0
    flag_assign((cpu->regL & 0x80), &newL, 0x01);
    //copy previous bit 7 to carry flag
    flag_assign((cpu->regL & 0x80), &cpu->regF, CARRY_FMASK);
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK | HALF_CARRY_FMASK);
    cpu->regL = newL;
    flag_assign(cpu->regL == 0, &cpu->regF, ZERO_FMASK);
    
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_RLC_derefHL(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint8_t data;
    if(0 != read_memory(emu, ((cpu->regH << 8) | cpu->regL), &data))
        destroy_emulator(emu, EXIT_FAILURE);  
    uint8_t newdata = data;
    newdata <<= 1;
    //copy previous bit 7 to new bit 0
    flag_assign((data & 0x80), &newdata, 0x01);
    //copy previous bit 7 to carry flag
    flag_assign((data & 0x80), &cpu->regF, CARRY_FMASK);
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK | HALF_CARRY_FMASK);
    data = newdata;
    flag_assign(data == 0, &cpu->regF, ZERO_FMASK);
    
    if(0 != write_memory(emu, ((cpu->regH << 8) | cpu->regL), data))
        destroy_emulator(emu, EXIT_FAILURE);
    
    cpu->t_cycles += emu->prefix_timing_table[op]; // 16;
}
void prefixed_RLC_A(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint8_t newA = cpu->regA;
    newA <<= 1;
    //copy previous bit 7 to new bit 0
    flag_assign((cpu->regA & 0x80), &newA, 0x01);
    //copy previous bit 7 to carry flag
    flag_assign((cpu->regA & 0x80), &cpu->regF, CARRY_FMASK);
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK | HALF_CARRY_FMASK);
    cpu->regA = newA;
    flag_assign(cpu->regA == 0, &cpu->regF, ZERO_FMASK);
    
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_RRC_B(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint8_t newB = cpu->regB;
    newB >>= 1;
    //copy previous bit 0 to new bit 7
    flag_assign((cpu->regB & 0x01), &newB, 0x80);
    //copy previous bit 0 to carry flag
    flag_assign((cpu->regB & 0x01), &cpu->regF, CARRY_FMASK);
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK | HALF_CARRY_FMASK);
    cpu->regB = newB;
    flag_assign(cpu->regB == 0, &cpu->regF, ZERO_FMASK);
    
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_RRC_C(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint8_t newC = cpu->regC;
    newC >>= 1;
    //copy previous bit 0 to new bit 7
    flag_assign((cpu->regC & 0x01), &newC, 0x80);
    //copy previous bit 0 to carry flag
    flag_assign((cpu->regC & 0x01), &cpu->regF, CARRY_FMASK);
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK | HALF_CARRY_FMASK);
    cpu->regC = newC;
    flag_assign(cpu->regC == 0, &cpu->regF, ZERO_FMASK);
    
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_RRC_D(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint8_t newD = cpu->regD;
    newD >>= 1;
    //copy previous bit 0 to new bit 7
    flag_assign((cpu->regD & 0x01), &newD, 0x80);
    //copy previous bit 0 to carry flag
    flag_assign((cpu->regD & 0x01), &cpu->regF, CARRY_FMASK);
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK | HALF_CARRY_FMASK);
    cpu->regD = newD;
    flag_assign(cpu->regD == 0, &cpu->regF, ZERO_FMASK);
    
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_RRC_E(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint8_t newE = cpu->regE;
    newE >>= 1;
    //copy previous bit 0 to new bit 7
    flag_assign((cpu->regE & 0x01), &newE, 0x80);
    //copy previous bit 0 to carry flag
    flag_assign((cpu->regE & 0x01), &cpu->regF, CARRY_FMASK);
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK | HALF_CARRY_FMASK);
    cpu->regE = newE;
    flag_assign(cpu->regE == 0, &cpu->regF, ZERO_FMASK);
    
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_RRC_H(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint8_t newH = cpu->regH;
    newH >>= 1;
    //copy previous bit 0 to new bit 7
    flag_assign((cpu->regH & 0x01), &newH, 0x80);
    //copy previous bit 0 to carry flag
    flag_assign((cpu->regH & 0x01), &cpu->regF, CARRY_FMASK);
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK | HALF_CARRY_FMASK);
    cpu->regH = newH;
    flag_assign(cpu->regH == 0, &cpu->regF, ZERO_FMASK);
    
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_RRC_L(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint8_t newL = cpu->regL;
    newL >>= 1;
    //copy previous bit 0 to new bit 7
    flag_assign((cpu->regL & 0x01), &newL, 0x80);
    //copy previous bit 0 to carry flag
    flag_assign((cpu->regL & 0x01), &cpu->regF, CARRY_FMASK);
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK | HALF_CARRY_FMASK);
    cpu->regL = newL;
    flag_assign(cpu->regL == 0, &cpu->regF, ZERO_FMASK);
    
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_RRC_derefHL(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint8_t data;
    if(0 != read_memory(emu, ((cpu->regH << 8) | cpu->regL), &data))
        destroy_emulator(emu, EXIT_FAILURE);  
    uint8_t newdata = data;
    newdata >>= 1;
    //copy previous bit 0 to new bit 7
    flag_assign((data & 0x01), &newdata, 0x80);
    //copy previous bit 0 to carry flag
    flag_assign((data & 0x01), &cpu->regF, CARRY_FMASK);
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK | HALF_CARRY_FMASK);
    data = newdata;
    flag_assign(data == 0, &cpu->regF, ZERO_FMASK);
    
    if(0 != write_memory(emu, ((cpu->regH << 8) | cpu->regL), data))
        destroy_emulator(emu, EXIT_FAILURE);
    
    cpu->t_cycles += emu->prefix_timing_table[op]; // 16;
}
void prefixed_RRC_A(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint8_t newA = cpu->regA;
    newA >>= 1;
    //copy previous bit 0 to new bit 7
    flag_assign((cpu->regA & 0x01), &newA, 0x80);
    //copy previous bit 0 to carry flag
    flag_assign((cpu->regA & 0x01), &cpu->regF, CARRY_FMASK);
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK | HALF_CARRY_FMASK);
    cpu->regA = newA;
    flag_assign(cpu->regA == 0, &cpu->regF, ZERO_FMASK);
    
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_RL_B(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint8_t newB = cpu->regB;
    newB <<= 1;
    flag_assign(cpu->regF & CARRY_FMASK, &newB, 0x01);
    flag_assign(cpu->regB & 0x80, &cpu->regF, CARRY_FMASK);
    flag_assign(newB == 0, &cpu->regF, ZERO_FMASK);
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK + HALF_CARRY_FMASK);
    cpu->regB = newB;
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_RL_C(void *arg, uint8_t op)
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
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_RL_D(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint8_t newD = cpu->regD;
    newD <<= 1;
    flag_assign(cpu->regF & CARRY_FMASK, &newD, 0x01);
    flag_assign(cpu->regD & 0x80, &cpu->regF, CARRY_FMASK);
    flag_assign(newD == 0, &cpu->regF, ZERO_FMASK);
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK + HALF_CARRY_FMASK);
    cpu->regD = newD;
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_RL_E(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint8_t newE = cpu->regE;
    newE <<= 1;
    flag_assign(cpu->regF & CARRY_FMASK, &newE, 0x01);
    flag_assign(cpu->regE & 0x80, &cpu->regF, CARRY_FMASK);
    flag_assign(newE == 0, &cpu->regF, ZERO_FMASK);
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK + HALF_CARRY_FMASK);
    cpu->regE = newE;
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_RL_H(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint8_t newH = cpu->regH;
    newH <<= 1;
    flag_assign(cpu->regF & CARRY_FMASK, &newH, 0x01);
    flag_assign(cpu->regH & 0x80, &cpu->regF, CARRY_FMASK);
    flag_assign(newH == 0, &cpu->regF, ZERO_FMASK);
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK + HALF_CARRY_FMASK);
    cpu->regH = newH;
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_RL_L(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint8_t newL = cpu->regL;
    newL <<= 1;
    flag_assign(cpu->regF & CARRY_FMASK, &newL, 0x01);
    flag_assign(cpu->regL & 0x80, &cpu->regF, CARRY_FMASK);
    flag_assign(newL == 0, &cpu->regF, ZERO_FMASK);
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK + HALF_CARRY_FMASK);
    cpu->regL = newL;
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_RL_derefHL(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint8_t data;
    if(0 != read_memory(emu, ((cpu->regH << 8) | cpu->regL), &data))
        destroy_emulator(emu, EXIT_FAILURE);  
    uint8_t newdata = data;
    newdata <<= 1;
    flag_assign(cpu->regF & CARRY_FMASK, &newdata, 0x01);
    flag_assign(data & 0x80, &cpu->regF, CARRY_FMASK);
    flag_assign(newdata == 0, &cpu->regF, ZERO_FMASK);
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK + HALF_CARRY_FMASK);
    data = newdata;
    
    if(0 != write_memory(emu, ((cpu->regH << 8) | cpu->regL), data))
        destroy_emulator(emu, EXIT_FAILURE);
    cpu->t_cycles += emu->prefix_timing_table[op]; // 16;
}
void prefixed_RL_A(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint8_t newA = cpu->regA;
    newA <<= 1;
    flag_assign(cpu->regF & CARRY_FMASK, &newA, 0x01);
    flag_assign(cpu->regA & 0x80, &cpu->regF, CARRY_FMASK);
    flag_assign(newA == 0, &cpu->regF, ZERO_FMASK);
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK + HALF_CARRY_FMASK);
    cpu->regA = newA;
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_RR_B(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint8_t newB = cpu->regB;
    newB >>= 1;
    //copy previous carry flag to bit 7
    flag_assign((cpu->regF & CARRY_FMASK), &newB, 0x80);
    //copy previous bit 0 to carry flag
    flag_assign((cpu->regB & 0x01), &cpu->regF, CARRY_FMASK);
    flag_assign(newB == 0, &cpu->regF, ZERO_FMASK);
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK | HALF_CARRY_FMASK);
    cpu->regB = newB;
    
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_RR_C(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint8_t newC = cpu->regC;
    newC >>= 1;
    //copy previous carry flag to bit 7
    flag_assign((cpu->regF & CARRY_FMASK), &newC, 0x80);
    //copy previous bit 0 to carry flag
    flag_assign((cpu->regC & 0x01), &cpu->regF, CARRY_FMASK);
    flag_assign(newC == 0, &cpu->regF, ZERO_FMASK);
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK | HALF_CARRY_FMASK);
    cpu->regC = newC;
    
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_RR_D(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint8_t newD = cpu->regD;
    newD >>= 1;
    //copy previous carry flag to bit 7
    flag_assign((cpu->regF & CARRY_FMASK), &newD, 0x80);
    //copy previous bit 0 to carry flag
    flag_assign((cpu->regD & 0x01), &cpu->regF, CARRY_FMASK);
    flag_assign(newD == 0, &cpu->regF, ZERO_FMASK);
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK | HALF_CARRY_FMASK);
    cpu->regD = newD;
    
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_RR_E(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint8_t newE = cpu->regE;
    newE >>= 1;
    //copy previous carry flag to bit 7
    flag_assign((cpu->regF & CARRY_FMASK), &newE, 0x80);
    //copy previous bit 0 to carry flag
    flag_assign((cpu->regE & 0x01), &cpu->regF, CARRY_FMASK);
    flag_assign(newE == 0, &cpu->regF, ZERO_FMASK);
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK | HALF_CARRY_FMASK);
    cpu->regE = newE;
    
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_RR_H(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint8_t newH = cpu->regH;
    newH >>= 1;
    //copy previous carry flag to bit 7
    flag_assign((cpu->regF & CARRY_FMASK), &newH, 0x80);
    //copy previous bit 0 to carry flag
    flag_assign((cpu->regH & 0x01), &cpu->regF, CARRY_FMASK);
    flag_assign(newH == 0, &cpu->regF, ZERO_FMASK);
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK | HALF_CARRY_FMASK);
    cpu->regH = newH;
    
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_RR_L(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint8_t newL = cpu->regL;
    newL >>= 1;
    //copy previous carry flag to bit 7
    flag_assign((cpu->regF & CARRY_FMASK), &newL, 0x80);
    //copy previous bit 0 to carry flag
    flag_assign((cpu->regL & 0x01), &cpu->regF, CARRY_FMASK);
    flag_assign(newL == 0, &cpu->regF, ZERO_FMASK);
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK | HALF_CARRY_FMASK);
    cpu->regL = newL;
    
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_RR_derefHL(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint8_t data;
    if(0 != read_memory(emu, ((cpu->regH << 8) | cpu->regL), &data))
        destroy_emulator(emu, EXIT_FAILURE);  
    uint8_t newdata = data;
    newdata >>= 1;
    //copy previous carry flag to bit 7
    flag_assign((cpu->regF & CARRY_FMASK), &newdata, 0x80);
    //copy previous bit 0 to carry flag
    flag_assign((data & 0x01), &cpu->regF, CARRY_FMASK);
    flag_assign(newdata == 0, &cpu->regF, ZERO_FMASK);
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK | HALF_CARRY_FMASK);
    data = newdata;
    
    if(0 != write_memory(emu, ((cpu->regH << 8) | cpu->regL), data))
        destroy_emulator(emu, EXIT_FAILURE);
    
    cpu->t_cycles += emu->prefix_timing_table[op]; // 16;
}
void prefixed_RR_A(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint8_t newA = cpu->regA;
    newA >>= 1;
    //copy previous carry flag to bit 7
    flag_assign((cpu->regF & CARRY_FMASK), &newA, 0x80);
    //copy previous bit 0 to carry flag
    flag_assign((cpu->regA & 0x01), &cpu->regF, CARRY_FMASK);
    flag_assign(newA == 0, &cpu->regF, ZERO_FMASK);
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK | HALF_CARRY_FMASK);
    cpu->regA = newA;
    
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_SLA_B(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    flag_assign(cpu->regB & 0x80, &cpu->regF, CARRY_FMASK);
    cpu->regB <<= 1;
    flag_assign(cpu->regB == 0, &cpu->regF, ZERO_FMASK);
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK + HALF_CARRY_FMASK);
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_SLA_C(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    flag_assign(cpu->regC & 0x80, &cpu->regF, CARRY_FMASK);
    cpu->regC <<= 1;
    flag_assign(cpu->regC == 0, &cpu->regF, ZERO_FMASK);
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK + HALF_CARRY_FMASK);
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_SLA_D(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    flag_assign(cpu->regD & 0x80, &cpu->regF, CARRY_FMASK);
    cpu->regD <<= 1;
    flag_assign(cpu->regD == 0, &cpu->regF, ZERO_FMASK);
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK + HALF_CARRY_FMASK);
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_SLA_E(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    flag_assign(cpu->regE & 0x80, &cpu->regF, CARRY_FMASK);
    cpu->regE <<= 1;
    flag_assign(cpu->regE == 0, &cpu->regF, ZERO_FMASK);
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK + HALF_CARRY_FMASK);
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_SLA_H(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    flag_assign(cpu->regH & 0x80, &cpu->regF, CARRY_FMASK);
    cpu->regH <<= 1;
    flag_assign(cpu->regH == 0, &cpu->regF, ZERO_FMASK);
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK + HALF_CARRY_FMASK);
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_SLA_L(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    flag_assign(cpu->regL & 0x80, &cpu->regF, CARRY_FMASK);
    cpu->regL <<= 1;
    flag_assign(cpu->regL == 0, &cpu->regF, ZERO_FMASK);
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK + HALF_CARRY_FMASK);
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_SLA_derefHL(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint8_t data;
    if(0 != read_memory(emu, ((cpu->regH << 8) | cpu->regL), &data))
        destroy_emulator(emu, EXIT_FAILURE);  
    
    flag_assign(data & 0x80, &cpu->regF, CARRY_FMASK);
    data <<= 1;
    flag_assign(data == 0, &cpu->regF, ZERO_FMASK);
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK + HALF_CARRY_FMASK);
    
    if(0 != write_memory(emu, ((cpu->regH << 8) | cpu->regL), data))
        destroy_emulator(emu, EXIT_FAILURE);
    cpu->t_cycles += emu->prefix_timing_table[op]; // 16;
}
void prefixed_SLA_A(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    flag_assign(cpu->regA & 0x80, &cpu->regF, CARRY_FMASK);
    cpu->regA <<= 1;
    flag_assign(cpu->regA == 0, &cpu->regF, ZERO_FMASK);
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK + HALF_CARRY_FMASK);
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_SRA_B(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint8_t newreg = cpu->regB;
    flag_assign(cpu->regB & 0x01, &cpu->regF, CARRY_FMASK);
    newreg >>= 1;
    flag_assign(cpu->regB & 0x80, &newreg, 0x80);
    cpu->regB = newreg;
    flag_assign(cpu->regB == 0, &cpu->regF, ZERO_FMASK);
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK + HALF_CARRY_FMASK);
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_SRA_C(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint8_t newreg = cpu->regC;
    flag_assign(cpu->regC & 0x01, &cpu->regF, CARRY_FMASK);
    newreg >>= 1;
    flag_assign(cpu->regC & 0x80, &newreg, 0x80);
    cpu->regC = newreg;
    flag_assign(cpu->regC == 0, &cpu->regF, ZERO_FMASK);
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK + HALF_CARRY_FMASK);
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_SRA_D(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint8_t newreg = cpu->regD;
    flag_assign(cpu->regD & 0x01, &cpu->regF, CARRY_FMASK);
    newreg >>= 1;
    flag_assign(cpu->regD & 0x80, &newreg, 0x80);
    cpu->regD = newreg;
    flag_assign(cpu->regD == 0, &cpu->regF, ZERO_FMASK);
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK + HALF_CARRY_FMASK);
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_SRA_E(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint8_t newreg = cpu->regE;
    flag_assign(cpu->regE & 0x01, &cpu->regF, CARRY_FMASK);
    newreg >>= 1;
    flag_assign(cpu->regE & 0x80, &newreg, 0x80);
    cpu->regE = newreg;
    flag_assign(cpu->regE == 0, &cpu->regF, ZERO_FMASK);
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK + HALF_CARRY_FMASK);
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_SRA_H(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint8_t newreg = cpu->regH;
    flag_assign(cpu->regH & 0x01, &cpu->regF, CARRY_FMASK);
    newreg >>= 1;
    flag_assign(cpu->regH & 0x80, &newreg, 0x80);
    cpu->regH = newreg;
    flag_assign(cpu->regH == 0, &cpu->regF, ZERO_FMASK);
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK + HALF_CARRY_FMASK);
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_SRA_L(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint8_t newreg = cpu->regL;
    flag_assign(cpu->regL & 0x01, &cpu->regF, CARRY_FMASK);
    newreg >>= 1;
    flag_assign(cpu->regL & 0x80, &newreg, 0x80);
    cpu->regL = newreg;
    flag_assign(cpu->regL == 0, &cpu->regF, ZERO_FMASK);
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK + HALF_CARRY_FMASK);
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_SRA_derefHL(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint8_t data;
    if(0 != read_memory(emu, ((cpu->regH << 8) | cpu->regL), &data))
        destroy_emulator(emu, EXIT_FAILURE);  
    uint8_t newdata = data;
    flag_assign(data & 0x01, &cpu->regF, CARRY_FMASK);
    newdata >>= 1;
    flag_assign(data & 0x80, &newdata, 0x80);
    data = newdata;
    flag_assign(data == 0, &cpu->regF, ZERO_FMASK);
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK + HALF_CARRY_FMASK);
    
    if(0 != write_memory(emu, ((cpu->regH << 8) | cpu->regL), data))
        destroy_emulator(emu, EXIT_FAILURE);
    
    cpu->t_cycles += emu->prefix_timing_table[op]; // 16;
}
void prefixed_SRA_A(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint8_t newreg = cpu->regA;
    flag_assign(cpu->regA & 0x01, &cpu->regF, CARRY_FMASK);
    newreg >>= 1;
    flag_assign(cpu->regA & 0x80, &newreg, 0x80);
    cpu->regA = newreg;
    flag_assign(cpu->regA == 0, &cpu->regF, ZERO_FMASK);
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK + HALF_CARRY_FMASK);
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_SWAP_B(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regB = ((cpu->regB & 0xF0) >> 4) | ((cpu->regB & 0x0F) << 4);
    
    flag_assign(cpu->regB == 0, &cpu->regF, ZERO_FMASK);
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK | HALF_CARRY_FMASK | CARRY_FMASK);
    
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_SWAP_C(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regC = ((cpu->regC & 0xF0) >> 4) | ((cpu->regC & 0x0F) << 4);
    
    flag_assign(cpu->regC == 0, &cpu->regF, ZERO_FMASK);
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK | HALF_CARRY_FMASK | CARRY_FMASK);
    
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_SWAP_D(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regD = ((cpu->regD & 0xF0) >> 4) | ((cpu->regD & 0x0F) << 4);
    
    flag_assign(cpu->regD == 0, &cpu->regF, ZERO_FMASK);
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK | HALF_CARRY_FMASK | CARRY_FMASK);
    
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_SWAP_E(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regE = ((cpu->regE & 0xF0) >> 4) | ((cpu->regE & 0x0F) << 4);
    
    flag_assign(cpu->regE == 0, &cpu->regF, ZERO_FMASK);
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK | HALF_CARRY_FMASK | CARRY_FMASK);
    
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_SWAP_H(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regH = ((cpu->regH & 0xF0) >> 4) | ((cpu->regH & 0x0F) << 4);
    
    flag_assign(cpu->regH == 0, &cpu->regF, ZERO_FMASK);
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK | HALF_CARRY_FMASK | CARRY_FMASK);
    
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_SWAP_L(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regL = ((cpu->regL & 0xF0) >> 4) | ((cpu->regL & 0x0F) << 4);
    
    flag_assign(cpu->regL == 0, &cpu->regF, ZERO_FMASK);
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK | HALF_CARRY_FMASK | CARRY_FMASK);
    
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_SWAP_derefHL(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint8_t data;
    if(0 != read_memory(emu, ((cpu->regH << 8) | cpu->regL), &data))
        destroy_emulator(emu, EXIT_FAILURE);  
    
    data = ((data & 0xF0) >> 4) | ((data & 0x0F) << 4);
    
    flag_assign(data == 0, &cpu->regF, ZERO_FMASK);
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK | HALF_CARRY_FMASK | CARRY_FMASK);
    
    if(0 != write_memory(emu, ((cpu->regH << 8) | cpu->regL), data))
        destroy_emulator(emu, EXIT_FAILURE);
    
    cpu->t_cycles += emu->prefix_timing_table[op]; // 16;
}
void prefixed_SWAP_A(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regA = ((cpu->regA & 0xF0) >> 4) | ((cpu->regA & 0x0F) << 4);
    
    flag_assign(cpu->regA == 0, &cpu->regF, ZERO_FMASK);
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK | HALF_CARRY_FMASK | CARRY_FMASK);
    
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_SRL_B(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    flag_assign(cpu->regB & 0x01, &cpu->regF, CARRY_FMASK);
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK | HALF_CARRY_FMASK);
    cpu->regB >>= 1;
    flag_assign(cpu->regB == 0, &cpu->regF, ZERO_FMASK);
    
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_SRL_C(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    flag_assign(cpu->regC & 0x01, &cpu->regF, CARRY_FMASK);
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK | HALF_CARRY_FMASK);
    cpu->regC >>= 1;
    flag_assign(cpu->regC == 0, &cpu->regF, ZERO_FMASK);
    
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_SRL_D(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    flag_assign(cpu->regD & 0x01, &cpu->regF, CARRY_FMASK);
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK | HALF_CARRY_FMASK);
    cpu->regD >>= 1;
    flag_assign(cpu->regD == 0, &cpu->regF, ZERO_FMASK);
    
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_SRL_E(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    flag_assign(cpu->regE & 0x01, &cpu->regF, CARRY_FMASK);
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK | HALF_CARRY_FMASK);
    cpu->regE >>= 1;
    flag_assign(cpu->regE == 0, &cpu->regF, ZERO_FMASK);
    
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_SRL_H(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    flag_assign(cpu->regH & 0x01, &cpu->regF, CARRY_FMASK);
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK | HALF_CARRY_FMASK);
    cpu->regH >>= 1;
    flag_assign(cpu->regH == 0, &cpu->regF, ZERO_FMASK);
    
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_SRL_L(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    flag_assign(cpu->regL & 0x01, &cpu->regF, CARRY_FMASK);
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK | HALF_CARRY_FMASK);
    cpu->regL >>= 1;
    flag_assign(cpu->regL == 0, &cpu->regF, ZERO_FMASK);
    
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_SRL_derefHL(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint8_t data;
    if(0 != read_memory(emu, ((cpu->regH << 8) | cpu->regL), &data))
        destroy_emulator(emu, EXIT_FAILURE);  
    
    flag_assign(data & 0x01, &cpu->regF, CARRY_FMASK);
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK | HALF_CARRY_FMASK);
    data >>= 1;
    flag_assign(data == 0, &cpu->regF, ZERO_FMASK);
    
    if(0 != write_memory(emu, ((cpu->regH << 8) | cpu->regL), data))
        destroy_emulator(emu, EXIT_FAILURE);
    
    cpu->t_cycles += emu->prefix_timing_table[op]; // 16;
}
void prefixed_SRL_A(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    flag_assign(cpu->regA & 0x01, &cpu->regF, CARRY_FMASK);
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK | HALF_CARRY_FMASK);
    cpu->regA >>= 1;
    flag_assign(cpu->regA == 0, &cpu->regF, ZERO_FMASK);
    
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_BIT_0_B(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    flag_assign(!(cpu->regB & 0x01), &cpu->regF, ZERO_FMASK);
    
    //half carry
    flag_assign(true, &cpu->regF, HALF_CARRY_FMASK);
    
    //negative flag
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK);
    
    emu->cpu.t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_BIT_0_C(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    flag_assign(!(cpu->regC & 0x01), &cpu->regF, ZERO_FMASK);
    
    //half carry
    flag_assign(true, &cpu->regF, HALF_CARRY_FMASK);
    
    //negative flag
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK);
    
    emu->cpu.t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_BIT_0_D(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    flag_assign(!(cpu->regD & 0x01), &cpu->regF, ZERO_FMASK);
    
    //half carry
    flag_assign(true, &cpu->regF, HALF_CARRY_FMASK);
    
    //negative flag
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK);
    
    emu->cpu.t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_BIT_0_E(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    flag_assign(!(cpu->regE & 0x01), &cpu->regF, ZERO_FMASK);
    
    //half carry
    flag_assign(true, &cpu->regF, HALF_CARRY_FMASK);
    
    //negative flag
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK);
    
    emu->cpu.t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_BIT_0_H(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    flag_assign(!(cpu->regH & 0x01), &cpu->regF, ZERO_FMASK);
    
    //half carry
    flag_assign(true, &cpu->regF, HALF_CARRY_FMASK);
    
    //negative flag
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK);
    
    emu->cpu.t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_BIT_0_L(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    flag_assign(!(cpu->regL & 0x01), &cpu->regF, ZERO_FMASK);
    
    //half carry
    flag_assign(true, &cpu->regF, HALF_CARRY_FMASK);
    
    //negative flag
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK);
    
    emu->cpu.t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_BIT_0_derefHL(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint8_t data;
    if(0 != read_memory(emu, ((cpu->regH << 8) | cpu->regL), &data))
        destroy_emulator(emu, EXIT_FAILURE);  
    
    flag_assign(!(data & 0x01), &cpu->regF, ZERO_FMASK);
    
    //half carry
    flag_assign(true, &cpu->regF, HALF_CARRY_FMASK);
    
    //negative flag
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK);
    
    if(0 != write_memory(emu, ((cpu->regH << 8) | cpu->regL), data))
        destroy_emulator(emu, EXIT_FAILURE);
    
    emu->cpu.t_cycles += emu->prefix_timing_table[op]; // 12;
}
void prefixed_BIT_0_A(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    flag_assign(!(cpu->regA & 0x01), &cpu->regF, ZERO_FMASK);
    
    //half carry
    flag_assign(true, &cpu->regF, HALF_CARRY_FMASK);
    
    //negative flag
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK);
    
    emu->cpu.t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_BIT_1_B(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    flag_assign(!(cpu->regB & 0x02), &cpu->regF, ZERO_FMASK);
    
    //half carry
    flag_assign(true, &cpu->regF, HALF_CARRY_FMASK);
    
    //negative flag
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK);
    
    emu->cpu.t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_BIT_1_C(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    flag_assign(!(cpu->regC & 0x02), &cpu->regF, ZERO_FMASK);
    
    //half carry
    flag_assign(true, &cpu->regF, HALF_CARRY_FMASK);
    
    //negative flag
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK);
    
    emu->cpu.t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_BIT_1_D(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    flag_assign(!(cpu->regD & 0x02), &cpu->regF, ZERO_FMASK);
    
    //half carry
    flag_assign(true, &cpu->regF, HALF_CARRY_FMASK);
    
    //negative flag
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK);
    
    emu->cpu.t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_BIT_1_E(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    flag_assign(!(cpu->regE & 0x02), &cpu->regF, ZERO_FMASK);
    
    //half carry
    flag_assign(true, &cpu->regF, HALF_CARRY_FMASK);
    
    //negative flag
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK);
    
    emu->cpu.t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_BIT_1_H(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    flag_assign(!(cpu->regH & 0x02), &cpu->regF, ZERO_FMASK);
    
    //half carry
    flag_assign(true, &cpu->regF, HALF_CARRY_FMASK);
    
    //negative flag
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK);
    
    emu->cpu.t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_BIT_1_L(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    flag_assign(!(cpu->regL & 0x02), &cpu->regF, ZERO_FMASK);
    
    //half carry
    flag_assign(true, &cpu->regF, HALF_CARRY_FMASK);
    
    //negative flag
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK);
    
    emu->cpu.t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_BIT_1_derefHL(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint8_t data;
    if(0 != read_memory(emu, ((cpu->regH << 8) | cpu->regL), &data))
        destroy_emulator(emu, EXIT_FAILURE);  
    
    flag_assign(!(data & 0x02), &cpu->regF, ZERO_FMASK);
    
    //half carry
    flag_assign(true, &cpu->regF, HALF_CARRY_FMASK);
    
    //negative flag
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK);
    
    if(0 != write_memory(emu, ((cpu->regH << 8) | cpu->regL), data))
        destroy_emulator(emu, EXIT_FAILURE);
    
    emu->cpu.t_cycles += emu->prefix_timing_table[op]; // 12;
}
void prefixed_BIT_1_A(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    flag_assign(!(cpu->regA & 0x02), &cpu->regF, ZERO_FMASK);
    
    //half carry
    flag_assign(true, &cpu->regF, HALF_CARRY_FMASK);
    
    //negative flag
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK);
    
    emu->cpu.t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_BIT_2_B(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    flag_assign(!(cpu->regB & 0x04), &cpu->regF, ZERO_FMASK);
    
    //half carry
    flag_assign(true, &cpu->regF, HALF_CARRY_FMASK);
    
    //negative flag
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK);
    
    emu->cpu.t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_BIT_2_C(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    flag_assign(!(cpu->regC & 0x04), &cpu->regF, ZERO_FMASK);
    
    //half carry
    flag_assign(true, &cpu->regF, HALF_CARRY_FMASK);
    
    //negative flag
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK);
    
    emu->cpu.t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_BIT_2_D(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    flag_assign(!(cpu->regD & 0x04), &cpu->regF, ZERO_FMASK);
    
    //half carry
    flag_assign(true, &cpu->regF, HALF_CARRY_FMASK);
    
    //negative flag
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK);
    
    emu->cpu.t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_BIT_2_E(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    flag_assign(!(cpu->regE & 0x04), &cpu->regF, ZERO_FMASK);
    
    //half carry
    flag_assign(true, &cpu->regF, HALF_CARRY_FMASK);
    
    //negative flag
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK);
    
    emu->cpu.t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_BIT_2_H(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    flag_assign(!(cpu->regH & 0x04), &cpu->regF, ZERO_FMASK);
    
    //half carry
    flag_assign(true, &cpu->regF, HALF_CARRY_FMASK);
    
    //negative flag
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK);
    
    emu->cpu.t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_BIT_2_L(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    flag_assign(!(cpu->regL & 0x04), &cpu->regF, ZERO_FMASK);
    
    //half carry
    flag_assign(true, &cpu->regF, HALF_CARRY_FMASK);
    
    //negative flag
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK);
    
    emu->cpu.t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_BIT_2_derefHL(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint8_t data;
    if(0 != read_memory(emu, ((cpu->regH << 8) | cpu->regL), &data))
        destroy_emulator(emu, EXIT_FAILURE);  
    
    flag_assign(!(data & 0x04), &cpu->regF, ZERO_FMASK);
    
    //half carry
    flag_assign(true, &cpu->regF, HALF_CARRY_FMASK);
    
    //negative flag
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK);
    
    if(0 != write_memory(emu, ((cpu->regH << 8) | cpu->regL), data))
        destroy_emulator(emu, EXIT_FAILURE);
    
    emu->cpu.t_cycles += emu->prefix_timing_table[op]; // 12;
}
void prefixed_BIT_2_A(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    flag_assign(!(cpu->regA & 0x04), &cpu->regF, ZERO_FMASK);
    
    //half carry
    flag_assign(true, &cpu->regF, HALF_CARRY_FMASK);
    
    //negative flag
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK);
    
    emu->cpu.t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_BIT_3_B(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    flag_assign(!(cpu->regB & 0x08), &cpu->regF, ZERO_FMASK);
    
    //half carry
    flag_assign(true, &cpu->regF, HALF_CARRY_FMASK);
    
    //negative flag
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK);
    
    emu->cpu.t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_BIT_3_C(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    flag_assign(!(cpu->regC & 0x08), &cpu->regF, ZERO_FMASK);
    
    //half carry
    flag_assign(true, &cpu->regF, HALF_CARRY_FMASK);
    
    //negative flag
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK);
    
    emu->cpu.t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_BIT_3_D(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    flag_assign(!(cpu->regD & 0x08), &cpu->regF, ZERO_FMASK);
    
    //half carry
    flag_assign(true, &cpu->regF, HALF_CARRY_FMASK);
    
    //negative flag
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK);
    
    emu->cpu.t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_BIT_3_E(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    flag_assign(!(cpu->regE & 0x08), &cpu->regF, ZERO_FMASK);
    
    //half carry
    flag_assign(true, &cpu->regF, HALF_CARRY_FMASK);
    
    //negative flag
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK);
    
    emu->cpu.t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_BIT_3_H(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    flag_assign(!(cpu->regH & 0x08), &cpu->regF, ZERO_FMASK);
    
    //half carry
    flag_assign(true, &cpu->regF, HALF_CARRY_FMASK);
    
    //negative flag
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK);
    
    emu->cpu.t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_BIT_3_L(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    flag_assign(!(cpu->regL & 0x08), &cpu->regF, ZERO_FMASK);
    
    //half carry
    flag_assign(true, &cpu->regF, HALF_CARRY_FMASK);
    
    //negative flag
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK);
    
    emu->cpu.t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_BIT_3_derefHL(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint8_t data;
    if(0 != read_memory(emu, ((cpu->regH << 8) | cpu->regL), &data))
        destroy_emulator(emu, EXIT_FAILURE);  
    
    flag_assign(!(data & 0x08), &cpu->regF, ZERO_FMASK);
    
    //half carry
    flag_assign(true, &cpu->regF, HALF_CARRY_FMASK);
    
    //negative flag
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK);
    
    if(0 != write_memory(emu, ((cpu->regH << 8) | cpu->regL), data))
        destroy_emulator(emu, EXIT_FAILURE);
    
    emu->cpu.t_cycles += emu->prefix_timing_table[op]; // 12;
}
void prefixed_BIT_3_A(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    flag_assign(!(cpu->regA & 0x08), &cpu->regF, ZERO_FMASK);
    
    //half carry
    flag_assign(true, &cpu->regF, HALF_CARRY_FMASK);
    
    //negative flag
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK);
    
    emu->cpu.t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_BIT_4_B(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    flag_assign(!(cpu->regB & 0x10), &cpu->regF, ZERO_FMASK);
    
    //half carry
    flag_assign(true, &cpu->regF, HALF_CARRY_FMASK);
    
    //negative flag
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK);
    
    emu->cpu.t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_BIT_4_C(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    flag_assign(!(cpu->regC & 0x10), &cpu->regF, ZERO_FMASK);
    
    //half carry
    flag_assign(true, &cpu->regF, HALF_CARRY_FMASK);
    
    //negative flag
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK);
    
    emu->cpu.t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_BIT_4_D(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    flag_assign(!(cpu->regD & 0x10), &cpu->regF, ZERO_FMASK);
    
    //half carry
    flag_assign(true, &cpu->regF, HALF_CARRY_FMASK);
    
    //negative flag
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK);
    
    emu->cpu.t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_BIT_4_E(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    flag_assign(!(cpu->regE & 0x10), &cpu->regF, ZERO_FMASK);
    
    //half carry
    flag_assign(true, &cpu->regF, HALF_CARRY_FMASK);
    
    //negative flag
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK);
    
    emu->cpu.t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_BIT_4_H(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    flag_assign(!(cpu->regH & 0x10), &cpu->regF, ZERO_FMASK);
    
    //half carry
    flag_assign(true, &cpu->regF, HALF_CARRY_FMASK);
    
    //negative flag
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK);
    
    emu->cpu.t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_BIT_4_L(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    flag_assign(!(cpu->regL & 0x10), &cpu->regF, ZERO_FMASK);
    
    //half carry
    flag_assign(true, &cpu->regF, HALF_CARRY_FMASK);
    
    //negative flag
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK);
    
    emu->cpu.t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_BIT_4_derefHL(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint8_t data;
    if(0 != read_memory(emu, ((cpu->regH << 8) | cpu->regL), &data))
        destroy_emulator(emu, EXIT_FAILURE);  
    
    flag_assign(!(data & 0x10), &cpu->regF, ZERO_FMASK);
    
    //half carry
    flag_assign(true, &cpu->regF, HALF_CARRY_FMASK);
    
    //negative flag
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK);
    
    if(0 != write_memory(emu, ((cpu->regH << 8) | cpu->regL), data))
        destroy_emulator(emu, EXIT_FAILURE);
    
    emu->cpu.t_cycles += emu->prefix_timing_table[op]; // 12;
}
void prefixed_BIT_4_A(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    flag_assign(!(cpu->regA & 0x10), &cpu->regF, ZERO_FMASK);
    
    //half carry
    flag_assign(true, &cpu->regF, HALF_CARRY_FMASK);
    
    //negative flag
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK);
    
    emu->cpu.t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_BIT_5_B(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    flag_assign(!(cpu->regB & 0x20), &cpu->regF, ZERO_FMASK);
    
    //half carry
    flag_assign(true, &cpu->regF, HALF_CARRY_FMASK);
    
    //negative flag
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK);
    
    emu->cpu.t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_BIT_5_C(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    flag_assign(!(cpu->regC & 0x20), &cpu->regF, ZERO_FMASK);
    
    //half carry
    flag_assign(true, &cpu->regF, HALF_CARRY_FMASK);
    
    //negative flag
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK);
    
    emu->cpu.t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_BIT_5_D(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    flag_assign(!(cpu->regD & 0x20), &cpu->regF, ZERO_FMASK);
    
    //half carry
    flag_assign(true, &cpu->regF, HALF_CARRY_FMASK);
    
    //negative flag
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK);
    
    emu->cpu.t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_BIT_5_E(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    flag_assign(!(cpu->regE & 0x20), &cpu->regF, ZERO_FMASK);
    
    //half carry
    flag_assign(true, &cpu->regF, HALF_CARRY_FMASK);
    
    //negative flag
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK);
    
    emu->cpu.t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_BIT_5_H(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    flag_assign(!(cpu->regH & 0x20), &cpu->regF, ZERO_FMASK);
    
    //half carry
    flag_assign(true, &cpu->regF, HALF_CARRY_FMASK);
    
    //negative flag
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK);
    
    emu->cpu.t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_BIT_5_L(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    flag_assign(!(cpu->regL & 0x20), &cpu->regF, ZERO_FMASK);
    
    //half carry
    flag_assign(true, &cpu->regF, HALF_CARRY_FMASK);
    
    //negative flag
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK);
    
    emu->cpu.t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_BIT_5_derefHL(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint8_t data;
    if(0 != read_memory(emu, ((cpu->regH << 8) | cpu->regL), &data))
        destroy_emulator(emu, EXIT_FAILURE);  
    
    flag_assign(!(data & 0x20), &cpu->regF, ZERO_FMASK);
    
    //half carry
    flag_assign(true, &cpu->regF, HALF_CARRY_FMASK);
    
    //negative flag
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK);
    
    if(0 != write_memory(emu, ((cpu->regH << 8) | cpu->regL), data))
        destroy_emulator(emu, EXIT_FAILURE);
    
    emu->cpu.t_cycles += emu->prefix_timing_table[op]; // 12;
}
void prefixed_BIT_5_A(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    flag_assign(!(cpu->regA & 0x20), &cpu->regF, ZERO_FMASK);
    
    //half carry
    flag_assign(true, &cpu->regF, HALF_CARRY_FMASK);
    
    //negative flag
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK);
    
    emu->cpu.t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_BIT_6_B(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    flag_assign(!(cpu->regB & 0x40), &cpu->regF, ZERO_FMASK);
    
    //half carry
    flag_assign(true, &cpu->regF, HALF_CARRY_FMASK);
    
    //negative flag
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK);
    
    emu->cpu.t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_BIT_6_C(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    flag_assign(!(cpu->regC & 0x40), &cpu->regF, ZERO_FMASK);
    
    //half carry
    flag_assign(true, &cpu->regF, HALF_CARRY_FMASK);
    
    //negative flag
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK);
    
    emu->cpu.t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_BIT_6_D(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    flag_assign(!(cpu->regD & 0x40), &cpu->regF, ZERO_FMASK);
    
    //half carry
    flag_assign(true, &cpu->regF, HALF_CARRY_FMASK);
    
    //negative flag
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK);
    
    emu->cpu.t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_BIT_6_E(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    flag_assign(!(cpu->regE & 0x40), &cpu->regF, ZERO_FMASK);
    
    //half carry
    flag_assign(true, &cpu->regF, HALF_CARRY_FMASK);
    
    //negative flag
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK);
    
    emu->cpu.t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_BIT_6_H(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    flag_assign(!(cpu->regH & 0x40), &cpu->regF, ZERO_FMASK);
    
    //half carry
    flag_assign(true, &cpu->regF, HALF_CARRY_FMASK);
    
    //negative flag
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK);
    
    emu->cpu.t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_BIT_6_L(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    flag_assign(!(cpu->regL & 0x40), &cpu->regF, ZERO_FMASK);
    
    //half carry
    flag_assign(true, &cpu->regF, HALF_CARRY_FMASK);
    
    //negative flag
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK);
    
    emu->cpu.t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_BIT_6_derefHL(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint8_t data;
    if(0 != read_memory(emu, ((cpu->regH << 8) | cpu->regL), &data))
        destroy_emulator(emu, EXIT_FAILURE);  
    
    flag_assign(!(data & 0x40), &cpu->regF, ZERO_FMASK);
    
    //half carry
    flag_assign(true, &cpu->regF, HALF_CARRY_FMASK);
    
    //negative flag
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK);
    
    if(0 != write_memory(emu, ((cpu->regH << 8) | cpu->regL), data))
        destroy_emulator(emu, EXIT_FAILURE);
    
    emu->cpu.t_cycles += emu->prefix_timing_table[op]; // 12;
}
void prefixed_BIT_6_A(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    flag_assign(!(cpu->regA & 0x40), &cpu->regF, ZERO_FMASK);
    
    //half carry
    flag_assign(true, &cpu->regF, HALF_CARRY_FMASK);
    
    //negative flag
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK);
    
    emu->cpu.t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_BIT_7_B(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    flag_assign(!(cpu->regB & 0x80), &cpu->regF, ZERO_FMASK);
    
    //half carry
    flag_assign(true, &cpu->regF, HALF_CARRY_FMASK);
    
    //negative flag
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK);
    
    emu->cpu.t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_BIT_7_C(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    flag_assign(!(cpu->regC & 0x80), &cpu->regF, ZERO_FMASK);
    
    //half carry
    flag_assign(true, &cpu->regF, HALF_CARRY_FMASK);
    
    //negative flag
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK);
    
    emu->cpu.t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_BIT_7_D(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    flag_assign(!(cpu->regD & 0x80), &cpu->regF, ZERO_FMASK);
    
    //half carry
    flag_assign(true, &cpu->regF, HALF_CARRY_FMASK);
    
    //negative flag
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK);
    
    emu->cpu.t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_BIT_7_E(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    flag_assign(!(cpu->regE & 0x80), &cpu->regF, ZERO_FMASK);
    
    //half carry
    flag_assign(true, &cpu->regF, HALF_CARRY_FMASK);
    
    //negative flag
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK);
    
    emu->cpu.t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_BIT_7_H(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    flag_assign(!(cpu->regH & 0x80), &cpu->regF, ZERO_FMASK);
    
    //half carry
    flag_assign(true, &cpu->regF, HALF_CARRY_FMASK);
    
    //negative flag
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK);
    
    emu->cpu.t_cycles += emu->prefix_timing_table[op]; // 8;
}

void prefixed_BIT_7_L(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    flag_assign(!(cpu->regL & 0x80), &cpu->regF, ZERO_FMASK);
    
    //half carry
    flag_assign(true, &cpu->regF, HALF_CARRY_FMASK);
    
    //negative flag
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK);
    
    emu->cpu.t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_BIT_7_derefHL(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint8_t data;
    if(0 != read_memory(emu, ((cpu->regH << 8) | cpu->regL), &data))
        destroy_emulator(emu, EXIT_FAILURE);  
    
    flag_assign(!(data & 0x80), &cpu->regF, ZERO_FMASK);
    
    //half carry
    flag_assign(true, &cpu->regF, HALF_CARRY_FMASK);
    
    //negative flag
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK);
    
    if(0 != write_memory(emu, ((cpu->regH << 8) | cpu->regL), data))
        destroy_emulator(emu, EXIT_FAILURE);
    
    emu->cpu.t_cycles += emu->prefix_timing_table[op]; // 12;
}
void prefixed_BIT_7_A(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    flag_assign(!(cpu->regA & 0x80), &cpu->regF, ZERO_FMASK);
    
    //half carry
    flag_assign(true, &cpu->regF, HALF_CARRY_FMASK);
    
    //negative flag
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK);
    
    emu->cpu.t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_RES_0_B(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regB &= ~(0x01);
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_RES_0_C(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regC &= ~(0x01);
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_RES_0_D(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regD &= ~(0x01);
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_RES_0_E(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regE &= ~(0x01);
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_RES_0_H(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regH &= ~(0x01);
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_RES_0_L(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regL &= ~(0x01);
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_RES_0_derefHL(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint8_t data;
    if(0 != read_memory(emu, ((cpu->regH << 8) | cpu->regL), &data))
        destroy_emulator(emu, EXIT_FAILURE);  
    
    data &= ~(0x01);
    
    if(0 != write_memory(emu, ((cpu->regH << 8) | cpu->regL), data))
        destroy_emulator(emu, EXIT_FAILURE);
    cpu->t_cycles += emu->prefix_timing_table[op]; // 16;
}
void prefixed_RES_0_A(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regA &= ~(0x01);
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_RES_1_B(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regB &= ~(0x02);
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_RES_1_C(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regC &= ~(0x02);
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_RES_1_D(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regD &= ~(0x02);
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_RES_1_E(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regE &= ~(0x02);
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_RES_1_H(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regH &= ~(0x02);
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_RES_1_L(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regL &= ~(0x02);
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_RES_1_derefHL(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint8_t data;
    if(0 != read_memory(emu, ((cpu->regH << 8) | cpu->regL), &data))
        destroy_emulator(emu, EXIT_FAILURE);  
    
    data &= ~(0x02);
    
    if(0 != write_memory(emu, ((cpu->regH << 8) | cpu->regL), data))
        destroy_emulator(emu, EXIT_FAILURE);
    cpu->t_cycles += emu->prefix_timing_table[op]; // 16;
}
void prefixed_RES_1_A(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regA &= ~(0x02);
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_RES_2_B(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regB &= ~(0x04);
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_RES_2_C(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regC &= ~(0x04);
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_RES_2_D(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regD &= ~(0x04);
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_RES_2_E(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regE &= ~(0x04);
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_RES_2_H(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regH &= ~(0x04);
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_RES_2_L(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regL &= ~(0x04);
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_RES_2_derefHL(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint8_t data;
    if(0 != read_memory(emu, ((cpu->regH << 8) | cpu->regL), &data))
        destroy_emulator(emu, EXIT_FAILURE);  
    
    data &= ~(0x04);
    
    if(0 != write_memory(emu, ((cpu->regH << 8) | cpu->regL), data))
        destroy_emulator(emu, EXIT_FAILURE);
    cpu->t_cycles += emu->prefix_timing_table[op]; // 16;
}
void prefixed_RES_2_A(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regA &= ~(0x04);
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_RES_3_B(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regB &= ~(0x08);
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_RES_3_C(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regC &= ~(0x08);
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_RES_3_D(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regD &= ~(0x08);
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_RES_3_E(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regE &= ~(0x08);
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_RES_3_H(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regH &= ~(0x08);
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_RES_3_L(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regL &= ~(0x08);
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_RES_3_derefHL(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint8_t data;
    if(0 != read_memory(emu, ((cpu->regH << 8) | cpu->regL), &data))
        destroy_emulator(emu, EXIT_FAILURE);  
    
    data &= ~(0x08);
    
    if(0 != write_memory(emu, ((cpu->regH << 8) | cpu->regL), data))
        destroy_emulator(emu, EXIT_FAILURE);
    cpu->t_cycles += emu->prefix_timing_table[op]; // 16;
}
void prefixed_RES_3_A(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regA &= ~(0x08);
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_RES_4_B(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regB &= ~(0x10);
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_RES_4_C(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regC &= ~(0x10);
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_RES_4_D(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regD &= ~(0x10);
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_RES_4_E(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regE &= ~(0x10);
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_RES_4_H(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regH &= ~(0x10);
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_RES_4_L(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regL &= ~(0x10);
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_RES_4_derefHL(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint8_t data;
    if(0 != read_memory(emu, ((cpu->regH << 8) | cpu->regL), &data))
        destroy_emulator(emu, EXIT_FAILURE);  
    
    data &= ~(0x10);
    
    if(0 != write_memory(emu, ((cpu->regH << 8) | cpu->regL), data))
        destroy_emulator(emu, EXIT_FAILURE);
    cpu->t_cycles += emu->prefix_timing_table[op]; // 16;
}
void prefixed_RES_4_A(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regA &= ~(0x10);
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_RES_5_B(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regB &= ~(0x20);
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_RES_5_C(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regC &= ~(0x20);
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_RES_5_D(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regD &= ~(0x20);
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_RES_5_E(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regE &= ~(0x20);
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_RES_5_H(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regH &= ~(0x20);
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_RES_5_L(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regL &= ~(0x20);
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_RES_5_derefHL(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint8_t data;
    if(0 != read_memory(emu, ((cpu->regH << 8) | cpu->regL), &data))
        destroy_emulator(emu, EXIT_FAILURE);  
    
    data &= ~(0x20);
    
    if(0 != write_memory(emu, ((cpu->regH << 8) | cpu->regL), data))
        destroy_emulator(emu, EXIT_FAILURE);
    cpu->t_cycles += emu->prefix_timing_table[op]; // 16;
}
void prefixed_RES_5_A(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regA &= ~(0x20);
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_RES_6_B(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regB &= ~(0x40);
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_RES_6_C(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regC &= ~(0x40);
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_RES_6_D(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regD &= ~(0x40);
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_RES_6_E(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regE &= ~(0x40);
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_RES_6_H(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regH &= ~(0x40);
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_RES_6_L(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regL &= ~(0x40);
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_RES_6_derefHL(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint8_t data;
    if(0 != read_memory(emu, ((cpu->regH << 8) | cpu->regL), &data))
        destroy_emulator(emu, EXIT_FAILURE);  
    
    data &= ~(0x40);
    
    if(0 != write_memory(emu, ((cpu->regH << 8) | cpu->regL), data))
        destroy_emulator(emu, EXIT_FAILURE);
    cpu->t_cycles += emu->prefix_timing_table[op]; // 16;
}
void prefixed_RES_6_A(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regA &= ~(0x40);
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_RES_7_B(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regB &= ~(0x80);
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_RES_7_C(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regC &= ~(0x80);
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_RES_7_D(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regD &= ~(0x80);
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_RES_7_E(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regE &= ~(0x80);
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_RES_7_H(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regH &= ~(0x80);
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_RES_7_L(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regL &= ~(0x80);
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_RES_7_derefHL(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint8_t data;
    if(0 != read_memory(emu, ((cpu->regH << 8) | cpu->regL), &data))
        destroy_emulator(emu, EXIT_FAILURE);  
    
    data &= ~(0x80);
    
    if(0 != write_memory(emu, ((cpu->regH << 8) | cpu->regL), data))
        destroy_emulator(emu, EXIT_FAILURE);
    cpu->t_cycles += emu->prefix_timing_table[op]; // 16;
}
void prefixed_RES_7_A(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regA &= ~(0x80);
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_SET_0_B(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regB |= 0x01;
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_SET_0_C(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regC |= 0x01;
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_SET_0_D(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regD |= 0x01;
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_SET_0_E(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regE |= 0x01;
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_SET_0_H(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regH |= 0x01;
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_SET_0_L(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regL |= 0x01;
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_SET_0_derefHL(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint8_t data;
    if(0 != read_memory(emu, ((cpu->regH << 8) | cpu->regL), &data))
        destroy_emulator(emu, EXIT_FAILURE);  
    
    data |= 0x01;
    
    if(0 != write_memory(emu, ((cpu->regH << 8) | cpu->regL), data))
        destroy_emulator(emu, EXIT_FAILURE);
    cpu->t_cycles += emu->prefix_timing_table[op]; // 16;
}
void prefixed_SET_0_A(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regA |= 0x01;
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_SET_1_B(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regB |= 0x02;
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_SET_1_C(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regC |= 0x02;
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_SET_1_D(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regD |= 0x02;
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_SET_1_E(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regE |= 0x02;
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_SET_1_H(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regH |= 0x02;
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_SET_1_L(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regL |= 0x02;
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_SET_1_derefHL(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint8_t data;
    if(0 != read_memory(emu, ((cpu->regH << 8) | cpu->regL), &data))
        destroy_emulator(emu, EXIT_FAILURE);  
    
    data |= 0x02;
    
    if(0 != write_memory(emu, ((cpu->regH << 8) | cpu->regL), data))
        destroy_emulator(emu, EXIT_FAILURE);
    cpu->t_cycles += emu->prefix_timing_table[op]; // 16;
}
void prefixed_SET_1_A(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regA |= 0x02;
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_SET_2_B(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regB |= 0x04;
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_SET_2_C(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regC |= 0x04;
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_SET_2_D(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regD |= 0x04;
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_SET_2_E(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regE |= 0x04;
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_SET_2_H(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regH |= 0x04;
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_SET_2_L(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regL |= 0x04;
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_SET_2_derefHL(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint8_t data;
    if(0 != read_memory(emu, ((cpu->regH << 8) | cpu->regL), &data))
        destroy_emulator(emu, EXIT_FAILURE);  
    
    data |= 0x04;
    
    if(0 != write_memory(emu, ((cpu->regH << 8) | cpu->regL), data))
        destroy_emulator(emu, EXIT_FAILURE);
    cpu->t_cycles += emu->prefix_timing_table[op]; // 16;
}
void prefixed_SET_2_A(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regA |= 0x04;
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_SET_3_B(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regB |= 0x08;
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_SET_3_C(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regC |= 0x08;
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_SET_3_D(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regD |= 0x08;
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_SET_3_E(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regE |= 0x08;
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_SET_3_H(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regH |= 0x08;
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_SET_3_L(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regL |= 0x08;
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_SET_3_derefHL(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint8_t data;
    if(0 != read_memory(emu, ((cpu->regH << 8) | cpu->regL), &data))
        destroy_emulator(emu, EXIT_FAILURE);  
    
    data |= 0x08;
    
    if(0 != write_memory(emu, ((cpu->regH << 8) | cpu->regL), data))
        destroy_emulator(emu, EXIT_FAILURE);
    cpu->t_cycles += emu->prefix_timing_table[op]; // 16;
}
void prefixed_SET_3_A(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regA |= 0x08;
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_SET_4_B(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regB |= 0x10;
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_SET_4_C(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regC |= 0x10;
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_SET_4_D(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regD |= 0x10;
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_SET_4_E(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regE |= 0x10;
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_SET_4_H(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regH |= 0x10;
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_SET_4_L(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regL |= 0x10;
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_SET_4_derefHL(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint8_t data;
    if(0 != read_memory(emu, ((cpu->regH << 8) | cpu->regL), &data))
        destroy_emulator(emu, EXIT_FAILURE);  
    
    data |= 0x10;
    
    if(0 != write_memory(emu, ((cpu->regH << 8) | cpu->regL), data))
        destroy_emulator(emu, EXIT_FAILURE);
    cpu->t_cycles += emu->prefix_timing_table[op]; // 16;
}
void prefixed_SET_4_A(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regA |= 0x10;
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_SET_5_B(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regB |= 0x20;
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_SET_5_C(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regC |= 0x20;
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_SET_5_D(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regD |= 0x20;
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_SET_5_E(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regE |= 0x20;
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_SET_5_H(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regH |= 0x20;
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_SET_5_L(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regL |= 0x20;
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_SET_5_derefHL(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint8_t data;
    if(0 != read_memory(emu, ((cpu->regH << 8) | cpu->regL), &data))
        destroy_emulator(emu, EXIT_FAILURE);  
    
    data |= 0x20;
    
    if(0 != write_memory(emu, ((cpu->regH << 8) | cpu->regL), data))
        destroy_emulator(emu, EXIT_FAILURE);
    cpu->t_cycles += emu->prefix_timing_table[op]; // 16;
}
void prefixed_SET_5_A(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regA |= 0x20;
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_SET_6_B(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regB |= 0x40;
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_SET_6_C(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regC |= 0x40;
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_SET_6_D(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regD |= 0x40;
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_SET_6_E(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regE |= 0x40;
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_SET_6_H(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regH |= 0x40;
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_SET_6_L(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regL |= 0x40;
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_SET_6_derefHL(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint8_t data;
    if(0 != read_memory(emu, ((cpu->regH << 8) | cpu->regL), &data))
        destroy_emulator(emu, EXIT_FAILURE);  
    
    data |= 0x40;
    
    if(0 != write_memory(emu, ((cpu->regH << 8) | cpu->regL), data))
        destroy_emulator(emu, EXIT_FAILURE);
    cpu->t_cycles += emu->prefix_timing_table[op]; // 16;
}
void prefixed_SET_6_A(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regA |= 0x40;
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_SET_7_B(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regB |= 0x80;
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_SET_7_C(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regC |= 0x80;
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_SET_7_D(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regD |= 0x80;
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_SET_7_E(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regE |= 0x80;
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_SET_7_H(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regH |= 0x80;
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_SET_7_L(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regL |= 0x80;
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}
void prefixed_SET_7_derefHL(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint8_t data;
    if(0 != read_memory(emu, ((cpu->regH << 8) | cpu->regL), &data))
        destroy_emulator(emu, EXIT_FAILURE);
    data |= 0x80;
    if(0 != write_memory(emu, ((cpu->regH << 8) | cpu->regL), data))
        destroy_emulator(emu, EXIT_FAILURE);
    cpu->t_cycles += emu->prefix_timing_table[op];
}
void prefixed_SET_7_A(void *arg, uint8_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regA |= 0x80;
    cpu->t_cycles += emu->prefix_timing_table[op]; // 8;
}

