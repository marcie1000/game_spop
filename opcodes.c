#include <stdio.h>
#include <stdlib.h>
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
    fprintf(stderr, "WARNING: instruction %s (0x%06X) unimplemented!\n", 
            emu->mnemonic_index[(op & 0x00ff0000) >> 16], op);
    emu->in.quit = SDL_TRUE;
}

void opcode_non_existant(void *arg, uint32_t op)
{
    s_emu *emu = arg;
    fprintf(stderr, "ERROR: instuction 0x%02X doesn't exist!\n", (op & 0xFF0000) >> 16);
    emu->in.quit = SDL_TRUE;
}

void NOP(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    emu->cpu.cycles += 4;
}
//void LD_BC_d16(void *arg, uint32_t op)
//void LD_derefBC_A(void *arg, uint32_t op)
void INC_BC(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    uint16_t BC = (cpu->regB << 8) + cpu->regC;
    BC++;
    cpu->regB = (BC & 0xff00) >> 8;
    cpu->regC = BC & 0x00ff;
    cpu->cycles += 8;
}
//void INC_B(void *arg, uint32_t op)
void DEC_B(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint8_t B4bit = (cpu->regB & 0x0F) + 0x10;
    B4bit--;
    bool half_carry = !(B4bit & 0x10);
    flag_assign(half_carry, &cpu->regF, 0x20);
    
    cpu->regB--;
    flag_assign(cpu->regB == 0, &cpu->regF, 0x80);
    flag_assign(true, &cpu->regF, 0x40);
    
    cpu->cycles += 4;
}
void LD_B_d8(void *arg, uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regB = (op & 0x0000ff00) >> 8;
    cpu->cycles += 8;
}

void RLCA(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint16_t newA = cpu->regA << 1;
    flag_assign(newA > 0xff, &cpu->regF, 0x10);
    flag_assign(false, &cpu->regF, 0xE0);
    cpu->regA = (newA & 0x00FF) + ((newA & 0x0100) >> 8);
    
    cpu->cycles += 4;
}
//void LD_derefa16_SP(void *arg, uint32_t op)
//void ADD_HL_BC(void *arg, uint32_t op)
//void LD_A_derefBC(void *arg, uint32_t op)
void DEC_BC(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    uint16_t BC = (cpu->regB << 8) + cpu->regC;
    BC--;
    cpu->regB = (BC & 0xff00) >> 8;
    cpu->regC = BC & 0x00ff;
    cpu->cycles += 8;
}

void INC_C(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint8_t c4bit = cpu->regC & 0x0f;
    c4bit++;
    flag_assign(c4bit > 0x0f, &cpu->regF, 0x20);
    
    cpu->regC++;
    flag_assign(cpu->regC == 0, &cpu->regF, 0x80);
    flag_assign(false, &cpu->regF, 0x40);
    
    cpu->cycles += 4;    
}

void DEC_C(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint8_t C4bit = (cpu->regC & 0x0F) + 0x10;
    C4bit--;
    bool half_carry = !(C4bit & 0x10);
    flag_assign(half_carry, &cpu->regF, 0x20);
    
    cpu->regC--;
    flag_assign(cpu->regC == 0, &cpu->regF, 0x80);
    flag_assign(true, &cpu->regF, 0x40);
    
    cpu->cycles += 4;
}

void LD_C_d8(void *arg, uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regC = (op & 0x0000ff00) >> 8;
    cpu->cycles += 8;
}

//void RRCA(void *arg, uint32_t op)
//void STOP_0(void *arg, uint32_t op)
void LD_DE_d16(void *arg, uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regE = (op & 0x0000ff00) >> 8;
    cpu->regD = (op & 0x000000ff);
    
    cpu->cycles += 12;
}
//void LD_derefDE_A(void *arg, uint32_t op)
void INC_DE(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    uint16_t DE = (cpu->regD << 8) + cpu->regE;
    DE++;
    cpu->regD = (DE & 0xff00) >> 8;
    cpu->regE = DE & 0x00ff;
    cpu->cycles += 8;
}
//void INC_D(void *arg, uint32_t op)
//void DEC_D(void *arg, uint32_t op)
//void LD_D_d8(void *arg, uint32_t op)
//void RLA(void *arg, uint32_t op)
void JR_r8(void *arg, uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    int8_t r8 = (op & 0x0000ff00) >> 8;
    cpu->pc += r8;
    cpu->cycles += 12;
}
//void ADD_HL_DE(void *arg, uint32_t op)
void LD_A_derefDE(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regA = cpu->mem[(cpu->regE << 8) + cpu->regD];
    cpu->cycles += 8;
}
//void DEC_DE(void *arg, uint32_t op)
//void INC_E(void *arg, uint32_t op)
//void DEC_E(void *arg, uint32_t op)
//void LD_E_d8(void *arg, uint32_t op)
//void RRA(void *arg, uint32_t op)

