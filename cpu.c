#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL.h>
#include "cpu.h"
#include "emulator.h"
#include "opcodes.h"
#include "prefixed_opcodes.h"
#include "gb_interrupts.h"
#include "graphics.h"
#include "audio.h"
#include "mbc.h"

int write_io_registers(s_emu *emu, uint16_t address, uint8_t data)
{
    s_cpu *cpu = &emu->cpu;
    s_io *io = &cpu->io;
    s_screen *scr = &emu->scr;
    s_audio *au = &emu->au;
    
    switch(address)
    {
        case 0xFF00:
            flag_assign(data & 0x20, &io->P1_JOYP, 0x20);
            flag_assign(data & 0x10, &io->P1_JOYP, 0x10);
            break;
        case 0xFF01:
            io->SB = data;
            break;
        case 0xFF02:
            io->SC = data;
            break;
        case 0xFF04:
            io->DIV = 0;
            cpu->timer_clock = 0;
            break;
        case 0xFF05:
            io->TIMA = data;
            break;
        case 0xFF06:
            io->TMA = data;
            break;
        case 0xFF07:
            io->TAC = data & 0x07;
            break;
        case 0xFF0F:
            io->IF = data;
            break;
        case 0xFF10:
            io->NR10 = data;
            break;
        case 0xFF11:
            io->NR11 = data;
            au->ch_duty_ratio[CH1]      = (io->NR11 & 0xC0) >> 6;
            au->ch_init_len_timer[CH1]  = (io->NR11 & 0x3F);
            break;
        case 0xFF12:
            io->NR12 = data;
            au->ch_init_volume[CH1]     = (io->NR12 & 0xF0) >> 4;
            au->ch_envl_dir[CH1]        = (io->NR12 & 0x08);
            au->ch_vol_sweep_pace[CH1]  = (io->NR12 & 0x07);
            break;
        case 0xFF13:
            io->NR13 = data;
            break;
        case 0xFF14:
            io->NR14 = data & 0xC7;
            au->ch_trigger[CH1] = (io->NR14 & 0x80);
            au->ch_sound_len_enable[CH1] = (io->NR14 & 0x40);
            break;
        case 0xFF16:
            io->NR21 = data;
            au->ch_duty_ratio[CH2]      = (io->NR21 & 0xC0) >> 6;
            au->ch_init_len_timer[CH2]  = (io->NR21 & 0x3F);
            break;
        case 0xFF17:
            io->NR22 = data;
            au->ch_init_volume[CH2]     = (io->NR22 & 0xF0) >> 4;
            au->ch_envl_dir[CH2]        = (io->NR22 & 0x08);
            au->ch_vol_sweep_pace[CH2]  = (io->NR22 & 0x07);
            break;
        case 0xFF18:
            io->NR23 = data;
            break;
        case 0xFF19:
            io->NR24 = data & 0xC7;
            au->ch_trigger[CH2] = (io->NR24 & 0x80);
            au->ch_sound_len_enable[CH2] = (io->NR24 & 0x40);
            break;
        case 0xFF1A:
            io->NR30 = data;
            au->ch3_dac_enable = data & 0x80;
            break;
        case 0xFF1B:
            io->NR31 = data;
            au->ch_init_len_timer[CH3] = data;
            break;
        case 0xFF1C:
            io->NR32 = data;
            au->ch3_output_level = (data & 0x60) >> 5;
            break;
        case 0xFF1D:
            io->NR33 = data;
            break;
        case 0xFF1E:
            io->NR34 = data;
            au->ch_trigger[CH3] = data & 0x80;
            au->ch_sound_len_enable[CH3] = data & 0x40;
            break;
        case 0xFF20:
            io->NR41 = data;
            au->ch_init_len_timer[CH4] = data & 0x3F;
            break;
        case 0xFF21:
            io->NR42 = data;
            au->ch_init_volume[CH4] = (data & 0xF0) >> 4;
            au->ch_envl_dir[CH4] = data & 0x08;
            au->ch_vol_sweep_pace[CH4] = data & 0x07;
            break;
        case 0xFF22:
            io->NR43 = data;
            au->ch4_clock_shift = (data & 0xF0) >> 4;
            au->ch4_lfsr_w      =  data & 0x08;
            au->ch4_clock_div   =  data & 0x07;
            break;
        case 0xFF23:
            io->NR44 = data;
            au->ch_trigger[CH4] = data & 0x80;
            au->ch_sound_len_enable[CH4] = data & 0x40;
            break;
        case 0xFF24:
            io->NR50 = data;
            au->VIN_l = (io->NR50 & 0x80);
            au->VIN_r = (io->NR50 & 0x08);
            au->l_output_vol = (io->NR50 & 0x70) >> 4;
            au->r_output_vol = (io->NR50 & 0x07);
            break;
        case 0xFF25:
            io->NR51 = data;
            au->ch_l[CH1] = (io->NR51 & 0x10);
            au->ch_r[CH1] = (io->NR51 & 0x01);
            au->ch_l[CH2] = (io->NR51 & 0x20);
            au->ch_r[CH2] = (io->NR51 & 0x02);
            au->ch_l[CH3] = (io->NR51 & 0x40);
            au->ch_r[CH3] = (io->NR51 & 0x04);
            au->ch_l[CH4] = (io->NR51 & 0x80);
            au->ch_r[CH4] = (io->NR51 & 0x08);
            break;
        case 0xFF26:
            flag_assign(data & 0x80, &io->NR52, 0x80);
            au->apu_enable = data & 0x80;
            break;
        case 0xFF30:
        case 0xFF31:
        case 0xFF32:
        case 0xFF33:
        case 0xFF34:
        case 0xFF35:
        case 0xFF36:
        case 0xFF37:
        case 0xFF38:
        case 0xFF39:
        case 0xFF3A:
        case 0xFF3B:
        case 0xFF3C:
        case 0xFF3D:
        case 0xFF3E:
        case 0xFF3F:
            io->wave_RAM[address - 0xFF30] = data;
            break;
        case 0xFF40:
        {
            io->LCDC = data;
                
            scr->LCD_PPU_enable          = io->LCDC & 0x80;
            scr->win_tile_map_area       = io->LCDC & 0x40;
            scr->window_enable           = io->LCDC & 0x20;
            scr->BG_win_tile_data_area   = io->LCDC & 0x10;
            scr->BG_tile_map_area        = io->LCDC & 0x08;
            scr->obj_size                = io->LCDC & 0x04;
            scr->obj_enable              = io->LCDC & 0x02;
            scr->bg_win_enable_priority  = io->LCDC & 0x01;
            
            break;
        }
        case 0xFF41:
            io->STAT = data & 0x78;
            break;
        case 0xFF42:
            io->SCY = data;
            break;
        case 0xFF43:
            io->SCX = data;
            break;
        case 0xFF44:
            fprintf(stderr, COLOR_RED "ERROR: attempt to write in I/O register"
                    " LY (0xFF44), read only!\n" COLOR_RESET);
            /* return EXIT_FAILURE; */
            break;
        case 0xFF45:
            io->LYC = data;
            break;
        case 0xFF46:
            io->DMA = data;
            if(0 != DMA_transfer(emu))
                return EXIT_FAILURE;
            break;
        case 0xFF47:
            io->BGP = data;
            break;
        case 0xFF48:
            io->OBP0 = data;
            break;
        case 0xFF49:
            io->OBP1 = data;
            break;
        case 0xFF4A:
            io->WY = data;
            break;
        case 0xFF4B:
            io->WX = data;
            break;
        case 0xFF4D:
            break;
        case 0xFF50:
            io->BANK = data;
            memcpy(&cpu->ROM_Bank[0][0], cpu->ROM_Bank_0_tmp, sizeof(uint8_t[ROM_BANK_SIZE]));
            break;
        case 0xFF7F:
            break;
        default:
            fprintf(stderr, COLOR_RED "WARNING: attempt to write I/O register"
                    " at address 0x%04X (unimplemented!)\n" COLOR_RESET, address);
            /* SDL_Delay(3000); */
            /* return EXIT_FAILURE; */
            break;
    }
    
    return EXIT_SUCCESS;
}

