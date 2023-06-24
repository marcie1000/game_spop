#ifndef EMULATOR_H
#define EMULATOR_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <SDL.h>

#define CPU_FREQ                    (4194304.0)
#define GB_VSNC                     (59.73)

#define MEM_SIZE                    (0xFFFF)
#define START_ADRESS                (0x0000)
#define OPCODE_NB                   (256)
#define CB_NB                       (256)
#define ROM_BANK_SIZE               (0x4000)
#define VRAM_SIZE                   (0x2000)
#define WRAM_SIZE                   (0x2000)
#define HRAM_SIZE                   (0x80)
#define OAM_SIZE                    (0xA0)
#define EXTERNAL_RAM_SIZE           (0x2000)
#define PIX_BY_W                    (160)
#define PIX_BY_H                    (144)
#define LY_LIMIT                    (154)
#define SPRITEPOS_X_LIMIT           (168)
#define OAM_SPRITES_MAX             (40)
#define SPRITES_PER_SCANLINE        (10)

#define ROM_BANKS_MAX               (512)
#define SRAM_BANKS_MAX              (16)

#define AUDIO_SAMPLES_PER_QUEUES    (1600) //800
#define AUDIO_SAMPLES_DRIVER        (400)
#define AUDIO_SAMPLE_RATE           (48000)
#define QUEUES_PER_FRAME            (1)

//to avoid compiler warning when a function parameter isn't used
#define UNUSED __attribute__((unused))

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

enum flags_masks {
    ZERO_FMASK = 0x80,
    NEGATIVE_FMASK = 0x40,
    HALF_CARRY_FMASK = 0x20,
    CARRY_FMASK = 0x10
};

enum ppu_modes_durations {
    PPU_MODE2 = 80,
    PPU_MODE3 = 200,
    PPU_MODE0 = 176,
    PPU_MODE1 = 4560
};

enum cgb_flags {
    CLASSIC_GB,
    CGB_BACKWARDS_COMPATIBLE,
    CGB_ONLY
};

enum cartridge_types {
    ROM_ONLY                            = 0x00,
    MBC1                                = 0x01,
    MBC1_P_RAM                          = 0x02,
    MBC1_P_RAM_P_BATT                   = 0x03,
    MBC2                                = 0x05,
    MBC2_P_BATT                         = 0x06,
    ROM_P_RAM                           = 0x08,
    ROM_P_RAM_P_BATT                    = 0x09,
    MMM01                               = 0x0B,
    MMM01_P_RAM                         = 0x0C,
    MMM01_P_RAM_P_BATT                  = 0x0D,
    MBC3_P_TIMER_P_BATT                 = 0x0F,
    MBC3_P_TIMER_P_RAM_P_BATT           = 0x10,
    MBC3                                = 0x11,
    MBC3_P_RAM                          = 0x12,
    MBC3_P_RAM_P_BATT                   = 0x13,
    MBC5                                = 0x19,
    MBC5_P_RAM                          = 0x1A,
    MBC5_P_RAM_P_BATT                   = 0x1B,
    MBC5_P_RUMBLE                       = 0x1C,
    MBC5_P_RUMBLE_P_RAM                 = 0x1D,
    MBC5_P_RUMBLE_P_RAM_P_BATT          = 0x1E,
    MBC6                                = 0x20,
    MBC7_P_SENSOR_P_RUMBLE_P_RAM_P_BATT = 0x22,
    POCKET_CAMERA                       = 0xFC,
    BANDAI_TAMA5                        = 0xFD,
    HuC3                                = 0xFE,
    HuC1_P_RAM_P_BATT                   = 0xFF,
};

typedef struct s_opt{
    bool bootrom, rom_argument, debug_info, breakpoints, step_by_step, gb_doctor,
         log_instrs, fast_forward, audio, audio_log;
    uint16_t breakpoint_value;
    char rom_filename[FILENAME_MAX];
    FILE *logfile;
}s_opt;

typedef struct s_input{
    SDL_Event event;
    SDL_bool key[SDL_NUM_SCANCODES];
    SDL_bool quit;
    int x, y, xrel, yrel;
    int xwheel, ywheel;
    SDL_bool mouse[6];
    SDL_bool resize;
} s_input;

//I/O registers
typedef struct s_io{
    uint8_t P1_JOYP,
            NR10, NR11, NR12, NR13, NR14,
            NR21, NR22, NR23, NR24,
            NR30, NR31, NR32, NR33, NR34,
            NR41, NR42, NR43, NR44,
            NR50, NR51, NR52, LCDC, 
            SCY, SCX, WY, WX, LY, BGP, BANK,
            IE, IF, STAT, LYC, SB, SC, OBP0, OBP1,
            TMA, DMA, TAC, TIMA, DIV;
    uint8_t wave_RAM[16];
    bool IME;
}s_io;

