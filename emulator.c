#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL.h>
#include "emulator.h"
#include "cpu.h"
#include "graphics.h"
#include "audio.h"
#include "instr_lists.h"

void flag_assign(bool cond, uint8_t *flag, uint8_t mask)
{
    *flag = cond ? mask | *flag : ~mask & *flag;
}

void flag_assign16(bool cond, uint16_t *flag, uint16_t mask)
{
    *flag = cond ? mask | *flag : ~mask & *flag;
}



int create_inifile(s_emu *emu)
{
    s_opt *opt = &emu->opt;
    
    opt->inifile = fopen("game_spop.ini", "w");
    if(NULL == opt->inifile)
    {
        perror("fopen inifile: ");
        return EXIT_FAILURE;
    }
    
    FILE *shrt = opt->inifile;
    
    fprintf(
        shrt,
        "; List of game_spop controls.\n"
        "; To modify a control, write a key name after the wanted control,\n"
        "; using one of the names provided in the `Key Name` column in the\n"
        "; [SDL_Keycode documentation](https://wiki.libsdl.org/SDL2/SDL_Keycode).\n"
    );
    fprintf(shrt, "[controls]\n");
    SDL_KeyCode keycode;
    
    //UP
    keycode = SDL_GetKeyFromScancode(SDL_SCANCODE_W);
    fprintf(shrt, "%s=%s\n", opt->ctrl_names[0], SDL_GetKeyName(keycode));
    
    //DOWN
    keycode = SDL_GetKeyFromScancode(SDL_SCANCODE_S);
    fprintf(shrt, "%s=%s\n", opt->ctrl_names[1], SDL_GetKeyName(keycode));
    
    //LEFT
    keycode = SDL_GetKeyFromScancode(SDL_SCANCODE_A);
    fprintf(shrt, "%s=%s\n", opt->ctrl_names[2], SDL_GetKeyName(keycode));
    
    //RIGHT
    keycode = SDL_GetKeyFromScancode(SDL_SCANCODE_D);
    fprintf(shrt, "%s=%s\n", opt->ctrl_names[3], SDL_GetKeyName(keycode));
    
    //START
    keycode = SDL_GetKeyFromScancode(SDL_SCANCODE_RETURN);
    fprintf(shrt, "%s=%s\n", opt->ctrl_names[4], SDL_GetKeyName(keycode));
    
    //SELECT
    keycode = SDL_GetKeyFromScancode(SDL_SCANCODE_RSHIFT);
    fprintf(shrt, "%s=%s\n", opt->ctrl_names[5], SDL_GetKeyName(keycode));
    
    //A
    keycode = SDL_GetKeyFromScancode(SDL_SCANCODE_L);
    fprintf(shrt, "%s=%s\n", opt->ctrl_names[6], SDL_GetKeyName(keycode));
    
    //B
    keycode = SDL_GetKeyFromScancode(SDL_SCANCODE_M);
    fprintf(shrt, "%s=%s\n", opt->ctrl_names[7], SDL_GetKeyName(keycode));
    
    //PAUSE
    fprintf(shrt, "%s=%s\n", opt->ctrl_names[8], SDL_GetKeyName(SDLK_p));
    
    //OPTION
    fprintf(shrt, "%s=%s\n", opt->ctrl_names[9], SDL_GetKeyName(SDLK_o));
    
    //NEXT FRAME
    fprintf(shrt, "%s=%s\n", opt->ctrl_names[10], SDL_GetKeyName(SDLK_n));
    
    //FAST FORWARD
    fprintf(shrt, "%s=%s\n", opt->ctrl_names[11], SDL_GetKeyName(SDLK_SPACE));
    
    //FULLSCREEN
    fprintf(shrt, "%s=%s\n", opt->ctrl_names[12], SDL_GetKeyName(SDLK_F11));
    
    fprintf(shrt, "\n");
    fprintf(shrt, "[options]\n");
    fprintf(shrt, "bootrom_path=boot_rom/dmg_rom.bin\n");
    fprintf(shrt, "audio_ch_1=ON\n");
    fprintf(shrt, "audio_ch_2=ON\n");
    fprintf(shrt, "audio_ch_3=ON\n");
    fprintf(shrt, "audio_ch_4=OFF\n");
    
    return EXIT_SUCCESS;
}

