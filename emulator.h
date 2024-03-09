#ifndef EMULATOR_H
#define EMULATOR_H

#include "SDL_render.h"
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include <SDL.h>

#define CPU_FREQ                    (4194304.0)
#define GB_VSNC                     (59.73)

#define MEM_SIZE                    (0xFFFF)    //size of memory bus
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

#define AUDIO_SAMPLES_PER_BUFFER    (1600)
#define AUDIO_SAMPLES_DRIVER        (400)
#define AUDIO_SAMPLE_RATE           (48000)
#define BUFFERS_PER_FRAME           (1)
#define COMMON_VOLUME               (0.4)

//to avoid compiler warning when a function parameter isn't used
#define UNUSED __attribute__((unused))

//coloring text output in console
#define COLOR_RED     "\x1b[31m"
#define COLOR_GREEN   "\x1b[32m"
#define COLOR_YELLOW  "\x1b[33m"
#define COLOR_BLUE    "\x1b[34m"
#define COLOR_MAGENTA "\x1b[35m"
#define COLOR_CYAN    "\x1b[36m"
#define COLOR_RESET   "\x1b[0m"

#define KEYOPT_NB (15)
#define INI_OPT_NB (5)

enum audio_channels {
    CH1,
    CH2,
    CH3,
    CH4,
    AUDIO_CH_NB
};

enum keyctrls {
    JOYP_UP,
    JOYP_DOWN,
    JOYP_LEFT,
    JOYP_RIGHT,
    JOYP_START,
    JOYP_SELECT,
    JOYP_A,
    JOYP_B,
    OPT_PAUSE,
    OPT_OPTIONS,
    OPT_NEXT_FRAME,
    OPT_FAST_FORWARD,
    OPT_FULLSCREEN,
    OPT_SAVESTATE,
    OPT_LOADSTATE
};

enum iniopts {
    BOOTROM_FILENAME,
    AUDIO_CH1,
    AUDIO_CH2,
    AUDIO_CH3,
    AUDIO_CH4
};

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
         log_instrs, fast_forward, audio, audio_log, fullscreen, plot_instructions;
    bool newframe; //for frame by frame step
    bool framebyframe;
    bool audio_ch[4];
    uint16_t breakpoint_value;
    char rom_filename[FILENAME_MAX];
    char sav_filename[FILENAME_MAX];
    char bootrom_filename[FILENAME_MAX];
    FILE *logfile;
    FILE *inifile;
    SDL_Scancode opt_scancodes[KEYOPT_NB];
    SDL_Scancode default_scancodes[KEYOPT_NB];
    char ctrl_names[KEYOPT_NB][25];
    char ini_opt_names[INI_OPT_NB][25];
}s_opt;

typedef struct s_input{
    SDL_Event event;
    SDL_bool scan[SDL_NUM_SCANCODES];
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
    s_io io;
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

typedef struct s_plot{
    SDL_Renderer *r;
    SDL_Window *w;
    SDL_Texture *plot;
    Uint32 *pixels;
    int pitch;
    SDL_PixelFormat *format;
    uint32_t height, width;
    bool window_maximized;
}s_plot;

typedef struct s_screen{
    s_plot plot;
    SDL_Renderer *r;
    SDL_Window *w;
    uint32_t pixel_h, pixel_w;
    SDL_Texture *scr;
    SDL_Texture *scrcpy;
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
    
    uint8_t old_STAT;
    uint8_t win_LY;
    
    bool window_maximized;
    int refresh_rate;
    int refresh_rate_mul;
    SDL_Rect render_dst;
    SDL_Rect *render_dst_ptr;
    //2: searching OAM, 3: reading OAM and VRAM,
    //0: HBlank, 1: VBlank
    uint8_t PPU_mode;

    uint8_t sprites_to_draw[SPRITES_PER_SCANLINE];
    uint8_t nb_sptd;
    bool is_OAM_scanned;
    bool is_scanline_drawn;
}s_screen;

typedef struct s_audio{
    SDL_AudioSpec spec_want;
    SDL_AudioSpec spec_have;
    SDL_AudioDeviceID dev;
    uint64_t samples_played;
    uint8_t DIV_APU;
    
    float fstream[AUDIO_SAMPLES_PER_BUFFER];
    
    bool apu_enable;
    
    bool VIN_l, VIN_r;
    uint8_t l_output_vol, r_output_vol;
    
    size_t buf_counter;
    
    unsigned ch_wavelen[2];
    uint16_t ch_len_timer[4];
    uint8_t ch_init_len_timer[4];
    uint8_t ch_vol_sweep_timer[4];
    uint8_t ch_vol_sweep_counter[4];
    uint8_t ch_duty_ratio[2];
    uint8_t ch_init_volume[4];
    bool    ch_envl_dir[4];
    uint8_t ch_vol_sweep_pace[4];
    bool    ch_l[4], ch_r[4];
    bool ch_sound_len_enable[4];
    unsigned ch_freq[4];
    bool ch_trigger[4];
    bool ch_reset[4];
    bool ch_enable[4];
    
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
    
    
    uint8_t  ch4_clock_shift;
    bool     ch4_lfsr_w;
    uint8_t  ch4_clock_div;
    uint16_t ch4_lfsr;


    double samples_timer;
    float duty_ratios[4];
    
    uint8_t buffers_since_last_frame;
}s_audio;

typedef struct s_rtc{
    uint8_t RTC_S, RTC_M, RTC_H, RTC_DL, RTC_DH;
}s_rtc;

typedef struct s_cart{
    char title[20];
    int cgb_flag;
    int type;
    int rom_banks; // total number of ROM banks
    int sram_banks; // total number of SRAM banks
    bool batt;
    bool RAM_enable;
    // FIXME these are duplicates with cpu->current_sram_bk, cur_low_rom_bk, cur_hi_rom_bk
    unsigned ROM_bank_number; // the CURRENT ROM bank number, used for MBC1
    unsigned RAM_bank_number; // the CURRENT RAM bank number

    bool banking_mode_select;

    // MBC3
    s_rtc rtc_internal;
    s_rtc rtc_latched;
    bool has_RTC;
    bool RTC_enable;
    uintmax_t epoch;
    uint8_t latch_register; // to store if $00 and then $01 are written to the latch register
}s_cart;

typedef struct s_emu{
    Uint64 frame_timer;
    s_screen scr;
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
    s_audio au;
    s_cart cart;
}s_emu;

void flag_assign(bool cond, uint8_t *flag, uint8_t mask);
void flag_assign16(bool cond, uint16_t *flag, uint16_t mask);
void joypad_update(s_emu *emu);
void update_event(s_emu *emu);
int initialize_SDL(void);
int initialize_emulator(s_emu *emu);
void destroy_emulator(s_emu *emu, int status);
void emulate(s_emu *emu);
int load_boot_rom(s_emu *emu);
int load_rom(s_emu *emu);
int parse_start_options(s_opt *opt, int argc, char *argv[]);
int parse_options_during_exec(s_opt *opt);
int parse_options(s_opt *opt, size_t argc, char *argv[], bool is_program_beginning);
void ask_breakpoint(s_opt *opt);
void pause_menu(s_emu *emu);
void log_instructions(s_emu *emu);
int read_cartridge_header(s_emu *emu);
int load_sav(s_emu *emu);
int create_inifile(s_emu *emu);
int open_inifile(s_emu *emu);
void fullscreen_toggle(s_emu *emu);
void savestate(s_emu *emu);
void loadstate(s_emu *emu);



#endif //EMULATOR_H

