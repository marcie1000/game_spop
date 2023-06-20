#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>
#include <assert.h>
#include "emulator.h"
#include "graphics.h"


int initialize_screen(s_emu *emu)
{
    s_screen *screen = &emu->screen;
    screen->r = NULL;
    screen->w = NULL;
    screen->scr = NULL;
    screen->pixel_w = 2;
    screen->pixel_h = 2;
    
    screen->window_maximized = false;
    
    screen->w = SDL_CreateWindow("Game_spop", SDL_WINDOWPOS_CENTERED,
                                 SDL_WINDOWPOS_CENTERED, PIX_BY_W * screen->pixel_w, PIX_BY_H * screen->pixel_h,
                                 SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if(screen->w == NULL)
    {
        fprintf(stderr, "Error SDL_CreateWindow: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }
    
    screen->r = SDL_CreateRenderer(screen->w, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if(screen->r == NULL)
    {
        fprintf(stderr, "Error SDL_CreateRenderer: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    SDL_SetRenderTarget(screen->r, NULL);
    SDL_SetRenderDrawColor(screen->r, 0, 0, 0, 255);
    
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
    
    emu->cpu.io_reg.STAT = 2;
    screen->LCD_PPU_enable = false;
    screen->win_tile_map_area = false;
    screen->window_enable = false;
    screen->BG_win_tile_data_area = false;
    screen->BG_tile_map_area = false;
    screen->obj_size = false;
    screen->obj_enable = false;
    screen->bg_win_enable_priority = false;
    
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
    if(NULL != screen->format)
        SDL_FreeFormat(screen->format);
    if(NULL != screen->scr)
        SDL_DestroyTexture(screen->scr);
    if(NULL != screen->r)
        SDL_DestroyRenderer(screen->r);
    if(NULL != screen->w)
        SDL_DestroyWindow(screen->w);
}

void resize_screen(s_screen *s)
{
    int w, h;
    SDL_GetWindowSize(s->w, &w, &h);
    s->pixel_w = w / PIX_BY_W;
    s->pixel_h = h / PIX_BY_H;
    
    if(s->pixel_w > s->pixel_h)
        s->pixel_h = s->pixel_w;
    if(s->pixel_h > s->pixel_w)
        s->pixel_w = s->pixel_h;
        
    int flags = SDL_GetWindowFlags(s->w);
    s->window_maximized = (flags & SDL_WINDOW_MAXIMIZED);
    
    if(!s->window_maximized)
    {
        SDL_SetWindowSize(s->w, s->pixel_w * PIX_BY_W, s->pixel_h * PIX_BY_H);
        s->render_dst_ptr = NULL;
    }
    else
    {
        if(w > h)
        {
            uint32_t ratio = h / PIX_BY_H;
            s->render_dst.w = ratio * PIX_BY_W;
            s->render_dst.h = h;
            s->render_dst.x = (w - s->render_dst.w)/2;
            s->render_dst.y = 0;
        }
        else
        {
            uint32_t ratio = w / PIX_BY_W;
            s->render_dst.h = ratio * PIX_BY_H;
            s->render_dst.w = w;
            s->render_dst.y = (h - s->render_dst.h)/2;
            s->render_dst.x = 0;
        }
        s->render_dst_ptr = &s->render_dst;
    }
}

int draw_background(s_emu *emu, size_t i, uint8_t *pixel)
{
    s_screen *screen = &emu->screen;
    s_cpu *cpu = &emu->cpu;
    s_io *io_reg = &cpu->io_reg;
    
    uint8_t Ytemp = io_reg->LY + io_reg->SCY;
    uint8_t Xtemp = io_reg->SCX + i;
    
    if(!screen->bg_win_enable_priority)
        return EXIT_SUCCESS;
        
    uint16_t bg_map_start_adress = screen->BG_tile_map_area ? 0x1C00 : 0x1800;
    uint16_t bg_win_data_start_adr = screen->BG_win_tile_data_area ? 0 : 0x1000;
    
    //relative adress of the tile in the tile map
    uint16_t rel_bg_tilemap_adress = (Ytemp) / 8;
    rel_bg_tilemap_adress *= 32;
    rel_bg_tilemap_adress += (Xtemp) / 8;
    if(rel_bg_tilemap_adress > 0x400)
    {
        fprintf(stderr, "ERROR: adress is out of tile map bounds!\n");
        return EXIT_FAILURE;
    }
    //number of the tile by its place in tile data
    int16_t tilenum = 0;
    
    assert((bg_map_start_adress + rel_bg_tilemap_adress) < VRAM_SIZE);
    
    if(screen->BG_win_tile_data_area)
    {
        tilenum = (uint8_t)cpu->VRAM[bg_map_start_adress + rel_bg_tilemap_adress];
    }
    else
    {
        tilenum = (int8_t) cpu->VRAM[bg_map_start_adress + rel_bg_tilemap_adress]; 
    }

    
    // adress of the two bytes in tiles data we want to read (corresponding
    // to the current scanline we are drawing)
    uint16_t bg_data_adress = bg_win_data_start_adr + 16 * tilenum;
    bg_data_adress += 2 * ((Ytemp) % 8);
    
    uint8_t bitmask = (0x80 >> ((Xtemp) % 8));

    assert((bg_data_adress + 1) < VRAM_SIZE);

    flag_assign((cpu->VRAM[bg_data_adress] & bitmask),
                 pixel, 0x01);
    flag_assign((cpu->VRAM[bg_data_adress + 1] & bitmask), 
                 pixel, 0x02);
    
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

int draw_OBJ_tile(s_emu *emu, size_t i, uint8_t *pixel, uint8_t sptd)
{
    assert((sptd + 3) < OAM_SIZE);
    
    s_screen *scr = &emu->screen;
    s_cpu *cpu = &emu->cpu;
    s_io *io = &cpu->io_reg;
    
    uint8_t pix_tmp = 0;
    
    //8*16 sprite handle
    //IF (sprite 8*16) AND (LY is in the second tile of the sprite)
    bool is_second_tile = ((scr->obj_size) && (cpu->OAM[sptd] + 16 - io->LY > 8));
    //bool bgwin_over_obj = (cpu->OAM[sptd + 3] & 0x80);
    bool yflip = (cpu->OAM[sptd + 3] & 0x40);
    bool xflip = (cpu->OAM[sptd + 3] & 0x20);
    bool OBPnum = (cpu->OAM[sptd + 3] & 0x10);
    
    uint16_t data_adress = 16 * (cpu->OAM[sptd + 2] + is_second_tile);
    data_adress += 2 * (io->LY - cpu->OAM[sptd] + 16);
    if(yflip) data_adress = 8 - data_adress;
    
    uint8_t bitmask;
    if(!xflip) bitmask = (0x80 >> (i - cpu->OAM[sptd + 1] + 8));
    else bitmask = (0x01 << (i - cpu->OAM[sptd + 1] + 8));
    
    assert((data_adress + 1) < VRAM_SIZE);
    
    flag_assign((cpu->VRAM[data_adress] & bitmask),
                 &pix_tmp, 0x01);
    flag_assign((cpu->VRAM[data_adress + 1] & bitmask), 
                 &pix_tmp, 0x02);
    
    //modify pixel color through the palette
    if(!OBPnum)
        pix_tmp = (io->OBP0 & (0x03 << 2 * pix_tmp)) >> 2 * pix_tmp;
    else
        pix_tmp = (io->OBP1 & (0x03 << 2 * pix_tmp)) >> 2 * pix_tmp;
    
    //transparency
    if(pix_tmp != 0)
        *pixel = pix_tmp;
    
    return EXIT_SUCCESS;
    
}

int draw_OBJ(s_emu *emu, size_t i, uint8_t *pixel, uint8_t sptd[SPRITES_PER_SCANLINE], uint8_t nb_sptd)
{
    s_screen *scr = &emu->screen;
    s_cpu *cpu = &emu->cpu;
    if(!scr->obj_enable)
        return EXIT_SUCCESS;
        
    //checks if sprite is on the pixel currently drawn
    for(size_t j = 0; j < SPRITES_PER_SCANLINE && j < nb_sptd; j++)
    {
        //if (spr_x <= pixel_x + 8) AND
        //   (spr_x + spr_w >= pixel_x + 8)
        if((cpu->OAM[sptd[j] + 1] <= i + 8) &&
           (cpu->OAM[sptd[j] + 1] + 8 > i + 8))
        {
            //if(cpu->io_reg.LY)

            draw_OBJ_tile(emu, i, pixel, sptd[j]);
            break;
        }
    }
    
    return EXIT_SUCCESS;
}

/**
 * @brief Scan OAM to select the sprites to draw.
 * @param emu
 * @param sprites_to_draw: an array of 10 uint8_t that contains relative
 *        adresses (in OAM array) to the first byte of each sprite to draw.
 */
void scan_OAM(s_emu *emu, uint8_t sprites_to_draw[SPRITES_PER_SCANLINE], uint8_t *nb_sptd)
{
    s_screen *scr = &emu->screen;
    s_cpu *cpu = &emu->cpu;
    s_io *io = &cpu->io_reg;
    if(!scr->obj_enable)
        return;
    
    *nb_sptd = 0;
    for (size_t i = 0; (i < OAM_SPRITES_MAX * 4) && (*nb_sptd < SPRITES_PER_SCANLINE); i += 4)
    {
        //ckeck if sprite is not on the scanline
        if(! ( (cpu->OAM[i] <= io->LY + 16) && 
               (cpu->OAM[i] + 8 + 8 * scr->obj_size > io->LY + 16) ) )
            continue;
        
        sprites_to_draw[*nb_sptd] = i;
        *nb_sptd += 1;
    }
    

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
    
    //for each pixel of the scanline
    
    uint8_t sprites_to_draw[SPRITES_PER_SCANLINE];
    uint8_t nb_sptd;
    memset(sprites_to_draw, 0, sizeof(uint8_t[SPRITES_PER_SCANLINE]));
    scan_OAM(emu, sprites_to_draw, &nb_sptd);
    
    for(size_t i = 0; i < PIX_BY_W; i++)
    {
        uint8_t pixel = 0;
        if(0 != draw_background(emu, i, &pixel))
            return EXIT_FAILURE;
        if(0 != draw_window(emu, i))
            return EXIT_FAILURE;
        if(0 != draw_OBJ(emu, i, &pixel, sprites_to_draw, nb_sptd))
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
    
    if(cpu->t_cycles >= (CPU_FREQ/57.7/154))
    {
        cpu->t_cycles -= (CPU_FREQ/57.7/154);
        //PPU enable : stat = mod2; else stat = mod 1 (VBlank)
        io_reg->STAT |= (screen->LCD_PPU_enable) ? 2 : 1;    
        if(0 != draw_scanline(emu))
            destroy_emulator(emu, EXIT_FAILURE);
        cpu->io_reg.LY++;
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
        //io_reg->IF &= ~0x01;
        return;
    }
    
    //set VBlank interrupt flag 
    io_reg->IF |= 0x01;
    io_reg->STAT &= ~0x03;
    io_reg->STAT |= 1;
    
    if(cpu->io_reg.LY < 154)
        return;
        
//    Uint64 elapsed = SDL_GetTicks64() - emu->frame_timer;
//    if(elapsed <= 17 && !emu->opt.fast_forward)
//    {
//        SDL_Delay(17 - elapsed);
//        elapsed = 17;
//    }
    if((emu->audio.queues_since_last_frame < QUEUES_PER_FRAME) && 
      (!emu->opt.fast_forward) && emu->opt.audio)
        return;
        
    emu->audio.queues_since_last_frame = 0;

    SDL_UnlockTexture(screen->scr);
    SDL_RenderClear(screen->r);
    SDL_RenderCopy(screen->r, screen->scr, NULL, screen->render_dst_ptr);
    SDL_RenderPresent(screen->r);
    if(0 != lockscreen(screen))
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
        SDL_SetWindowTitle(screen->w, tmp);
    }
    
    cpu->io_reg.LY = 0;
    //clear VBlank flag
    //io_reg->IF &= (~0x01);
}

int DMA_transfer(s_emu *emu)
{
    s_cpu *cpu = &emu->cpu;
    s_io *io_reg = &cpu->io_reg;
    
    if(io_reg->DMA > 0xDF)
    {
        fprintf(stderr, "ERROR: DMA value exceed 0xDF! (DMA = 0x%02X)\n", io_reg->DMA);
        return EXIT_FAILURE;
    }
    
    //ROM bank 00
    if(io_reg->DMA <= 0x3F)
    {
        memcpy(cpu->OAM, &cpu->ROM_Bank[cpu->cur_low_rom_bk][io_reg->DMA << 8], OAM_SIZE);
    }
    
    //ROM bank 01
    else if(io_reg->DMA <= 0x7F)
    {
        uint16_t start_adress = (io_reg->DMA << 8) - 0x4000;
        memcpy(cpu->OAM, &cpu->ROM_Bank[cpu->cur_hi_rom_bk][start_adress], OAM_SIZE);
    }
    
    //VRAM
    else if(io_reg->DMA <= 0x9F)
    {
        uint16_t start_adress = (io_reg->DMA << 8) - 0x8000;
        memcpy(cpu->OAM, &cpu->VRAM[start_adress], OAM_SIZE);
    }
    
    //external RAM
    else if(io_reg->DMA <= 0xBF)
    {
        uint16_t start_adress = (io_reg->DMA << 8) - 0xA000;
        memcpy(cpu->OAM, &cpu->SRAM[cpu->current_sram_bk][start_adress], OAM_SIZE);
    }
    
    //external WRAM
    else if(io_reg->DMA <= 0xDF)
    {
        uint16_t start_adress = (io_reg->DMA << 8) - 0xC000;
        memcpy(cpu->OAM, &cpu->WRAM[start_adress], OAM_SIZE);
    }
    
    return EXIT_SUCCESS;
}