void JR_NZ_r8(void *arg, uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    if(!(cpu->regF & 0x80)) //if Z flag is 0
    {
        int8_t value = (op & 0x0000ff00) >> 8;
        cpu->pc += value;
        emu->cpu.cycles += 12;
    }
    else
        emu->cpu.cycles += 8;
}

void LD_HL_d16(void *arg, uint32_t op)
{
    s_emu *emu = arg;
    emu->cpu.regL = (op & 0x0000ff00) >> 8;
    emu->cpu.regH = (op & 0x000000ff);
    emu->cpu.cycles += 12;
}

void LD_derefHLplus_A(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    cpu->mem[(cpu->regL << 8) + cpu->regH] = cpu->regA;
    uint16_t HL = (cpu->regH << 8) + cpu->regL;
    HL++;
    cpu->regH = (HL & 0xff00) >> 8;
    cpu->regL = HL & 0x00ff;
    cpu->cycles += 8;    
}
void INC_HL(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    uint16_t HL = (cpu->regH << 8) + cpu->regL;
    HL++;
    cpu->regH = (HL & 0xff00) >> 8;
    cpu->regL = HL & 0x00ff;
    cpu->cycles += 8;
}
//void INC_H(void *arg, uint32_t op)
//void DEC_H(void *arg, uint32_t op)
//void LD_H_d8(void *arg, uint32_t op)
//void DAA(void *arg, uint32_t op)
void JR_Z_r8(void *arg, uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    if(cpu->regF & 0x80)
    {
        int8_t r8 = (op & 0x0000ff00) >> 8;
        cpu->pc += r8;
        cpu->cycles += 12;
    }
    else
    {
        cpu->cycles += 8;
    }
}
//void ADD_HL_HL(void *arg, uint32_t op)
//void LD_A_derefHLplus(void *arg, uint32_t op)
void DEC_HL(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    uint16_t HL = (cpu->regH << 8) + cpu->regL;
    HL--;
    cpu->regH = (HL & 0xff00) >> 8;
    cpu->regL = HL & 0x00ff;
    cpu->cycles += 8;
    
}
//void INC_L(void *arg, uint32_t op)
//void DEC_L(void *arg, uint32_t op)
void LD_L_d8(void *arg, uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regA = (op & 0x0000ff00) >> 8;
    cpu->cycles += 8;
}
//void CPL(void *arg, uint32_t op)
//void JR_NC_r8(void *arg, uint32_t op)

void LD_SP_d16(void *arg, uint32_t op)
{
    s_emu *emu = arg;
    emu->cpu.sp = (op & 0x0000ffff);
    emu->cpu.cycles += 12;
}

void LD_derefHLminus_A(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    cpu->mem[(cpu->regL << 8) + cpu->regH] = cpu->regA;
    uint16_t HL = (cpu->regH << 8) + cpu->regL;
    HL--;
    cpu->regH = (HL & 0xff00) >> 8;
    cpu->regL = HL & 0x00ff;
    cpu->cycles += 8;    
}

//void INC_SP(void *arg, uint32_t op)
//void INC_derefHL(void *arg, uint32_t op)
//void DEC_derefHL(void *arg, uint32_t op)
//void LD_derefHL_d8(void *arg, uint32_t op)
//void SCF(void *arg, uint32_t op)
//void JR_C_r8(void *arg, uint32_t op)
//void ADD_HL_SP(void *arg, uint32_t op)
//void LD_A_derefHLminus(void *arg, uint32_t op)
//void DEC_SP(void *arg, uint32_t op)
//void INC_A(void *arg, uint32_t op)
void DEC_A(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint8_t A4bit = (cpu->regA & 0x0F) + 0x10;
    A4bit--;
    bool half_carry = !(A4bit & 0x10);
    flag_assign(half_carry, &cpu->regF, 0x20);
    
    cpu->regA--;
    flag_assign(cpu->regA == 0, &cpu->regF, 0x80);
    flag_assign(true, &cpu->regF, 0x40);
    
    cpu->cycles += 4;
}

void LD_A_d8(void *arg, uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->regA = (op & 0x0000ff00) >> 8;
    cpu->cycles += 8;
}

