#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>
#include <SDL.h>

#include "cpu.h"
#include "emulator.h"
#include "opcodes.h"

void flag_assign(bool cond, uint8_t *flag, uint8_t mask)
{
    *flag = cond ? mask | *flag : ~mask & *flag;
}

void opcode_unimplemented(void *arg, uint32_t op)
{
    s_emu *emu = arg;
    fprintf(stderr, ANSI_COLOR_RED "WARNING: instruction %s (0x%06X) unimplemented!\n" ANSI_COLOR_RESET, 
            emu->mnemonic_index[(op & 0x00ff0000) >> 16], op);
    SDL_Delay(2000);
    destroy_emulator(emu, EXIT_FAILURE);
}

void opcode_non_existant(void *arg, uint32_t op)
{
    s_emu *emu = arg;
    fprintf(stderr, ANSI_COLOR_RED "ERROR: instuction 0x%02X doesn't exist!\n" ANSI_COLOR_RESET, (op & 0xFF0000) >> 16);
    destroy_emulator(emu, EXIT_FAILURE);
}

void NOP(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    emu->cpu.t_cycles += 4;
}

void LD_BC_d16(void *arg, uint32_t op)
{
    s_emu *emu = arg;
    emu->cpu.regC = (op & 0x0000ff00) >> 8;
    emu->cpu.regB = (op & 0x000000ff);
    emu->cpu.t_cycles += 12;
}

void LD_derefBC_A(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    if(0 != write_memory(emu, (cpu->regB << 8) + cpu->regC, cpu->regA))
        destroy_emulator(emu, EXIT_FAILURE);
    cpu->t_cycles += 8;
}
void INC_BC(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    uint16_t BC = (cpu->regB << 8) + cpu->regC;
    BC++;
    cpu->regB = (BC & 0xff00) >> 8;
    cpu->regC = BC & 0x00ff;
    cpu->t_cycles += 8;
}

void INC_B(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint8_t b4bit = cpu->regB & 0x0f;
    b4bit++;
    flag_assign(b4bit > 0x0f, &cpu->regF, HALF_CARRY_FMASK);
    
    cpu->regB++;
    flag_assign(cpu->regB == 0, &cpu->regF, ZERO_FMASK);
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK);
    
    cpu->t_cycles += 4;   
}

void DEC_B(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint8_t B4bit = cpu->regB & 0x0F;
    flag_assign(1 > B4bit, &cpu->regF, HALF_CARRY_FMASK);
    
    cpu->regB--;
    flag_assign(cpu->regB == 0, &cpu->regF, ZERO_FMASK);
    flag_assign(true, &cpu->regF, NEGATIVE_FMASK);
    
    cpu->t_cycles += 4;
}
void LD_B_d8(void *arg, uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regB = (op & 0x0000ff00) >> 8;
    cpu->t_cycles += 8;
}

void RLCA(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint16_t newA = cpu->regA << 1;
    flag_assign(newA > 0xff, &cpu->regF, CARRY_FMASK);
    flag_assign(false, &cpu->regF, ZERO_FMASK | NEGATIVE_FMASK | HALF_CARRY_FMASK);
    cpu->regA = (newA & 0x00FF) + ((newA & 0x0100) >> 8);
    
    cpu->t_cycles += 4;
}
//void LD_derefa16_SP(void *arg, uint32_t op)
//void ADD_HL_BC(void *arg, UNUSED uint32_t op)
//void LD_A_derefBC(void *arg, UNUSED uint32_t op)
void DEC_BC(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    uint16_t BC = (cpu->regB << 8) + cpu->regC;
    BC--;
    cpu->regB = (BC & 0xff00) >> 8;
    cpu->regC = BC & 0x00ff;
    cpu->t_cycles += 8;
}

void INC_C(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint8_t c4bit = cpu->regC & 0x0f;
    c4bit++;
    flag_assign(c4bit > 0x0f, &cpu->regF, HALF_CARRY_FMASK);
    
    cpu->regC++;
    flag_assign(cpu->regC == 0, &cpu->regF, ZERO_FMASK);
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK);
    
    cpu->t_cycles += 4;    
}

void DEC_C(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint8_t C4bit = cpu->regC & 0x0F;
    flag_assign(1 > C4bit, &cpu->regF, HALF_CARRY_FMASK);
    
    cpu->regC--;
    flag_assign(cpu->regC == 0, &cpu->regF, ZERO_FMASK);
    flag_assign(true, &cpu->regF, NEGATIVE_FMASK);
    
    cpu->t_cycles += 4;
}

void LD_C_d8(void *arg, uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regC = (op & 0x0000ff00) >> 8;
    cpu->t_cycles += 8;
}

//void RRCA(void *arg, UNUSED uint32_t op)
//void STOP_0(void *arg, UNUSED uint32_t op)
void LD_DE_d16(void *arg, uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regE = (op & 0x0000ff00) >> 8;
    cpu->regD = (op & 0x000000ff);
    
    cpu->t_cycles += 12;
}
void LD_derefDE_A(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    if(0 != write_memory(emu, (cpu->regD << 8) + cpu->regE, cpu->regA))
        destroy_emulator(emu, EXIT_FAILURE);
    cpu->t_cycles += 8;
}
void INC_DE(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    uint16_t DE = (cpu->regD << 8) + cpu->regE;
    DE++;
    cpu->regD = (DE & 0xff00) >> 8;
    cpu->regE = DE & 0x00ff;
    cpu->t_cycles += 8;
}
void INC_D(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint8_t D4bit = cpu->regD & 0x0f;
    D4bit++;
    flag_assign(D4bit > 0x0f, &cpu->regF, HALF_CARRY_FMASK);
    
    cpu->regD++;
    flag_assign(cpu->regD == 0, &cpu->regF, ZERO_FMASK);
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK);
    
    cpu->t_cycles += 4; 
}
void DEC_D(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint8_t D4bit = cpu->regD & 0x0F;
    flag_assign(1 > D4bit, &cpu->regF, HALF_CARRY_FMASK);
    
    cpu->regD--;
    flag_assign(cpu->regD == 0, &cpu->regF, ZERO_FMASK);
    flag_assign(true, &cpu->regF, NEGATIVE_FMASK);
    
    cpu->t_cycles += 4;
}

void LD_D_d8(void *arg, uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regD = (op & 0x0000ff00) >> 8;
    cpu->t_cycles += 8;
}

void RLA(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint8_t newA = cpu->regA;
    newA <<= 1;
    flag_assign(cpu->regF & CARRY_FMASK, &newA, 0x01);
    flag_assign(cpu->regA & 0x80, &cpu->regF, CARRY_FMASK);
    flag_assign(false, &cpu->regF, ZERO_FMASK | NEGATIVE_FMASK | HALF_CARRY_FMASK);
    cpu->regA = newA;
    cpu->t_cycles += 4;
}

void JR_r8(void *arg, uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    int8_t r8 = (op & 0x0000ff00) >> 8;
    cpu->pc += r8;
    cpu->t_cycles += 12;
}

void ADD_HL_DE(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint16_t HL = (cpu->regH << 8) + cpu->regL;
    uint16_t DE = (cpu->regD << 8) + cpu->regE;
    
    uint16_t HL12bits = HL & 0x0FFF;
    uint16_t DE12bits = DE & 0x0FFF;
    
    flag_assign(HL12bits + DE12bits > 0x0FFF, &cpu->regF, HALF_CARRY_FMASK);
    flag_assign(HL > UINT16_MAX - DE, &cpu->regF, CARRY_FMASK);
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK);
    
    HL += DE;
    
    cpu->regH = (HL & 0xFF00) >> 8;
    cpu->regL = HL & 0x00FF;
    
    cpu->t_cycles += 8;
}

void LD_A_derefDE(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint8_t data;
    if(0 != read_memory(emu, (cpu->regD << 8) + cpu->regE, &data))
        destroy_emulator(emu, EXIT_FAILURE);
    cpu->regA = data;
    cpu->t_cycles += 8;
}
//void DEC_DE(void *arg, UNUSED uint32_t op)
void INC_E(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint8_t E4bit = cpu->regE & 0x0f;
    E4bit++;
    flag_assign(E4bit > 0x0f, &cpu->regF, HALF_CARRY_FMASK);
    
    cpu->regE++;
    flag_assign(cpu->regE == 0, &cpu->regF, ZERO_FMASK);
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK);
    
    cpu->t_cycles += 4; 
}
void DEC_E(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint8_t E4bit = cpu->regE & 0x0F;
    flag_assign(1 > E4bit, &cpu->regF, HALF_CARRY_FMASK);
    
    cpu->regE--;
    flag_assign(cpu->regE == 0, &cpu->regF, ZERO_FMASK);
    flag_assign(true, &cpu->regF, NEGATIVE_FMASK);
    
    cpu->t_cycles += 4;
}