int open_inifile(s_emu *emu)
{
    s_opt *opt = &emu->opt;
    
    snprintf(opt->ctrl_names[0], 25, "JOYP_UP");
    snprintf(opt->ctrl_names[1], 25, "JOYP_DOWN");
    snprintf(opt->ctrl_names[2], 25, "JOYP_LEFT");
    snprintf(opt->ctrl_names[3], 25, "JOYP_RIGHT");
    snprintf(opt->ctrl_names[4], 25, "JOYP_START");
    snprintf(opt->ctrl_names[5], 25, "JOYP_SELECT");
    snprintf(opt->ctrl_names[6], 25, "JOYP_A");
    snprintf(opt->ctrl_names[7], 25, "JOYP_B");
    snprintf(opt->ctrl_names[8], 25, "OPT_PAUSE");
    snprintf(opt->ctrl_names[9], 25, "OPT_OPTIONS");
    snprintf(opt->ctrl_names[10], 25, "OPT_NEXT_FRAME");
    snprintf(opt->ctrl_names[11], 25, "OPT_FAST_FORWARD");
    snprintf(opt->ctrl_names[12], 25, "OPT_FULLSCREEN");
    
    snprintf(opt->bootrom_filename, FILENAME_MAX, "boot_rom/dmg_rom.bin");
    
    snprintf(opt->ini_opt_names[0], 25, "bootrom_filename");
    snprintf(opt->ini_opt_names[1], 25, "audio_ch_1");
    snprintf(opt->ini_opt_names[2], 25, "audio_ch_2");
    snprintf(opt->ini_opt_names[3], 25, "audio_ch_3");
    snprintf(opt->ini_opt_names[4], 25, "audio_ch_4");
    
    opt->default_scancodes[JOYP_UP] = SDL_SCANCODE_W;
    opt->default_scancodes[JOYP_DOWN] = SDL_SCANCODE_S;
    opt->default_scancodes[JOYP_LEFT] = SDL_SCANCODE_A;
    opt->default_scancodes[JOYP_RIGHT] = SDL_SCANCODE_D;
    opt->default_scancodes[JOYP_START] = SDL_SCANCODE_RETURN;
    opt->default_scancodes[JOYP_SELECT] = SDL_SCANCODE_RSHIFT;
    opt->default_scancodes[JOYP_A] = SDL_SCANCODE_L;
    opt->default_scancodes[JOYP_B] = SDL_SCANCODE_M;
    opt->default_scancodes[OPT_PAUSE] = SDL_GetScancodeFromKey(SDLK_p);
    opt->default_scancodes[OPT_OPTIONS] = SDL_GetScancodeFromKey(SDLK_o);
    opt->default_scancodes[OPT_NEXT_FRAME] = SDL_GetScancodeFromKey(SDLK_n);
    opt->default_scancodes[OPT_FAST_FORWARD] = SDL_GetScancodeFromKey(SDLK_SPACE);
    opt->default_scancodes[OPT_FULLSCREEN] = SDL_GetScancodeFromKey(SDLK_F11);
    
    opt->opt_scancodes[JOYP_UP] = opt->default_scancodes[JOYP_UP];
    opt->opt_scancodes[JOYP_DOWN] = opt->default_scancodes[JOYP_DOWN];
    opt->opt_scancodes[JOYP_LEFT] = opt->default_scancodes[JOYP_LEFT];
    opt->opt_scancodes[JOYP_RIGHT] = opt->default_scancodes[JOYP_RIGHT];
    opt->opt_scancodes[JOYP_START] = opt->default_scancodes[JOYP_START];
    opt->opt_scancodes[JOYP_SELECT] = opt->default_scancodes[JOYP_SELECT];
    opt->opt_scancodes[JOYP_A] = opt->default_scancodes[JOYP_A];
    opt->opt_scancodes[JOYP_B] = opt->default_scancodes[JOYP_B];
    opt->opt_scancodes[OPT_PAUSE] = opt->default_scancodes[OPT_PAUSE];
    opt->opt_scancodes[OPT_OPTIONS] = opt->default_scancodes[OPT_OPTIONS];
    opt->opt_scancodes[OPT_NEXT_FRAME] = opt->default_scancodes[OPT_NEXT_FRAME];
    opt->opt_scancodes[OPT_FAST_FORWARD] = opt->default_scancodes[OPT_FAST_FORWARD];
    opt->opt_scancodes[OPT_FULLSCREEN] = opt->default_scancodes[OPT_FULLSCREEN];
    
    opt->audio_ch[0] = true;
    opt->audio_ch[1] = true;
    opt->audio_ch[2] = true;
    opt->audio_ch[3] = false;
    
    opt->inifile = fopen("game_spop.ini", "r");
    if(NULL == opt->inifile)
    {
        if(0 != create_inifile(emu))
            return EXIT_FAILURE;
            
        fclose(opt->inifile);
        return EXIT_SUCCESS;
    }
    
    //read file
    char buf[255] = "";
    bool controls = false;
    bool options = false;
    while(NULL != fgets(buf, 255, opt->inifile))
    {
        if(buf[0] == ';' || buf[0] == ' ' || buf[0] == '\0')
            continue;

        if(0 == strncmp(buf, "[controls]", 10))
        {
            controls = true;
            options = false;
            continue;
        }
        else if(0 == strncmp(buf, "[options]", 9))
        {
            controls = false;
            options = true;
            continue;
        }
        
        for(int i = 0; (i < KEYOPT_NB) && controls; i++)
        {
            if(NULL == strstr(buf, opt->ctrl_names[i]))
                continue;
                
            char *name = strchr(buf, '=');
            if(NULL == name)
            {
                opt->opt_scancodes[i] = opt->default_scancodes[i];
                break;
            }
            
            name++;
            char *sub = strtok(name, "\n");
            SDL_KeyCode key = SDL_GetKeyFromName(sub);
            if(key == SDLK_UNKNOWN)
            {
                opt->opt_scancodes[i] = opt->default_scancodes[i];
                break;
            }
            
            opt->opt_scancodes[i] = SDL_GetScancodeFromKey(key);
            break;
        }
        
        for(int i = 0; (i < INI_OPT_NB) && options; i++)
        {
            if(NULL == strstr(buf, opt->ini_opt_names[i]))
                continue;
            
            char *param = strchr(buf, '=');
            if(NULL == param)
                continue;
            
            param++;
            char *sub = strtok(param, "\n");
            
            switch(i)
            {
                case BOOTROM_FILENAME:
                    snprintf(opt->bootrom_filename, FILENAME_MAX, "%s", sub);
                    break;
                case AUDIO_CH1:
                case AUDIO_CH2:
                case AUDIO_CH3:
                case AUDIO_CH4:
                    if(0 == strncmp(sub, "ON", 2))
                        opt->audio_ch[i - AUDIO_CH1] = true;
                    else if(0 == strncmp(sub, "OFF", 3))
                        opt->audio_ch[i - AUDIO_CH1] = false;
                    break;
            }
        }
    }
    
    fclose(opt->inifile);
    
    return EXIT_SUCCESS;
}

