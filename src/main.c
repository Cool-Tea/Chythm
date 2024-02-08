#include "Application.h"

/* TODO: clear out the #includes */

void InitGame();
void QuitGame();

int main() {

    InitGame();
    if (app.error_level >= 2) {
        fprintf(stderr, "[Chythm]Failed to init game\n");
        return 1;
    }
    ApplicationStart();
    while (app.is_running&& app.error_level < 2) {
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
    if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_AUDIO | SDL_INIT_EVENTS | SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "[Chythm]Failed to init SDL: %s\n", SDL_GetError());
        app.error_level = app.error_level > 2 ? app.error_level : 2;
        return;
    }
    else if (!(IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG) & (IMG_INIT_JPG | IMG_INIT_PNG))) {
        fprintf(stderr, "[Chythm]Failed to init IMG: %s\n", IMG_GetError());
        app.error_level = app.error_level > 2 ? app.error_level : 2;
        return;
    }
    else if (!(Mix_Init(MIX_INIT_MP3) & MIX_INIT_MP3)) {
        fprintf(stderr, "[Chythm]Failed to init MIX: %s\n", Mix_GetError());
        app.error_level = app.error_level > 2 ? app.error_level : 2;
        return;
    }
    else if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 4096) < 0) {
        fprintf(stderr, "[Chythm]Failed to init MIX: %s\n", Mix_GetError());
        app.error_level = app.error_level > 2 ? app.error_level : 2;
        return;
    }
    else if (TTF_Init() < 0) {
        fprintf(stderr, "[Chythm]Failed to init TTF: %s\n", TTF_GetError());
        app.error_level = app.error_level > 2 ? app.error_level : 2;
        return;
    }
    InitConstants();
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