void LD_E_d8(void *arg, uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regE = (op & 0x0000ff00) >> 8;
    cpu->t_cycles += 8;
}
void RRA(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint8_t newA = cpu->regA;
    newA >>= 1;
    //copy previous carry flag to bit 7
    flag_assign((cpu->regF & CARRY_FMASK), &newA, 0x80);
    //copy previous bit 0 to carry flag
    flag_assign((cpu->regA & 0x01), &cpu->regF, CARRY_FMASK);
    flag_assign(false, &cpu->regF, ZERO_FMASK | NEGATIVE_FMASK | HALF_CARRY_FMASK);
    cpu->regA = newA;
    
    cpu->t_cycles += 4;
}
void JR_NZ_r8(void *arg, uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    if(!(cpu->regF & ZERO_FMASK)) //if Z flag is 0
    {
        int8_t value = (op & 0x0000ff00) >> 8;
        cpu->pc += value;
        emu->cpu.t_cycles += 12;
    }
    else
        emu->cpu.t_cycles += 8;
}

void LD_HL_d16(void *arg, uint32_t op)
{
    s_emu *emu = arg;
    emu->cpu.regL = (op & 0x0000ff00) >> 8;
    emu->cpu.regH = (op & 0x000000ff);
    emu->cpu.t_cycles += 12;
}

void LD_derefHLplus_A(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    if(0 != write_memory(emu, (cpu->regH << 8) + cpu->regL, cpu->regA))
        destroy_emulator(emu, EXIT_FAILURE);
    uint16_t HL = (cpu->regH << 8) + cpu->regL;
    HL++;
    cpu->regH = (HL & 0xff00) >> 8;
    cpu->regL = HL & 0x00ff;
    cpu->t_cycles += 8;    
}
void INC_HL(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    uint16_t HL = (cpu->regH << 8) + cpu->regL;
    HL++;
    cpu->regH = (HL & 0xff00) >> 8;
    cpu->regL = HL & 0x00ff;
    cpu->t_cycles += 8;
}

void INC_H(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint8_t h4bit = cpu->regH & 0x0f;
    h4bit++;
    flag_assign(h4bit > 0x0f, &cpu->regF, HALF_CARRY_FMASK);
    
    cpu->regH++;
    flag_assign(cpu->regH == 0, &cpu->regF, ZERO_FMASK);
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK);
    
    cpu->t_cycles += 4;    
}
void DEC_H(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint8_t H4bit = cpu->regH & 0x0F;
    flag_assign(1 > H4bit, &cpu->regF, HALF_CARRY_FMASK);
    
    cpu->regH--;
    flag_assign(cpu->regH == 0, &cpu->regF, ZERO_FMASK);
    flag_assign(true, &cpu->regF, NEGATIVE_FMASK);
    
    cpu->t_cycles += 4;
}
void LD_H_d8(void *arg, uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regH = (op & 0x0000ff00) >> 8;
    cpu->t_cycles += 8;
}
void DAA(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    if(!(cpu->regF & NEGATIVE_FMASK))
    {
        if((cpu->regF & CARRY_FMASK) || cpu->regA > 0x99)
        {
            cpu->regA += 0x60;
            flag_assign(true, &cpu->regF, CARRY_FMASK);
        }
        if((cpu->regF & HALF_CARRY_FMASK) || (cpu->regA & 0x0F) > 0x09)
            cpu->regA += 0x6;
    }
    else
    {
        if(cpu->regF & CARRY_FMASK)
            cpu->regA -= 0x60;
        if(cpu->regF & HALF_CARRY_FMASK)
            cpu->regA -= 0x6;
    }
    
    flag_assign(cpu->regA == 0, &cpu->regF, ZERO_FMASK);
    flag_assign(false, &cpu->regF, HALF_CARRY_FMASK);
    
    cpu->t_cycles += 4;
}
void JR_Z_r8(void *arg, uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    if(cpu->regF & ZERO_FMASK)
    {
        int8_t r8 = (op & 0x0000ff00) >> 8;
        cpu->pc += r8;
        cpu->t_cycles += 12;
    }
    else
    {
        cpu->t_cycles += 8;
    }
}
void ADD_HL_HL(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint16_t HL = (cpu->regH << 8) + cpu->regL;
    
    uint16_t HL12bits = HL & 0x0FFF;
    
    flag_assign(HL12bits * 2 > 0x0FFF, &cpu->regF, HALF_CARRY_FMASK);
    flag_assign(HL > UINT16_MAX - HL, &cpu->regF, CARRY_FMASK);
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK);
    
    HL *= 2;
    
    cpu->regH = (HL & 0xFF00) >> 8;
    cpu->regL = HL & 0x00FF;
    
    cpu->t_cycles += 8;
}
void LD_A_derefHLplus(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint8_t data;
    uint16_t HL = (cpu->regH << 8) + cpu->regL;
    if(0 != read_memory(emu, HL, &data))
        destroy_emulator(emu, EXIT_FAILURE);
    cpu->regA = data;
    
    HL++;
    cpu->regH = (HL & 0xFF00) >> 8;
    cpu->regL = HL & 0x00FF;
    
    cpu->t_cycles += 8;
}

void DEC_HL(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    uint16_t HL = (cpu->regH << 8) + cpu->regL;
    HL--;
    cpu->regH = (HL & 0xff00) >> 8;
    cpu->regL = HL & 0x00ff;
    cpu->t_cycles += 8;
    
}
void INC_L(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint8_t L4bit = cpu->regL & 0x0f;
    L4bit++;
    flag_assign(L4bit > 0x0f, &cpu->regF, HALF_CARRY_FMASK);
    
    cpu->regL++;
    flag_assign(cpu->regL == 0, &cpu->regF, ZERO_FMASK);
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK);
    
    cpu->t_cycles += 4; 
}

void DEC_L(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint8_t L4bit = cpu->regL & 0x0F;
    flag_assign(1 > L4bit, &cpu->regF, HALF_CARRY_FMASK);
    
    cpu->regL--;
    flag_assign(cpu->regL == 0, &cpu->regF, ZERO_FMASK);
    flag_assign(true, &cpu->regF, NEGATIVE_FMASK);
    
    cpu->t_cycles += 4;
}

void LD_L_d8(void *arg, uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regL = (op & 0x0000ff00) >> 8;
    cpu->t_cycles += 8;
}

void CPL(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regA = ~cpu->regA;
    
    flag_assign(true, &cpu->regF, NEGATIVE_FMASK | HALF_CARRY_FMASK);
    flag_assign(false, &cpu->regF, ZERO_FMASK | CARRY_FMASK);
    
    cpu->t_cycles += 4;
}
void JR_NC_r8(void *arg, uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    if(!(cpu->regF & CARRY_FMASK)) //if Z flag is 0
    {
        int8_t value = (op & 0x0000ff00) >> 8;
        cpu->pc += value;
        emu->cpu.t_cycles += 12;
    }
    else
        emu->cpu.t_cycles += 8;
}
void LD_SP_d16(void *arg, uint32_t op)
{
    s_emu *emu = arg;
    emu->cpu.sp = (op &  0x0000ff00) >> 8;
    emu->cpu.sp += (op & 0x000000ff) << 8;
    emu->cpu.t_cycles += 12;
}

void LD_derefHLminus_A(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    if(0 != write_memory(emu, (cpu->regH << 8) + cpu->regL, cpu->regA))
        destroy_emulator(emu, EXIT_FAILURE);
    uint16_t HL = (cpu->regH << 8) + cpu->regL;
    HL--;
    cpu->regH = (HL & 0xff00) >> 8;
    cpu->regL = HL & 0x00ff;
    cpu->t_cycles += 8;    
}

//void INC_SP(void *arg, UNUSED uint32_t op)
void INC_derefHL(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint8_t data;
    if(0 != read_memory(emu, (cpu->regH << 8) + cpu->regL, &data))
        destroy_emulator(emu, EXIT_FAILURE);
    uint8_t data4bit = data & 0x0F;
    flag_assign(data4bit + 1 > 0x0F, &cpu->regF, HALF_CARRY_FMASK);
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK);
    data++;
    flag_assign(data == 0, &cpu->regF, ZERO_FMASK);
    if(0 != write_memory(emu, (cpu->regH << 8) + cpu->regL, data))
        destroy_emulator(emu, EXIT_FAILURE);
        
    cpu->t_cycles += 12;
    
}
void DEC_derefHL(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint8_t data;
    if(0 != read_memory(emu, (cpu->regH << 8) + cpu->regL, &data))
        destroy_emulator(emu, EXIT_FAILURE);
    uint8_t data4bit = data & 0x0F;
    flag_assign(1 > data4bit, &cpu->regF, HALF_CARRY_FMASK);
    flag_assign(true, &cpu->regF, NEGATIVE_FMASK);
    data--;
    flag_assign(data == 0, &cpu->regF, ZERO_FMASK);
    if(0 != write_memory(emu, (cpu->regH << 8) + cpu->regL, data))
        destroy_emulator(emu, EXIT_FAILURE);
        
    cpu->t_cycles += 12;
    
}
void LD_derefHL_d8(void *arg, uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    if(0 != write_memory(emu, (cpu->regH << 8) + cpu->regL, (op & 0x0000FF00) >> 8))
        destroy_emulator(emu, EXIT_FAILURE);
    cpu->t_cycles += 12;
}
//void SCF(void *arg, UNUSED uint32_t op)
void JR_C_r8(void *arg, uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    if(cpu->regF & CARRY_FMASK) //if Z flag is 0
    {
        int8_t value = (op & 0x0000ff00) >> 8;
        cpu->pc += value;
        emu->cpu.t_cycles += 12;
    }
    else
        emu->cpu.t_cycles += 8;
}
//void ADD_HL_SP(void *arg, UNUSED uint32_t op)
//void LD_A_derefHLminus(void *arg, UNUSED uint32_t op)
//void DEC_SP(void *arg, UNUSED uint32_t op)
void INC_A(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint8_t A4bit = cpu->regA & 0x0f;
    A4bit++;
    flag_assign(A4bit > 0x0f, &cpu->regF, HALF_CARRY_FMASK);
    
    cpu->regA++;
    flag_assign(cpu->regA == 0, &cpu->regF, ZERO_FMASK);
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK);
    
    cpu->t_cycles += 4; 
}
void DEC_A(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint8_t A4bit = cpu->regA & 0x0F;
    flag_assign(1 > A4bit, &cpu->regF, HALF_CARRY_FMASK);
    
    cpu->regA--;
    flag_assign(cpu->regA == 0, &cpu->regF, ZERO_FMASK);
    flag_assign(true, &cpu->regF, NEGATIVE_FMASK);
    
    cpu->t_cycles += 4;
}