void update_event(s_emu *emu)
{
    s_input *input = &emu->in;
    while(SDL_PollEvent(&input->event))
    {
        switch(input->event.type)
        {
            case(SDL_QUIT):
                input->quit = SDL_TRUE;
                break;
            case(SDL_KEYDOWN):
                input->scan[input->event.key.keysym.scancode] = SDL_TRUE;                
                break;
            case(SDL_KEYUP):
                input->scan[input->event.key.keysym.scancode] = SDL_FALSE;
                break;
            case(SDL_MOUSEMOTION):
                input->x = input->event.motion.x;
                input->y = input->event.motion.y;
                input->xrel = input->event.motion.xrel;
                input->yrel = input->event.motion.yrel;
                break;
            case(SDL_MOUSEWHEEL):
                input->xwheel = input->event.wheel.x;
                input->ywheel = input->event.wheel.y;
                break;
            case(SDL_MOUSEBUTTONDOWN):
                input->mouse[input->event.button.button] = SDL_TRUE;
                break;
            case(SDL_MOUSEBUTTONUP):
                input->mouse[input->event.button.button] = SDL_FALSE;
                break;
            case(SDL_WINDOWEVENT):
                if(input->event.window.event == SDL_WINDOWEVENT_RESIZED)
                    input->resize = SDL_TRUE;
                break;
        }
    }
}

int read_cartridge_header(s_emu *emu)
{
    s_cart *cr = &emu->cart;
    s_cpu *cpu = &emu->cpu;
    memset(cr, 0, sizeof(s_cart));
    
    cr->mbc.ROM_bank_number = 1;
    
    if(!emu->opt.rom_argument)
        return EXIT_SUCCESS;
        
    for(int i = 0; i < 12; i++)
        cr->title[i] = cpu->ROM_Bank_0_tmp[i + 0x0134];
    cr->title[12] = '\n';
    
    switch(cpu->ROM_Bank_0_tmp[0x0143])
    {
        case 0x80:
            cr->cgb_flag = CGB_BACKWARDS_COMPATIBLE;
            break;
        case 0xC0:
            cr->cgb_flag = CGB_ONLY;
            fprintf(stderr, "ERROR: This game is GameBoy Color only "
                    "(not supported!)\n");
            return EXIT_FAILURE;
            break;
        default:
            cr->cgb_flag = CLASSIC_GB;
            break;
    }
    
    cr->type = cpu->ROM_Bank_0_tmp[0x0147];
    
    switch(cpu->ROM_Bank_0_tmp[0x0148])
    {
        case 0x00:
            cr->rom_banks = 2;
            break;
        case 0x01:
            cr->rom_banks = 4;
            break;
        case 0x02:
            cr->rom_banks = 8;
            break;
        case 0x03:
            cr->rom_banks = 16;
            break;
        case 0x04: 
            cr->rom_banks = 32;
            break;
        case 0x05:
            cr->rom_banks = 64;
            break;
        case 0x06:
            cr->rom_banks = 128;
            break;
        case 0x07:
            cr->rom_banks = 256;
            break;
        case 0x08:
            cr->rom_banks = 512;
            break;
        case 0x52:
            cr->rom_banks = 72;
            break;
        case 0x53:
            cr->rom_banks = 80;
            break;
        case 0x54:
            cr->rom_banks = 96;
            break;
        default:
            fprintf(stderr, "ERROR: ROM size value indicated in cartridge "
                    "header is undefined.\n");
            return EXIT_FAILURE;
            break;
    }
    
    switch(cpu->ROM_Bank_0_tmp[0x0149])
    {
        case 0x00:
            cr->sram_banks = 0;
            break;
        case 0x02:
            cr->sram_banks = 1;
            break;
        case 0x03:
            cr->sram_banks = 4;
            break;
        case 0x04:
            cr->sram_banks = 16;
            break;
        case 0x05:
            cr->sram_banks = 8;
            break;
        default:
            fprintf(stderr, "ERROR: SRAM size value indicated in cartridge "
                    "header is undefined.\n");
            return EXIT_FAILURE;
            break;
    }
    
    switch(cr->type)
    {
        case ROM_ONLY:
        case MBC1:
        case MBC1_P_RAM:
        case MBC1_P_RAM_P_BATT:
            break;
        default:
            fprintf(stderr, "WARNING: MBC code %02X (unimplemented)!\n", cr->type);
            return EXIT_FAILURE;
            break;
    }
    
    return EXIT_SUCCESS;
}