typedef struct s_cpu {
    //uint8_t mem[MEM_SIZE];
    uint8_t ROM_Bank_0_tmp[ROM_BANK_SIZE]; //to replace ROM_Bank[0] content after disabling boot rom
    uint8_t **ROM_Bank;
    uint8_t VRAM[VRAM_SIZE];
    uint8_t WRAM[WRAM_SIZE];
    uint8_t HRAM[HRAM_SIZE];
    uint8_t OAM[OAM_SIZE];
    s_io io_reg;
    uint8_t SRAM[SRAM_BANKS_MAX][EXTERNAL_RAM_SIZE];
    uint8_t regA, regB, regC, regD, regE, regH, regL, regF; //registers
    uint16_t sp; //stack pointer
    uint16_t pc; //program counter
    double t_cycles; //t_cycles counter at 4,194,304 Hz
    uint16_t timer_clock;
    size_t debug_clock;
    bool quit_halt, in_halt;
    size_t inst_counter;
    int current_sram_bk;
    int cur_low_rom_bk;
    int cur_hi_rom_bk;
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
    
    bool window_maximized;
    SDL_Rect render_dst;
    SDL_Rect *render_dst_ptr;
    //2: searching OAM, 3: reading OAM and VRAM,
    //0: HBlank, 1: VBlank
    uint8_t PPU_mode;
}s_screen;

typedef struct s_audio{
    SDL_AudioSpec spec_want;
    SDL_AudioSpec spec_have;
    SDL_AudioDeviceID dev;
    uint64_t samples_played;
    uint8_t DIV_APU;
    
    float fstream[AUDIO_SAMPLES_PER_QUEUES];
    
    bool VIN_l, VIN_r;
    uint8_t l_output_vol, r_output_vol;
    
    
    uint16_t ch_wavelen[2];
    uint8_t ch_len_timer[3];
    uint8_t ch_init_len_timer[3];
    uint8_t ch_vol_sweep_timer[2];
    uint8_t ch_vol_sweep_counter[2];
    uint8_t ch_duty_ratio[2];
    uint8_t ch_init_volume[2];
    bool    ch_envl_dir[2];
    uint8_t ch_vol_sweep_pace[2];
    bool    ch_l[3], ch_r[3];
    bool ch_sound_len_enable[3];
    uint16_t ch_freq[3];
    bool ch_trigger[3];
    bool ch_reset[3];
    bool ch_enable[3];
    
    uint8_t ch1_wl_sweep_timer;
    uint8_t ch1_wl_sweep_counter;
    uint8_t ch1_wl_sweep_pace;
    bool    ch1_wl_sweep_dir;
    uint8_t ch1_wl_sweep_slope_ctr;
    
    bool     ch3_dac_enable;
    uint8_t  ch3_output_level;
    uint16_t ch3_period;
    uint32_t ch3_sample_rate;
    uint8_t  ch3_samples_counter;


    double samples_timer;
    float duty_ratios[4];
    
    uint8_t queues_since_last_frame;
}s_audio;

typedef struct s_mbc{
    bool RAM_enable;
    unsigned ROM_bank_number;
    unsigned RAM_bank_number;
    bool banking_mode_select;
}s_mbc;

typedef struct s_cart{
    char title[20];
    int cgb_flag;
    int type;
    int rom_banks;
    int sram_banks;
    s_mbc mbc;
}s_cart;

typedef struct s_emu{
    Uint64 frame_timer;
    s_screen screen;
    uint8_t length_table[OPCODE_NB];
    
    //timing_table[0] => timing without branch
    //timing_table[1] => timing with branch
    uint8_t timing_table[2][OPCODE_NB];
    
    uint8_t prefix_timing_table[CB_NB];
    char mnemonic_index[OPCODE_NB][15];
    char prefixed_mnemonic_index[CB_NB][15];
    void (*opcode_functions[OPCODE_NB])(void*, uint32_t);
    void (*cb_functions[CB_NB]) (void*, uint8_t);
    s_cpu cpu;
    s_input in;
    s_opt opt;
    s_audio audio;
    s_cart cart;
}s_emu;

extern void flag_assign(bool cond, uint8_t *flag, uint8_t mask);
extern void joypad_update(s_emu *emu);
extern void update_event(s_emu *emu);
extern int initialize_SDL(void);
extern int initialize_emulator(s_emu *emu);
extern void destroy_emulator(s_emu *emu, int status);
extern void emulate(s_emu *emu);
extern int load_boot_rom(s_cpu *cpu);
extern int load_rom(s_emu *emu);
extern int parse_start_options(s_opt *opt, int argc, char *argv[]);
extern int parse_options_during_exec(s_opt *opt);
extern int parse_options(s_opt *opt, size_t argc, char *argv[], bool is_program_beginning);
extern void ask_breakpoint(s_opt *opt);
extern void pause_menu(s_emu *emu);
extern void log_instructions(s_emu *emu);
extern int read_cartridge_header(s_emu *emu);


#endif //EMULATOR_H

