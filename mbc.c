#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>
#include <string.h>
#include <time.h>
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

int rtc_latch(s_emu *emu, uint16_t address, uint8_t data)
{
    s_cart *cr = &emu->cart;
    s_cpu *cpu = &emu->cpu;

    bool canlatch = false;

    // Latch is done if $00 and then $01 is written here
    if(cr->latch_register == 0 && data == 1 && cr->has_RTC)
        canlatch = true;

    cr->latch_register = data;

    if(!canlatch)
        return EXIT_SUCCESS;

    time_t cur = time(NULL);
    if(cur == (time_t)(-1))
    {
        perror("MBC3: couldn't get current time: ");
        return EXIT_FAILURE;
    }

    if(cr->epoch == 0)
        cr->epoch = (uintmax_t)cur;

    // if halt
    if(cr->rtc_internal.RTC_DH & 0x40)
    {
        memcpy(&cr->rtc_latched, &cr->rtc_internal, sizeof(s_rtc));
        // write current time to saved epoch
        cr->epoch = (uintmax_t)cur;

        printf("Latched: DH=%u DL=%u H=%u M=%u S=%u Carry=%u Halt=%u\n", cr->rtc_latched.RTC_DH,
        cr->rtc_latched.RTC_DL, cr->rtc_latched.RTC_H, cr->rtc_latched.RTC_M, cr->rtc_latched.RTC_S, cr->rtc_latched.RTC_DH & 0x80,
        cr->rtc_latched.RTC_DH & 0x40);
        return EXIT_SUCCESS;
    }

    unsigned days = 0;
    unsigned hours = 0;
    unsigned min = 0;
    unsigned sec = 0;

    // and not HALT
    if(cr->epoch != 0)
    {
        // compare old and new date
        uintmax_t delta = (uintmax_t)cur - cr->epoch;
        unsigned delta_days = delta / 86400; // 86400 seconds in a day
        delta -= delta_days * 86400;
        unsigned delta_hours = delta / 3600; // 3600 seconds in an hour
        delta -= delta_hours * 3600;
        unsigned delta_min = delta / 60; // Come on!
        delta -= delta_min * 60;
        unsigned delta_sec = delta;

        // store current epoch as he last latched epoch
        cr->epoch = (uintmax_t)cur;

        // calculate new real values
        days = (cr->rtc_internal.RTC_DH << 8) + cr->rtc_internal.RTC_DL + delta_days;
        hours = cr->rtc_internal.RTC_H + delta_hours;
        min = cr->rtc_internal.RTC_M + delta_min;
        sec = cr->rtc_internal.RTC_S + delta_sec;

        min += sec / 60;
        sec = sec % 60;

        hours += min / 60;
        min = min % 60;

        days += hours / 24;
        hours = hours % 24;
    }

    // store new values
    flag_assign(days & 0x0100, &cr->rtc_internal.RTC_DH, 0x01);
    cr->rtc_internal.RTC_DL = days & 0x00FF;
    cr->rtc_internal.RTC_H = hours;
    cr->rtc_internal.RTC_M = min;
    cr->rtc_internal.RTC_S = sec;

    // flags:
    // carry
    flag_assign(~(days & 0x01FF), &cr->rtc_internal.RTC_DH, 0x80);

    /* fprintf(stderr, COLOR_RED "WARNING: MBC3 Latch Clock Data 6000-7FFF not implemented!\n" COLOR_RESET); */
    /* return EXIT_FAILURE; */

    memcpy(&cr->rtc_latched, &cr->rtc_internal, sizeof(s_rtc));
    printf("Latched: DH=%u DL=%u H=%u M=%u S=%u Carry=%u Halt=%u\n", cr->rtc_latched.RTC_DH,
        cr->rtc_latched.RTC_DL, cr->rtc_latched.RTC_H, cr->rtc_latched.RTC_M, cr->rtc_latched.RTC_S, cr->rtc_latched.RTC_DH & 0x80,
        cr->rtc_latched.RTC_DH & 0x40);

    return EXIT_SUCCESS;
}