int read_io_registers(s_emu *emu, uint16_t address, uint8_t *data)
{
    s_cpu *cpu = &emu->cpu;
    s_io *io = &cpu->io;
    
    switch(address)
    {
        case 0xFF00:
            *data = io->P1_JOYP;
            break;
        case 0xFF01:
            *data = io->SB;
            break;
        case 0xFF02:
            *data = io->SC;
            break;
        case 0xFF04:
            *data = io->DIV;
            break;
        case 0xFF05:
            *data = io->TIMA;
            break;
        case 0xFF06:
            *data = io->TMA;
            break;
        case 0xFF07:
            *data = io->TAC & 0x07;
            break;
        case 0xFF0F:
            *data = io->IF;
            break;
        case 0xFF10:
            *data = io->NR10;
            break;
        case 0xFF11:
            *data = io->NR11 & 0xC0;
            break;
        case 0xFF12:
            *data = io->NR12;
            break;
        case 0xFF13:
            fprintf(stderr, COLOR_RED "ERROR: attempt to read at address FF13,"
                    " NR13 I/O register (write only)\n" COLOR_RESET);
            /* return EXIT_FAILURE; */
            break;
        case 0xFF14:
            *data = 0x40 & io->NR14;
            break;
        case 0xFF16:
            *data = io->NR21 & 0xC0;
            break;
        case 0xFF17:
            *data = io->NR22;
            break;
        case 0xFF18:
            fprintf(stderr, COLOR_RED "ERROR: attempt to read at address FF18,"
                    " NR23 I/O register (write only)\n" COLOR_RESET);
            /* return EXIT_FAILURE; */
            break;
        case 0xFF19:
            *data = 0x40 & io->NR24;
            break;
        case 0xFF1A:
            *data = io->NR30;
            break;
        case 0xFF1B:
            fprintf(stderr, COLOR_RED "ERROR: attempt to read at address FF1B,"
                    " NR31 I/O register (write only)\n" COLOR_RESET);
            /* return EXIT_FAILURE; */
            break;
        case 0xFF1C:
            *data = io->NR32;
            break;
        case 0xFF1D:
            fprintf(stderr, COLOR_RED "ERROR: attempt to read at address FF1D,"
                    " NR33 I/O register (write only)\n" COLOR_RESET);
            /* return EXIT_FAILURE; */
            break;
        case 0xFF1E:
            *data = io->NR34 & 0x40;
            break;
        case 0xFF20:
            fprintf(stderr, COLOR_RED "ERROR: attempt to read at address FF1D,"
                    " NR33 I/O register (write only)\n" COLOR_RESET);
            /* return EXIT_FAILURE; */
            break;
        case 0xFF21:
            *data = io->NR42;
            break;
        case 0xFF22:
            *data = io->NR43;
            break;
        case 0xFF23:
            *data = io->NR44 & 0x40;
            break;
        case 0xFF24:
            *data = io->NR50;
            break;
        case 0xFF25:
            *data = io->NR51;
            break;
        case 0xFF26:
            *data = io->NR52;
            break;
        case 0xFF30:
        case 0xFF31:
        case 0xFF32:
        case 0xFF33:
        case 0xFF34:
        case 0xFF35:
        case 0xFF36:
        case 0xFF37:
        case 0xFF38:
        case 0xFF39:
        case 0xFF3A:
        case 0xFF3B:
        case 0xFF3C:
        case 0xFF3D:
        case 0xFF3E:
        case 0xFF3F:
            *data = io->wave_RAM[address - 0xFF30];
            break;
        case 0xFF40:
            *data = io->LCDC;
            break;
        case 0xFF41:
            *data = io->STAT;
            break;
        case 0xFF42:
            *data = io->SCY;
            break;
        case 0xFF43:
            *data = io->SCX;
            break;
        case 0xFF44:
            if(!emu->opt.gb_doctor)
                *data = io->LY;
            //hardcode for gb_doctor
            else
                *data = 0x90;
            break;
        case 0xFF45:
            *data = io->LYC;
            break;
        case 0xFF46:
            *data = io->DMA;
            break;
        case 0xFF47:
            *data = io->BGP;
            break;
        case 0xFF48:
            *data = io->OBP0;
            break;
        case 0xFF49:
            *data = io->OBP1;
            break;
        case 0xFF4A:
            *data = io->WY;
            break;
        case 0xFF4B:
            *data = io->WX;
            break;
        case 0xFF4D:
            *data = 0;
            break;
        default:
            fprintf(stderr, COLOR_RED "WARNING: attempt to read I/O register"
                    " at address 0x%04X (unimplemented!)\n" COLOR_RESET, address);
            /* SDL_Delay(3000); */
            /* return EXIT_FAILURE; */
            break;
    }
    
    return EXIT_SUCCESS;
}

