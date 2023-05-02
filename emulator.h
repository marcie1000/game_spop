#ifndef EMULATOR_H
#define EMULATOR_H

#include <stdint.h>
#include <SDL.h>

#define RAM_SIZE (8192) //8 * 1024 = 8 KiB
#define VRAM_SIZE (8192)
#define START_ADRESS (0x0000)
#define OPCODE_NB (256)
#define UNUSED __attribute__((unused))

typedef struct s_input{
    SDL_bool key[SDL_NUM_SCANCODES];
    SDL_bool quit;
    int x, y, xrel, yrel;
    int xwheel, ywheel;
    SDL_bool mouse[6];
    SDL_bool resize;
} s_input;

typedef struct s_jump {
    uint8_t length[OPCODE_NB];
} s_jump;

typedef struct s_cpu {
    uint8_t ram[RAM_SIZE];
    uint8_t vram[VRAM_SIZE];
    uint8_t reg_A; //accumulator
    uint8_t reg_B, reg_C, reg_D, reg_E, reg_H, reg_L; //normal registers
    uint8_t reg_F; //flags
    uint16_t sp; //stack pointer
    uint16_t pc; //program counter
    s_jump jump_table;
} s_cpu;

typedef struct s_emu{
    s_cpu cpu;
    s_input in;
}s_emu;

extern void update_event(s_input *input);
extern int initialize_SDL(void);
extern int initialize_emulator(s_emu *emu, void (*opcode_functions[OPCODE_NB])(s_emu*, uint32_t));
extern void destroy_emulator(void);
extern void destroy_SDL(void);
extern void emulate(s_emu *emu, void (*opcode_functions[OPCODE_NB])(s_emu*, uint32_t));

#endif //EMULATOR_H