void LD_A_d8(void *arg, uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regA = (op & 0x0000ff00) >> 8;
    cpu->t_cycles += 8;
}

//void CCF(void *arg, UNUSED uint32_t op)
void LD_B_B(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regB = cpu->regB;
    cpu->t_cycles += 4;
}
void LD_B_C(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regB = cpu->regC;
    cpu->t_cycles += 4;
}
void LD_B_D(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regB = cpu->regD;
    cpu->t_cycles += 4;
}
void LD_B_E(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regB = cpu->regE;
    cpu->t_cycles += 4;
}
void LD_B_H(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regB = cpu->regH;
    cpu->t_cycles += 4;
}
void LD_B_L(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regB = cpu->regL;
    cpu->t_cycles += 4;
}
void LD_B_derefHL(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    if(0 != read_memory(emu, (cpu->regH << 8) + cpu->regL, &cpu->regB))
        destroy_emulator(emu, EXIT_FAILURE);

    cpu->t_cycles += 8;
}
void LD_B_A(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regB = cpu->regA;
    cpu->t_cycles += 4;
}
void LD_C_B(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regC = cpu->regB;
    cpu->t_cycles += 4;
}
void LD_C_C(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regC = cpu->regC;
    cpu->t_cycles += 4;
}
void LD_C_D(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regC = cpu->regD;
    cpu->t_cycles += 4;
}
void LD_C_E(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regC = cpu->regE;
    cpu->t_cycles += 4;
}
void LD_C_H(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regC = cpu->regH;
    cpu->t_cycles += 4;
}
void LD_C_L(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regC = cpu->regL;
    cpu->t_cycles += 4;
}
void LD_C_derefHL(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    if(0 != read_memory(emu, (cpu->regH << 8) + cpu->regL, &cpu->regC))
        destroy_emulator(emu, EXIT_FAILURE);

    cpu->t_cycles += 8;
}
void LD_C_A(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regC = cpu->regA;
    cpu->t_cycles += 4;
}
void LD_D_B(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regD = cpu->regB;
    cpu->t_cycles += 4;
}
void LD_D_C(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regD = cpu->regC;
    cpu->t_cycles += 4;
}
void LD_D_D(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regD = cpu->regD;
    cpu->t_cycles += 4;
}
void LD_D_E(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regD = cpu->regE;
    cpu->t_cycles += 4;
}
void LD_D_H(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regD = cpu->regH;
    cpu->t_cycles += 4;
}
void LD_D_L(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regD = cpu->regL;
    cpu->t_cycles += 4;
}
void LD_D_derefHL(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    if(0 != read_memory(emu, (cpu->regH << 8) + cpu->regL, &cpu->regD))
        destroy_emulator(emu, EXIT_FAILURE);

    cpu->t_cycles += 8;
}
void LD_D_A(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regD = cpu->regA;
    cpu->t_cycles += 4;
}
void LD_E_B(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regE = cpu->regB;
    cpu->t_cycles += 4;
}
void LD_E_C(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regE = cpu->regC;
    cpu->t_cycles += 4;
}
void LD_E_D(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regE = cpu->regD;
    cpu->t_cycles += 4;
}
void LD_E_E(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regE = cpu->regE;
    cpu->t_cycles += 4;
}
void LD_E_H(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regE = cpu->regH;
    cpu->t_cycles += 4;
}
void LD_E_L(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regE = cpu->regL;
    cpu->t_cycles += 4;
}
void LD_E_derefHL(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    if(0 != read_memory(emu, (cpu->regH << 8) + cpu->regL, &cpu->regE))
        destroy_emulator(emu, EXIT_FAILURE);

    cpu->t_cycles += 8;
}
void LD_E_A(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regE = cpu->regA;
    cpu->t_cycles += 4;
}
void LD_H_B(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regH = cpu->regB;
    cpu->t_cycles += 4;
}
void LD_H_C(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regH = cpu->regC;
    cpu->t_cycles += 4;
}
void LD_H_D(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regH = cpu->regD;
    cpu->t_cycles += 4;
}
void LD_H_E(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regH = cpu->regE;
    cpu->t_cycles += 4;
}
void LD_H_H(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regH = cpu->regH;
    cpu->t_cycles += 4;
}
void LD_H_L(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regH = cpu->regL;
    cpu->t_cycles += 4;
}
void LD_H_derefHL(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    if(0 != read_memory(emu, (cpu->regH << 8) + cpu->regL, &cpu->regH))
        destroy_emulator(emu, EXIT_FAILURE);

    cpu->t_cycles += 8;
}
void LD_H_A(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regH = cpu->regA;
    cpu->t_cycles += 4;
}
void LD_L_B(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regL = cpu->regB;
    cpu->t_cycles += 4;
}
void LD_L_C(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regL = cpu->regC;
    cpu->t_cycles += 4;
}
void LD_L_D(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regL = cpu->regD;
    cpu->t_cycles += 4;
}
void LD_L_E(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regL = cpu->regE;
    cpu->t_cycles += 4;
}
void LD_L_H(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regL = cpu->regH;
    cpu->t_cycles += 4;
}
void LD_L_L(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regL = cpu->regL;
    cpu->t_cycles += 4;
}
void LD_L_derefHL(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    if(0 != read_memory(emu, (cpu->regH << 8) + cpu->regL, &cpu->regL))
        destroy_emulator(emu, EXIT_FAILURE);

    cpu->t_cycles += 8;
}
void LD_L_A(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regL = cpu->regA;
    cpu->t_cycles += 4;
}
void LD_derefHL_B(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    if(0 != write_memory(emu, (cpu->regH << 8) + cpu->regL, cpu->regB))
        destroy_emulator(emu, EXIT_FAILURE);
    cpu->t_cycles += 8;
}
void LD_derefHL_C(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    if(0 != write_memory(emu, (cpu->regH << 8) + cpu->regL, cpu->regC))
        destroy_emulator(emu, EXIT_FAILURE);
    cpu->t_cycles += 8;
}
void LD_derefHL_D(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    if(0 != write_memory(emu, (cpu->regH << 8) + cpu->regL, cpu->regD))
        destroy_emulator(emu, EXIT_FAILURE);
    cpu->t_cycles += 8;
}
void LD_derefHL_E(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    if(0 != write_memory(emu, (cpu->regH << 8) + cpu->regL, cpu->regE))
        destroy_emulator(emu, EXIT_FAILURE);
    cpu->t_cycles += 8;
}
void LD_derefHL_H(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    if(0 != write_memory(emu, (cpu->regH << 8) + cpu->regL, cpu->regH))
        destroy_emulator(emu, EXIT_FAILURE);
    cpu->t_cycles += 8;
}
void LD_derefHL_L(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    if(0 != write_memory(emu, (cpu->regH << 8) + cpu->regL, cpu->regL))
        destroy_emulator(emu, EXIT_FAILURE);
    cpu->t_cycles += 8;
}
//void HALT(void *arg, UNUSED uint32_t op)
void LD_derefHL_A(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    if(0 != write_memory(emu, (cpu->regH << 8) + cpu->regL, cpu->regA))
        destroy_emulator(emu, EXIT_FAILURE);
    cpu->t_cycles += 8;
}

void LD_A_B(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regA = cpu->regB;
    cpu->t_cycles += 4;
}

void LD_A_C(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    cpu->regA = cpu->regC;
    cpu->t_cycles += 4;   
}
void LD_A_D(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    cpu->regA = cpu->regD;
    cpu->t_cycles += 4;   
}
void LD_A_E(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    cpu->regA = cpu->regE;
    cpu->t_cycles += 4;   
}
void LD_A_H(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    cpu->regA = cpu->regH;
    cpu->t_cycles += 4;    
}