//void CCF(void *arg, uint32_t op)
//void LD_B_B(void *arg, uint32_t op)
//void LD_B_C(void *arg, uint32_t op)
//void LD_B_D(void *arg, uint32_t op)
//void LD_B_E(void *arg, uint32_t op)
//void LD_B_H(void *arg, uint32_t op)
//void LD_B_L(void *arg, uint32_t op)
//void LD_B_derefHL(void *arg, uint32_t op)
//void LD_B_A(void *arg, uint32_t op)
//void LD_C_B(void *arg, uint32_t op)
//void LD_C_C(void *arg, uint32_t op)
//void LD_C_D(void *arg, uint32_t op)
//void LD_C_E(void *arg, uint32_t op)
//void LD_C_H(void *arg, uint32_t op)
//void LD_C_L(void *arg, uint32_t op)
//void LD_C_derefHL(void *arg, uint32_t op)
//void LD_C_A(void *arg, uint32_t op)
//void LD_D_B(void *arg, uint32_t op)
//void LD_D_C(void *arg, uint32_t op)
//void LD_D_D(void *arg, uint32_t op)
//void LD_D_E(void *arg, uint32_t op)
//void LD_D_H(void *arg, uint32_t op)
//void LD_D_L(void *arg, uint32_t op)
//void LD_D_derefHL(void *arg, uint32_t op)
//void LD_D_A(void *arg, uint32_t op)
//void LD_E_B(void *arg, uint32_t op)
//void LD_E_C(void *arg, uint32_t op)
//void LD_E_D(void *arg, uint32_t op)
//void LD_E_E(void *arg, uint32_t op)
//void LD_E_H(void *arg, uint32_t op)
//void LD_E_L(void *arg, uint32_t op)
//void LD_E_derefHL(void *arg, uint32_t op)
//void LD_E_A(void *arg, uint32_t op)
//void LD_H_B(void *arg, uint32_t op)
//void LD_H_C(void *arg, uint32_t op)
//void LD_H_D(void *arg, uint32_t op)
//void LD_H_E(void *arg, uint32_t op)
//void LD_H_H(void *arg, uint32_t op)
//void LD_H_L(void *arg, uint32_t op)
//void LD_H_derefHL(void *arg, uint32_t op)
//void LD_H_A(void *arg, uint32_t op)
//void LD_L_B(void *arg, uint32_t op)
//void LD_L_C(void *arg, uint32_t op)
//void LD_L_D(void *arg, uint32_t op)
//void LD_L_E(void *arg, uint32_t op)
//void LD_L_H(void *arg, uint32_t op)
//void LD_L_L(void *arg, uint32_t op)
//void LD_L_derefHL(void *arg, uint32_t op)
//void LD_L_A(void *arg, uint32_t op)
//void LD_derefHL_B(void *arg, uint32_t op)
//void LD_derefHL_C(void *arg, uint32_t op)
//void LD_derefHL_D(void *arg, uint32_t op)
//void LD_derefHL_E(void *arg, uint32_t op)
//void LD_derefHL_H(void *arg, uint32_t op)
//void LD_derefHL_L(void *arg, uint32_t op)
//void HALT(void *arg, uint32_t op)
void LD_derefHL_A(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    cpu->mem[(cpu->regL << 8) + cpu->regH] = cpu->regA;
    cpu->cycles += 8;
}
//void LD_A_B(void *arg, uint32_t op)
//void LD_A_C(void *arg, uint32_t op)
//void LD_A_D(void *arg, uint32_t op)
void LD_A_E(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    cpu->regA = cpu->regE;
    cpu->cycles += 4;   
}

void LD_A_H(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    cpu->regA = cpu->regH;
    cpu->cycles += 4;    
}

