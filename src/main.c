#include "../inc/Application.h"

void InitGame();
void QuitGame();

int main() {

    InitGame();
    if (is_error) {
        printf("[Chythm]Failed to init game\n");
        return 1;
    }
    ApplicationStart();
    while (is_running) {
        ApplicationUpdate();
        ApplicationDraw();
        ApplicationTick();
    }
    ApplicationStop();
    QuitGame();

    return 0;
}

void InitGame() {
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        printf("[Chythm]Failed to init SDL: %s\n", SDL_GetError());
        return;
    }
    else if (!(IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG) & (IMG_INIT_JPG | IMG_INIT_PNG))) {
        printf("[Chythm]Failed to init IMG: %s\n", IMG_GetError());
        return;
    }
    else if (!(Mix_Init(MIX_INIT_MP3) & MIX_INIT_MP3)) {
        printf("[Chythm]Failed to init MIX: %s\n", Mix_GetError());
        return;
    }
    else if (TTF_Init() < 0) {
        printf("[Chythm]Failed to init TTF: %s\n", TTF_GetError());
        return;
    }
    app = CreateApplication();
}
void QuitGame() {
    DestroyApplication();
    TTF_Quit();
    Mix_Quit();
    IMG_Quit();
    SDL_Quit();
}