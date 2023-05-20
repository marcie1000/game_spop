#ifndef EMULATOR_H
#define EMULATOR_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <SDL.h>

#define MEM_SIZE (0xFFFF)
#define START_ADRESS (0x0000)
#define OPCODE_NB (256)
#define CB_NB (256)
#define ROM_BANK_SIZE (0x4000)
#define VRAM_SIZE (0x2000)
#define WRAM_SIZE (0x2000)
#define HRAM_SIZE (0x80)
#define OAM_SIZE (0xA0)
#define EXTERNAL_RAM_SIZE (0x2000)
#define PIX_BY_W (160)
#define PIX_BY_H (144)
#define LY_LIMIT (154)
#define SPRITEPOS_X_LIMIT (168)
#define UNUSED __attribute__((unused))

enum flags_masks {
    ZERO_FMASK = 0x80,
    NEGATIVE_FMASK = 0x40,
    HALF_CARRY_FMASK = 0x20,
    CARRY_FMASK = 0x10
};

typedef struct s_opt{
    bool bootrom, rom_argument, debug_info;
    char rom_filename[FILENAME_MAX];
}s_opt;

typedef struct s_input{
    SDL_bool key[SDL_NUM_SCANCODES];
    SDL_bool quit;
    int x, y, xrel, yrel;
    int xwheel, ywheel;
    SDL_bool mouse[6];
    SDL_bool resize;
} s_input;

//I/O registers
typedef struct s_io{
    uint8_t NR11, NR12, NR13, NR14, NR50, NR51, NR52, LCDC, SCY, SCX, LY, BGP, BANK;
}s_io;

typedef struct s_cpu {
    //uint8_t mem[MEM_SIZE];
    uint8_t ROM_Bank_0_tmp[ROM_BANK_SIZE]; //to replace ROM_Bank[0] content after disabling boot rom
    uint8_t ROM_Bank[2][ROM_BANK_SIZE];
    uint8_t VRAM[VRAM_SIZE];
    uint8_t WRAM[WRAM_SIZE];
    uint8_t HRAM[HRAM_SIZE];
    uint8_t OAM[OAM_SIZE];
    s_io io_reg;
    uint8_t external_RAM[EXTERNAL_RAM_SIZE];
    uint8_t regA; //accumulator
    uint8_t regB, regC, regD, regE, regH, regL; //normal registers
    uint8_t regF; //flags
    uint16_t sp; //stack pointer
    uint16_t pc; //program counter
    size_t cycles; //cycles counter
} s_cpu;

typedef struct s_screen{
    SDL_Renderer *r;
    SDL_Window *w;
    uint32_t pixel_h, pixel_w;
    SDL_Texture *scr;
    int pitch;
    Uint32 *pixels;
    SDL_PixelFormat *format;
    
    bool LCD_PPU_enable;
    bool win_tile_map_area;
    bool window_enable;
    bool BG_win_tile_data_area;
    bool BG_tile_map_area;
    bool obj_size;
    bool obj_enable;
    bool bg_win_enable_priority;
}s_screen;

typedef struct s_emu{
    Uint64 frame_timer;
    s_screen screen;
    uint8_t length_table[OPCODE_NB];
    char mnemonic_index[OPCODE_NB][15];
    char prefixed_mnemonic_index[CB_NB][15];
    void (*opcode_functions[OPCODE_NB])(void*, uint32_t);
    void (*cb_functions[CB_NB]) (void*, uint8_t);
    s_cpu cpu;
    s_input in;
    s_opt opt;
}s_emu;

extern void update_event(s_input *input);
extern int initialize_SDL(void);
extern int initialize_emulator(s_emu *emu);
extern void destroy_emulator(s_emu *emu, int status);
extern void destroy_SDL(void);
extern void emulate(s_emu *emu);
extern int load_boot_rom(s_cpu *cpu);
extern int load_rom(s_emu *emu);
extern void init_mnemonic_index(s_emu *emu);
extern void init_prefix_mnemonic_index(s_emu *emu);
extern int parse_options(s_opt *opt, int argc, char *argv[]);


#endif //EMULATOR_H