int mbc3_registers(s_emu *emu, uint16_t address, uint8_t data)
{
    s_cart *cr = &emu->cart;
    s_cpu *cpu = &emu->cpu;

    //RAM and Timer Enable (Write Only)
    if(address <= 0x1FFF)
    {
        if(data == 0)
        {
            cr->RAM_enable = false;
            cr->RTC_enable = false;
        }
        else if(data == 0x0A)
        {
            if(cr->sram_banks > 0)
                cr->RAM_enable = true;
            if(cr->has_RTC)
                cr->RTC_enable = true;
        }
    }

    //ROM Bank Number (Write Only)
    else if(/* (address >= 0x2000) && */ (address <= 0x3FFF))
    {
        assert((data & 0x7F) < cr->rom_banks);
        cpu->cur_hi_rom_bk = data & 0x7F;
        if (cpu->cur_hi_rom_bk == 0)
            cpu->cur_hi_rom_bk = 1;
    }

    //RAM Bank Number — or — RTC Register Select (Write Only)
    else if(/* (address >= 0x4000) && */ (address <= 0x5FFF))
    {
        if(!cr->has_RTC)
        {
            if(data <= 0x03)
                cpu->current_sram_bk = data;
            else
            {
                fprintf(stderr, COLOR_YELLOW "PANIC! mbc3_registers, data = 0x%02X\n" COLOR_RESET, data);
                return EXIT_FAILURE;
            }
        }
        else
        {
            if(data <= 0x03 || (data >= 0x08 && data <= 0x0C))
            {
                printf("Select 0x%02X bank\n", data);
                cpu->current_sram_bk = data;
            }
        }
    }

    //TODO Latch Clock Data (Write Only)
    else if(/* (address >= 0x6000) && */ (address <= 0x7FFF))
    {
        if(0 != rtc_latch(emu, address, data))
            return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int mbc2_registers(s_emu *emu, uint16_t address, uint8_t data)
{
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
            cr->RAM_enable = true;
        else
            cr->RAM_enable = false;
    }
    return EXIT_SUCCESS;
}

int mbc1_registers(s_emu *emu, uint16_t address, uint8_t data)
{
    s_cart *cr = &emu->cart;
    s_cpu *cpu = &emu->cpu;
    
    //RAM Enable (Write Only)
    if(address <= 0x1FFF)
    {
        if(data == 0)
            cr->RAM_enable = false;
        else if(data == 0x0A && cr->sram_banks > 0)
        {
            cr->RAM_enable = true;
        }
    }
    
    //ROM Bank Number (Write Only)
    else if(/* (address >= 0x2000) && */ (address <= 0x3FFF))
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
            cr->ROM_bank_number &= ~0x1F;
            cr->ROM_bank_number |= bank;
        }
        else
            cr->ROM_bank_number = bank;
            
        emu->cpu.cur_hi_rom_bk = cr->ROM_bank_number;
    }
    
    //FIXME RAM Bank Number — or — Upper Bits of ROM Bank Number (Write Only)
    else if(/* (address >= 0x4000) && */ (address <= 0x5FFF))
    {
        uint8_t ram_bank = data & 0x03;

        //if 1 MiB ROM or superior
        if(cr->rom_banks >= 64)
        {
            cr->ROM_bank_number &= ~0x60;
            cr->ROM_bank_number |= data & 0x60;
            cpu->cur_hi_rom_bk = cr->ROM_bank_number;
        }

        // if banking mode enables 0000-3FFF and A000-BFFF banks switching
        if(cr->banking_mode_select)
        {
            if(ram_bank < cr->sram_banks)
            {
                cr->RAM_bank_number = ram_bank;
                emu->cpu.current_sram_bk = ram_bank;
            }
            cpu->cur_low_rom_bk = cr->ROM_bank_number & 0x60;
        }
        else
        {
            cpu->cur_low_rom_bk = 0;
            cpu->current_sram_bk = 0;
        }
    }
    
    //Banking Mode Select (Write Only)
    else if(/* (address >= 0x6000) && */ (address <= 0x7FFF))
    {
        cr->banking_mode_select = data & 0x01;

        uint8_t ram_bank = cr->ROM_bank_number & 0x60;

        if(cr->banking_mode_select)
        {
            if(ram_bank < cr->sram_banks)
            {
                cr->RAM_bank_number = ram_bank;
                emu->cpu.current_sram_bk = ram_bank;
            }
            cpu->cur_low_rom_bk = cr->ROM_bank_number & 0x60;
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
    s_cart *cr = &emu->cart;

    if(emu->opt.rom_argument && emu->cart.RAM_enable)
    {
        if(emu->cart.type == MBC2 || emu->cart.type == MBC2_P_BATT)
        {
            uint16_t relative = (address - 0xA000) % 0x200;
            cpu->SRAM[0][relative] = data & 0x0F;
        }
        // write RTC registers instead of SRAM
        else if(emu->cart.type != MBC3_P_TIMER_P_BATT &&
                emu->cart.type != MBC3_P_TIMER_P_RAM_P_BATT)
            cpu->SRAM[cpu->current_sram_bk][address - 0xA000] = data;

    }

    if(cr->RTC_enable)
    {
        printf("writing at 0x%02X\n", cpu->current_sram_bk);
        switch(cpu->current_sram_bk)
        {
            case 0x00:
            case 0x01:
            case 0x02:
            case 0x03:
                cpu->SRAM[cpu->current_sram_bk][address - 0xA000] = data;
                break;
            case 0x08:
                cr->rtc_internal.RTC_S = data;
                printf("RTC: write 0x%02X in S\n", data);
                break;
            case 0x09:
                cr->rtc_internal.RTC_M = data;
                printf("RTC: write 0x%02X in M\n", data);
                break;
            case 0x0A:
                cr->rtc_internal.RTC_H = data;
                printf("RTC: write 0x%02X in H\n", data);
                break;
            case 0x0B:
                cr->rtc_internal.RTC_DL = data;
                printf("RTC: write 0x%02X in DL\n", data);
                break;
            case 0x0C:
                cr->rtc_internal.RTC_DH = data;
                printf("RTC: write 0x%02X in DH\n", data);
                break;
            default:
                fprintf(stderr, COLOR_RED "ERROR: MBC3: Invalid SRAM bank / RTC register (0x%02X)\n" COLOR_RESET, cpu->current_sram_bk);
                return EXIT_FAILURE;
                break;
        }
    }

    return EXIT_SUCCESS;
}

int read_external_RAM(s_emu *emu, uint16_t address, uint8_t *data)
{
    s_cpu *cpu = &emu->cpu;
    s_cart *cr = &emu->cart;

    if(emu->cart.RAM_enable)
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
                    *data = cr->rtc_latched.RTC_S;
                    break;
                case 0x09:
                    *data = cr->rtc_latched.RTC_M;
                    break;
                case 0x0A:
                    *data = cr->rtc_latched.RTC_H;
                    break;
                case 0x0B:
                    *data = cr->rtc_latched.RTC_DL;
                    break;
                case 0x0C:
                    *data = cr->rtc_latched.RTC_DH;
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
        *data = 0xFF;
    return EXIT_SUCCESS;
}
