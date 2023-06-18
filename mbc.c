#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>
#include <assert.h>
#include "emulator.h"
#include "mbc.h"

int write_mbc_registers(s_emu *emu, uint16_t adress, uint8_t data)
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
            if(0 != mbc1_registers(emu, adress, data))
                return EXIT_FAILURE;
            break;
        default:
            fprintf(stderr, "WARNING: MBC registers unimplemented!\n");
            break;
    }
    
    return EXIT_SUCCESS;
}

int mbc1_registers(s_emu *emu, uint16_t adress, uint8_t data)
{
    s_mbc *mbc = &emu->cart.mbc;
    s_cart *cr = &emu->cart;
    if(adress <= 0x1FFF)
    {
        if(data == 0)
            mbc->RAM_enable = false;
        else if(data == 0x0A)
            mbc->RAM_enable = true;
    }
    else if((adress >= 0x2000) && (adress <= 0x3FFF))
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
    else if((adress >= 0x4000) && (adress <= 0x5FFF))
    {
        uint8_t ram_bank = data & 0x10;
        if(ram_bank < cr->sram_banks)
        {
            mbc->RAM_bank_number = ram_bank;
            emu->cpu.current_sram_bk = ram_bank;
        }
        
        //if 1 MiB ROM or superior
        if(cr->rom_banks >= 64)
        {
            mbc->ROM_bank_number &= ~0x60;
            mbc->ROM_bank_number |= data & 0x60;
            emu->cpu.cur_hi_rom_bk = mbc->ROM_bank_number;
        }
    }
    else if((adress >= 0x6000) && (adress <= 0x7FFF))
    {
        mbc->banking_mode_select = data;
        fprintf(stderr, "Banking mode 1 unimplemented!\n");
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}


