#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>
#include <assert.h>
#include "emulator.h"
#include "mbc.h"

int write_mbc_registers(s_emu *emu, uint16_t address, uint8_t data)
{
    s_cart *cr = &emu->cart;
    switch(cr->type)
    {
        case ROM_ONLY:
            return EXIT_SUCCESS;
            break;
        case MBC1:
        case MBC1_P_RAM:
        case MBC1_P_RAM_P_BATT:
            if(0 != mbc1_registers(emu, address, data))
                return EXIT_FAILURE;
            break;
        case MBC2:
        case MBC2_P_BATT:
            if(0 != mbc2_registers(emu, address, data))
                return EXIT_FAILURE;
            break;
        case MBC3:
        case MBC3_P_RAM:
        case MBC3_P_RAM_P_BATT:
        case MBC3_P_TIMER_P_BATT:
        case MBC3_P_TIMER_P_RAM_P_BATT:
            if(0 != mbc3_registers(emu, address, data))
                return EXIT_FAILURE;
            break;
        default:
            fprintf(stderr, "WARNING: MBC registers unimplemented!\n");
            break;
    }
    
    return EXIT_SUCCESS;
}

int mbc3_registers(s_emu *emu, uint16_t address, uint8_t data)
{

    return EXIT_SUCCESS;
}

int mbc2_registers(s_emu *emu, uint16_t address, uint8_t data)
{
    s_mbc *mbc = &emu->cart.mbc;
    s_cart *cr = &emu->cart;
    s_cpu *cpu = &emu->cpu;

    if(address & 0x0100) // ROM
    {
        cpu->cur_hi_rom_bk = data & 0x0F;
        if(cpu->cur_hi_rom_bk == 0)
            cpu->cur_hi_rom_bk = 1;
    }
    else // RAM
    {
        if(data == 0x0A)
            mbc->RAM_enable = true;
        else
            mbc->RAM_enable = false;
    }
    return EXIT_SUCCESS;
}

int mbc1_registers(s_emu *emu, uint16_t address, uint8_t data)
{
    s_mbc *mbc = &emu->cart.mbc;
    s_cart *cr = &emu->cart;
    s_cpu *cpu = &emu->cpu;
    
    //RAM Enable (Write Only)
    if(address <= 0x1FFF)
    {
        if(data == 0)
            mbc->RAM_enable = false;
        else if(data == 0x0A && cr->sram_banks > 0)
        {
            mbc->RAM_enable = true;
        }
    }
    
    //ROM Bank Number (Write Only)
    else if((address >= 0x2000) && (address <= 0x3FFF))
    {
        uint8_t reg5bit = data & 0x1F;
        uint8_t bank = reg5bit;
        if(reg5bit >= cr->rom_banks)
        {
            fprintf(stderr, "WARNING: reg5bit >= cr->rom_banks!!\n");
            return EXIT_FAILURE;
        }
        if(bank == 0)
            bank = 1;
            
        //if 1 MiB ROM or superior, keep upper bits of rom bank number
        if(cr->rom_banks >= 64)
        {
            mbc->ROM_bank_number &= ~0x1F;
            mbc->ROM_bank_number |= bank;
        }
        else
            mbc->ROM_bank_number = bank;
            
        emu->cpu.cur_hi_rom_bk = mbc->ROM_bank_number;
    }
    
    //RAM Bank Number — or — Upper Bits of ROM Bank Number (Write Only)
    else if((address >= 0x4000) && (address <= 0x5FFF))
    {
        uint8_t ram_bank = data & 0x10;

        //if 1 MiB ROM or superior
        if(cr->rom_banks >= 64)
        {
            mbc->ROM_bank_number &= ~0x60;
            mbc->ROM_bank_number |= data & 0x60;
            cpu->cur_hi_rom_bk = mbc->ROM_bank_number;
        }
        
        if(mbc->banking_mode_select)
        {
            if(ram_bank < cr->sram_banks)
            {
                mbc->RAM_bank_number = ram_bank;
                emu->cpu.current_sram_bk = ram_bank;
            }
            cpu->cur_low_rom_bk = mbc->ROM_bank_number & 0x60;
        }
        else
        {
            cpu->cur_low_rom_bk = 0;
            cpu->current_sram_bk = 0;
        }
    }
    
    //Banking Mode Select (Write Only)
    else if((address >= 0x6000) && (address <= 0x7FFF))
    {
        mbc->banking_mode_select = data;

        uint8_t ram_bank = mbc->ROM_bank_number & 0x60;

        if(mbc->banking_mode_select)
        {
            if(ram_bank < cr->sram_banks)
            {
                mbc->RAM_bank_number = ram_bank;
                emu->cpu.current_sram_bk = ram_bank;
            }
            cpu->cur_low_rom_bk = mbc->ROM_bank_number & 0x60;
        }
        else
        {
            cpu->cur_low_rom_bk = 0;
            cpu->current_sram_bk = 0;
        }
    }
    
    return EXIT_SUCCESS;
}

int write_external_RAM(s_emu *emu, uint16_t address, uint8_t data)
{
    s_cpu *cpu = &emu->cpu;
    s_mbc *mbc = &emu->cart.mbc;

    if(emu->opt.rom_argument && emu->cart.mbc.RAM_enable)
    {
        if(!(emu->cart.type == MBC2 || emu->cart.type == MBC2_P_BATT))
            cpu->SRAM[cpu->current_sram_bk][address - 0xA000] = data;
        else
        {
            uint16_t relative = (address - 0xA000) % 0x200;
            cpu->SRAM[0][relative] = data & 0x0F;
        }
    }
    return EXIT_SUCCESS;
}

int read_external_RAM(s_emu *emu, uint16_t address, uint8_t *data)
{
    s_cpu *cpu = &emu->cpu;
    s_mbc *mbc = &emu->cart.mbc;

    if(emu->cart.mbc.RAM_enable)
    {
        if(emu->cart.type == MBC2 || emu->cart.type == MBC2_P_BATT)
        {
            uint16_t relative = (address - 0xA000) % 0x200;
            *data = cpu->SRAM[0][relative];
        }
        // read RTC registers instead of SRAM
        else if(emu->cart.type == MBC3_P_TIMER_P_BATT ||
                emu->cart.type == MBC3_P_TIMER_P_RAM_P_BATT)
        {
            switch(cpu->current_sram_bk)
            {
                case 0x00:
                case 0x01:
                case 0x02:
                case 0x03:
                    *data = cpu->SRAM[cpu->current_sram_bk][address - 0xA000];
                    break;
                case 0x08:
                    *data = mbc->RTC_S;
                    break;
                case 0x09:
                    *data = mbc->RTC_M;
                    break;
                case 0x0A:
                    *data = mbc->RTC_H;
                    break;
                case 0x0B:
                    *data = mbc->RTC_DL;
                    break;
                case 0x0C:
                    *data = mbc->RTC_DH;
                    break;
                default:
                    fprintf(stderr, COLOR_RED "ERROR: MBC3: Invalid SRAM bank / RTC register (0x%02X)\n" COLOR_RESET, cpu->current_sram_bk);
                    return EXIT_FAILURE;
                    break;
            }
        }
        else
            *data = cpu->SRAM[cpu->current_sram_bk][address - 0xA000];
    }
    else
        *data = 0;
    return EXIT_SUCCESS;
}
