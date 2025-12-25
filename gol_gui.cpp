#include "gol.hpp"
#include <SDL2/SDL.h>
#include <iostream>

const int WINDOW_WIDTH = 1500;
const int WINDOW_HEIGHT = 900;

int main() {

    std::cout << "Hello Game of lifer! Here we go again.";

    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window* window = SDL_CreateWindow(
        "Game of Life",           
        SDL_WINDOWPOS_CENTERED,   
        SDL_WINDOWPOS_CENTERED,   
        WINDOW_WIDTH,             
        WINDOW_HEIGHT,            
        SDL_WINDOW_SHOWN          
    );

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    bool running = true;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}