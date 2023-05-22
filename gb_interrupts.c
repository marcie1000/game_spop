#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>
#include "emulator.h"
#include "gb_interrupts.h"
#include "cpu.h"

void LCD_STAT_interrupt_flags(s_emu *emu)
{
    s_cpu *cpu = &emu->cpu;
    s_io *io_reg = &cpu->io_reg;
    
    io_reg->IF &= ~0x02;
    
    //if interrupt not enabled
    if(!(io_reg->IE & 0x02))
        return;
        
    //for each STAT enable flags
    //LYC == LY ?
    if((io_reg->STAT & 0x40) && (io_reg->LYC == io_reg->LY))
    {
        io_reg->IF |= 0x02;
        return;
    }
    //PPU mode 2 ?
    else if((io_reg->STAT & 0x20) && ((io_reg->STAT & 0x03) == 2))
    {
        io_reg->IF |= 0x02;
        return;
    }
    //PPU mode 1 ?
    else if((io_reg->STAT & 0x10) && ((io_reg->STAT & 0x03) == 1))
    {
        io_reg->IF |= 0x02;
        return;
    }
    //PPU mode 0 ?
    else if((io_reg->STAT & 0x08) && ((io_reg->STAT & 0x03) == 0))
    {
        io_reg->IF |= 0x02;
        return;
    }
}

void interrupt_handler(s_emu *emu)
{
    s_cpu *cpu = &emu->cpu;
    s_io *io_reg = &cpu->io_reg;
    if(!io_reg->IME)
        return;
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
            
        //push PC into stack
        cpu->sp--;
        if(0 != write_memory(emu, cpu->sp, (cpu->pc & 0xff00) >> 8))
            destroy_emulator(emu, EXIT_FAILURE);
        cpu->sp--;
        if(0 != write_memory(emu, cpu->sp, (cpu->pc & 0x00ff)))
            destroy_emulator(emu, EXIT_FAILURE);
        
        //load adress of interrupt handler in PC
        cpu->pc = 0x40 + i * 0x8;
        printf("INTERRUPTION: pc = %u\n", cpu->pc);
        
        cpu->t_cycles += 20;
        //the loop stops at the higher priority interrupt
        return;
    }
}