//void LD_A_L(void *arg, uint32_t op)
//void LD_A_derefHL(void *arg, uint32_t op)
//void LD_A_A(void *arg, uint32_t op)
void ADD_A_B(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint16_t new_A = cpu->regA;
    uint16_t new_B = cpu->regB;
    
    uint8_t A_4bit = new_A & 0x0F;
    uint8_t B_4bit = new_B & 0x0F;
    
    bool half_carry = ((A_4bit + B_4bit) > 0x0F);
    flag_assign(half_carry, &cpu->regF, 0x20);
    
    new_A += new_B;
    bool carry = (new_A > 0xFF00);
    flag_assign(carry, &cpu->regF, 0x10);
    
    bool zero = (new_A == 0);
    flag_assign(zero, &cpu->regF, 0x80);
    
    //negative flag
    flag_assign(false, &cpu->regF, 0x40);
    
    new_A &= 0x00FF;
    cpu->regA = new_A;
    
    cpu->cycles += 4;

}
//void ADD_A_C(void *arg, uint32_t op)
//void ADD_A_D(void *arg, uint32_t op)
//void ADD_A_E(void *arg, uint32_t op)
//void ADD_A_H(void *arg, uint32_t op)
//void ADD_A_L(void *arg, uint32_t op)
//void ADD_A_derefHL(void *arg, uint32_t op)
//void ADD_A_A(void *arg, uint32_t op)
//void ADC_A_B(void *arg, uint32_t op)
//void ADC_A_C(void *arg, uint32_t op)
//void ADC_A_D(void *arg, uint32_t op)
//void ADC_A_E(void *arg, uint32_t op)
//void ADC_A_H(void *arg, uint32_t op)
//void ADC_A_L(void *arg, uint32_t op)
//void ADC_A_derefHL(void *arg, uint32_t op)
//void ADC_A_A(void *arg, uint32_t op)
//void SUB_B(void *arg, uint32_t op)
//void SUB_C(void *arg, uint32_t op)
//void SUB_D(void *arg, uint32_t op)
//void SUB_E(void *arg, uint32_t op)
//void SUB_H(void *arg, uint32_t op)
//void SUB_L(void *arg, uint32_t op)
//void SUB_derefHL(void *arg, uint32_t op)
//void SUB_A(void *arg, uint32_t op)
//void SBC_A_B(void *arg, uint32_t op)
//void SBC_A_C(void *arg, uint32_t op)
//void SBC_A_D(void *arg, uint32_t op)
//void SBC_A_E(void *arg, uint32_t op)
//void SBC_A_H(void *arg, uint32_t op)
//void SBC_A_L(void *arg, uint32_t op)
//void SBC_A_derefHL(void *arg, uint32_t op)
//void SBC_A_A(void *arg, uint32_t op)
//void AND_B(void *arg, uint32_t op)
//void AND_C(void *arg, uint32_t op)
//void AND_D(void *arg, uint32_t op)
//void AND_E(void *arg, uint32_t op)
//void AND_H(void *arg, uint32_t op)
//void AND_L(void *arg, uint32_t op)
//void AND_derefHL(void *arg, uint32_t op)
//void AND_A(void *arg, uint32_t op)
//void XOR_B(void *arg, uint32_t op)
//void XOR_C(void *arg, uint32_t op)
//void XOR_D(void *arg, uint32_t op)
//void XOR_E(void *arg, uint32_t op)
//void XOR_H(void *arg, uint32_t op)
//void XOR_L(void *arg, uint32_t op)
//void XOR_derefHL(void *arg, uint32_t op)

void XOR_A(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    cpu->regA ^= cpu->regA;
    flag_assign(cpu->regA == 0, &cpu->regF, 0x80);
    flag_assign(false, &cpu->regF, 0x70);
    cpu->cycles += 4;
}

//void OR_B(void *arg, uint32_t op)
//void OR_C(void *arg, uint32_t op)
//void OR_D(void *arg, uint32_t op)
//void OR_E(void *arg, uint32_t op)
//void OR_H(void *arg, uint32_t op)
//void OR_L(void *arg, uint32_t op)
//void OR_derefHL(void *arg, uint32_t op)
//void OR_A(void *arg, uint32_t op)
//void CP_B(void *arg, uint32_t op)
//void CP_C(void *arg, uint32_t op)
//void CP_D(void *arg, uint32_t op)
//void CP_E(void *arg, uint32_t op)
//void CP_H(void *arg, uint32_t op)
//void CP_L(void *arg, uint32_t op)
//void CP_derefHL(void *arg, uint32_t op)
//void CP_A(void *arg, uint32_t op)
//void RET_NZ(void *arg, uint32_t op)
//void POP_BC(void *arg, uint32_t op)
//void JP_NZ_a16(void *arg, uint32_t op)
//void JP_a16(void *arg, uint32_t op)
//void CALL_NZ_a16(void *arg, uint32_t op)
//void PUSH_BC(void *arg, uint32_t op)
//void ADD_A_d8(void *arg, uint32_t op)
//void RST_00H(void *arg, uint32_t op)
//void RET_Z(void *arg, uint32_t op)
void RET(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->pc = 0x00ff & cpu->mem[cpu->sp];
    cpu->sp++;
    cpu->pc += 0xff00 & cpu->mem[cpu->sp];
    cpu->sp++;
    
    //take the pc incrementation in the interpret function into account
    cpu->pc -= 1;
    
    cpu->cycles += 16;

}
//void JP_Z_a16(void *arg, uint32_t op)

void PREFIX_CB(void *arg, uint32_t op)
{
    s_emu *emu = arg;
    uint8_t cb_opcode = get_cb_opcode(op);
    (emu->cb_functions[cb_opcode] (emu, cb_opcode));
}

