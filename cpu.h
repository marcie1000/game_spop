#ifndef CPU_H
#define CPU_H

#include <stdint.h>
#include "emulator.h"

extern void initialize_cpu(s_cpu *cpu);
extern int load_boot_rom(s_cpu *cpu);
extern uint32_t get_opcode(s_cpu *cpu);
extern uint8_t get_action(uint32_t opcode);
extern void interpret(s_emu *emu, void (*opcode_functions[OPCODE_NB])(s_emu *, uint32_t));
extern void initialize_length_table(s_jump *table);
extern void init_opcodes_pointers(void (*opcode_functions[OPCODE_NB])(s_emu *, uint32_t));

#endif //CPU_H

