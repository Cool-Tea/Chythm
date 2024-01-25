#include "../inc/Application.h"

/* TODO: clear out the #includes */

void InitGame();
void QuitGame();

int main() {

    InitGame();
    if (app.is_error) {
        fprintf(stderr, "[Chythm]Failed to init game\n");
        return 1;
    }
    ApplicationStart();
    while (app.is_running) {
        ApplicationHandleKey();
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
        fprintf(stderr, "[Chythm]Failed to init SDL: %s\n", SDL_GetError());
        app.is_error = 1;
        return;
    }
    else if (!(IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG) & (IMG_INIT_JPG | IMG_INIT_PNG))) {
        fprintf(stderr, "[Chythm]Failed to init IMG: %s\n", IMG_GetError());
        app.is_error = 1;
        return;
    }
    else if (!(Mix_Init(MIX_INIT_MP3) & MIX_INIT_MP3)) {
        fprintf(stderr, "[Chythm]Failed to init MIX: %s\n", Mix_GetError());
        app.is_error = 1;
        return;
    }
    else if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 4096) < 0) {
        fprintf(stderr, "[Chythm]Failed to init MIX: %s\n", Mix_GetError());
        app.is_error = 1;
        return;
    }
    else if (TTF_Init() < 0) {
        fprintf(stderr, "[Chythm]Failed to init TTF: %s\n", TTF_GetError());
        app.is_error = 1;
        return;
    }
    InitApplication();
}
void QuitGame() {
    DestroyApplication();
    TTF_Quit();
    Mix_CloseAudio();
    Mix_Quit();
    IMG_Quit();
    SDL_Quit();
}