#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>
#include "graphics.h"

int initialize_screen(s_screen *screen)
{
    screen->r = NULL;
    screen->w = NULL;
    screen->pixel_w = 1;
    screen->pixel_h = 1;
    
    screen->w = SDL_CreateWindow("Game_spop emulator", SDL_WINDOWPOS_CENTERED,
                                 SDL_WINDOWPOS_CENTERED, PIX_BY_W, PIX_BY_H,
                                 SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if(screen->w == NULL)
    {
        fprintf(stderr, "Error SDL_CreateWindow: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }
    
    screen->r = SDL_CreateRenderer(screen->w, -1, SDL_RENDERER_ACCELERATED);
    if(screen->r == NULL)
    {
        fprintf(stderr, "Error SDL_CreateRenderer: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}

void destroy_screen(s_screen *screen)
{
    if(NULL != screen->r)
        SDL_DestroyRenderer(screen->r);
    if(NULL != screen->w)
        SDL_DestroyWindow(screen->w);
}

void resize_screen(s_screen *screen)
{
    int w, h;
    SDL_GetWindowSize(screen->w, &w, &h);
    screen->pixel_w = w / PIX_BY_W;
    screen->pixel_h = h / PIX_BY_H;
}