void LD_A_L(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regA = cpu->regL;
    cpu->t_cycles += 4;
}
void LD_A_derefHL(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;

    if(0 != read_memory(emu, (cpu->regH << 8) + cpu->regL, &cpu->regA))
        destroy_emulator(emu, EXIT_FAILURE);
    
    cpu->t_cycles += 8;
}
void LD_A_A(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    cpu->regA = cpu->regA;
    cpu->t_cycles += 4;   
}
void ADD_A_B(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint16_t newA = cpu->regA;
    
    uint8_t A4bit = cpu->regA & 0x0F;
    uint8_t B4bit = cpu->regB & 0x0F;
    
    flag_assign(A4bit + B4bit > 0x0F, &cpu->regF, HALF_CARRY_FMASK);
    
    newA += cpu->regB;
    flag_assign(newA > 0xFF, &cpu->regF, CARRY_FMASK);
    
    cpu->regA += cpu->regB;
    flag_assign(cpu->regA == 0, &cpu->regF, ZERO_FMASK);
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK);
    
    cpu->t_cycles += 4;
}
void ADD_A_C(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint16_t newA = cpu->regA;
    
    uint8_t A4bit = cpu->regA & 0x0F;
    uint8_t C4bit = cpu->regC & 0x0F;
    
    flag_assign(A4bit + C4bit > 0x0F, &cpu->regF, HALF_CARRY_FMASK);
    
    newA += cpu->regC;
    flag_assign(newA > 0xFF, &cpu->regF, CARRY_FMASK);
    
    cpu->regA += cpu->regC;
    flag_assign(cpu->regA == 0, &cpu->regF, ZERO_FMASK);
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK);
    
    cpu->t_cycles += 4;
}
void ADD_A_D(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint16_t newA = cpu->regA;
    
    uint8_t A4bit = cpu->regA & 0x0F;
    uint8_t D4bit = cpu->regD & 0x0F;
    
    flag_assign(A4bit + D4bit > 0x0F, &cpu->regF, HALF_CARRY_FMASK);
    
    newA += cpu->regD;
    flag_assign(newA > 0xFF, &cpu->regF, CARRY_FMASK);
    
    cpu->regA += cpu->regD;
    flag_assign(cpu->regA == 0, &cpu->regF, ZERO_FMASK);
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK);
    
    cpu->t_cycles += 4;
}
void ADD_A_E(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint16_t newA = cpu->regA;
    
    uint8_t A4bit = cpu->regA & 0x0F;
    uint8_t E4bit = cpu->regE & 0x0F;
    
    flag_assign(A4bit + E4bit > 0x0F, &cpu->regF, HALF_CARRY_FMASK);
    
    newA += cpu->regE;
    flag_assign(newA > 0xFF, &cpu->regF, CARRY_FMASK);
    
    cpu->regA += cpu->regE;
    flag_assign(cpu->regA == 0, &cpu->regF, ZERO_FMASK);
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK);
    
    cpu->t_cycles += 4;
}
void ADD_A_H(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint16_t newA = cpu->regA;
    
    uint8_t A4bit = cpu->regA & 0x0F;
    uint8_t H4bit = cpu->regH & 0x0F;
    
    flag_assign(A4bit + H4bit > 0x0F, &cpu->regF, HALF_CARRY_FMASK);
    
    newA += cpu->regH;
    flag_assign(newA > 0xFF, &cpu->regF, CARRY_FMASK);
    
    cpu->regA += cpu->regH;
    flag_assign(cpu->regA == 0, &cpu->regF, ZERO_FMASK);
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK);
    
    cpu->t_cycles += 4;
}
void ADD_A_L(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint16_t newA = cpu->regA;
    
    uint8_t A4bit = cpu->regA & 0x0F;
    uint8_t L4bit = cpu->regL & 0x0F;
    
    flag_assign(A4bit + L4bit > 0x0F, &cpu->regF, HALF_CARRY_FMASK);
    
    newA += cpu->regL;
    flag_assign(newA > 0xFF, &cpu->regF, CARRY_FMASK);
    
    cpu->regA += cpu->regL;
    flag_assign(cpu->regA == 0, &cpu->regF, ZERO_FMASK);
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK);

    
    cpu->t_cycles += 4;
}
void ADD_A_derefHL(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint16_t newA = cpu->regA;
    uint8_t data;
    if(0 != read_memory(emu, (cpu->regH << 8) + cpu->regL, &data))
        destroy_emulator(emu, EXIT_FAILURE);
        
    uint8_t A4bit = newA & 0x0F;
    uint8_t data4bit = data & 0x0F;
    flag_assign(A4bit + data4bit > 0x0F, &cpu->regF, HALF_CARRY_FMASK);
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK);
    newA += data;
    flag_assign(newA > 0xFF, &cpu->regF, CARRY_FMASK);
    cpu->regA += data;
    flag_assign(cpu->regA == 0, &cpu->regF, ZERO_FMASK);
    
    cpu->t_cycles += 8;
}
void ADD_A_A(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint16_t newA = cpu->regA;
    
    uint8_t A4bit = cpu->regA & 0x0F;
    
    flag_assign(A4bit * 2 > 0x0F, &cpu->regF, HALF_CARRY_FMASK);
    
    newA *= 2;
    flag_assign(newA > 0xFF, &cpu->regF, CARRY_FMASK);
    
    cpu->regA *= 2;
    flag_assign(cpu->regA == 0, &cpu->regF, ZERO_FMASK);
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK);
    
    cpu->t_cycles += 4;
}
void ADC_A_B(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint16_t newA = cpu->regA;
    
    uint8_t A4bit = cpu->regA & 0x0F;
    uint8_t B4bit = cpu->regB & 0x0F;
    bool carry = cpu->regF & CARRY_FMASK;
    
    flag_assign(A4bit + B4bit + carry > 0x0F, &cpu->regF, HALF_CARRY_FMASK);
    
    newA += cpu->regB + carry;
    
    cpu->regA += cpu->regB + carry;
    flag_assign(newA > 0xFF, &cpu->regF, CARRY_FMASK);
    flag_assign(cpu->regA == 0, &cpu->regF, ZERO_FMASK);
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK);
    
    cpu->t_cycles += 4;
}
void ADC_A_C(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint16_t newA = cpu->regA;
    
    uint8_t A4bit = cpu->regA & 0x0F;
    uint8_t C4bit = cpu->regC & 0x0F;
    bool carry = cpu->regF & CARRY_FMASK;
    
    flag_assign(A4bit + C4bit + carry > 0x0F, &cpu->regF, HALF_CARRY_FMASK);
    
    newA += cpu->regC + carry;
    
    cpu->regA += cpu->regC + carry;
    flag_assign(newA > 0xFF, &cpu->regF, CARRY_FMASK);
    flag_assign(cpu->regA == 0, &cpu->regF, ZERO_FMASK);
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK);
    
    cpu->t_cycles += 4;
}
void ADC_A_D(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint16_t newA = cpu->regA;
    
    uint8_t A4bit = cpu->regA & 0x0F;
    uint8_t D4bit = cpu->regD & 0x0F;
    bool carry = cpu->regF & CARRY_FMASK;
    
    flag_assign(A4bit + D4bit + carry > 0x0F, &cpu->regF, HALF_CARRY_FMASK);
    
    newA += cpu->regD + carry;
    
    cpu->regA += cpu->regD + carry;
    flag_assign(newA > 0xFF, &cpu->regF, CARRY_FMASK);
    flag_assign(cpu->regA == 0, &cpu->regF, ZERO_FMASK);
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK);
    
    cpu->t_cycles += 4;
}
void ADC_A_E(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint16_t newA = cpu->regA;
    
    uint8_t A4bit = cpu->regA & 0x0F;
    uint8_t E4bit = cpu->regE & 0x0F;
    bool carry = cpu->regF & CARRY_FMASK;
    
    flag_assign(A4bit + E4bit + carry > 0x0F, &cpu->regF, HALF_CARRY_FMASK);
    
    newA += cpu->regE + carry;
    
    cpu->regA += cpu->regE + carry;
    flag_assign(newA > 0xFF, &cpu->regF, CARRY_FMASK);
    flag_assign(cpu->regA == 0, &cpu->regF, ZERO_FMASK);
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK);
    
    cpu->t_cycles += 4;
}
void ADC_A_H(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint16_t newA = cpu->regA;
    
    uint8_t A4bit = cpu->regA & 0x0F;
    uint8_t H4bit = cpu->regH & 0x0F;
    bool carry = cpu->regF & CARRY_FMASK;
    
    flag_assign(A4bit + H4bit + carry > 0x0F, &cpu->regF, HALF_CARRY_FMASK);
    
    newA += cpu->regH + carry;
    
    cpu->regA += cpu->regH + carry;
    flag_assign(newA > 0xFF, &cpu->regF, CARRY_FMASK);
    flag_assign(cpu->regA == 0, &cpu->regF, ZERO_FMASK);
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK);
    
    cpu->t_cycles += 4;
}
void ADC_A_L(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint16_t newA = cpu->regA;
    
    uint8_t A4bit = cpu->regA & 0x0F;
    uint8_t L4bit = cpu->regL & 0x0F;
    bool carry = cpu->regF & CARRY_FMASK;
    
    flag_assign(A4bit + L4bit + carry > 0x0F, &cpu->regF, HALF_CARRY_FMASK);
    
    newA += cpu->regL + carry;
    
    cpu->regA += cpu->regL + carry;
    flag_assign(newA > 0xFF, &cpu->regF, CARRY_FMASK);
    flag_assign(cpu->regA == 0, &cpu->regF, ZERO_FMASK);
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK);
    
    cpu->t_cycles += 4;
}
//void ADC_A_derefHL(void *arg, UNUSED uint32_t op)
void ADC_A_A(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint16_t newA = cpu->regA;
    
    uint8_t A4bit = cpu->regA & 0x0F;
    bool carry = cpu->regF & CARRY_FMASK;
    
    flag_assign(A4bit * 2 + carry > 0x0F, &cpu->regF, HALF_CARRY_FMASK);
    
    newA += cpu->regA + carry;
    
    cpu->regA += cpu->regA + carry;
    flag_assign(newA > 0xFF, &cpu->regF, CARRY_FMASK);
    flag_assign(cpu->regA == 0, &cpu->regF, ZERO_FMASK);
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK);
    
    cpu->t_cycles += 4;
}
void SUB_B(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint8_t A_4bit = cpu->regA & 0x0F;
    uint8_t B_4bit = cpu->regB & 0x0F;
    flag_assign(B_4bit > A_4bit, &cpu->regF, HALF_CARRY_FMASK);
    
    flag_assign(cpu->regB > cpu->regA, &cpu->regF, CARRY_FMASK);
    flag_assign(true, &cpu->regF, NEGATIVE_FMASK);
    cpu->regA -= cpu->regB;
    flag_assign(cpu->regA == 0, &cpu->regF, ZERO_FMASK);
    cpu->t_cycles += 4;    
}
void SUB_C(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint8_t A_4bit = cpu->regA & 0x0F;
    uint8_t C_4bit = cpu->regC & 0x0F;
    flag_assign(C_4bit > A_4bit, &cpu->regF, HALF_CARRY_FMASK);
    
    flag_assign(cpu->regC > cpu->regA, &cpu->regF, CARRY_FMASK);
    flag_assign(true, &cpu->regF, NEGATIVE_FMASK);
    cpu->regA -= cpu->regC;
    flag_assign(cpu->regA == 0, &cpu->regF, ZERO_FMASK);
    cpu->t_cycles += 4;    
}
void SUB_D(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint8_t A_4bit = cpu->regA & 0x0F;
    uint8_t D_4bit = cpu->regD & 0x0F;
    flag_assign(D_4bit > A_4bit, &cpu->regF, HALF_CARRY_FMASK);
    
    flag_assign(cpu->regD > cpu->regA, &cpu->regF, CARRY_FMASK);
    flag_assign(true, &cpu->regF, NEGATIVE_FMASK);
    cpu->regA -= cpu->regD;
    flag_assign(cpu->regA == 0, &cpu->regF, ZERO_FMASK);
    cpu->t_cycles += 4;    
}
void SUB_E(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint8_t A_4bit = cpu->regA & 0x0F;
    uint8_t E_4bit = cpu->regE & 0x0F;
    flag_assign(E_4bit > A_4bit, &cpu->regF, HALF_CARRY_FMASK);
    
    flag_assign(cpu->regE > cpu->regA, &cpu->regF, CARRY_FMASK);
    flag_assign(true, &cpu->regF, NEGATIVE_FMASK);
    cpu->regA -= cpu->regE;
    flag_assign(cpu->regA == 0, &cpu->regF, ZERO_FMASK);
    cpu->t_cycles += 4;    
}
void SUB_H(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint8_t A_4bit = cpu->regA & 0x0F;
    uint8_t H_4bit = cpu->regH & 0x0F;
    flag_assign(H_4bit > A_4bit, &cpu->regF, HALF_CARRY_FMASK);
    
    flag_assign(cpu->regH > cpu->regA, &cpu->regF, CARRY_FMASK);
    flag_assign(true, &cpu->regF, NEGATIVE_FMASK);
    cpu->regA -= cpu->regH;
    flag_assign(cpu->regA == 0, &cpu->regF, ZERO_FMASK);
    cpu->t_cycles += 4;    
}
void SUB_L(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint8_t A_4bit = cpu->regA & 0x0F;
    uint8_t L_4bit = cpu->regL & 0x0F;
    flag_assign(L_4bit > A_4bit, &cpu->regF, HALF_CARRY_FMASK);
    
    flag_assign(cpu->regL > cpu->regA, &cpu->regF, CARRY_FMASK);
    flag_assign(true, &cpu->regF, NEGATIVE_FMASK);
    cpu->regA -= cpu->regL;
    flag_assign(cpu->regA == 0, &cpu->regF, ZERO_FMASK);
    cpu->t_cycles += 4;    
}
//void SUB_derefHL(void *arg, UNUSED uint32_t op)
void SUB_A(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;

    flag_assign(false, &cpu->regF, HALF_CARRY_FMASK | CARRY_FMASK);
    flag_assign(true, &cpu->regF, NEGATIVE_FMASK | ZERO_FMASK);
    cpu->regA = 0;
    cpu->t_cycles += 4;    
}
//void SBC_A_B(void *arg, UNUSED uint32_t op)
//void SBC_A_C(void *arg, UNUSED uint32_t op)
//void SBC_A_D(void *arg, UNUSED uint32_t op)
//void SBC_A_E(void *arg, UNUSED uint32_t op)
//void SBC_A_H(void *arg, UNUSED uint32_t op)
//void SBC_A_L(void *arg, UNUSED uint32_t op)
//void SBC_A_derefHL(void *arg, UNUSED uint32_t op)
//void SBC_A_A(void *arg, UNUSED uint32_t op)
void AND_B(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    cpu->regA &= cpu->regB;
    flag_assign(cpu->regA == 0, &cpu->regF, ZERO_FMASK);
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK | CARRY_FMASK);
    flag_assign(true, &cpu->regF, HALF_CARRY_FMASK);
    cpu->t_cycles += 4;
}
void AND_C(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    cpu->regA &= cpu->regC;
    flag_assign(cpu->regA == 0, &cpu->regF, ZERO_FMASK);
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK | CARRY_FMASK);
    flag_assign(true, &cpu->regF, HALF_CARRY_FMASK);
    cpu->t_cycles += 4;
}
void AND_D(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    cpu->regA &= cpu->regD;
    flag_assign(cpu->regA == 0, &cpu->regF, ZERO_FMASK);
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK | CARRY_FMASK);
    flag_assign(true, &cpu->regF, HALF_CARRY_FMASK);
    cpu->t_cycles += 4;
}
void AND_E(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    cpu->regA &= cpu->regE;
    flag_assign(cpu->regA == 0, &cpu->regF, ZERO_FMASK);
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK | CARRY_FMASK);
    flag_assign(true, &cpu->regF, HALF_CARRY_FMASK);
    cpu->t_cycles += 4;
}
void AND_H(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    cpu->regA &= cpu->regH;
    flag_assign(cpu->regA == 0, &cpu->regF, ZERO_FMASK);
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK | CARRY_FMASK);
    flag_assign(true, &cpu->regF, HALF_CARRY_FMASK);
    cpu->t_cycles += 4;
}
void AND_L(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    cpu->regA &= cpu->regL;
    flag_assign(cpu->regA == 0, &cpu->regF, ZERO_FMASK);
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK | CARRY_FMASK);
    flag_assign(true, &cpu->regF, HALF_CARRY_FMASK);
    cpu->t_cycles += 4;
}
//void AND_derefHL(void *arg, UNUSED uint32_t op)
void AND_A(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    cpu->regA &= cpu->regA;
    flag_assign(cpu->regA == 0, &cpu->regF, ZERO_FMASK);
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK | CARRY_FMASK);
    flag_assign(true, &cpu->regF, HALF_CARRY_FMASK);
    cpu->t_cycles += 4;
}
void XOR_B(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    cpu->regA ^= cpu->regB;
    flag_assign(cpu->regA == 0, &cpu->regF, ZERO_FMASK);
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK | HALF_CARRY_FMASK | CARRY_FMASK);
    cpu->t_cycles += 4;
}
void XOR_C(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    cpu->regA ^= cpu->regC;
    flag_assign(cpu->regA == 0, &cpu->regF, ZERO_FMASK);
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK | HALF_CARRY_FMASK | CARRY_FMASK);
    cpu->t_cycles += 4;
}
void XOR_D(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    cpu->regA ^= cpu->regD;
    flag_assign(cpu->regA == 0, &cpu->regF, ZERO_FMASK);
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK | HALF_CARRY_FMASK | CARRY_FMASK);
    cpu->t_cycles += 4;
}
void XOR_E(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    cpu->regA ^= cpu->regE;
    flag_assign(cpu->regA == 0, &cpu->regF, ZERO_FMASK);
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK | HALF_CARRY_FMASK | CARRY_FMASK);
    cpu->t_cycles += 4;
}
void XOR_H(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    cpu->regA ^= cpu->regH;
    flag_assign(cpu->regA == 0, &cpu->regF, ZERO_FMASK);
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK | HALF_CARRY_FMASK | CARRY_FMASK);
    cpu->t_cycles += 4;
}
void XOR_L(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    cpu->regA ^= cpu->regL;
    flag_assign(cpu->regA == 0, &cpu->regF, ZERO_FMASK);
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK | HALF_CARRY_FMASK | CARRY_FMASK);
    cpu->t_cycles += 4;
}
void XOR_derefHL(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    uint8_t data;
    if(0 != read_memory(emu, ((cpu->regH << 8) | cpu->regL), &data))
        destroy_emulator(emu, EXIT_FAILURE);
    cpu->regA ^= data;
    flag_assign(cpu->regA == 0, &cpu->regF, ZERO_FMASK);
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK | HALF_CARRY_FMASK | CARRY_FMASK);
    cpu->t_cycles += 8;
}
void XOR_A(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    cpu->regA ^= cpu->regA;
    flag_assign(cpu->regA == 0, &cpu->regF, ZERO_FMASK);
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK | HALF_CARRY_FMASK | CARRY_FMASK);
    cpu->t_cycles += 4;
}

