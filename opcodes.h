#ifndef OPCODES_H
#define OPCODES_H

#include <stdint.h>
#include "cpu.h"
#include "emulator.h"

extern void opcode_unimplemented(s_emu *emu, uint32_t op);

#endif //OPCODES_H

