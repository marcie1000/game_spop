#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>
#include "emulator.h"
#include "graphics.h"

int initialize_screen(s_screen *screen)
{
    screen->r = NULL;
    screen->w = NULL;
    screen->scr = NULL;
    screen->format = NULL;
    screen->pixel_w = 2;
    screen->pixel_h = 2;
    
    screen->w = SDL_CreateWindow("Game_spop emulator", SDL_WINDOWPOS_CENTERED,
                                 SDL_WINDOWPOS_CENTERED, PIX_BY_W * screen->pixel_w, PIX_BY_H * screen->pixel_h,
                                 SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if(screen->w == NULL)
    {
        fprintf(stderr, "Error SDL_CreateWindow: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }
    
    screen->r = SDL_CreateRenderer(screen->w, -1, SDL_RENDERER_ACCELERATED /*| SDL_RENDERER_PRESENTVSYNC*/);
    if(screen->r == NULL)
    {
        fprintf(stderr, "Error SDL_CreateRenderer: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }
    
    SDL_SetRenderTarget(screen->r, NULL);
    
    screen->scr = SDL_CreateTexture(screen->r, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, PIX_BY_W, PIX_BY_H);
    if(NULL == screen->scr)
    {
        fprintf(stderr, "Error SDL_CreateTexture: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }
    
    if(0 != lockscreen(screen))
        return EXIT_FAILURE;
    
    screen->format = SDL_AllocFormat(SDL_PIXELFORMAT_RGBA8888);
    if(NULL == screen->format)
    {
        fprintf(stderr, "Error SDL_AllocFormat: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}

int lockscreen(s_screen *screen)
{
    void *tmp;
    
    if(0 != SDL_LockTexture(screen->scr, NULL, &tmp, &screen->pitch))
    {
        fprintf(stderr, "Error SDL_LockTexture: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }
    
    screen->pixels = tmp;
    return EXIT_SUCCESS;
}

void destroy_screen(s_screen *screen)
{
    if(NULL != screen->r)
        SDL_DestroyRenderer(screen->r);
    if(NULL != screen->w)
        SDL_DestroyWindow(screen->w);
    if(NULL != screen->scr)
        SDL_DestroyTexture(screen->scr);
    if(NULL != screen->format)
        SDL_FreeFormat(screen->format);
}

void resize_screen(s_screen *screen)
{
    int w, h;
    SDL_GetWindowSize(screen->w, &w, &h);
    screen->pixel_w = w / PIX_BY_W;
    screen->pixel_h = h / PIX_BY_H;
}

//int read_tilemap(s_emu *emu)
//{
//    s_screen *screen = &emu->screen;
//    s_cpu *cpu = &emu->cpu;
//    
//    uint16_t map_start_adress = screen->BG_tile_map_area ? 0x9C00 : 0x9800;
//    uint16_t data_start_adress = screen->BG_win_tile_data_area ? 0x8000 : 0x8800;
//    
//    return EXIT_FAILURE;
//}

int draw_scanline(s_emu *emu)
{
    s_screen *screen = &emu->screen;
    s_cpu *cpu = &emu->cpu;
    s_io *io_reg = &cpu->io_reg;
    
    if(io_reg->LY >= 144)
        return EXIT_SUCCESS;
    
    screen->LCD_PPU_enable          = io_reg->LCDC & 0x80;
    screen->win_tile_map_area       = io_reg->LCDC & 0x40;
    screen->window_enable           = io_reg->LCDC & 0x20;
    screen->BG_win_tile_data_area   = io_reg->LCDC & 0x10;
    screen->BG_tile_map_area        = io_reg->LCDC & 0x08;
    screen->obj_size                = io_reg->LCDC & 0x04;
    screen->obj_enable              = io_reg->LCDC & 0x02;
    screen->bg_win_enable_priority  = io_reg->LCDC & 0x01;
    
    memset(screen->current_row, 0, sizeof(screen->current_row));

    uint16_t map_start_adress = screen->BG_tile_map_area ? 0x1C00 : 0x1800;
    uint16_t data_start_adress = screen->BG_win_tile_data_area ? 0 : 0x800;

    if(io_reg->LY + io_reg->SCY > 255)
    {
        fprintf(stderr, "WARNING: LY + SCY > 255 !!\n");
        return EXIT_FAILURE;
    }
    //95 + 160 = 255
    if(io_reg->SCX > 95)
    {
        fprintf(stderr, "WARNING: SCX > 95 !!\n");
        return EXIT_FAILURE;
    }
    
    for(size_t i = 0; i < PIX_BY_W; i++)
    {
        uint16_t relative_adress = (io_reg->LY + io_reg->SCY) / 8;
        relative_adress *= 32;
        relative_adress += (io_reg->SCX + i) / 8;
        if(relative_adress > 0x400)
        {
            fprintf(stderr, "ERROR: Relative tile map adress > 0x400 !!\n");
            return EXIT_FAILURE;
        }
        uint8_t tileno = cpu->VRAM[map_start_adress + relative_adress];            
        uint16_t data_adress = data_start_adress + 16 * tileno;
        data_adress += 2 * ((io_reg->LY + io_reg->SCY) % 8);
        screen->current_row[i] |= cpu->VRAM[data_adress]        & (0x80 >> ((i + io_reg->SCX) % 8));
        screen->current_row[i] |= cpu->VRAM[data_adress + 1]    & (0x80 >> ((i + io_reg->SCX) % 8));
        
        screen->pixels[io_reg->LY * PIX_BY_W + i] = SDL_MapRGBA(
            screen->format, 
            255 - 85 * screen->current_row[i], 
            255 - 85 * screen->current_row[i], 
            255 - 85 * screen->current_row[i], 
            255
        );
    }
    
    return EXIT_SUCCESS;
}

//int load_tile(s_emu *emu, uint8_t x_pos, uint8_t y_pos, uint8_t index, UNUSED uint8_t attributes)
//{
//    s_screen *screen = &emu->screen;
//    s_cpu *cpu = &emu->cpu;
//    
//    uint8_t shift = cpu->io_reg.LY - y_pos;
//    if(shift >= 7)
//    {
//        fprintf(stderr, "ERROR load_tile: tile not in current scanline.\n");
//        return EXIT_FAILURE;
//    }
//    
//    for(size_t j = x_pos; j < SPRITEPOS_X_LIMIT; j++)
//    {
//        screen->current_row[j] |= cpu->VRAM[16 * index + shift * 2]     & (0x80 >> j);
//        screen->current_row[j] |= cpu->VRAM[16 * index + shift * 2 + 1] & (0x80 >> j);
//    }
//    
//    return EXIT_SUCCESS;
//}
//
//int scan_OAM(s_emu *emu)
//{
//    s_screen *screen = &emu->screen;
//    s_cpu *cpu = &emu->cpu;
//    
//    for(size_t i = 0; i < OAM_SIZE; i += 4)
//    {
////        printf("i=%lu, %02X, %02X, %02X, %02X\n", i, cpu->OAM[i], cpu->OAM[i+1], cpu->OAM[i+2], cpu->OAM[i+3]);
//        //Y position
//        if((cpu->OAM[i] <= cpu->io_reg.LY) && (cpu->OAM[i] + 8 * screen->obj_size >= cpu->io_reg.LY))
//        {
//            //X position
//            if(cpu->OAM[i + 1] <= SPRITEPOS_X_LIMIT)
//            {
//                if(0 != load_tile(emu, cpu->OAM[i + 1], cpu->OAM[i], cpu->OAM[i + 2], cpu->OAM[i + 3]))
//                    return EXIT_FAILURE;
//            }
//        }
//    }
//    return EXIT_SUCCESS;
//}
//