void OR_B(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    cpu->regA |= cpu->regB;
    flag_assign(cpu->regA == 0, &cpu->regF, ZERO_FMASK);
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK | HALF_CARRY_FMASK | CARRY_FMASK);
    cpu->t_cycles += 4;   
}

void OR_C(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    cpu->regA |= cpu->regC;
    flag_assign(cpu->regA == 0, &cpu->regF, ZERO_FMASK);
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK | HALF_CARRY_FMASK | CARRY_FMASK);
    cpu->t_cycles += 4;
}
void OR_D(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    cpu->regA |= cpu->regD;
    flag_assign(cpu->regA == 0, &cpu->regF, ZERO_FMASK);
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK | HALF_CARRY_FMASK | CARRY_FMASK);
    cpu->t_cycles += 4;
}
void OR_E(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    cpu->regA |= cpu->regE;
    flag_assign(cpu->regA == 0, &cpu->regF, ZERO_FMASK);
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK | HALF_CARRY_FMASK | CARRY_FMASK);
    cpu->t_cycles += 4;
}
void OR_H(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    cpu->regA |= cpu->regH;
    flag_assign(cpu->regA == 0, &cpu->regF, ZERO_FMASK);
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK | HALF_CARRY_FMASK | CARRY_FMASK);
    cpu->t_cycles += 4;
}
void OR_L(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    cpu->regA |= cpu->regL;
    flag_assign(cpu->regA == 0, &cpu->regF, ZERO_FMASK);
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK | HALF_CARRY_FMASK | CARRY_FMASK);
    cpu->t_cycles += 4;
}
void OR_derefHL(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    uint8_t data;
    if(0 != read_memory(emu, ((cpu->regH << 8) | cpu->regL), &data))
        destroy_emulator(emu, EXIT_FAILURE);
    cpu->regA |= data;
    flag_assign(cpu->regA == 0, &cpu->regF, ZERO_FMASK);
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK | HALF_CARRY_FMASK | CARRY_FMASK);
    cpu->t_cycles += 8;   
}
void OR_A(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    cpu->regA |= cpu->regA;
    flag_assign(cpu->regA == 0, &cpu->regF, ZERO_FMASK);
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK | HALF_CARRY_FMASK | CARRY_FMASK);
    cpu->t_cycles += 4;
}
void CP_B(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint8_t B4bit = cpu->regB & 0x0f;
    uint8_t A4bit = cpu->regA & 0x0F;
    flag_assign(B4bit > A4bit, &cpu->regF, HALF_CARRY_FMASK);
    flag_assign(cpu->regA - cpu->regB == 0, &cpu->regF, ZERO_FMASK);
    flag_assign(true, &cpu->regF, NEGATIVE_FMASK);
    flag_assign(cpu->regB > cpu->regA, &cpu->regF, CARRY_FMASK);
    
    cpu->t_cycles += 4;
}
void CP_C(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint8_t C4bit = cpu->regC & 0x0f;
    uint8_t A4bit = cpu->regA & 0x0F;
    flag_assign(C4bit > A4bit, &cpu->regF, HALF_CARRY_FMASK);
    flag_assign(cpu->regA - cpu->regC == 0, &cpu->regF, ZERO_FMASK);
    flag_assign(true, &cpu->regF, NEGATIVE_FMASK);
    flag_assign(cpu->regC > cpu->regA, &cpu->regF, CARRY_FMASK);
    
    cpu->t_cycles += 4;
}
void CP_D(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint8_t D4bit = cpu->regD & 0x0f;
    uint8_t A4bit = cpu->regA & 0x0F;
    flag_assign(D4bit > A4bit, &cpu->regF, HALF_CARRY_FMASK);
    flag_assign(cpu->regA - cpu->regD == 0, &cpu->regF, ZERO_FMASK);
    flag_assign(true, &cpu->regF, NEGATIVE_FMASK);
    flag_assign(cpu->regD > cpu->regA, &cpu->regF, CARRY_FMASK);
    
    cpu->t_cycles += 4;
}
void CP_E(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint8_t E4bit = cpu->regE & 0x0f;
    uint8_t A4bit = cpu->regA & 0x0F;
    flag_assign(E4bit > A4bit, &cpu->regF, HALF_CARRY_FMASK);
    flag_assign(cpu->regA - cpu->regE == 0, &cpu->regF, ZERO_FMASK);
    flag_assign(true, &cpu->regF, NEGATIVE_FMASK);
    flag_assign(cpu->regE > cpu->regA, &cpu->regF, CARRY_FMASK);
    
    cpu->t_cycles += 4;
}
void CP_H(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint8_t H4bit = cpu->regH & 0x0f;
    uint8_t A4bit = cpu->regA & 0x0F;
    flag_assign(H4bit > A4bit, &cpu->regF, HALF_CARRY_FMASK);
    flag_assign(cpu->regA - cpu->regH == 0, &cpu->regF, ZERO_FMASK);
    flag_assign(true, &cpu->regF, NEGATIVE_FMASK);
    flag_assign(cpu->regH > cpu->regA, &cpu->regF, CARRY_FMASK);
    
    cpu->t_cycles += 4;
}
void CP_L(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint8_t L4bit = cpu->regL & 0x0f;
    uint8_t A4bit = cpu->regA & 0x0F;
    flag_assign(L4bit > A4bit, &cpu->regF, HALF_CARRY_FMASK);
    flag_assign(cpu->regA - cpu->regL == 0, &cpu->regF, ZERO_FMASK);
    flag_assign(true, &cpu->regF, NEGATIVE_FMASK);
    flag_assign(cpu->regL > cpu->regA, &cpu->regF, CARRY_FMASK);
    
    cpu->t_cycles += 4;
}
void CP_derefHL(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint8_t A4bit = cpu->regA & 0x0F;
    uint8_t data;
    if(0 != read_memory(emu, (cpu->regH << 8) + cpu->regL, &data))
        destroy_emulator(emu, EXIT_FAILURE);
    uint8_t data4bit = data & 0x0F;
    flag_assign(data4bit > A4bit, &cpu->regF, HALF_CARRY_FMASK);
    flag_assign(true, &cpu->regF, NEGATIVE_FMASK);
    flag_assign(data > cpu->regA, &cpu->regF, CARRY_FMASK);
    flag_assign(cpu->regA == data, &cpu->regF, ZERO_FMASK);
    
    cpu->t_cycles += 8;    
}
void CP_A(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    flag_assign(false, &cpu->regF, HALF_CARRY_FMASK | CARRY_FMASK);
    flag_assign(true, &cpu->regF, ZERO_FMASK | NEGATIVE_FMASK);
    
    cpu->t_cycles += 4;
}
void RET_NZ(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    if(cpu->regF & ZERO_FMASK)
    {
        cpu->t_cycles += 8;
        return;
    }
    
    uint8_t data;
    if(0 != read_memory(emu, cpu->sp, &data))
        destroy_emulator(emu, EXIT_FAILURE);
    cpu->pc = data;
    cpu->sp++;
    if(0 != read_memory(emu, cpu->sp, &data))
        destroy_emulator(emu, EXIT_FAILURE);
    cpu->pc |= data << 8;
    cpu->sp++;
    
    //take the pc incrementation in the interpret function into account
    cpu->pc -= 1;
    
    cpu->t_cycles += 20;
}
void POP_BC(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    if(0 != read_memory(emu, cpu->sp, &cpu->regC))
        destroy_emulator(emu, EXIT_FAILURE);
    cpu->sp++;
    if(0 != read_memory(emu, cpu->sp, &cpu->regB))
        destroy_emulator(emu, EXIT_FAILURE);
    cpu->sp++;
    
    cpu->t_cycles += 12;
}
void JP_NZ_a16(void *arg, uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    if(cpu->regF & ZERO_FMASK)
    {
        cpu->t_cycles += 12;
        return;
    }
    
    cpu->pc = ((op & 0x0000FF00) >> 8) + ((op & 0x000000FF) << 8);
    cpu->pc -= 3;
    cpu->t_cycles += 16;
}
void JP_a16(void *arg, uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->pc = ((op & 0x0000FF00) >> 8) + ((op & 0x000000FF) << 8);
    cpu->pc -= 3;
    cpu->t_cycles += 16;
}

