#ifndef CPU_H
#define CPU_H

#include <stdint.h>
#include "emulator.h"

int write_io_registers(s_emu *emu, uint16_t adress, uint8_t data);
int read_io_registers(s_emu *emu, uint16_t adress, uint8_t *data);
int write_memory(s_emu *emu, uint16_t adress, uint8_t data);
int read_memory(s_emu *emu, uint16_t adress, uint8_t *data);
int initialize_cpu(s_cpu *cpu);
uint32_t get_opcode(s_emu *emu);
uint8_t get_action(uint32_t opcode);
uint8_t get_cb_opcode(uint32_t op32);
void interpret(s_emu *emu, void (*opcode_functions[OPCODE_NB])(void *, uint32_t));
void breakpoint_handle(s_emu *emu, uint8_t action);
void step_by_step_handle(s_emu *emu);
void div_handle(s_emu *emu);

#endif //CPU_H

