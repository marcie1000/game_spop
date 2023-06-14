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
                input->key[input->event.key.keysym.scancode] = SDL_TRUE;                
                break;
            case(SDL_KEYUP):
                input->key[input->event.key.keysym.scancode] = SDL_FALSE;
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
    
    if(0 != load_rom(emu))
        return EXIT_FAILURE;
    
    if(emu->opt.bootrom)
    {
        if(0 != load_boot_rom(&emu->cpu))
            return EXIT_FAILURE;
    }
    
    //gb doctor log file
//    if(!opt->gb_doctor && !opt->log_instrs)
//        return EXIT_SUCCESS;
        
    opt->logfile = fopen("gb_insts.csv", "w");
    if(opt->logfile == NULL)
    {
        fprintf(stderr, "fopen gb_insts.csv: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }
    fprintf(opt->logfile, "fstream;volume;ch_vol_sweep_counter[0];ch_vol_sweep_timer[0];"
    "period_counter;npsp;duty;samples_played\n");
    
    return EXIT_SUCCESS;
}

int load_boot_rom(s_cpu *cpu)
{
    FILE *bootrom = fopen("boot_rom/dmg_rom.bin", "rb");
    if(NULL == bootrom)
    {
        perror("ERROR: cannot open boot_rom/dmg_rom.bin: ");
        return EXIT_FAILURE;
    }
    
    fread(&cpu->ROM_Bank[0][0], sizeof(cpu->ROM_Bank[0][0]), 0x100, bootrom);
    fclose(bootrom);
    
    printf("Boot rom loaded.\n");
    
    return EXIT_SUCCESS;
}

int load_rom(s_emu *emu)
{
    s_cpu *cpu = &emu->cpu;
    if(!emu->opt.rom_argument)
    {
        memset(cpu->ROM_Bank, 0xFF, sizeof(cpu->ROM_Bank));
        return EXIT_SUCCESS;
    }
    
    FILE *rom = fopen(emu->opt.rom_filename, "rb");
    if(NULL == rom)
    {
        fprintf(stderr, "ERROR: cannot open '%s': %s\n", emu->opt.rom_filename, strerror(errno));
        return EXIT_FAILURE;
    }
    
    fread(&cpu->ROM_Bank[0][0], sizeof(cpu->ROM_Bank[0][0]), ROM_BANK_SIZE, rom);
    //keeps the rom bytes separatly during bootrom execution
    memcpy(cpu->ROM_Bank_0_tmp, cpu->ROM_Bank[0], sizeof(cpu->ROM_Bank[0]));
    fread(&cpu->ROM_Bank[1][0], sizeof(cpu->ROM_Bank[1][0]), ROM_BANK_SIZE, rom);
    fclose(rom);
    
    printf("Rom loaded.\n");
    
    return EXIT_SUCCESS;
}

void destroy_emulator(s_emu *emu, int status)
{
    destroy_screen(&emu->screen);
    destroy_audio(emu);
    SDL_Quit();
    if((NULL != emu->opt.logfile) /*&& (emu->opt.gb_doctor || emu->opt.log_instrs)*/)
        fclose(emu->opt.logfile);
    exit(status);
}

void bypass_bootrom(s_emu *emu)
{
    s_cpu *cpu = &emu->cpu;
    s_io *io = &cpu->io_reg;
    
    cpu->pc = 0x100;
    cpu->regA = 0x01;
    cpu->regF = 0xB0;
    cpu->regC = 0x13;
    cpu->regE = 0xD8;
    cpu->regH = 0x01;
    cpu->regL = 0x4D;
    cpu->sp = 0xFFFE;
    
    io->SCX = 0;
    io->SCY = 0;
    io->LCDC = 0x91;
    io->NR10 = 0x80;
    io->NR11 = 0xbf;
    io->NR12 = 0xf3;
    io->NR13 = 0xff;
    io->NR14 = 0xbf;
    io->NR50 = 0x77;
    io->NR51 = 0xf3;
    io->NR52 = 0xf1;
    io->BGP = 0xfc;    
}

void fast_forward_toggle(s_emu *emu)
{
    s_opt *opt = &emu->opt;
    static bool previous = false;
    if(emu->in.key[SDL_SCANCODE_SPACE] != previous && previous == false)
    {
        if(!opt->fast_forward)
        {
            opt->fast_forward = true;
            if(0 != SDL_RenderSetVSync(emu->screen.r, 0))
            {
                fprintf(stderr, "Unable to set VSync to off: %s\n", SDL_GetError());
                destroy_emulator(emu, EXIT_FAILURE);
            }
        }
        else
        {
            opt->fast_forward = false;
            if(0 != SDL_RenderSetVSync(emu->screen.r, 1))
            {
                fprintf(stderr, "Unable to set VSync to on: %s\n", SDL_GetError());
                destroy_emulator(emu, EXIT_FAILURE);
            }
        }
        
    }
    previous = emu->in.key[SDL_SCANCODE_SPACE];
}

void emulate(s_emu *emu)
{
    s_cpu *cpu = &emu->cpu;
    cpu->t_cycles = 0;
    emu->frame_timer = SDL_GetTicks64();
    
    if(!emu->opt.bootrom)
        bypass_bootrom(emu);
    
    if(emu->opt.audio)
        SDL_PauseAudioDevice(emu->audio.dev, 0);
    
    while(!emu->in.quit)
    {
        update_event(emu);
        if(emu->in.resize)
        {
            resize_screen(&emu->screen);
            emu->in.resize = SDL_FALSE;
        }
        if(emu->in.key[SDL_SCANCODE_P])
        {
            pause_menu(emu);
        }

        fast_forward_toggle(emu);

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
        
    }
}

void joypad_update(s_emu *emu)
{
    s_io *io = &emu->cpu.io_reg;
    s_input *in = &emu->in;
    //action buttons
    if(!(io->P1_JOYP & 0x20))
    {
        //start
        flag_assign(!in->key[SDL_SCANCODE_RETURN],
                    &io->P1_JOYP,
                    0x08);
        //select
        flag_assign(!in->key[SDL_SCANCODE_BACKSPACE],
                    &io->P1_JOYP,
                    0x04);
        //B
        flag_assign(!in->key[SDL_SCANCODE_L],
                    &io->P1_JOYP,
                    0x02);
        //A
        flag_assign(!in->key[SDL_SCANCODE_M],
                    &io->P1_JOYP,
                    0x01);
    }
    else if (!(io->P1_JOYP & 0x10))
    {
        //down
        flag_assign(!in->key[SDL_SCANCODE_S],
                    &io->P1_JOYP,
                    0x08);
        //up
        flag_assign(!in->key[SDL_SCANCODE_W],
                    &io->P1_JOYP,
                    0x04);
        //left
        flag_assign(!in->key[SDL_SCANCODE_A],
                    &io->P1_JOYP,
                    0x02);
        //right
        flag_assign(!in->key[SDL_SCANCODE_D],
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
    while(!emu->in.quit && emu->in.key[SDL_SCANCODE_P])
    {
        update_event(emu);
        SDL_Delay(5);
    }
    
    printf(
        "Emulator paused.\n"
        "Press P to continue, O to options.\n"
    );
    
    while(!emu->in.quit)
    {
        update_event(emu);
        if(emu->in.key[SDL_SCANCODE_P])
        {
            while(emu->in.key[SDL_SCANCODE_P])
                update_event(emu);
            return;
        }
        if(emu->in.key[SDL_SCANCODE_O])
        {
            if(0 == parse_options_during_exec(&emu->opt))
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
    "   --audio, -a          = enable audio (expermimental).\n"
    "   --breakpoint, -p     = enable debugging with breakpoints. The program will\n"
    "                          ask to enter a PC value breakpoint at start, and will\n"
    "                          ask for a new breakpoint when the previous one is\n"
    "                          reached.\n"
    "   --bypass-bootrom, -b = launch directly the ROM (only if a rom is passed\n"
    "                          in argument). This option can only be provided at\n"
    "                          launch.\n"
    "   --debug-info, -i     = at every new instruction, prints the mnemonic, the\n"
    "                          3 bytes object code, and all registers, PC, SP and\n"
    "                          register F flags values in the console. Emulator is\n"
    "                          much slower when this option is enabled.\n"
    "   --gb-doctor, -d      = log cpu state into a file to be used with the Gameboy\n"
    "                          doctor tool (https://github.com/robert/gameboy-doctor), \n"
    "                          (only at launch). Emulator behavior might be inaccurate\n"
    "                          since LY reading always send 0x90 in this mode.\n"
    "   --log-instrs, -l     = log cpu state into a file for comparison with other\n"
    "                          emulators (only at launch).\n"
    "   --step, -s           = enable step by step debugging. Emulator will stop\n"
    "                          at each new instruction and ask to continue or edit\n"
    "                          options.\n"
    "   --help, -h           = show this help message and exit.\n";
    
    const char help_msg_during_exec[] = 
    "Options\n"
    "   --breakpoint, -p     = enable debugging with breakpoints. The program will\n"
    "                          ask to enter a PC value breakpoint at start, and will\n"
    "                          ask for a new breakpoint when the previous one is\n"
    "                          reached.\n"
    "   --debug-info, -i     = at every new instruction, prints the mnemonic, the\n"
    "                          3 bytes object code, and all registers, PC, SP and\n"
    "                          register F flags values in the console. Emulator is\n"
    "                          much slower when this option is enabled.\n"
    "   --step, -s           = enable step by step debugging. Emulator will stop\n"
    "                          at each new instruction and ask to continue or edit\n"
    "                          options.\n"
    "   --help, -h           = show this help message and exit.\n";
    
    for(size_t i = 0 + is_program_beginning; i < argc; i++)
    {
        if(((0 == strcmp(argv[i], "--audio")) || (0 == strcmp(argv[i], "-a"))) && (is_program_beginning))
            opt->audio = true;
        else if(((0 == strcmp(argv[i], "--bypass-bootrom")) || (0 == strcmp(argv[i], "-b"))) && (is_program_beginning))
            opt->bootrom = false;
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
            opt->gb_doctor = !opt->gb_doctor;
        }
        else if(((0 == strcmp(argv[i], "--log-instrs")) || (0 == strcmp(argv[i], "-l"))) && (is_program_beginning))
        {
            opt->log_instrs = !opt->log_instrs;
        }
        else if(0 == strcmp(argv[i], "--step") || (0 == strcmp(argv[i], "-s")))
        {
            opt->step_by_step = !opt->step_by_step;
        }
        else if(0 == strncmp(argv[i], "--", 2))
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
    opt->bootrom = true;
    opt->rom_argument = false;
    opt->debug_info = false;
    opt->breakpoints = false;
    opt->step_by_step = false;
    opt->gb_doctor = false;
    opt->log_instrs = false;
    opt->fast_forward = false;
    opt->audio = false;
    if(argc <= 1)
        return EXIT_SUCCESS;
    
    if(0 != parse_options(opt, (size_t) argc, argv, true))
        return EXIT_FAILURE;
    
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
        printf("Enter a breakpoint value OR press ENTER to start/continue program normally OR\n"
               "enter 'O' to see options menu:\n");
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
            printf("Starting/continuing execution with no breakpoint.\n");
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
    else if(opt->log_instrs)
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

        if(0 > fprintf(
            opt->logfile, 
            "A:%02x F:%c%c%c%c BC:%02X%02x DE:%02x%02x HL:%02x%02x "
            "SP:%04x PC:%04x PCMEM:%-9s  %s\n",
            cpu->regA, z, n, h, c, cpu->regB, cpu->regC, cpu->regD, cpu->regE,
            cpu->regH, cpu->regL, cpu->sp, cpu->pc, pcmem, emu->mnemonic_index[pc0]
        ))
        {
            perror("log_instructions fprintf: ");
            destroy_emulator(emu, EXIT_FAILURE);
        }
    }
}