void CALL_NZ_a16(void *arg, uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    if(cpu->regF & ZERO_FMASK)
    {
        cpu->t_cycles += 12;
        return;
    }
    
    uint16_t pc_old_value = cpu->pc + 3;
    cpu->sp--;
    if(0 != write_memory(emu, cpu->sp, (pc_old_value & 0xff00) >> 8))
        destroy_emulator(emu, EXIT_FAILURE);
    cpu->sp--;

    if(0 != write_memory(emu, cpu->sp, (pc_old_value & 0x00ff)))
        destroy_emulator(emu, EXIT_FAILURE);
    cpu->pc = ((op & 0x0000ff00) >> 8) + ((op & 0x000000ff) << 8);
    //take the pc incrementation in the interpret function into account
    cpu->pc -= 3;
    cpu->t_cycles += 24;    
}

void PUSH_BC(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;

    cpu->sp--;
    if(0 != write_memory(emu, cpu->sp, cpu->regB))
        destroy_emulator(emu, EXIT_FAILURE);
    cpu->sp--;
    if(0 != write_memory(emu, cpu->sp, cpu->regC))
        destroy_emulator(emu, EXIT_FAILURE);
    
    cpu->t_cycles += 16;
}

void ADD_A_d8(void *arg, uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint16_t newA = cpu->regA;
    uint8_t d8 = (op & 0x0000FF00) >> 8;
    
    uint8_t A4bit = cpu->regA & 0x0F;
    uint8_t d8_4bit = d8 & 0x0F;
    
    flag_assign(A4bit + d8_4bit > 0x0F, &cpu->regF, HALF_CARRY_FMASK);
    
    newA += d8;
    flag_assign(newA > 0xFF, &cpu->regF, CARRY_FMASK);
    
    cpu->regA += d8;
    flag_assign(cpu->regA == 0, &cpu->regF, ZERO_FMASK);
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK);
    
    cpu->t_cycles += 8;
}

