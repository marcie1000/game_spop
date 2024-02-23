#ifndef MBC_H
#define MBC_H

#include "stdint.h"
#include "emulator.h"

int write_mbc_registers(s_emu *emu, uint16_t address, uint8_t data);
int mbc1_registers(s_emu *emu, uint16_t address, uint8_t data);
int mbc2_registers(s_emu *emu, uint16_t address, uint8_t data);
int mbc3_registers(s_emu *emu, uint16_t address, uint8_t data);
int read_external_RAM(s_emu *emu, uint16_t address, uint8_t *data);
int write_external_RAM(s_emu *emu, uint16_t address, uint8_t data);
int rtc_latch(s_emu *emu, uint16_t address, uint8_t data);

#endif //MBC_H