int write_memory(s_emu *emu, uint16_t address, uint8_t data)
{
    s_cpu *cpu = &emu->cpu;
    
    if((address < 0x3FFF) /* && (address != 0x2000) && (address != 0x1B08) && (cpu->pc != 0x0254)*/)
    {
        if(0 != write_mbc_registers(emu, address, data))
            return EXIT_FAILURE;
    }
    else if(/* (address >= 0x4000) && */ (address <= 0x7FFF))
    {
        if(0 != write_mbc_registers(emu, address, data))
            return EXIT_FAILURE;
    }
    //VRAM
    else if(/* (address >= 0x8000) && */ (address <= 0x9FFF))
    {
        cpu->VRAM[address - 0x8000] = data;
    }
    //8 KiB External RAM 
    else if(/* (address >= 0xA000) && */ (address <= 0xBFFF))
    {
        if(0 != write_external_RAM(emu, address, data))
            return EXIT_FAILURE;
    }
    //WRAM
    else if(/* (address >= 0xC000) && */ (address <= 0xDFFF))
    {
        cpu->WRAM[address - 0xC000] = data;
    }
    //ECHO RAM
    else if(/* (address >= 0xE000) && */ (address <= 0xFDFF))
    {
        fprintf(stderr, COLOR_RED "WARNING: attempt to write in ECHO RAM at address 0x%04X (prohibited)\n" COLOR_RESET, address);
        //return EXIT_FAILURE;
    }
    //sprite attribute table (OAM)
    else if(/* (address >= 0xFE00) && */ (address <= 0xFE9F))
    {
        cpu->OAM[address - 0xFE00] = data;
    }
    else if(/* (address >= 0xFEA0) && */ (address <= 0xFEFF))
    {
        if(data != 0)
        {
            fprintf(stderr, COLOR_RED "ERROR: attempt to write at address 0x%04X (prohibited)\n" COLOR_RESET, address);
            /* return EXIT_FAILURE; */
        }
    }
    else if(/* (address >= 0xFF00) && */ (address <= 0xFF7F))
    {
        if(0 != write_io_registers(emu, address, data))
            return EXIT_FAILURE;
    }    
    //HRAM
    else if(/* (address >= 0xFF80) && */ (address <= 0xFFFE))
    {
        cpu->HRAM[address - 0xFF80] = data;
    }
    else if(address == 0xFFFF)
    {
        cpu->io.IE = data;
    }
    
//    if(emu->opt.debug_info)
//    {
//        printf(COLOR_YELLOW "MEMORY: WRITE 0x%02X AT ADRESS 0x%04X" COLOR_RESET "\n", data, address);
//    }
    
    return EXIT_SUCCESS;
}