//void RST_00H(void *arg, UNUSED uint32_t op)
void RET_Z(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    if(!(cpu->regF & ZERO_FMASK))
    {
        cpu->t_cycles += 8;
        return;
    }
    
    uint8_t data;
    if(0 != read_memory(emu, cpu->sp, &data))
        destroy_emulator(emu, EXIT_FAILURE);
    cpu->pc = data;
    cpu->sp++;
    if(0 != read_memory(emu, cpu->sp, &data))
        destroy_emulator(emu, EXIT_FAILURE);
    cpu->pc |= data << 8;
    cpu->sp++;
    
    //take the pc incrementation in the interpret function into account
    cpu->pc -= 1;
    
    cpu->t_cycles += 20;
}

void RET(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint8_t data;
    if(0 != read_memory(emu, cpu->sp, &data))
        destroy_emulator(emu, EXIT_FAILURE);
    cpu->pc = data;
    cpu->sp++;
    if(0 != read_memory(emu, cpu->sp, &data))
        destroy_emulator(emu, EXIT_FAILURE);
    cpu->pc |= data << 8;
    cpu->sp++;
    
    //take the pc incrementation in the interpret function into account
    cpu->pc -= 1;
    
    cpu->t_cycles += 16;
}
void JP_Z_a16(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    if(!(cpu->regF & ZERO_FMASK))
    {
        cpu->t_cycles += 12;
        return;
    }
    
    cpu->pc = ((op & 0x0000FF00) >> 8) + ((op & 0x000000FF) << 8);
    cpu->pc -= 3;
    cpu->t_cycles += 16;
}
//void CA
void PREFIX_CB(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    uint8_t cb_opcode = get_cb_opcode(op);
    (emu->cb_functions[cb_opcode] (emu, cb_opcode));
}

//void CALL_Z_a16(void *arg, UNUSED uint32_t op)

void CALL_a16(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint16_t pc_old_value = cpu->pc + 3;
    cpu->sp--;
    if(0 != write_memory(emu, cpu->sp, (pc_old_value & 0xff00) >> 8))
        destroy_emulator(emu, EXIT_FAILURE);
    cpu->sp--;

    if(0 != write_memory(emu, cpu->sp, (pc_old_value & 0x00ff)))
        destroy_emulator(emu, EXIT_FAILURE);
    cpu->pc = ((op & 0x0000ff00) >> 8) + ((op & 0x000000ff) << 8);
    //take the pc incrementation in the interpret function into account
    cpu->pc -= 3;
    cpu->t_cycles += 24;    
}

void ADC_A_d8(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint16_t newA = cpu->regA;
    uint8_t d8 = ((op & 0x0000FF00) >> 8);
    
    uint8_t A4bit = cpu->regA & 0x0F;
    uint8_t d8_4bit = d8 & 0x0F;
    bool carry = cpu->regF & CARRY_FMASK;
    
    flag_assign(A4bit + d8_4bit + carry > 0x0F, &cpu->regF, HALF_CARRY_FMASK);
    
    newA += d8 + carry;
    
    cpu->regA += d8 + carry;
    flag_assign(newA > 0xFF, &cpu->regF, CARRY_FMASK);
    flag_assign(cpu->regA == 0, &cpu->regF, ZERO_FMASK);
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK);
    
    cpu->t_cycles += 8;
}
//void RST_08H(void *arg, uint32_t op)
void RET_NC(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    if(cpu->regF & CARRY_FMASK)
    {
        cpu->t_cycles += 8;
        return;
    }
    
    uint8_t data;
    if(0 != read_memory(emu, cpu->sp, &data))
        destroy_emulator(emu, EXIT_FAILURE);
    cpu->pc = data;
    cpu->sp++;
    if(0 != read_memory(emu, cpu->sp, &data))
        destroy_emulator(emu, EXIT_FAILURE);
    cpu->pc |= data << 8;
    cpu->sp++;
    
    //take the pc incrementation in the interpret function into account
    cpu->pc -= 1;
    
    cpu->t_cycles += 20;
}
void POP_DE(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    if(0 != read_memory(emu, cpu->sp, &cpu->regE))
        destroy_emulator(emu, EXIT_FAILURE);
    cpu->sp++;
    if(0 != read_memory(emu, cpu->sp, &cpu->regD))
        destroy_emulator(emu, EXIT_FAILURE);
    cpu->sp++;
    
    cpu->t_cycles += 12;
}
//void JP_NC_a16(void *arg, UNUSED uint32_t op)
//void dont_exist(void *arg, uint32_t op)
//void CALL_NC_a16(void *arg, uint32_t op)
void PUSH_DE(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;

    cpu->sp--;
    if(0 != write_memory(emu, cpu->sp, cpu->regD))
        destroy_emulator(emu, EXIT_FAILURE);
    cpu->sp--;
    if(0 != write_memory(emu, cpu->sp, cpu->regE))
        destroy_emulator(emu, EXIT_FAILURE);
    
    cpu->t_cycles += 16;
}
void SUB_d8(void *arg, uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint8_t d8 = (op & 0x0000FF00) >> 8;
    uint8_t A4bit = cpu->regA & 0x0F;
    uint8_t d8_4bit = d8 & 0x0F;
    flag_assign(d8_4bit > A4bit, &cpu->regF, HALF_CARRY_FMASK);
    
    flag_assign(d8 > cpu->regA, &cpu->regF, CARRY_FMASK);
    flag_assign(true, &cpu->regF, NEGATIVE_FMASK);
    cpu->regA -= d8;
    flag_assign(cpu->regA == 0, &cpu->regF, ZERO_FMASK);
    cpu->t_cycles += 8;   
}
//void RST_10H(void *arg, UNUSED uint32_t op)
void RET_C(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    if(!(cpu->regF & CARRY_FMASK))
    {
        cpu->t_cycles += 8;
        return;
    }
    
    uint8_t data;
    if(0 != read_memory(emu, cpu->sp, &data))
        destroy_emulator(emu, EXIT_FAILURE);
    cpu->pc = data;
    cpu->sp++;
    if(0 != read_memory(emu, cpu->sp, &data))
        destroy_emulator(emu, EXIT_FAILURE);
    cpu->pc |= data << 8;
    cpu->sp++;
    
    //take the pc incrementation in the interpret function into account
    cpu->pc -= 1;
    
    cpu->t_cycles += 20;
}
void RETI(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint8_t data;
    if(0 != read_memory(emu, cpu->sp, &data))
        destroy_emulator(emu, EXIT_FAILURE);
    cpu->pc = data;
    cpu->sp++;
    if(0 != read_memory(emu, cpu->sp, &data))
        destroy_emulator(emu, EXIT_FAILURE);
    cpu->pc |= data << 8;
    cpu->sp++;
    
    cpu->io_reg.IME = true;
    
    //take the pc incrementation in the interpret function into account
    cpu->pc -= 1;
    
    cpu->t_cycles += 16;
}
//void JP_C_a16(void *arg, uint32_t op)
//void dont_exist(void *arg, uint32_t op)
//void CALL_C_a16(void *arg, uint32_t op)
//void dont_exist(void *arg, uint32_t op)
//void SBC_A_d8(void *arg, uint32_t op)
//void RST_18H(void *arg, UNUSED uint32_t op)
void LDH_derefa8_A(void *arg, uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    if(0 != write_memory(emu, 0xFF00 + ((op & 0x0000FF00) >> 8), cpu->regA))
        destroy_emulator(emu, EXIT_FAILURE);
    cpu->t_cycles += 12;
}
void POP_HL(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    if(0 != read_memory(emu, cpu->sp, &cpu->regL))
        destroy_emulator(emu, EXIT_FAILURE);
    cpu->sp++;
    if(0 != read_memory(emu, cpu->sp, &cpu->regH))
        destroy_emulator(emu, EXIT_FAILURE);
    cpu->sp++;
    
    cpu->t_cycles += 12;
}

