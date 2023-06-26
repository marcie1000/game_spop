#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>
#include "emulator.h"
#include "gb_interrupts.h"
#include "cpu.h"

void LCD_STAT_interrupt_flags(s_emu *emu)
{
    s_cpu *cpu = &emu->cpu;
    s_io *io = &cpu->io_reg;
    
    //io->IF &= ~0x02;
    static uint8_t oldSTAT = 0;
    static uint8_t oldLY = 0;
    //if interrupt not enabled
    if(!(io->IE & 0x02))
    {
        oldSTAT = io->STAT;
        oldLY = io->LY;
        return;
    }
        
    //for each STAT enable flags
    //LYC == LY ?
    if((io->STAT & 0x40) && (io->LYC == io->LY) && (oldLY != io->LY))
    {
        io->IF |= 0x02;
    }
    //PPU mode 2 ?
    else if((io->STAT & 0x20) && ((io->STAT & 0x03) == 2) && (oldSTAT != io->STAT))
    {
        io->IF |= 0x02;
    }
    //PPU mode 1 ?
    else if((io->STAT & 0x10) && ((io->STAT & 0x03) == 1) && (oldSTAT != io->STAT))
    {
        io->IF |= 0x02;
    }
    //PPU mode 0 ?
    else if((io->STAT & 0x08) && ((io->STAT & 0x03) == 0) && (oldSTAT != io->STAT))
    {
        io->IF |= 0x02;
    }
    
    oldSTAT = io->STAT;
    oldLY = io->LY;
}

void interrupt_handler(s_emu *emu)
{
    s_cpu *cpu = &emu->cpu;
    s_io *io_reg = &cpu->io_reg;
    
    if(!io_reg->IME && !cpu->in_halt)
        return;
    
//    cpu->quit_halt = false;
    LCD_STAT_interrupt_flags(emu);
    for(size_t i = 0; i <= 4; i++)
    {
        //check for each interrupt type
        //if not enabled
        if(!(io_reg->IE & (0x01 << i)))
            continue;
        //if no flag
        if(!(io_reg->IF & (0x01 << i)))
            continue;
        
        if(cpu->in_halt)
            cpu->quit_halt = true;
        
        if(!io_reg->IME)
            return;
        //disables interrupt when entering in interrupt routine
        io_reg->IME = false;
        //disables the IF flag
        io_reg->IF &= ~(0x01 << i);
        //push PC into stack
        cpu->sp--;
        if(0 != write_memory(emu, cpu->sp, (cpu->pc & 0xff00) >> 8))
            destroy_emulator(emu, EXIT_FAILURE);
        cpu->sp--;
        if(0 != write_memory(emu, cpu->sp, (cpu->pc & 0x00ff)))
            destroy_emulator(emu, EXIT_FAILURE);
        
        //load adress of interrupt handler in PC
        cpu->pc = 0x40 + i * 0x8;
//        printf("INTERRUPTION: pc = 0x%02X\n", cpu->pc);
        
        cpu->t_cycles += 20;
        //the loop stops at the higher priority interrupt
        return;
    }
}

void timer_handle(s_emu *emu)
{
    s_cpu *cpu = &emu->cpu;
    s_io *io_reg = &cpu->io_reg;
    
//    static Uint64 init_time = 0;
    static uint16_t old_timer = 0;
//    static uint8_t old_TIMA = 0;
    static bool overflow = false;
    //if not timer enable
    if(!(io_reg->TAC & 0x04))
    {
        //cpu->timer_clock = 0;
        old_timer = cpu->timer_clock;
        return;
    }
        
    unsigned clock_div;
    switch(io_reg->TAC & 0x03)
    {
        case 0:
            clock_div = 1024;
            break;
        case 1:
            clock_div = 16;
            break;
        case 2:
            clock_div = 64;
            break;
        case 3:
            clock_div = 256;
            break;
    }
    
    //if((cpu->timer_clock % clock_div == 0) && (cpu->timer_clock != 0))
    
    //when TIMA overflows, it becames equal to 0 for 4 cycles before taking
    //the TMA value.
//    if(overflow)
//    {
//        overflow = false;
//        io_reg->TIMA = io_reg->TMA;
//        io_reg->IF |= 0x04;
//    }
    
    
//    if((old_timer & (clock_div >> 1)) && !(cpu->timer_clock & (clock_div >> 1)))
//    {
//        if(io_reg->TIMA == 0xFF)
//        {
//            io_reg->TIMA = 0;
//            overflow = true;
//        }
//        else
//            io_reg->TIMA++;
////        cpu->timer_clock -= clock_div;
//    }
    
    uint16_t prev = old_timer;
    static uint16_t timer_when_overflow = 0;
    for(size_t i = old_timer + 1; i <= cpu->timer_clock; i++)
    {
        if(overflow && (i == timer_when_overflow + 4))
        {
            overflow = false;
            io_reg->TIMA = io_reg->TMA;
            io_reg->IF |= 0x04;
        }
        
        if((prev & (clock_div >> 1)) && !(i & (clock_div >> 1)))
        {
            if(io_reg->TIMA == 0xFF)
            {
                io_reg->TIMA = 0;
                overflow = true;
                timer_when_overflow = i;
            }
            else
                io_reg->TIMA++;
        }
        prev = i;
    }
    
    old_timer = cpu->timer_clock;
//    old_TIMA  = io_reg->TIMA;
}