int read_memory(s_emu *emu, uint16_t address, uint8_t *data)
{
    s_cpu *cpu = &emu->cpu;

    if(address <= 0x3FFF)
    {
        *data = cpu->ROM_Bank[cpu->cur_low_rom_bk][address];
    }
    else if(/* (address >= 0x4000) && */ (address <= 0x7FFF))
    {
        *data = cpu->ROM_Bank[cpu->cur_hi_rom_bk][address - 0x4000];
    }
    //VRAM
    else if(/* (address >= 0x8000) && */ (address <= 0x9FFF))
    {
        *data = cpu->VRAM[address - 0x8000];
    }
    //8 KiB External RAM 
    else if(/* (address >= 0xA000) && */ (address <= 0xBFFF))
    {
        if(0 != read_external_RAM(emu, address, data))
            return EXIT_FAILURE;
    }
    //WRAM
    else if(/* (address >= 0xC000) && */ (address <= 0xDFFF))
    {
        *data = cpu->WRAM[address - 0xC000];
    }
    //ECHO RAM
    else if(/* (address >= 0xE000) && */ (address <= 0xFDFF))
    {
        fprintf(stderr, COLOR_RED "WARNING: attempt to read in ECHO RAM at address 0x%04X (prohibited)\n" COLOR_RESET, address);
    }
    //sprite attribute table (OAM)
    else if(/* (address >= 0xFE00) && */ (address <= 0xFE9F))
    {
        *data = cpu->OAM[address - 0xFE00];
    }
    else if(/* (address >= 0xFEA0) && */ (address <= 0xFEFF))
    {
        fprintf(stderr, COLOR_RED "ERROR: attempt to read at address 0x%04X (prohibited)\n" COLOR_RESET, address);
        /* return EXIT_FAILURE; */
    }
    else if(/* (address >= 0xFF00) && */ (address <= 0xFF7F))
    {
        if(0 != read_io_registers(emu, address, data))
            return EXIT_FAILURE;
    }    
    //HRAM
    else if(/* (address >= 0xFF80) && */ (address <= 0xFFFE))
    {
        *data = cpu->HRAM[address - 0xFF80];
    }
    else if(address == 0xFFFF)
    {
        *data = cpu->io.IE;
    }
    
//    if(emu->opt.debug_info)
//    {
//        printf(COLOR_YELLOW "MEMORY: READ 0x%02X FROM ADRESS 0x%04X" COLOR_RESET "\n", *data, address);
//    }
    
    return EXIT_SUCCESS;
}

