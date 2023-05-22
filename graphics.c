#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>
#include "emulator.h"
#include "graphics.h"

int initialize_screen(s_emu *emu)
{
    s_screen *screen = &emu->screen;
    screen->r = NULL;
    screen->w = NULL;
    screen->scr = NULL;
    screen->format = NULL;
    screen->pixel_w = 2;
    screen->pixel_h = 2;
    
    screen->w = SDL_CreateWindow("Game_spop", SDL_WINDOWPOS_CENTERED,
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
    
    emu->cpu.io_reg.STAT |= 2;
    
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

int draw_background(s_emu *emu, size_t i, uint8_t *pixel)
{
    s_screen *screen = &emu->screen;
    s_cpu *cpu = &emu->cpu;
    s_io *io_reg = &cpu->io_reg;
    
    if(!screen->bg_win_enable_priority)
        return EXIT_SUCCESS;
        
    uint16_t bg_map_start_adress = screen->BG_tile_map_area ? 0x1C00 : 0x1800;
    uint16_t bg_win_data_start_adr = screen->BG_win_tile_data_area ? 0 : 0x800;
    
    //relative adress of the tile in the tile map
    uint16_t rel_bg_tilemap_adress = (io_reg->LY + io_reg->SCY) / 8;
    rel_bg_tilemap_adress *= 32;
    rel_bg_tilemap_adress += (io_reg->SCX + i) / 8;
    if(rel_bg_tilemap_adress > 0x400)
    {
        fprintf(stderr, "ERROR: adress is out of tile map bounds!\n");
        return EXIT_FAILURE;
    }
    //number of the tile by its place in tile data
    uint8_t tilenum = cpu->VRAM[bg_map_start_adress + rel_bg_tilemap_adress]; 
    // adress of the two bytes in tiles data we want to read (corresponding
    // to the current scanline we are drawing)
    uint16_t bg_data_adress = bg_win_data_start_adr + 16 * tilenum;
    bg_data_adress += 2 * ((io_reg->LY + io_reg->SCY) % 8);
    
    uint8_t bitmask = (0x80 >> ((i + io_reg->SCX) % 8));
    
    *pixel =     (cpu->VRAM[bg_data_adress] 
                //Selects the pixel we are currently drawing
                & bitmask)
                //Shifts to the lsb
                >> (7 - i % 8);
                
    //does the same for the second byte of data
    *pixel |=    (cpu->VRAM[bg_data_adress + 1]
                & bitmask)
                >> (6 - i % 8);
    
    //modify pixel color through the palette
    *pixel = (io_reg->BGP & (0x03 << 2 * *pixel)) >> 2 * *pixel;
    
    return EXIT_SUCCESS;
}

int draw_window(s_emu *emu, UNUSED size_t i)
{
    s_screen *screen = &emu->screen;
    if(!screen->window_enable)
        return EXIT_SUCCESS;
        
    fprintf(stderr, "WARNING: Attempt to draw window! (unimplemented)\n");
    return EXIT_FAILURE;
}

int draw_OBJ(s_emu *emu, UNUSED size_t i)
{
    s_screen *screen = &emu->screen;
    if(!screen->obj_enable)
        return EXIT_SUCCESS;
        
    fprintf(stderr, "WARNING: Attempt to draw OBJ! (unimplemented)\n");
    return EXIT_FAILURE;
}

int draw_scanline(s_emu *emu)
{
    //s_screen *screen = &emu->screen;
    s_cpu *cpu = &emu->cpu;
    s_io *io_reg = &cpu->io_reg;
    s_screen *screen = &emu->screen;
    
    if(io_reg->LY >= 144)
        return EXIT_SUCCESS;

    if(!screen->LCD_PPU_enable)
    {
        for(size_t i = 0; i < PIX_BY_W; i++)
        {
            //draw blank line
            screen->pixels[io_reg->LY * PIX_BY_W + i] = SDL_MapRGBA(
                screen->format, 255, 255, 255, 255
            );
        }
        return EXIT_SUCCESS;
    }
    //uint16_t win_map_start_adress = screen->win_tile_map_area ? 0x9C00 : 0x9800;

    if(io_reg->LY + io_reg->SCY > 255)
    {
        fprintf(stderr, "WARNING: LY + SCY is out of tile map area!!\n");
        return EXIT_FAILURE;
    }
    //95 + 160 = 255
    if(io_reg->SCX > 95)
    {
        fprintf(stderr, "WARNING: SCX + screen width is out of tilemap area!!\n");
        return EXIT_FAILURE;
    }
    
    //for each pixel of the scanline
    for(size_t i = 0; i < PIX_BY_W; i++)
    {
        uint8_t pixel = 0;
        if(0 != draw_background(emu, i, &pixel))
            return EXIT_FAILURE;
        if(0 != draw_window(emu, i))
            return EXIT_FAILURE;
        if(0 != draw_OBJ(emu, i))
            return EXIT_FAILURE;
            
        //convert to 4 possible grayscales [0, 255] values
        pixel = 255 - 85 * pixel;
        //draw in texture
        screen->pixels[io_reg->LY * PIX_BY_W + i] = SDL_MapRGBA(
            screen->format, pixel, pixel, pixel, 255
        );
    }
    
    return EXIT_SUCCESS;
}

void ppu_modes_and_scanlines(s_emu *emu)
{
    s_cpu *cpu = &emu->cpu;
    s_io *io_reg = &cpu->io_reg;
    s_screen *screen = &emu->screen;
    
    io_reg->STAT &= ~0x03;
    
    if(cpu->t_cycles >= 456)
    {
        cpu->io_reg.LY++;
        cpu->t_cycles -= 456;
        //PPU enable : stat = mod2; else stat = mod 1 (VBlank)
        io_reg->STAT |= (screen->LCD_PPU_enable) ? 2 : 1;    
        if(0 != draw_scanline(emu))
            destroy_emulator(emu, EXIT_FAILURE);
        return;
    }
    
    if(!screen->LCD_PPU_enable)
    {
        //vblank
        io_reg->STAT |= 1;
        return;
    }
    
    if(cpu->t_cycles < PPU_MODE2)
    {
        //Searching OAM for OBJs whose Y coordinate overlap this line
        io_reg->STAT |= 2;
        return;
    }
    
    if(cpu->t_cycles < PPU_MODE3 + PPU_MODE2)
    {
        //Reading OAM and VRAM to generate the picture
        io_reg->STAT |= 3;
        return;
    }
    
    if(cpu->t_cycles < PPU_MODE0 + PPU_MODE3 + PPU_MODE2)
    {
        //Nothing (HBlank)
        io_reg->STAT |= 0;
        return;
    }
}

void render_frame_and_vblank_if_needed(s_emu *emu)
{
    s_cpu *cpu = &emu->cpu;
    s_screen *screen = &emu->screen;
    s_io *io_reg = &cpu->io_reg;
    
    if(io_reg->LY < 144)
    {
        io_reg->IF &= ~0x01;
        return;
    }
    
    //set VBlank interrupt flag 
    io_reg->IF |= 0x01;
    io_reg->STAT &= ~0x03;
    io_reg->STAT |= 1;
    
    if(cpu->io_reg.LY < 154)
        return;
        
    Uint64 elapsed = SDL_GetTicks64() - emu->frame_timer;
    if(elapsed <= 16)
        SDL_Delay(16 - elapsed);
    SDL_UnlockTexture(screen->scr);
    SDL_RenderCopy(screen->r, screen->scr, NULL, NULL);
    SDL_RenderPresent(screen->r);
    if(0 != lockscreen(screen))
        destroy_emulator(emu, EXIT_FAILURE);
    if(elapsed > 16)
        printf("last frame = %lu ms\n", elapsed);
    emu->frame_timer = SDL_GetTicks64();
    cpu->io_reg.LY = 0;
    //clear VBlank flag
    io_reg->IF &= (~0x01);
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
