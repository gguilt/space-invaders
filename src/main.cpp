// Space Invaders SDL2 by Radon Therapy
// 2023-07-01

// Header files
#include <SDL2/SDL.h>

// Variables and constants

// - Window settings
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define FRAME_DELAY_MS (1000 / 60) // 60 FPS

// Game class
class Game {
public:
    Game() : window(nullptr), renderer(nullptr) {}

    bool Initialize(const char* title, int width, int height)
    {
        // Initialize SDL
        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "[error] Couldn't initialize SDL2: %s", SDL_GetError());
            return false;
        }

        // Create window
        window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN);
        if (window == nullptr) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "[error] Couldn't create window: %s", SDL_GetError());
            SDL_Quit();
            return false;
        }

        // Create renderer
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        if (renderer == nullptr) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "[error] Couldn't create renderer: %s", SDL_GetError());
            SDL_DestroyWindow(window);
            SDL_Quit();
            return false;
        }

        return true;
    }

    void Run()
    {
        bool quit = false;
        SDL_Event event;

        Uint32 frameStart, frameTime;

        while (!quit) {
            frameStart = SDL_GetTicks();

            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT) {
                    quit = true;
                }
            }

            Update();
            Render();

            // Delay leftover time
            frameTime = SDL_GetTicks() - frameStart;
            if (FRAME_DELAY_MS > frameTime) {
                SDL_Delay(FRAME_DELAY_MS - frameTime);
            }
        }
    }

    void Update() {
        // Update game
    }

    void Render()
    {
        // Clean-up
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Update screen
        SDL_RenderPresent(renderer);
    }

    void Shutdown()
    {
        // Final clean-up and shutdown
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);

        SDL_Quit();
    }

private:
    SDL_Window* window;
    SDL_Renderer* renderer;
};

int main(int argc, char* argv[])
{
    Game game;

    if (game.Initialize("Space Invaders", WINDOW_WIDTH, WINDOW_HEIGHT)) {
        game.Run();
    }

    game.Shutdown();

    return 0;
}