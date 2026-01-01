#include "gol.hpp"
#include <SDL2/SDL.h>
#include <iostream>
#include <chrono>

const int WINDOW_WIDTH = 1600;
const int WINDOW_HEIGHT = 1000;
const int CELL_SIZE = 5;
const int GRID_WIDTH = WINDOW_WIDTH / CELL_SIZE;
const int GRID_HEIGHT = WINDOW_HEIGHT / CELL_SIZE;

// Color constants
const SDL_Color COLOR_BACKGROUND = {15, 15, 15, 255};
const SDL_Color COLOR_GRID = {40, 40, 40, 255};
const SDL_Color COLOR_ALIVE = {0, 255, 100, 255};  // Green
const SDL_Color COLOR_DEAD = {20, 20, 20, 255};
const SDL_Color COLOR_TEXT = {200, 200, 200, 255};

void drawCell(SDL_Renderer* renderer, int x, int y, bool alive) {
    SDL_Rect cell = {x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE};
    
    if (alive) {
        SDL_SetRenderDrawColor(renderer, COLOR_ALIVE.r, COLOR_ALIVE.g, COLOR_ALIVE.b, COLOR_ALIVE.a);
        SDL_RenderFillRect(renderer, &cell);
    }
    
    // Draw grid lines
    SDL_SetRenderDrawColor(renderer, COLOR_GRID.r, COLOR_GRID.g, COLOR_GRID.b, COLOR_GRID.a);
    SDL_RenderDrawRect(renderer, &cell);
}

void renderStats(SDL_Renderer* renderer, int generation, int alive_count, int fps, bool paused) {
    std::string stats = "Gen: " + std::to_string(generation) + 
                       " | Alive: " + std::to_string(alive_count) + 
                       " | FPS: " + std::to_string(fps) +
                       (paused ? " | PAUSED (SPACE to resume)" : "");
    
    SDL_SetRenderDrawColor(renderer, COLOR_BACKGROUND.r, COLOR_BACKGROUND.g, COLOR_BACKGROUND.b, COLOR_BACKGROUND.a);
    SDL_Rect stat_bg = {0, 0, WINDOW_WIDTH, 30};
    SDL_RenderFillRect(renderer, &stat_bg);
    
    SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
    SDL_RenderDrawRect(renderer, &stat_bg);
    
    // Simple text rendering using SDL primitives (since TTF loading can be complex)
    std::cout << stats << "\n";
}

int countAlive(const Gol<GRID_WIDTH, GRID_HEIGHT>& gol) {
    int count = 0;
    for (size_t y = 0; y < GRID_HEIGHT; ++y) {
        for (size_t x = 0; x < GRID_WIDTH; ++x) {
            if (gol.getCell(x, y)) count++;
        }
    }
    return count;
}

int main() {
    std::cout << "\n=== Game of Life Visualization ===\n";
    std::cout << "Grid size: " << GRID_WIDTH << " x " << GRID_HEIGHT << "\n";
    std::cout << "Cell size: " << CELL_SIZE << " pixels\n";
    std::cout << "\nControls:\n";
    std::cout << "  SPACE - Pause/Resume\n";
    std::cout << "  UP    - Increase speed\n";
    std::cout << "  DOWN  - Decrease speed\n";
    std::cout << "  R     - Reset with random pattern\n";
    std::cout << "  Q/ESC - Quit\n";
    std::cout << "==================================\n\n";

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL initialization failed: " << SDL_GetError() << "\n";
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow(
        "Game of Life - Optimized Visualization",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_SHOWN
    );

    if (!window) {
        std::cerr << "Window creation failed: " << SDL_GetError() << "\n";
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        std::cerr << "Renderer creation failed: " << SDL_GetError() << "\n";
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Initialize Game of Life
    Gol<GRID_WIDTH, GRID_HEIGHT> gol(true);  // Random initialization

    // Control variables
    bool running = true;
    bool paused = false;
    int generation = 0;
    int update_delay_ms = 50;  // Milliseconds between generations
    
    auto last_update = std::chrono::high_resolution_clock::now();
    auto frame_start = std::chrono::high_resolution_clock::now();
    int frame_count = 0;
    int current_fps = 0;

    while (running) {
        frame_start = std::chrono::high_resolution_clock::now();
        
        // Handle events
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    running = false;
                    break;
                case SDL_KEYDOWN:
                    switch (event.key.keysym.sym) {
                        case SDLK_SPACE:
                            paused = !paused;
                            break;
                        case SDLK_UP:
                            update_delay_ms = std::max(10, update_delay_ms - 10);
                            std::cout << "Speed increased. Delay: " << update_delay_ms << "ms\n";
                            break;
                        case SDLK_DOWN:
                            update_delay_ms = std::min(500, update_delay_ms + 10);
                            std::cout << "Speed decreased. Delay: " << update_delay_ms << "ms\n";
                            break;
                        case SDLK_r:
                            gol = Gol<GRID_WIDTH, GRID_HEIGHT>(true);
                            generation = 0;
                            std::cout << "Reset with random pattern.\n";
                            break;
                        case SDLK_q:
                        case SDLK_ESCAPE:
                            running = false;
                            break;
                        default:
                            break;
                    }
                    break;
                default:
                    break;
            }
        }

        // Update game state
        auto now = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_update).count();
        
        if (!paused && elapsed >= update_delay_ms) {
            gol.update();
            generation++;
            last_update = now;
        }

        // Render
        SDL_SetRenderDrawColor(renderer, COLOR_BACKGROUND.r, COLOR_BACKGROUND.g, COLOR_BACKGROUND.b, COLOR_BACKGROUND.a);
        SDL_RenderClear(renderer);

        // Draw all cells
        for (size_t y = 0; y < GRID_HEIGHT; ++y) {
            for (size_t x = 0; x < GRID_WIDTH; ++x) {
                drawCell(renderer, x, y, gol.getCell(x, y));
            }
        }

        // Calculate stats
        int alive_count = countAlive(gol);
        
        // Calculate FPS
        frame_count++;
        auto frame_elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::high_resolution_clock::now() - frame_start
        ).count();
        
        if (frame_elapsed >= 1000) {
            current_fps = frame_count;
            frame_count = 0;
            frame_start = std::chrono::high_resolution_clock::now();
        }

        // Render stats
        renderStats(renderer, generation, alive_count, current_fps, paused);

        SDL_RenderPresent(renderer);
    }

    // Cleanup
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    std::cout << "\nSimulation ended at generation " << generation << ".\n";
    std::cout << "Final population: " << countAlive(gol) << " cells.\n";

    return 0;
}