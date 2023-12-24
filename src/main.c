#include "../inc/Application.h"

int main() {

    /* Init the system */
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        printf("Failed to Init SDL: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }
    else if (!(IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG) & (IMG_INIT_PNG | IMG_INIT_JPG))) {
        printf("Failed to Init IMG: %s\n", IMG_GetError());
        IMG_Quit();
        SDL_Quit();
        return 1;
    }
    else if (TTF_Init() < 0) {
        printf("Failed to Init TTF: %s\n", TTF_GetError());
        TTF_Quit();
        IMG_Quit();
        SDL_Quit();
        return 1;
    }
    else if (!Mix_Init(MIX_INIT_MP3)) {
        printf("Failed to Init MIX: %s\n", Mix_GetError());
        Mix_Quit();
        TTF_Quit();
        IMG_Quit();
        SDL_Quit();
        return 1;
    }
    else if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 4096) == -1) {
        printf("Failed to open audio: %s\n", Mix_GetError());
        Mix_Quit();
        TTF_Quit();
        IMG_Quit();
        SDL_Quit();
    }

    Application* app = CreateApplication();
    if (isError) {
        printf("Failed to init game\n");
        DestroyApplication(app);
        Mix_Quit();
        TTF_Quit();
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    /* Start Game */
    Uint8 isRunning = 1;
    StartApplication(app);
    while (isRunning) {

        isRunning = EventApplication(app);

        DrawApplication(app);

        UpdateApplication(app);
    }

    /* End Game */
    DestroyApplication(app);
    Mix_Quit();
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();

    return 0;
}