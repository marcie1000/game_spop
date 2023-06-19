#ifndef INSTR_LISTS_H
#define INSTR_LISTS_H

#include "emulator.h"

void initialize_length_table(s_emu *emu);
void init_opcodes_pointers(void (*opcode_functions[OPCODE_NB])(void *, uint32_t));
void init_cb_pointers(void (*cb_functions[CB_NB]) (void *, uint8_t));
void init_mnemonic_index(s_emu *emu);
void init_prefix_mnemonic_index(s_emu *emu);
void init_prefix_timing_table(s_emu *emu);
void init_timing_table(s_emu *emu);


#endif //INSTR_LISTS_H

