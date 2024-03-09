#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>
#include <assert.h>
#include "SDL_pixels.h"
#include "SDL_render.h"
#include "SDL_video.h"
#include "emulator.h"
#include "graphics.h"


int initialize_screen(s_emu *emu)
{
    s_screen *scr = &emu->scr;
    scr->r = NULL;
    scr->w = NULL;
    scr->scr = NULL;
    scr->scrcpy = NULL;
    scr->pixel_w = 2;
    scr->pixel_h = 2;
    
    scr->window_maximized = false;
    scr->win_LY = 0;
    
    scr->w = SDL_CreateWindow(
        "Game_spop", SDL_WINDOWPOS_CENTERED,
         SDL_WINDOWPOS_CENTERED, PIX_BY_W * scr->pixel_w, PIX_BY_H * scr->pixel_h,
         SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
    );
    if(scr->w == NULL)
    {
        fprintf(stderr, "Error SDL_CreateWindow: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    unsigned renderer_flags = SDL_RENDERER_ACCELERATED;
    if(!emu->opt.fast_forward)
        renderer_flags |= SDL_RENDERER_PRESENTVSYNC;

    scr->r = SDL_CreateRenderer(scr->w, -1, renderer_flags);
    if(scr->r == NULL)
    {
        fprintf(stderr, "Error SDL_CreateRenderer: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    SDL_SetRenderTarget(scr->r, NULL);
    SDL_SetRenderDrawBlendMode(scr->r, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(scr->r, 0, 0, 0, 255);
    
    //texture containing main output
    scr->scr = SDL_CreateTexture(scr->r, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, PIX_BY_W, PIX_BY_H);
    if(NULL == scr->scr)
    {
        fprintf(stderr, "Error SDL_CreateTexture scr: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }
    
    //texture containing copy of the last frame that will be shown at 50% transparency for better compatibility
    //with graphic effects used in some games (like transparent effects in Zelda link's awakening).
    scr->scrcpy = SDL_CreateTexture(scr->r, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, PIX_BY_W, PIX_BY_H);
    if(NULL == scr->scrcpy)
    {
        fprintf(stderr, "Error SDL_CreateTexture scrcpy: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }
    if(0 != SDL_SetTextureAlphaMod(scr->scrcpy, 127))
    {
        fprintf(stderr, "Error SDL_SetTextureAlphaMod: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }
    SDL_SetTextureBlendMode(scr->scrcpy, SDL_BLENDMODE_BLEND);
    
    if(0 != lockscreen(scr))
        return EXIT_FAILURE;
    
    scr->format = SDL_AllocFormat(SDL_PIXELFORMAT_RGBA8888);
    if(NULL == scr->format)
    {
        fprintf(stderr, "Error SDL_AllocFormat: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }
    
    emu->cpu.io.STAT = 2;
    scr->LCD_PPU_enable = false;
    scr->win_tile_map_area = false;
    scr->window_enable = false;
    scr->BG_win_tile_data_area = false;
    scr->BG_tile_map_area = false;
    scr->obj_size = false;
    scr->obj_enable = false;
    scr->bg_win_enable_priority = false;
    
    return EXIT_SUCCESS;
}

int initialize_plot_win(s_emu *emu)
{
    s_plot *plot = &emu->scr.plot;
    plot->r = NULL;
    plot->w = NULL;
    plot->plot = NULL;
    /* scr->scrcpy = NULL; */

    if(!emu->opt.plot_instructions)
        return EXIT_SUCCESS;

    plot->width = 1000;
    plot->height = 1000;

    plot->window_maximized = false;
    /* scr->win_LY = 0; */

    plot->w = SDL_CreateWindow(
        "Game_spop", SDL_WINDOWPOS_CENTERED,
         SDL_WINDOWPOS_CENTERED, plot->width, plot->height,
         SDL_WINDOW_SHOWN
    );
    if(plot->w == NULL)
    {
        fprintf(stderr, "Error SDL_CreateWindow: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    plot->r = SDL_CreateRenderer(plot->w, -1, SDL_RENDERER_ACCELERATED);
    if(plot->r == NULL)
    {
        fprintf(stderr, "Error SDL_CreateRenderer: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    SDL_SetRenderTarget(plot->r, NULL);
    SDL_SetRenderDrawBlendMode(plot->r, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(plot->r, 255, 0, 0, 255);

    //texture containing main output
    plot->plot = SDL_CreateTexture(plot->r, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, plot->width, plot->height);
    if(NULL == plot->plot)
    {
        fprintf(stderr, "Error SDL_CreateTexture plot: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    if(0 != lockscreen_plot(plot))
        return EXIT_FAILURE;

    plot->format = SDL_AllocFormat(SDL_PIXELFORMAT_RGBA8888);
    if(NULL == plot->format)
    {
        fprintf(stderr, "Error SDL_AllocFormat: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    // paint texture in white
    for(size_t i = 0; i < plot->width * plot->height; i++)
    {
        plot->pixels[i] = SDL_MapRGBA(plot->format, 255, 255, 255, 255);
    }
    SDL_UnlockTexture(plot->plot);
    SDL_RenderCopy(plot->r, plot->plot, NULL, NULL);
    SDL_RenderPresent(plot->r);

    if(0 != lockscreen_plot(plot))
        return EXIT_FAILURE;
    //texture containing copy of the last frame that will be shown at 50% transparency for better compatibility
    //with graphic effects used in some games (like transparent effects in Zelda link's awakening).
    /* scr->scrcpy = SDL_CreateTexture(scr->r, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, PIX_BY_W, PIX_BY_H); */
    /* if(NULL == scr->scrcpy) */
    /* { */
    /*     fprintf(stderr, "Error SDL_CreateTexture scrcpy: %s\n", SDL_GetError()); */
    /*     return EXIT_FAILURE; */
    /* } */
    /* if(0 != SDL_SetTextureAlphaMod(scr->scrcpy, 127)) */
    /* { */
    /*     fprintf(stderr, "Error SDL_SetTextureAlphaMod: %s\n", SDL_GetError()); */
    /*     return EXIT_FAILURE; */
    /* } */
    /* SDL_SetTextureBlendMode(scr->scrcpy, SDL_BLENDMODE_BLEND); */


    /* emu->cpu.io.STAT = 2; */
    /* scr->LCD_PPU_enable = false; */
    /* scr->win_tile_map_area = false; */
    /* scr->window_enable = false; */
    /* scr->BG_win_tile_data_area = false; */
    /* scr->BG_tile_map_area = false; */
    /* scr->obj_size = false; */
    /* scr->obj_enable = false; */
    /* scr->bg_win_enable_priority = false; */

    return EXIT_SUCCESS;
}

int lockscreen(s_screen *scr)
{
    void *tmp;
    
    if(0 != SDL_LockTexture(scr->scr, NULL, &tmp, &scr->pitch))
    {
        fprintf(stderr, "Error SDL_LockTexture: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }
    
    scr->pixels = tmp;
    return EXIT_SUCCESS;
}


int lockscreen_plot(s_plot *plot)
{
    void *tmp;

    if(0 != SDL_LockTexture(plot->plot, NULL, &tmp, &plot->pitch))
    {
        fprintf(stderr, "Error SDL_LockTexture: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    plot->pixels = tmp;
    return EXIT_SUCCESS;
}

void destroy_screen(s_screen *scr)
{
    if(NULL != scr->format)
        SDL_FreeFormat(scr->format);
    if(NULL != scr->scr)
        SDL_DestroyTexture(scr->scr);
    if(NULL != scr->scrcpy)
        SDL_DestroyTexture(scr->scrcpy);
    if(NULL != scr->r)
        SDL_DestroyRenderer(scr->r);
    if(NULL != scr->w)
        SDL_DestroyWindow(scr->w);
}

void destroy_plot_win(s_plot *plot)
{
    if(NULL != plot->format)
        SDL_FreeFormat(plot->format);
    if(NULL != plot->plot)
        SDL_DestroyTexture(plot->plot);
    /* if(NULL != plot->plotcpy) */
    /*     SDL_DestroyTexture(plot->plotcpy); */
    if(NULL != plot->r)
        SDL_DestroyRenderer(plot->r);
    if(NULL != plot->w)
        SDL_DestroyWindow(plot->w);
}

void resize_screen(s_screen *s)
{
    int w, h;
    SDL_GetWindowSize(s->w, &w, &h);
    s->pixel_w = w / PIX_BY_W;
    s->pixel_h = h / PIX_BY_H;
    
    if(s->pixel_w > s->pixel_h)
        s->pixel_w = s->pixel_h;
    if(s->pixel_h > s->pixel_w)
        s->pixel_h = s->pixel_w;
        
    int flags = SDL_GetWindowFlags(s->w);
    s->window_maximized = (flags & SDL_WINDOW_MAXIMIZED) || (flags & SDL_WINDOW_FULLSCREEN_DESKTOP);
    
    if(!s->window_maximized)
    {
        SDL_SetWindowSize(s->w, s->pixel_w * PIX_BY_W, s->pixel_h * PIX_BY_H);
        s->render_dst_ptr = NULL;
        return;
    }
    //else
    if(w > h)
    {
        float ratio = (float)h / PIX_BY_H;
        s->render_dst.w = ratio * PIX_BY_W;
        s->render_dst.h = h;
        s->render_dst.x = (w - s->render_dst.w)/2;
        s->render_dst.y = 0;
    }
    else
    {
        float ratio = (float)w / PIX_BY_W;
        s->render_dst.h = ratio * PIX_BY_H;
        s->render_dst.w = w;
        s->render_dst.y = (h - s->render_dst.h)/2;
        s->render_dst.x = 0;
    }
    s->render_dst_ptr = &s->render_dst;
    
}

int draw_background(s_emu *emu, int i, uint8_t *pixel)
{
    s_screen *scr = &emu->scr;
    s_cpu *cpu = &emu->cpu;
    s_io *io = &cpu->io;
    
    uint8_t Ytemp = io->LY + io->SCY;
    uint8_t Xtemp = io->SCX + i;
    
    if(!scr->bg_win_enable_priority)
        return EXIT_SUCCESS;
        
    uint16_t bg_map_start_address = scr->BG_tile_map_area ? 0x1C00 : 0x1800;
    uint16_t bg_win_data_start_adr = scr->BG_win_tile_data_area ? 0 : 0x1000;
    
    //relative address of the tile in the tile map
    uint16_t rel_bg_tilemap_address = (Ytemp) / 8;
    rel_bg_tilemap_address *= 32;
    rel_bg_tilemap_address += (Xtemp) / 8;
    if(rel_bg_tilemap_address > 0x400)
    {
        fprintf(stderr, "ERROR: address is out of bg tile map bounds!\n");
        return EXIT_FAILURE;
    }
    //number of the tile by its place in tile data
    int16_t tilenum = 0;
    
    assert((bg_map_start_address + rel_bg_tilemap_address) < VRAM_SIZE);
    
    if(scr->BG_win_tile_data_area)
    {
        tilenum = (uint8_t)cpu->VRAM[bg_map_start_address + rel_bg_tilemap_address];
    }
    else
    {
        tilenum = (int8_t) cpu->VRAM[bg_map_start_address + rel_bg_tilemap_address]; 
    }

    
    // address of the two bytes in tiles data we want to read (corresponding
    // to the current scanline we are drawing)
    uint16_t bg_data_address = bg_win_data_start_adr + 16 * tilenum;
    bg_data_address += 2 * ((Ytemp) % 8);
    
    uint8_t bitmask = (0x80 >> ((Xtemp) % 8));

    assert((bg_data_address + 1) < VRAM_SIZE);

    flag_assign((cpu->VRAM[bg_data_address] & bitmask),
                 pixel, 0x01);
    flag_assign((cpu->VRAM[bg_data_address + 1] & bitmask), 
                 pixel, 0x02);
    
    //modify pixel color through the palette
    *pixel = (io->BGP & (0x03 << 2 * *pixel)) >> 2 * *pixel;
    
    return EXIT_SUCCESS;
}

int draw_window(s_emu *emu, int i, uint8_t *pixel)
{
    s_screen *scr = &emu->scr;
    s_cpu *cpu = &emu->cpu;
    s_io *io = &cpu->io;
    
//    uint8_t Ytemp = io->LY + io->WY;

    
    if((!scr->bg_win_enable_priority) || (!scr->window_enable) || (io->LY < io->WY))
    {
        /* scr->win_LY = 0; */
        return EXIT_SUCCESS;
    }
    if(i < ((int)io->WX - 7))
        return EXIT_SUCCESS;

    if(io->LY == io->WY)
        scr->win_LY = 0;
        
    uint8_t Xtemp = i - io->WX + 7;
        
    uint16_t win_map_start_address = scr->win_tile_map_area ? 0x1C00 : 0x1800;
    uint16_t bg_win_data_start_adr = scr->BG_win_tile_data_area ? 0 : 0x1000;
    
    //relative address of the tile in the tile map
    uint16_t rel_win_tilemap_address = (scr->win_LY) / 8;
    rel_win_tilemap_address *= 32;
    rel_win_tilemap_address += (Xtemp) / 8;
    if(rel_win_tilemap_address > 0x400)
    {
        fprintf(stderr, "ERROR: address is out of win tile map bounds!\n");
        return EXIT_FAILURE;
    }
    //number of the tile by its place in tile data
    int16_t tilenum = 0;
    
    assert((win_map_start_address + rel_win_tilemap_address) < VRAM_SIZE);
    
    if(scr->BG_win_tile_data_area)
    {
        tilenum = (uint8_t)cpu->VRAM[win_map_start_address + rel_win_tilemap_address];
    }
    else
    {
        tilenum = (int8_t) cpu->VRAM[win_map_start_address + rel_win_tilemap_address]; 
    }

    
    // address of the two bytes in tiles data we want to read (corresponding
    // to the current scanline we are drawing)
    uint16_t win_data_address = bg_win_data_start_adr + 16 * tilenum;
    win_data_address += 2 * ((scr->win_LY) % 8);
    
    uint8_t bitmask = (0x80 >> ((Xtemp) % 8));

    assert((win_data_address + 1) < VRAM_SIZE);

    flag_assign((cpu->VRAM[win_data_address] & bitmask),
                 pixel, 0x01);
    flag_assign((cpu->VRAM[win_data_address + 1] & bitmask), 
                 pixel, 0x02);
    
    //modify pixel color through the palette
    *pixel = (io->BGP & (0x03 << 2 * *pixel)) >> 2 * *pixel;
    
    return EXIT_SUCCESS;
}

int draw_OBJ_tile(s_emu *emu, int i, uint8_t *pixel, uint8_t sptd)
{
    assert((sptd + 3) < OAM_SIZE);
    
    s_screen *scr = &emu->scr;
    s_cpu *cpu = &emu->cpu;
    s_io *io = &cpu->io;
    
    uint8_t pix_tmp = 0;
    
    //8*16 sprite handle
    //IF (sprite 8*16) AND (LY is in the second tile of the sprite)
    //enforced by hardware
    bool is_second_tile = ((scr->obj_size) && (io->LY - (cpu->OAM[sptd] - 16) >= 8));

    bool bg_win_over_OBJ = (cpu->OAM[sptd + 3] & 0x80);
    if(bg_win_over_OBJ && *pixel != 0)
        return EXIT_SUCCESS;

    bool yflip = (cpu->OAM[sptd + 3] & 0x40);
    bool xflip = (cpu->OAM[sptd + 3] & 0x20);
    bool OBPnum = (cpu->OAM[sptd + 3] & 0x10);

    // Object Tile Adress: discart lsb when 8*16 objects
    uint8_t OAM_byte2 = cpu->OAM[sptd + 2];
    if(scr->obj_size)
        OAM_byte2 &= 0xFE;

    uint16_t data_address = 16 * (OAM_byte2 + is_second_tile);
    /* data_address  */
    if(!yflip)
        data_address += 2 * ((io->LY - cpu->OAM[sptd] + 16) - 8 * is_second_tile);
    else
        data_address += 2 * (7 - ((io->LY - cpu->OAM[sptd] + 16) - 8 * (!is_second_tile && scr->obj_size)));
    
    uint8_t bitmask;
    if(!xflip) bitmask = (0x80 >> (i - cpu->OAM[sptd + 1] + 8));
    else bitmask = (0x01 << (i - cpu->OAM[sptd + 1] + 8));

    assert((data_address + 1) < VRAM_SIZE);
    
    flag_assign((cpu->VRAM[data_address] & bitmask),
                 &pix_tmp, 0x01);
    flag_assign((cpu->VRAM[data_address + 1] & bitmask), 
                 &pix_tmp, 0x02);
    
    if(pix_tmp == 0)
        return EXIT_SUCCESS;
    
    //modify pixel color through the palette
    if(!OBPnum)
        pix_tmp = (io->OBP0 & (0x03 << 2 * pix_tmp)) >> 2 * pix_tmp;
    else
        pix_tmp = (io->OBP1 & (0x03 << 2 * pix_tmp)) >> 2 * pix_tmp;
//    
//    if(pix_tmp == 0)
//        return EXIT_SUCCESS;

    *pixel = pix_tmp;
    
    return EXIT_SUCCESS;
    
}

int draw_OBJ(s_emu *emu, int i, uint8_t *pixel)//, uint8_t sptd[SPRITES_PER_SCANLINE], uint8_t nb_sptd)
{
    s_screen *scr = &emu->scr;
    s_cpu *cpu = &emu->cpu;
    if(!scr->obj_enable)
        return EXIT_SUCCESS;
        
    //checks if sprite is on the pixel currently drawn
    for(int j = scr->nb_sptd - 1; j >= 0; j--)
    {
        //if (spr_x <= pixel_x + 8) AND
        //   (spr_x + spr_w >= pixel_x + 8)
        if((cpu->OAM[scr->sprites_to_draw[j] + 1] <= i + 8) &&
           (cpu->OAM[scr->sprites_to_draw[j] + 1] + 8 > i + 8))
        {
            draw_OBJ_tile(emu, i, pixel, scr->sprites_to_draw[j]);
        }
    }
    
    return EXIT_SUCCESS;
}

/**
 * @brief Scan OAM to select the sprites to draw.
 * @param emu
 * @param sprites_to_draw: an array of 10 uint8_t that contains relative
 *        addresses (in OAM array) to the first byte of each sprite to draw.
 */
void scan_OAM(s_emu *emu)
{
    s_screen *scr = &emu->scr;
    s_cpu *cpu = &emu->cpu;
    s_io *io = &cpu->io;
    if(!scr->obj_enable || scr->is_OAM_scanned)
        return;

    memset(scr->sprites_to_draw, 0, sizeof(uint8_t[SPRITES_PER_SCANLINE]));

    scr->nb_sptd = 0;
    for (int i = 0; (i < OAM_SPRITES_MAX * 4) && (scr->nb_sptd < SPRITES_PER_SCANLINE); i += 4)
    {
        //ckeck if sprite is not on the scanline
        if(! ( (cpu->OAM[i] <= io->LY + 16) && 
               (cpu->OAM[i] + 8 + 8 * scr->obj_size > io->LY + 16) ) )
            continue;
        
        scr->sprites_to_draw[scr->nb_sptd] = i;
        scr->nb_sptd += 1;
    }
    scr->is_OAM_scanned = true;
}

int draw_scanline(s_emu *emu)
{
    //s_screen *scr = &emu->scr;
    s_cpu *cpu = &emu->cpu;
    s_io *io = &cpu->io;
    s_screen *scr = &emu->scr;
    
    if(io->LY >= 144 || scr->is_scanline_drawn)
        return EXIT_SUCCESS;

    if(!scr->LCD_PPU_enable)
    {
        for(int i = 0; i < PIX_BY_W; i++)
        {
            //draw blank line
            scr->pixels[io->LY * PIX_BY_W + i] = SDL_MapRGBA(
                scr->format, 255, 255, 255, 255
            );
        }
        scr->is_scanline_drawn = true;
        return EXIT_SUCCESS;
    }
    
    //for each pixel of the scanline
    
    /* uint8_t sprites_to_draw[SPRITES_PER_SCANLINE]; */
    /* uint8_t nb_sptd; */
    /* memset(sprites_to_draw, 0, sizeof(uint8_t[SPRITES_PER_SCANLINE])); */
    
    for(int i = 0; i < PIX_BY_W; i++)
    {
        uint8_t pixel = 0;
        if(0 != draw_background(emu, i, &pixel))
            return EXIT_FAILURE;
        if(0 != draw_window(emu, i, &pixel))
            return EXIT_FAILURE;
        if(0 != draw_OBJ(emu, i, &pixel))
            return EXIT_FAILURE;
            
        //convert to 4 possible grayscales [0, 255] values
        pixel = 255 - 85 * pixel;
        //draw in texture
        scr->pixels[io->LY * PIX_BY_W + i] = SDL_MapRGBA(
            scr->format, pixel, pixel, pixel, 255
        );
    }

    scr->is_OAM_scanned = false;
    scr->is_scanline_drawn = true;
    
    return EXIT_SUCCESS;
}

void ppu_modes_and_scanlines(s_emu *emu)
{
    s_cpu *cpu = &emu->cpu;
    s_io *io = &cpu->io;
    s_screen *scr = &emu->scr;

    // Reset PPU mode bits
    io->STAT &= ~0x03;

    // MODE 2
    // If a LINE CYCLE is COMPLETE, change to MODE 2 and begin a new line
    if(cpu->t_cycles >= (CPU_FREQ / GB_VSNC / 154))
    {
        cpu->t_cycles -= (CPU_FREQ / GB_VSNC / 154);
        //PPU enable : stat = mod2; else stat = mod 1 (VBlank)
        io->STAT |= (scr->LCD_PPU_enable) ? 2 : 1;
        scr->is_scanline_drawn = false;

        if(scr->LCD_PPU_enable)
        {
            cpu->io.LY++;
            scr->win_LY++;
            scan_OAM(emu);
        }
        return;
    }

    if(!scr->LCD_PPU_enable)
    {
        cpu->io.LY = 0;
        scr->win_LY = 0;
        return;
    }

    // MODE 2
    // If PPU is still in mode 2, set mode 2 flags
    if(cpu->t_cycles < PPU_MODE2)
    {
        //Searching OAM for OBJs whose Y coordinate overlap this line
        io->STAT |= 2;
        return;
    }

    // MODE 3
    if(cpu->t_cycles < PPU_MODE3 + PPU_MODE2)
    {
        //Reading OAM and VRAM to generate the picture
        io->STAT |= 3;
        return;
    }

    // MODE 0
    if(cpu->t_cycles < PPU_MODE0 + PPU_MODE3 + PPU_MODE2)
    {
        //Nothing (HBlank)
        if(0 != draw_scanline(emu))
            destroy_emulator(emu, EXIT_FAILURE);

        // Now, the game can change the registers safely.
        return;
    }
}

void draw_plot(s_emu *emu)
{
    if(!emu->opt.plot_instructions)
        return;

    s_cpu *cpu = &emu->cpu;
    s_plot *plot = &emu->scr.plot;

    size_t posx = (double)cpu->t_cycles / (CPU_FREQ / GB_VSNC) * (emu->cpu.io.LY + 1) *  plot->width;
    size_t posy = cpu->pc;

    if(cpu->pc > 0x9FFF) // VRAM
        posy -= 0x2000;
    if(cpu->pc > 0xBE80) // 0xE000 <-> 0xFF80
        posy -= 0x180;

    posy = (double)posy / 0xDE80 * plot->height;

    size_t i = posy * plot->width + posx;

    plot->pixels[i] = SDL_MapRGBA(plot->format, 255, 0, 0, 255);
}

int render_plot(s_emu *emu)
{
    s_plot *plot = &emu->scr.plot;
    if(!emu->opt.plot_instructions)
        return EXIT_SUCCESS;

    SDL_UnlockTexture(plot->plot);
    SDL_RenderClear(plot->r);
    SDL_RenderCopy(plot->r, plot->plot, NULL, NULL);
    SDL_RenderPresent(plot->r);

    // paint texture in white
    for(size_t i = 0; i < plot->width * plot->height; i++)
    {
        plot->pixels[i] = SDL_MapRGBA(plot->format, 255, 255, 255, 255);
    }

    size_t posy = (double)emu->in.y / plot->height * 0xDE80;
    if(posy > 0x9FFF) // VRAM
        posy += 0x2000;
    if(posy > 0xE000) // 0xE000 <-> 0xFF80
        posy += 0x180;

    char title[255] = "";
    snprintf(title, 254, "pc: 0x%04lX - game_spop plot", posy);
    SDL_SetWindowTitle(plot->w, title);

    if(0 != lockscreen_plot(plot))
        destroy_emulator(emu, EXIT_FAILURE);

    return EXIT_SUCCESS;
}

void render_frame_and_vblank_if_needed(s_emu *emu)
{
    s_cpu *cpu = &emu->cpu;
    s_screen *scr = &emu->scr;
    s_io *io = &cpu->io;
    
    if(io->LY < 144)
    {
        //io->IF &= ~0x01;
        scr->old_STAT = io->STAT;
        return;
    }
    
    //set VBlank interrupt flag 
    if((scr->old_STAT & 0x03) != 1)
    {
        io->IF |= 0x01;
    }
    
    io->STAT &= ~0x03;
    io->STAT |= 1;
    
    scr->old_STAT = io->STAT;
    
    if(cpu->io.LY < 154)
        return;
        
//    Uint64 elapsed = SDL_GetTicks64() - emu->frame_timer;
//    if(elapsed <= 17 && !emu->opt.fast_forward)
//    {
//        SDL_Delay(17 - elapsed);
//        elapsed = 17;
//    }
    if((emu->au.buffers_since_last_frame < BUFFERS_PER_FRAME) && 
      (!emu->opt.fast_forward) && emu->opt.audio && emu->au.apu_enable)
        return;
        
    emu->au.buffers_since_last_frame = 0;
    emu->opt.newframe = true;

    SDL_UnlockTexture(scr->scr);

    for(int i = scr->refresh_rate_mul; i > 0; i--)
    {
        SDL_RenderClear(scr->r);
        SDL_RenderCopy(scr->r, scr->scr, NULL, scr->render_dst_ptr);
        SDL_RenderCopy(scr->r, scr->scrcpy, NULL, scr->render_dst_ptr);
        SDL_RenderPresent(scr->r);
    }

    render_plot(emu);

    //screen copy, for transparence effects
    SDL_SetRenderTarget(scr->r, scr->scrcpy);
    SDL_RenderCopy(scr->r, scr->scr, NULL, NULL);
    SDL_SetRenderTarget(scr->r, NULL);
    
    if(0 != lockscreen(scr))
        destroy_emulator(emu, EXIT_FAILURE);

//    Uint64 elapsed = SDL_GetTicks64() - emu->frame_timer;
//    static uint64_t elapsed_sum = 0;
//    elapsed_sum += elapsed;
    static int sum_cnt = 0;
    sum_cnt++;
    
    //calculate fps once per second
    if(sum_cnt >= 60)
    {
        double fps;
        double elapsed_sum = (SDL_GetTicks64() - emu->frame_timer);
        fps = 1 / ((elapsed_sum / 60) / 1000);
        elapsed_sum = 0;
        sum_cnt = 0;
        emu->frame_timer = SDL_GetTicks64();
        
        char tmp[25] = "";
        snprintf(tmp, 25, "game_spop %.2f fps", fps);
        SDL_SetWindowTitle(scr->w, tmp);
    }
    
    cpu->io.LY = 0;
    scr->win_LY = 0;
    //clear VBlank flag
    //io->IF &= (~0x01);
}

int DMA_transfer(s_emu *emu)
{
    s_cpu *cpu = &emu->cpu;
    s_io *io = &cpu->io;
    
    if(io->DMA > 0xDF)
    {
        fprintf(stderr, "ERROR: DMA value exceed 0xDF! (DMA = 0x%02X)\n", io->DMA);
        return EXIT_FAILURE;
    }
    
    //ROM bank 00
    if(io->DMA <= 0x3F)
    {
        memcpy(cpu->OAM, &cpu->ROM_Bank[cpu->cur_low_rom_bk][io->DMA << 8], OAM_SIZE);
    }
    
    //ROM bank 01
    else if(io->DMA <= 0x7F)
    {
        uint16_t start_address = (io->DMA << 8) - 0x4000;
        memcpy(cpu->OAM, &cpu->ROM_Bank[cpu->cur_hi_rom_bk][start_address], OAM_SIZE);
    }
    
    //VRAM
    else if(io->DMA <= 0x9F)
    {
        uint16_t start_address = (io->DMA << 8) - 0x8000;
        memcpy(cpu->OAM, &cpu->VRAM[start_address], OAM_SIZE);
    }
    
    //external RAM
    else if(io->DMA <= 0xBF)
    {
        uint16_t start_address = (io->DMA << 8) - 0xA000;
        memcpy(cpu->OAM, &cpu->SRAM[cpu->current_sram_bk][start_address], OAM_SIZE);
    }
    
    //external WRAM
    else if(io->DMA <= 0xDF)
    {
        uint16_t start_address = (io->DMA << 8) - 0xC000;
        memcpy(cpu->OAM, &cpu->WRAM[start_address], OAM_SIZE);
    }
    
    return EXIT_SUCCESS;
}