void LD_derefC_A(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    if(0 != write_memory(emu, 0xFF00 + cpu->regC, cpu->regA))
        destroy_emulator(emu, EXIT_FAILURE);
    cpu->t_cycles += 8;
}

//void dont_exist(void *arg, uint32_t op)
//void dont_exist(void *arg, uint32_t op)
void PUSH_HL(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;

    cpu->sp--;
    if(0 != write_memory(emu, cpu->sp, cpu->regH))
        destroy_emulator(emu, EXIT_FAILURE);
    cpu->sp--;
    if(0 != write_memory(emu, cpu->sp, cpu->regL))
        destroy_emulator(emu, EXIT_FAILURE);
    
    cpu->t_cycles += 16;
}
void AND_d8(void *arg, uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regA &= (op & 0x0000FF00) >> 8;
    flag_assign(cpu->regA == 0, &cpu->regF, ZERO_FMASK);
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK | CARRY_FMASK);
    flag_assign(true, &cpu->regF, HALF_CARRY_FMASK);
    
    cpu->t_cycles += 8;
}
void RST_20H(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;

    cpu->sp--;
    if(0 != write_memory(emu, cpu->sp, (cpu->pc & 0xff00) >> 8))
        destroy_emulator(emu, EXIT_FAILURE);
    cpu->sp--;
    if(0 != write_memory(emu, cpu->sp, (cpu->pc & 0x00ff)))
        destroy_emulator(emu, EXIT_FAILURE);
    
    cpu->pc = 0x0020 - 1;
    cpu->t_cycles += 16;
}
//void ADD_SP_r8(void *arg, uint32_t op)
void JP_derefHL(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->pc = (cpu->regH << 8) + cpu->regL;
    cpu->pc -= 1;
    cpu->t_cycles += 4;
}
void LD_derefa16_A(void *arg, uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint16_t adress = ((op & 0x0000ff00) >> 8) + ((op & 0x000000ff) << 8);
    if(0 != write_memory(emu, adress, cpu->regA))
        destroy_emulator(emu, EXIT_FAILURE);
    
    cpu->t_cycles += 16;
}
//void dont_exist(void *arg, uint32_t op)
//void dont_exist(void *arg, uint32_t op)
//void dont_exist(void *arg, uint32_t op)
void XOR_d8(void *arg, uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    cpu->regA ^= ((op & 0x0000ff00) >> 8);
    flag_assign(cpu->regA == 0, &cpu->regF, ZERO_FMASK);
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK | HALF_CARRY_FMASK | CARRY_FMASK);
    cpu->t_cycles += 8;
}
void RST_28H(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;

    cpu->sp--;
    if(0 != write_memory(emu, cpu->sp, (cpu->pc & 0xff00) >> 8))
        destroy_emulator(emu, EXIT_FAILURE);
    cpu->sp--;
    if(0 != write_memory(emu, cpu->sp, (cpu->pc & 0x00ff)))
        destroy_emulator(emu, EXIT_FAILURE);
    
    cpu->pc = 0x0028 - 1;
    cpu->t_cycles += 16;
}
void LDH_A_derefa8(void *arg, uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    if(0 != read_memory(emu, 0xFF00 + ((op & 0x0000FF00) >> 8), &cpu->regA))
        destroy_emulator(emu, EXIT_FAILURE);
    cpu->t_cycles += 12;
}
void POP_AF(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    if(0 != read_memory(emu, cpu->sp, &cpu->regF))
        destroy_emulator(emu, EXIT_FAILURE);
    cpu->regF &= 0xF0;
    cpu->sp++;
    if(0 != read_memory(emu, cpu->sp, &cpu->regA))
        destroy_emulator(emu, EXIT_FAILURE);
    cpu->sp++;
    
    cpu->t_cycles += 12;
}

//void LD_A_derefC(void *arg, UNUSED uint32_t op)
void DI(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->io_reg.IME = false;
    cpu->t_cycles += 4;
}
//void dont_exist(void *arg, uint32_t op)
void PUSH_AF(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;

    cpu->sp--;
    if(0 != write_memory(emu, cpu->sp, cpu->regA))
        destroy_emulator(emu, EXIT_FAILURE);
    cpu->sp--;
    if(0 != write_memory(emu, cpu->sp, cpu->regF))
        destroy_emulator(emu, EXIT_FAILURE);
    
    cpu->t_cycles += 16;
}
void OR_d8(void *arg, uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    cpu->regA |= (op & 0x0000ff00) >> 8;
    flag_assign(cpu->regA == 0, &cpu->regF, ZERO_FMASK);
    flag_assign(false, &cpu->regF, NEGATIVE_FMASK | HALF_CARRY_FMASK | CARRY_FMASK);
    cpu->t_cycles += 8;   
}

//void RST_30H(void *arg, UNUSED uint32_t op)
void LD_HL_SPplusr8(void *arg, uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    int8_t r8 = (op & 0x0000FF00) >> 8;
    int8_t r8_4bit = r8 & 0x0F;
    uint8_t SP4bit = cpu->sp & 0x0f;
    uint16_t HL;
    
    if(r8_4bit >= 0)
    {
        flag_assign(SP4bit + r8_4bit > 0x0f, &cpu->regF, HALF_CARRY_FMASK);
        flag_assign(SP4bit > UINT16_MAX - r8_4bit, &cpu->regF, CARRY_FMASK);
    }
    else
    {
        flag_assign(r8_4bit > SP4bit, &cpu->regF, HALF_CARRY_FMASK);
        flag_assign(abs(r8_4bit) > SP4bit, &cpu->regF, CARRY_FMASK);
    }
    
    flag_assign(false, &cpu->regF, ZERO_FMASK | NEGATIVE_FMASK);
    HL = cpu->sp + r8;
    cpu->regH = (HL & 0xFF00) >> 8;
    cpu->regL = HL & 0x00FF;
    
    cpu->t_cycles += 12;    
}
void LD_SP_HL(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->sp = (cpu->regH << 8) + cpu->regL;
    cpu->t_cycles += 8;
}
void LD_A_derefa16(void *arg, uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    if(0 != read_memory(emu, ((op & 0x0000ff00) >> 8) + ((op & 0x000000ff) << 8), &cpu->regA))
        destroy_emulator(emu, EXIT_FAILURE);
    
    cpu->t_cycles += 16;
}

void EI(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->io_reg.IME = true;
    cpu->t_cycles += 4;
}
//void dont_exist(void *arg, uint32_t op)
//void dont_exist(void *arg, uint32_t op)
void CP_d8(void *arg, uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint8_t d8 = (op & 0x0000ff00) >> 8;
    uint8_t d4bit = d8 & 0x0F;
    uint8_t A4bit = cpu->regA & 0x0F;
    flag_assign(d4bit > A4bit, &cpu->regF, HALF_CARRY_FMASK);
    flag_assign(cpu->regA - d8 == 0, &cpu->regF, ZERO_FMASK);
    flag_assign(true, &cpu->regF, NEGATIVE_FMASK);
    flag_assign(d8 > cpu->regA, &cpu->regF, CARRY_FMASK);
    
    cpu->t_cycles += 8;
}

//void RST_38H(void *arg, UNUSED uint32_t op)
//{
//    s_emu *emu = arg;
//    s_cpu *cpu = &emu->cpu;
//    
////    uint16_t pc_old_value = cpu->pc + 3;
////    cpu->sp--;
////    if(0 != write_memory(emu, cpu->sp, (pc_old_value & 0xff00) >> 8))
////        destroy_emulator(emu, EXIT_FAILURE);
////    cpu->sp--;
////
////    if(0 != write_memory(emu, cpu->sp, (pc_old_value & 0x00ff)))
////        destroy_emulator(emu, EXIT_FAILURE);
////    cpu->pc = ((op & 0x0000ff00) >> 8) + ((op & 0x000000ff) << 8);
////    //take the pc incrementation in the interpret function into account
////    cpu->pc -= 3;
////    cpu->t_cycles += 24;   
//
//    cpu->sp--;
//    if(0 != write_memory(emu, cpu->sp, (cpu->pc & 0xff00) >> 8))
//        destroy_emulator(emu, EXIT_FAILURE);
//    cpu->sp--;
//
//    if(0 != write_memory(emu, cpu->sp, (cpu->pc & 0x00ff)))
//        destroy_emulator(emu, EXIT_FAILURE);
//    
//    cpu->pc = 0x0038;
//}
