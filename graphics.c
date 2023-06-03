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
    
    emu->cpu.io_reg.STAT = 2;
    
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
    
    uint8_t Ytemp = io_reg->LY + io_reg->SCY;
    uint8_t Xtemp = io_reg->SCX + i;
    
    if(!screen->bg_win_enable_priority)
        return EXIT_SUCCESS;
        
    uint16_t bg_map_start_adress = screen->BG_tile_map_area ? 0x1C00 : 0x1800;
    uint16_t bg_win_data_start_adr = screen->BG_win_tile_data_area ? 0 : 0x800;
    
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
    uint8_t tilenum = cpu->VRAM[bg_map_start_adress + rel_bg_tilemap_adress]; 
    // adress of the two bytes in tiles data we want to read (corresponding
    // to the current scanline we are drawing)
    uint16_t bg_data_adress = bg_win_data_start_adr + 16 * tilenum;
    bg_data_adress += 2 * ((Ytemp) % 8);
    
    uint8_t bitmask = (0x80 >> ((Xtemp) % 8));
    
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

int draw_OBJ_tile(s_emu *emu, size_t i, uint8_t *pixel, uint8_t sptd)
{
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
    data_adress += 2 * (io->LY % 8);
    if(yflip) data_adress = 8 - data_adress;
    
    uint8_t bitmask;
    if(!xflip) bitmask = (0x80 >> (i % 8));
    else bitmask = (0x01 << (i % 8));
    
    pix_tmp =     (cpu->VRAM[data_adress] 
                //Selects the pixel we are currently drawing
                & bitmask)
                //Shifts to the lsb
                >> (7 - i % 8);
                
    //does the same for the second byte of data
    pix_tmp |=    (cpu->VRAM[data_adress + 1]
                & bitmask)
                >> (6 - i % 8);
    
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

int draw_OBJ(s_emu *emu, size_t i, uint8_t *pixel, uint8_t sptd[SPRITES_PER_SCANLINE])
{
    s_screen *scr = &emu->screen;
    s_cpu *cpu = &emu->cpu;
//    s_io *io = &cpu->io_reg;
    if(!scr->obj_enable)
        return EXIT_SUCCESS;
        
    //fprintf(stderr, "WARNING: Attempt to draw OBJ! (unimplemented)\n");
    
    //checks if sprite is on the pixel currently drawn
    for(size_t j = 0; j < SPRITES_PER_SCANLINE; j++)
    {
        //if (spr_x <= pixel_x + 8) AND
        //   (spr_x + spr_w >= pixel_x + 8)
        if((cpu->OAM[sptd[j] + 1] <= i + 8) &&
           (cpu->OAM[sptd[j] + 1] + 8 >= i + 8))
        {
            draw_OBJ_tile(emu, i, pixel, j);
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
void scan_OAM(s_emu *emu, uint8_t sprites_to_draw[SPRITES_PER_SCANLINE])
{
    s_screen *scr = &emu->screen;
    s_cpu *cpu = &emu->cpu;
    s_io *io = &cpu->io_reg;
    if(!scr->obj_enable)
        return;
    
    int sprite_counter = 0;
    for (size_t i = 0; (i < OAM_SPRITES_MAX * 4) && (sprite_counter < SPRITES_PER_SCANLINE); i += 4)
    {
        //ckeck if sprite is not on the scanline
        if(! ( (cpu->OAM[i] <= io->LY + 16) && 
               (cpu->OAM[i] + 8 + 8 * scr->obj_size >= io->LY + 16) ) )
            continue;
        
        sprites_to_draw[sprite_counter] = i;
        sprite_counter++;
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
    scan_OAM(emu, sprites_to_draw);
    
    for(size_t i = 0; i < PIX_BY_W; i++)
    {
        uint8_t pixel = 0;
        if(0 != draw_background(emu, i, &pixel))
            return EXIT_FAILURE;
        if(0 != draw_window(emu, i))
            return EXIT_FAILURE;
        if(0 != draw_OBJ(emu, i, &pixel, sprites_to_draw))
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
        //io_reg->IF &= ~0x01;
        return;
    }
    
    //set VBlank interrupt flag 
    io_reg->IF |= 0x01;
    io_reg->STAT &= ~0x03;
    io_reg->STAT |= 1;
    
    if(cpu->io_reg.LY < 154)
        return;
        
    Uint64 elapsed = SDL_GetTicks64() - emu->frame_timer;
    if(elapsed <= 17)
    {
        SDL_Delay(17 - elapsed);
        elapsed = 17;
    }
    SDL_UnlockTexture(screen->scr);
    SDL_RenderCopy(screen->r, screen->scr, NULL, NULL);
    SDL_RenderPresent(screen->r);
    if(0 != lockscreen(screen))
        destroy_emulator(emu, EXIT_FAILURE);
    
    static uint64_t elapsed_sum = 0;
    elapsed_sum += elapsed;
    static int sum_cnt = 0;
    sum_cnt++;
    
    //calculate fps once per second
    static double fps = 58.82;
    if(sum_cnt >= 59)
    {
        fps = 1 / (((double)elapsed_sum / 60) / 1000);
        elapsed_sum = 0;
        sum_cnt = 0;
    }
    
    static char tmp[25] = "";
    snprintf(tmp, 25, "game_spop %.2f fps", fps);
    SDL_SetWindowTitle(screen->w, tmp);
    
    emu->frame_timer = SDL_GetTicks64();
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
        memcpy(cpu->OAM, &cpu->ROM_Bank[0][io_reg->DMA << 8], OAM_SIZE);
    }
    
    //ROM bank 01
    else if(io_reg->DMA <= 0x7F)
    {
        uint16_t start_adress = (io_reg->DMA << 8) - 0x4000;
        memcpy(cpu->OAM, &cpu->ROM_Bank[1][start_adress], OAM_SIZE);
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
        memcpy(cpu->OAM, &cpu->external_RAM[start_adress], OAM_SIZE);
    }
    
    //external WRAM
    else if(io_reg->DMA <= 0xDF)
    {
        uint16_t start_adress = (io_reg->DMA << 8) - 0xC000;
        memcpy(cpu->OAM, &cpu->WRAM[start_adress], OAM_SIZE);
    }
    
    return EXIT_SUCCESS;
}

