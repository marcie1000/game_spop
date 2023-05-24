#ifndef CPU_H
#define CPU_H

#include <stdint.h>
#include "emulator.h"

extern int write_io_registers(s_emu *emu, uint16_t adress, uint8_t data);
extern int read_io_registers(s_emu *emu, uint16_t adress, uint8_t *data);
extern int write_memory(s_emu *emu, uint16_t adress, uint8_t data);
extern int read_memory(s_emu *emu, uint16_t adress, uint8_t *data);
extern int initialize_cpu(s_cpu *cpu);
extern uint32_t get_opcode(s_emu *emu);
extern uint8_t get_action(uint32_t opcode);
extern uint8_t get_cb_opcode(uint32_t op32);
extern void interpret(s_emu *emu, void (*opcode_functions[OPCODE_NB])(void *, uint32_t));
extern void initialize_length_table(s_emu *emu);
extern void init_opcodes_pointers(void (*opcode_functions[OPCODE_NB])(void *, uint32_t));
extern void init_cb_pointers(void (*cb_functions[CB_NB]) (void *, uint8_t));
extern void breakpoint_handle(s_emu *emu, uint8_t action);
extern void step_by_step_handle(s_emu *emu);

#endif //CPU_H

