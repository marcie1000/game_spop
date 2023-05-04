#ifndef EMULATOR_H
#define EMULATOR_H

#include <stdint.h>
#include <SDL.h>

#define MEM_SIZE (0xFFFF)
#define START_ADRESS (0x0000)
#define OPCODE_NB (256)
#define CB_NB (256)
#define UNUSED __attribute__((unused))

typedef struct s_input{
    SDL_bool key[SDL_NUM_SCANCODES];
    SDL_bool quit;
    int x, y, xrel, yrel;
    int xwheel, ywheel;
    SDL_bool mouse[6];
    SDL_bool resize;
} s_input;

typedef struct s_cpu {
    uint8_t mem[MEM_SIZE];
    uint8_t reg_A; //accumulator
    uint8_t reg_B, reg_C, reg_D, reg_E, reg_H, reg_L; //normal registers
    uint8_t reg_F; //flags
    uint16_t sp; //stack pointer
    uint16_t pc; //program counter
    size_t cycles; //cycles counter
} s_cpu;


typedef struct s_emu{
    uint8_t length_table[OPCODE_NB];
    void (*opcode_functions[OPCODE_NB])(void*, uint32_t);
    void (*cb_functions[CB_NB]) (void*, uint8_t);
    s_cpu cpu;
    s_input in;
}s_emu;

extern void update_event(s_input *input);
extern int initialize_SDL(void);
extern int initialize_emulator(s_emu *emu);
extern void destroy_emulator(void);
extern void destroy_SDL(void);
extern void emulate(s_emu *emu);

#endif //EMULATOR_H