int initialize_SDL(void)
{
    if(0 != SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO))
    {
        fprintf(stderr, "Error SDL_Init: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}

int initialize_emulator(s_emu *emu)
{
    s_opt *opt = &emu->opt;
    opt->logfile = NULL;
    
    memset(&emu->in, 0, sizeof(s_input));
    if(0 != initialize_SDL())
        return EXIT_FAILURE;
    
    if(0 != open_inifile(emu))
        return EXIT_FAILURE;
    
    if(0 != initialize_screen(emu))
        return EXIT_FAILURE;
    if(0 != initialize_cpu(&emu->cpu))
        return EXIT_FAILURE;
    if(0 != init_audio(emu))
        return EXIT_FAILURE;
    initialize_length_table(emu);
    init_opcodes_pointers(emu->opcode_functions);
    init_cb_pointers(emu->cb_functions);
    init_mnemonic_index(emu);
    init_prefix_mnemonic_index(emu);
    init_timing_table(emu);
    init_prefix_timing_table(emu);
    
    if(0 != load_rom(emu))
        return EXIT_FAILURE;
    
    if(0 != load_sav(emu))
        return EXIT_FAILURE;
    
    if(emu->opt.bootrom)
    {
        if(0 != load_boot_rom(emu))
            return EXIT_FAILURE;
    }
    
    //gb doctor log file
    if(!opt->gb_doctor && !opt->log_instrs && !opt->audio_log)
        return EXIT_SUCCESS;
        
    opt->logfile = fopen("gb_logs.log", "w");
    if(opt->logfile == NULL)
    {
        fprintf(stderr, "fopen gb_logs.log: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }
    
    if(opt->audio_log)
        fprintf(opt->logfile, "fstream;volume;ch_vol_sweep_counter[0];ch_vol_sweep_timer[0];"
        "period_counter;npsp;duty;samples_played\n");
    
    return EXIT_SUCCESS;
}

int load_boot_rom(s_emu *emu)
{
    s_cpu *cpu = &emu->cpu;
    FILE *bootrom = fopen(emu->opt.bootrom_filename, "rb");
    if(NULL == bootrom)
    {
        fprintf(stderr, "ERROR: cannot open %s: %s", emu->opt.bootrom_filename, strerror(errno));
        return EXIT_FAILURE;
    }
    
    fread(&cpu->ROM_Bank[0][0], sizeof(cpu->ROM_Bank[0][0]), 0x100, bootrom);
    fclose(bootrom);
    
    printf("Boot rom loaded.\n");
    
    return EXIT_SUCCESS;
}

int load_sav(s_emu *emu)
{
    s_cpu *cpu = &emu->cpu;
    s_cart *cr = &emu->cart;
    if(!emu->opt.rom_argument)
        return EXIT_SUCCESS;
    if(cr->sram_banks < 1)
        return EXIT_SUCCESS;
        
    FILE *sav = fopen(emu->opt.sav_filename, "rb");
    if(NULL == sav)
        return EXIT_SUCCESS;
        
    size_t ret = fread(&cpu->SRAM[0][0], sizeof(uint8_t[EXTERNAL_RAM_SIZE]), cr->sram_banks, sav);
    
    if(ret != (size_t)cr->sram_banks)
    {
        perror("Error reading sav: ");
        fclose(sav);
        return EXIT_FAILURE;
    }
    
    fclose(sav);
    
    printf("sav loaded.\n");
    
    return EXIT_SUCCESS;
}

int load_rom(s_emu *emu)
{
    s_cpu *cpu = &emu->cpu;
    s_cart *cr = &emu->cart;
    if(!emu->opt.rom_argument)
        return EXIT_SUCCESS;
    
    FILE *rom = fopen(emu->opt.rom_filename, "rb");
    if(NULL == rom)
    {
        fprintf(stderr, "ERROR: cannot open '%s': %s\n", emu->opt.rom_filename, strerror(errno));
        return EXIT_FAILURE;
    }
    
    size_t size = sizeof(cpu->ROM_Bank[0][0]);
    fread(&cpu->ROM_Bank[0][0], size, ROM_BANK_SIZE, rom);
    //keeps the rom bytes separatly during bootrom execution
    memcpy(cpu->ROM_Bank_0_tmp, &cpu->ROM_Bank[0][0], sizeof(uint8_t[ROM_BANK_SIZE]));
    
    if(0 != read_cartridge_header(emu))
    {
        fclose(rom);
        return EXIT_FAILURE;
    }
    
    for(int i = 1; i < cr->rom_banks; i++)
    {
        if(ROM_BANK_SIZE != fread(&cpu->ROM_Bank[i][0], size, ROM_BANK_SIZE, rom))
        {
            fprintf(stderr, "ERROR: ROM file size doesn't match the ROM size value"
                    "indicated in its header.\n");
            fclose(rom);
            return EXIT_FAILURE;
        }
    }
    fclose(rom);
    
    printf("Rom loaded.\n");
    
    return EXIT_SUCCESS;
}

int save_sav(s_emu *emu)
{
    s_cpu *cpu = &emu->cpu;
    s_cart *cr = &emu->cart;
    if(!emu->opt.rom_argument)
        return EXIT_SUCCESS;
        
    if(cr->sram_banks < 1)
        return EXIT_SUCCESS;
        
    FILE *sav = fopen(emu->opt.sav_filename, "wb");
    if(NULL == sav)
    {
        perror("fopen: ");
        return EXIT_FAILURE;
    }
    
    size_t ret = fwrite(&cpu->SRAM[0][0], sizeof(uint8_t[EXTERNAL_RAM_SIZE]), cr->sram_banks, sav);
    if(ret != (size_t)cr->sram_banks)
    {
        perror("Error writing sav: ");
        fclose(sav);
        return EXIT_FAILURE;
    }
    
    fclose(sav);
    return EXIT_SUCCESS;
}

void destroy_emulator(s_emu *emu, int status)
{
    s_cpu *cpu = &emu->cpu;
    
    if(0 != save_sav(emu))
        status = EXIT_FAILURE;
    
    destroy_screen(&emu->scr);
    destroy_audio(emu);
    
    for(size_t i = 0; i < ROM_BANKS_MAX; i++)
    {
        if(NULL != cpu->ROM_Bank[i])
            free(cpu->ROM_Bank[i]);
    }
    
    if((NULL != cpu->ROM_Bank))
        free(cpu->ROM_Bank);
    
    SDL_Quit();
    if((NULL != emu->opt.logfile))
        fclose(emu->opt.logfile);
    exit(status);
}

void bypass_bootrom(s_emu *emu)
{
    s_cpu *cpu = &emu->cpu;
    s_io *io = &cpu->io;
    s_screen *scr = &emu->scr;
    
    cpu->t_cycles = 14;
    
    cpu->pc = 0x100;
    cpu->regA = 0x01;
    cpu->regF = 0xB0;
    cpu->regC = 0x13;
    cpu->regE = 0xD8;
    cpu->regH = 0x01;
    cpu->regL = 0x4D;
    cpu->sp = 0xFFFE;
    
    io->DIV = 0xAB;
    io->TAC = 0xF8;
    io->IF = 0xE1;
    
    io->SCX = 0;
    io->SCY = 0;
    io->LCDC = 0x91;
    io->NR10 = 0x80;
    io->NR11 = 0xbf;
    io->NR12 = 0xf3;
    io->NR13 = 0xff;
    io->NR14 = 0xbf;
    io->NR21 = 0x3f;
    io->NR23 = 0xff;
    io->NR24 = 0xbf;
    io->NR30 = 0x7f;
    io->NR31 = 0xff;
    io->NR32 = 0x9f;
    io->NR33 = 0xff;
    io->NR34 = 0xbf;
    io->NR41 = 0xff;
    io->NR44 = 0xbf;
    io->NR50 = 0x77;
    io->NR51 = 0xf3;
    io->NR52 = 0xf1;
    io->LCDC = 0x91;
    io->STAT = 0x85;
    io->LY = 0;
    io->DMA = 0xff;
    io->BGP = 0xfc;    
    
    scr->LCD_PPU_enable          = io->LCDC & 0x80;
    scr->win_tile_map_area       = io->LCDC & 0x40;
    scr->window_enable           = io->LCDC & 0x20;
    scr->BG_win_tile_data_area   = io->LCDC & 0x10;
    scr->BG_tile_map_area        = io->LCDC & 0x08;
    scr->obj_size                = io->LCDC & 0x04;
    scr->obj_enable              = io->LCDC & 0x02;
    scr->bg_win_enable_priority  = io->LCDC & 0x01;
}

void fullscreen_toggle(s_emu *emu)
{
    s_opt *opt = &emu->opt;
    s_screen *scr = &emu->scr;
    static bool previous = false;
    static int prev_w = 0, prev_h = 0;
    if(emu->in.scan[opt->opt_scancodes[OPT_FULLSCREEN]] != previous && previous == false)
    {
        
        
        if(!opt->fullscreen)
        {
            SDL_GetWindowSize(scr->w, &prev_w, &prev_h);
            opt->fullscreen = true;
            if(0 != SDL_SetWindowFullscreen(emu->scr.w, SDL_WINDOW_FULLSCREEN_DESKTOP))
            {
                fprintf(stderr, "SDL_SetWindowFullscreen: %s\n", SDL_GetError());
                //destroy_emulator(emu, EXIT_FAILURE);
            }
            resize_screen(scr);
        }
        else
        {
            opt->fullscreen = false;
            if(0 != SDL_SetWindowFullscreen(emu->scr.w, 0))
            {
                fprintf(stderr, "SDL_SetWindowFullscreen: %s\n", SDL_GetError());
                //destroy_emulator(emu, EXIT_FAILURE);
            }
            SDL_SetWindowSize(scr->w, prev_w, prev_h);
            resize_screen(scr);
        }
        
        
    }

    previous = emu->in.scan[opt->opt_scancodes[OPT_FULLSCREEN]];
}

void fast_forward_toggle(s_emu *emu)
{
    s_opt *opt = &emu->opt;
    static bool previous = false;
    if(emu->in.scan[opt->opt_scancodes[OPT_FAST_FORWARD]] != previous && previous == false)
    {
        if(!opt->fast_forward)
        {
            opt->fast_forward = true;
            if(0 != SDL_RenderSetVSync(emu->scr.r, 0))
            {
                fprintf(stderr, "Unable to set VSync to off: %s\n", SDL_GetError());
                destroy_emulator(emu, EXIT_FAILURE);
            }
        }
        else
        {
            opt->fast_forward = false;
            if(0 != SDL_RenderSetVSync(emu->scr.r, 1))
            {
                fprintf(stderr, "Unable to set VSync to on: %s\n", SDL_GetError());
                destroy_emulator(emu, EXIT_FAILURE);
            }
        }
        
    }
    previous = emu->in.scan[opt->opt_scancodes[OPT_FAST_FORWARD]];
}

void emulate(s_emu *emu)
{
    s_cpu *cpu = &emu->cpu;
    s_opt *opt = &emu->opt;
    cpu->t_cycles = 0;
    emu->frame_timer = SDL_GetTicks64();
    
    if(!emu->opt.bootrom)
        bypass_bootrom(emu);
    
    if(emu->opt.audio)
        SDL_PauseAudioDevice(emu->au.dev, 0);
    
    while(!emu->in.quit)
    {
        update_event(emu);
        if(emu->in.resize)
        {
            resize_screen(&emu->scr);
            emu->in.resize = SDL_FALSE;
        }
        if((emu->in.scan[opt->opt_scancodes[OPT_PAUSE]]) || 
           (emu->opt.framebyframe && emu->opt.newframe))
        {
            pause_menu(emu);
        }

        fast_forward_toggle(emu);
        fullscreen_toggle(emu);

        interpret(emu, emu->opcode_functions);   
        interpret(emu, emu->opcode_functions);
        interpret(emu, emu->opcode_functions);
        interpret(emu, emu->opcode_functions);

        ppu_modes_and_scanlines(emu);
        audio_update(emu);
        render_frame_and_vblank_if_needed(emu);
        
        interpret(emu, emu->opcode_functions);   
        interpret(emu, emu->opcode_functions);
        interpret(emu, emu->opcode_functions);
        interpret(emu, emu->opcode_functions);

        ppu_modes_and_scanlines(emu);
        audio_update(emu);
        render_frame_and_vblank_if_needed(emu);
        
        interpret(emu, emu->opcode_functions);   
        interpret(emu, emu->opcode_functions);
        interpret(emu, emu->opcode_functions);
        interpret(emu, emu->opcode_functions);

        ppu_modes_and_scanlines(emu);
        audio_update(emu);
        render_frame_and_vblank_if_needed(emu);
        
        interpret(emu, emu->opcode_functions);   
        interpret(emu, emu->opcode_functions);
        interpret(emu, emu->opcode_functions);
        interpret(emu, emu->opcode_functions);

        ppu_modes_and_scanlines(emu);
        audio_update(emu);
        render_frame_and_vblank_if_needed(emu);   
//        
    }
}

void joypad_update(s_emu *emu)
{
    s_io *io = &emu->cpu.io;
    s_input *in = &emu->in;
    s_opt *opt = &emu->opt;
    //action buttons
    if(!(io->P1_JOYP & 0x20))
    {
        //start
        flag_assign(!in->scan[opt->opt_scancodes[JOYP_START]],
                    &io->P1_JOYP,
                    0x08);
        //select
        flag_assign(!in->scan[opt->opt_scancodes[JOYP_SELECT]],
                    &io->P1_JOYP,
                    0x04);
        //B
        flag_assign(!in->scan[opt->opt_scancodes[JOYP_B]],
                    &io->P1_JOYP,
                    0x02);
        //A
        flag_assign(!in->scan[opt->opt_scancodes[JOYP_A]],
                    &io->P1_JOYP,
                    0x01);
    }
    else if (!(io->P1_JOYP & 0x10))
    {
        //down
        flag_assign(!in->scan[opt->opt_scancodes[JOYP_DOWN]],
                    &io->P1_JOYP,
                    0x08);
        //up
        flag_assign(!in->scan[opt->opt_scancodes[JOYP_UP]],
                    &io->P1_JOYP,
                    0x04);
        //left
        flag_assign(!in->scan[opt->opt_scancodes[JOYP_LEFT]],
                    &io->P1_JOYP,
                    0x02);
        //right
        flag_assign(!in->scan[opt->opt_scancodes[JOYP_RIGHT]],
                    &io->P1_JOYP,
                    0x01);
    }
    
    //joypad interrupt
    if((~io->P1_JOYP) & 0x0F)
        io->IF |= 0x10;
}

void pause_menu(s_emu *emu)
{
    //wait for P key release
    s_opt *opt = &emu->opt;
    while(!emu->in.quit && (emu->in.scan[opt->opt_scancodes[OPT_PAUSE]]))
    {
        update_event(emu);
        SDL_Delay(5);
    }
    
    printf(
        "Emulator paused.\n"
        "Press P to continue, N for next frame, O for options.\n"
    );
    
    emu->opt.newframe = false;
    emu->opt.framebyframe = false;
    
    while(!emu->in.quit)
    {
        update_event(emu);
        if(emu->in.scan[opt->opt_scancodes[OPT_PAUSE]])
        {
            while(emu->in.scan[opt->opt_scancodes[OPT_PAUSE]])
            {
                SDL_Delay(5);
                update_event(emu);
            }
            return;
        }
        if(emu->in.scan[opt->opt_scancodes[OPT_OPTIONS]])
        {
            if(0 == parse_options_during_exec(&emu->opt))
                return;
        }
        if(emu->in.scan[opt->opt_scancodes[OPT_NEXT_FRAME]])
        {
            while(emu->in.scan[opt->opt_scancodes[OPT_NEXT_FRAME]])
            {
                SDL_Delay(5);
                update_event(emu);
            }
            emu->opt.framebyframe = true;
            return;
        }
        SDL_Delay(5);
    }
}

/**
 * @brief Handle options at program launch or when O is pressed during pause.
 * @param opt
 * @param argc
 * @param argv
 * @param is_program_beginning: to distinguish both cases.
 * @returns 0 if user input OK, 1 if need to exit/re-ask user input.
 */
int parse_options(s_opt *opt, size_t argc, char *argv[], bool is_program_beginning)
{
    const char help_msg_beginning[] = 
    "Usage\n"
    "\n"
    "   ./game_spop <ROM file> [option]\n"
    "\n"
    "Options\n"
    "   --audio,      -a     = disable audio.\n"
    "   --audio-log          = print various audio variables in a file at each \n"
    "                          sample.\n"
    "   --bootrom,    -b     = launch the DMG bootrom before ROM. If no ROM is\n"
    "                          provided, this option is always on. The default file path\n"
    "                          is \"boot_rom/dmg_rom.bin\" but can be modified in\n"
    "                          game_spop.ini.\n"
    "   --breakpoint, -p     = enable debugging with breakpoints. The program will\n"
    "                          ask to enter a PC value breakpoint at start, and will\n"
    "                          ask for a new breakpoint when the previous one is\n"
    "                          reached.\n"
    "   --debug-info, -i     = print cpu state at each instruction.\n"
    "   --gb-doctor,  -d     = log cpu state into a file to be used with the Gameboy\n"
    "                          doctor tool (https://github.com/robert/gameboy-doctor), \n"
    "                          (only at launch). Emulator behavior might be inaccurate\n"
    "                          since LY reading always send 0x90 in this mode.\n"
    "   --log-instrs, -l     = log cpu state into a file for comparison with other\n"
    "                          emulators (only at launch).\n"
    "   --step,       -s     = enable step by step debugging. Emulator will stop\n"
    "                          at each new instruction and ask to continue or edit\n"
    "                          options.\n"
    "   --help,       -h     = show this help message and exit.\n";
    
    const char help_msg_during_exec[] = 
    "Options\n"
    "   --breakpoint, -p     = enable debugging with breakpoints. The program will\n"
    "                          ask to enter a PC value breakpoint at start, and will\n"
    "                          ask for a new breakpoint when the previous one is\n"
    "                          reached.\n"
    "   --debug-info, -i     = print cpu state at each instruction.\n"
    "   --step,       -s     = enable step by step debugging. Emulator will stop\n"
    "                          at each new instruction and ask to continue or edit\n"
    "                          options.\n"
    "   --help,       -h     = show this help message and exit.\n";
    
    for(size_t i = 0 + is_program_beginning; i < argc; i++)
    {
        if(((0 == strcmp(argv[i], "--audio")) || (0 == strcmp(argv[i], "-a"))) && (is_program_beginning))
            opt->audio = false;
        else if((0 == strcmp(argv[i], "--audio-log")) && is_program_beginning)
        {
            opt->audio_log = true;
            opt->gb_doctor = false;
            opt->log_instrs = false;
        }
        else if(((0 == strcmp(argv[i], "--bootrom")) || (0 == strcmp(argv[i], "-b"))) && (is_program_beginning))
            opt->bootrom = true;
        else if((0 == strcmp(argv[i], "--debug-info")) || (0 == strcmp(argv[i], "-i")))
            opt->debug_info = !opt->debug_info;
        else if((0 == strcmp(argv[i], "--help")) || (0 == strcmp(argv[i], "-h")))
        {
            if(is_program_beginning)
                printf("%s", help_msg_beginning);
            else
                printf("%s", help_msg_during_exec);
            return EXIT_FAILURE;
        }
        else if((0 == strcmp(argv[i], "--breakpoint")) || (0 == strcmp(argv[i], "-b")))
        {
            opt->breakpoints = !opt->breakpoints;
        }
        else if(((0 == strcmp(argv[i], "--gb-doctor")) || (0 == strcmp(argv[i], "-d"))) && (is_program_beginning))
        {
            opt->gb_doctor = true;
            opt->log_instrs = false;
            opt->audio_log = false;
        }
        else if(((0 == strcmp(argv[i], "--log-instrs")) || (0 == strcmp(argv[i], "-l"))) && (is_program_beginning))
        {
            opt->log_instrs = true;
            opt->audio_log = false;
            opt->gb_doctor = false;
        }
        else if(0 == strcmp(argv[i], "--step") || (0 == strcmp(argv[i], "-s")))
        {
            opt->step_by_step = !opt->step_by_step;
        }
        else if(0 == strncmp(argv[i], "--", 2) || ((argv[i][0] == '-') && (strlen(argv[i]) == 2)))
        {
            if(is_program_beginning)
                fprintf(stderr, "Unknown argument '%s', abort.\n\n%s", argv[i], help_msg_beginning);
            else
                fprintf(stderr, "Unknown argument '%s', abort.\n\n%s", argv[i], help_msg_during_exec);
            return EXIT_FAILURE;
        }
        else if(is_program_beginning)
        {
            opt->rom_argument = true;
            snprintf(opt->rom_filename, FILENAME_MAX, "%s", argv[i]);
            snprintf(opt->sav_filename, FILENAME_MAX, "%s.sav", argv[i]);
        }
    }

    ask_breakpoint(opt);
    
    return EXIT_SUCCESS;
}

/**
 * @brief Handle the options menu during pause.
 */
int parse_options_during_exec(s_opt *opt)
{
    bool quit = false;
    char entry[FILENAME_MAX] = "";
    char *sub;
    size_t argc = 0;
    //count number of args
    while(!quit)
    {
        printf("Active options:\n");
        if(opt->breakpoints)
            printf("  --breakpoint\n");
        if(opt->debug_info)
            printf("  --debug-info\n");
        if(opt->step_by_step)
            printf("  --step\n");
        if(!opt->breakpoints && !opt->debug_info && !opt->step_by_step)
            printf("  none.\n");
        
        printf(
            "\nEnter an option to toggle or press ENTER to continue without changes.\n"
            "See --help to have a list of available options.\n"
        );
        if(NULL == fgets(entry, FILENAME_MAX, stdin))
            continue;
        if(entry[0] == '\0')
            continue;
        if(entry[0] == '\n')
            return EXIT_SUCCESS;
        //argc count
        sub = strtok(entry, " \n");
        while(NULL != sub)
        {
            argc++;
            sub = strtok(NULL, " \n");
        }
        char argv[argc][30];
        char *ptr[30];
        //copy to argv
        sub = strtok(entry, " \n");
        snprintf(argv[0], 30, "%s", sub);
        ptr[0] = argv[0];
        for(size_t i = 1; i < argc; i++)
        {
            sub = strtok(NULL, " \n");
            snprintf(argv[i], 30, "%s", sub);
            ptr[i] = argv[i];
        }
        if(0 == parse_options(opt, argc, ptr, false))
            quit = true;
    }
    return EXIT_SUCCESS;
}

/**
 * @brief Handle the command lines arguments gived to the program
 * when lauching. 
 */
int parse_start_options(s_opt *opt, int argc, char *argv[])
{
    opt->bootrom = false;
    opt->rom_argument = false;
    opt->debug_info = false;
    opt->breakpoints = false;
    opt->step_by_step = false;
    opt->gb_doctor = false;
    opt->log_instrs = false;
    opt->fast_forward = false;
    opt->audio = true;
    opt->audio_log = false;
    opt->newframe = false;
    opt->framebyframe = false;
    opt->fullscreen = false;
    if(argc > 1)
    {
        if(0 != parse_options(opt, (size_t) argc, argv, true))
            return EXIT_FAILURE;
    }
    
    if(!opt->rom_argument && !opt->bootrom)
    {
        printf("No ROM provided: boot rom will be executed.\n");
        opt->bootrom = true;
    }
    
    return EXIT_SUCCESS;
}

void ask_breakpoint(s_opt *opt)
{
    if(!opt->breakpoints && !opt->step_by_step)
        return;
        
    bool quit = false;
    char bp[10] = "";
    while(!quit)
    {
        printf("Breakpoint value / ENTER (continue) / O (options)\n");
        if(NULL == fgets(bp, 10, stdin))
            continue;
        if(bp[0] == 'O' || bp[0] == 'o')
        {
            while(0 != parse_options_during_exec(opt));
            return;
        }
        if(bp[0] == '\n')
        {
            opt->breakpoints = false;
            //printf("Starting/continuing execution with no breakpoint.\n");
            break;
        }
        errno = 0;
        char *endptr;
        long val = strtol(bp, &endptr, 0);
        if(errno != 0)
        {
            fprintf(stderr, "strtol: %s\n", strerror(errno));
            continue;
        }
        if(val == 0 && endptr == bp)
        {
            fprintf(stderr, "Error: no digits were found.\n");
            continue;
        }
        if(val > 0xFFFF)
        {
            fprintf(stderr, "Error: breakpoint value must not exceed 0xFFFF!\n");
            continue;
        }
        if(val < 0)
        {
            fprintf(stderr, "Error: breakpoint value cannot be negative\n");
            continue;
        }
        opt->breakpoint_value = val;
        printf("Breakpoint value set to 0x%04X.\n", opt->breakpoint_value);
        quit = true;            
    }
}

void log_instructions(s_emu *emu)
{
    s_cpu *cpu = &emu->cpu;
    s_io *io = &cpu->io;
    s_opt *opt = &emu->opt;
    
    cpu->inst_counter++;

    if(opt->gb_doctor)
    {
        uint8_t pc0, pc1, pc2, pc3;
        read_memory(emu, cpu->pc, &pc0);
        read_memory(emu, cpu->pc + 1, &pc1);
        read_memory(emu, cpu->pc + 2, &pc2);
        read_memory(emu, cpu->pc + 3, &pc3);
        
        //original gameboy doctor
        if(0 > fprintf(
            opt->logfile, 
            "A:%02X F:%02X B:%02X C:%02X D:%02X E:%02X H:%02X L:%02X "
            "SP:%04X PC:%04X PCMEM:%02X,%02X,%02X,%02X\n",
            cpu->regA, cpu->regF, cpu->regB, cpu->regC, cpu->regD, cpu->regE,
            cpu->regH, cpu->regL, cpu->sp, cpu->pc, pc0, pc1, pc2, pc3
        ))
        {
            perror("log_instructions fprintf: ");
            destroy_emulator(emu, EXIT_FAILURE);
        }
    }
    else if(opt->log_instrs || opt->debug_info)
    {
        uint8_t pc0, pc1, pc2, pc3;
        read_memory(emu, cpu->pc, &pc0);
        read_memory(emu, cpu->pc + 1, &pc1);
        read_memory(emu, cpu->pc + 2, &pc2);
        read_memory(emu, cpu->pc + 3, &pc3);
        
        //custom log syntax
        char z = '-';
        char n = '-';
        char h = '-';
        char c = '-';
        char pcmem[50] = "";
        char tmp[10];
        snprintf(pcmem, 50, " %02x", pc0);
        if(emu->length_table[pc0] >= 2)
        {
            snprintf(tmp, 10, " %02x", pc1);
            strncat(pcmem, tmp, 50);
        }
        if(emu->length_table[pc0] >= 3)
        {
            snprintf(tmp, 10, " %02x", pc2);
            strncat(pcmem, tmp, 50);
        }  
        if(cpu->regF & ZERO_FMASK) z = 'Z';
        if(cpu->regF & NEGATIVE_FMASK) n = 'N';
        if(cpu->regF & HALF_CARRY_FMASK) h = 'H';
        if(cpu->regF & CARRY_FMASK) c = 'C';

        if(opt->debug_info)
        {
            fprintf(
                stdout, 
                ANSI_COLOR_CYAN
                "A:%02x F:%c%c%c%c BC:%02X%02x DE:%02x%02x HL:%02x%02x " ANSI_COLOR_MAGENTA
                "SP:%04x PC:%04x" ANSI_COLOR_GREEN " (cy: %lu) ppu:+%u PCMEM:%-9s  " ANSI_COLOR_YELLOW "%s\n" ANSI_COLOR_RESET,
                cpu->regA, z, n, h, c, cpu->regB, cpu->regC, cpu->regD, cpu->regE,
                cpu->regH, cpu->regL, cpu->sp, cpu->pc, cpu->debug_clock, io->STAT & 0x03,
                pcmem, emu->mnemonic_index[pc0]
            );
        }
        if(opt->log_instrs)
        {
            fprintf(
                opt->logfile, 
                "A:%02x F:%c%c%c%c BC:%02X%02x DE:%02x%02x HL:%02x%02x "
                "SP:%04x PC:%04x (cy: %lu) ppu:+%u PCMEM:%-9s  %s\n",
                cpu->regA, z, n, h, c, cpu->regB, cpu->regC, cpu->regD, cpu->regE,
                cpu->regH, cpu->regL, cpu->sp, cpu->pc, cpu->debug_clock, io->STAT & 0x03,
                pcmem, emu->mnemonic_index[pc0]
            );
        }
    }
}