int initialize_cpu(s_cpu *cpu)
{
    memset(cpu, 0, sizeof(s_cpu));
    cpu->ROM_Bank = NULL;
    
    //allocate space for ROM banks
    cpu->ROM_Bank = malloc(sizeof(uint8_t*[ROM_BANKS_MAX]));
    if(NULL == cpu->ROM_Bank)
    {
        perror("malloc (in fct initialize_cpu): ");
        return EXIT_FAILURE;
    }
    
    for(size_t i = 0; i < ROM_BANKS_MAX; i++)
        cpu->ROM_Bank[i] = NULL;
    
    //create each bank
    for(size_t i = 0; i < ROM_BANKS_MAX; i++)
    {
        cpu->ROM_Bank[i] = malloc(sizeof(uint8_t[ROM_BANK_SIZE]));
        if(NULL == cpu->ROM_Bank[i])
        {
            perror("calloc (in fct initialize_cpu): ");
            return EXIT_FAILURE;
        }
        memset(cpu->ROM_Bank[i], 0xFF, sizeof(uint8_t[ROM_BANK_SIZE]));
    }
    
    cpu->pc = START_ADRESS;
    cpu->io.P1_JOYP = 0xEF;
    cpu->cur_hi_rom_bk = 1;
    
//    memset(cpu->SRAM, 0xFF, sizeof(uint8_t[SRAM_BANKS_MAX][EXTERNAL_RAM_SIZE]));
    
    return EXIT_SUCCESS;
}

/**
 * @brief Reads the 3 next bytes of program instructions. 
 * @param emu
 */
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

/**
 * @brief Returns the single byte instruction opcode.
 * @param opcode
 */
uint8_t get_action(uint32_t opcode)
{
    return (opcode & 0xFF0000) >> 16;
}

uint8_t get_cb_opcode(uint32_t op32)
{
    return (op32 & 0x0000FF00) >> 8;
}

void breakpoint_handle(s_emu *emu, uint8_t action)
{
    if(!emu->opt.breakpoints)
        return;
    s_cpu *cpu = &emu->cpu;
    
    if(cpu->pc <= emu->opt.breakpoint_value &&
       cpu->pc + emu->length_table[action] >= emu->opt.breakpoint_value)
    {
        printf(COLOR_GREEN "Breakpoint 0x%04X reached!" COLOR_RESET "\n", emu->opt.breakpoint_value);
        ask_breakpoint(&emu->opt);
    }
}

void step_by_step_handle(s_emu *emu)
{
    if(!emu->opt.step_by_step)
        return;
    ask_breakpoint(&emu->opt);
}

void div_handle(s_emu *emu)
{
    s_cpu *cpu = &emu->cpu;
    static uint8_t old_DIV = 0;

    cpu->io.DIV = (cpu->timer_clock & 0xFF00) >> 8;

    if((old_DIV & 0x10) && !(cpu->io.DIV & 0x10))
    {
        emu->au.DIV_APU++;
    }
    
    old_DIV = cpu->io.DIV;
}

void interpret(s_emu *emu, void (*opcode_functions[OPCODE_NB])(void *, uint32_t))
{
    s_cpu *cpu = &emu->cpu;

    pause_menu(emu);

    joypad_update(emu);
    interrupt_handler(emu);
    uint32_t opcode = get_opcode(emu);
    uint8_t action = get_action(opcode);
    breakpoint_handle(emu, action);
    step_by_step_handle(emu);
    log_instructions(emu);
    
    size_t t_cycles_old = cpu->t_cycles;

    (*opcode_functions[action])(emu, opcode);
    
    size_t time_diff = cpu->t_cycles - t_cycles_old;
    cpu->timer_clock += time_diff;
    cpu->debug_clock += time_diff;
    emu->au.samples_timer += time_diff;

    draw_plot(emu);

    cpu->pc += emu->length_table[action];
    
    timer_handle(emu);
    div_handle(emu);
}



