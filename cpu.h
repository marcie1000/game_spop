#ifndef CPU_H
#define CPU_H

#include <stdint.h>
#include "emulator.h"

extern int initialize_cpu(s_cpu *cpu);
extern int load_boot_rom(s_cpu *cpu);
extern uint32_t get_opcode(s_cpu *cpu);
extern uint8_t get_action(uint32_t opcode);
extern uint8_t get_cb_opcode(uint32_t op32);
extern void interpret(s_emu *emu, void (*opcode_functions[OPCODE_NB])(void *, uint32_t));
extern void initialize_length_table(s_emu *emu);
extern void init_opcodes_pointers(void (*opcode_functions[OPCODE_NB])(void *, uint32_t));
extern void init_cb_pointers(void (*cb_functions[CB_NB]) (void *, uint8_t));

#endif //CPU_H