//void CALL_Z_a16(void *arg, uint32_t op)

void CALL_a16(void *arg, uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint16_t pc_old_value = cpu->pc + 3;
    cpu->sp--;
    cpu->mem[cpu->sp] = (pc_old_value & 0xff00) >> 8;
    cpu->sp--;
    cpu->mem[cpu->sp] = (pc_old_value & 0x00ff);
    cpu->pc = ((op & 0x0000ff00) >> 8) + ((op & 0x000000ff) << 8);
    //take the pc incrementation in the interpret function into account
    cpu->pc -= 3;
    cpu->cycles += 24;    
}

//void ADC_A_d8(void *arg, uint32_t op)
//void RST_08H(void *arg, uint32_t op)
//void RET_NC(void *arg, uint32_t op)
//void POP_DE(void *arg, uint32_t op)
//void JP_NC_a16(void *arg, uint32_t op)
//void dont_exist(void *arg, uint32_t op)
//void CALL_NC_a16(void *arg, uint32_t op)
//void PUSH_DE(void *arg, uint32_t op)
//void SUB_d8(void *arg, uint32_t op)
//void RST_10H(void *arg, uint32_t op)
//void RET_C(void *arg, uint32_t op)
//void RETI(void *arg, uint32_t op)
//void JP_C_a16(void *arg, uint32_t op)
//void dont_exist(void *arg, uint32_t op)
//void CALL_C_a16(void *arg, uint32_t op)
//void dont_exist(void *arg, uint32_t op)
//void SBC_A_d8(void *arg, uint32_t op)
//void RST_18H(void *arg, uint32_t op)
void LDH_derefa8_A(void *arg, uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->mem[0xFF00 + ((op & 0x0000FF00) >> 8)] = cpu->regA;
    cpu->cycles += 12;
}
//void POP_HL(void *arg, uint32_t op)

void LD_derefC_A(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    cpu->mem[0xFF00 + cpu->regC] = cpu->regA;
    cpu->cycles += 8;
}

//void dont_exist(void *arg, uint32_t op)
//void dont_exist(void *arg, uint32_t op)
//void PUSH_HL(void *arg, uint32_t op)
//void AND_d8(void *arg, uint32_t op)
//void RST_20H(void *arg, uint32_t op)
//void ADD_SP_r8(void *arg, uint32_t op)
//void JP_derefHL(void *arg, uint32_t op)
void LD_derefa16_A(void *arg, uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint16_t adress = ((op & 0x0000ff00) >> 8) + (op & 0x000000ff);
    cpu->mem[adress] = cpu->regA;
    
    cpu->cycles += 16;
}
//void dont_exist(void *arg, uint32_t op)
//void dont_exist(void *arg, uint32_t op)
//void dont_exist(void *arg, uint32_t op)
//void XOR_d8(void *arg, uint32_t op)
//void RST_28H(void *arg, uint32_t op)
//void LDH_A_derefa8(void *arg, uint32_t op)
//void POP_AF(void *arg, uint32_t op)
//void LD_A_derefC(void *arg, uint32_t op)
void DI(void *arg, UNUSED uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    flag_assign(false, &cpu->mem[0xFFFF], 0x1F);
    cpu->cycles += 4;
}
//void dont_exist(void *arg, uint32_t op)
//void PUSH_AF(void *arg, uint32_t op)
//void OR_d8(void *arg, uint32_t op)
//void RST_30H(void *arg, uint32_t op)
//void LD_HL_SPplusr8(void *arg, uint32_t op)
//void LD_SP_HL(void *arg, uint32_t op)
//void LD_A_derefa16(void *arg, uint32_t op)
//void EI(void *arg, uint32_t op)
//void dont_exist(void *arg, uint32_t op)
//void dont_exist(void *arg, uint32_t op)
void CP_d8(void *arg, uint32_t op)
{
    s_emu *emu = arg;
    s_cpu *cpu = &emu->cpu;
    
    uint8_t d8 = (op & 0x0000ff00) >> 8;
    uint8_t d4bit = d8 & 0x0F;
    uint8_t A4bit = (cpu->regA & 0x0F) + 0x10;
    bool half_carry = !(A4bit - d4bit);
    flag_assign(half_carry, &cpu->regF, 0x20);
    flag_assign(cpu->regA - d8 == 0, &cpu->regF, 0x80);
    flag_assign(true, &cpu->regF, 0x40);
    flag_assign(d8 > cpu->regA, &cpu->regF, 0x80);
    
    cpu->cycles += 8;
}
//void RST_38H(void *arg, uint32_t op)

