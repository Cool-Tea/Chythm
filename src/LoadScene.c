#include "LoadScene.h"

LoadScene* load_scene = NULL;

LoadScene* CreateLoadScene() {
    load_scene = malloc(sizeof(LoadScene));
    if (load_scene == NULL) {
        fprintf(stderr, "[LoadScene]Failed to malloc load scene\n");
        app.is_error = 1;
        return load_scene;
    }

#if !USE_DEFAULT_BACKGROUND
    load_scene->background = IMG_LoadTexture(app.ren, LOAD_SCENE_BACKGROUND);
    if (load_scene->background == NULL) {
        fprintf(stderr, "[LoadScene]Failed to load background: %s\n", IMG_GetError());
        app.is_error = 1;
        return load_scene;
    }
#endif

    InitEffect(&load_scene->loading_effect, CIRCLE, 1);
    load_scene->loading_effect.is_active = 1;
    return load_scene;
}

void DestroyLoadScene() {
    if (load_scene != NULL) {
        FreeEffect(&load_scene->loading_effect);
        if (load_scene->background != NULL) {
            SDL_DestroyTexture(load_scene->background);
        }
        free(load_scene);
        load_scene = NULL;
    }
}

void LoadSceneUpdate() {
    SDL_LockMutex(app.mutex);
    if (app.is_loaded) {
        app.cur_scene = GAME;
        SDL_UnlockMutex(app.mutex);
        return;
    }
    SDL_UnlockMutex(app.mutex);
    EffectUpdate(&load_scene->loading_effect);
}

void LoadSceneDraw() {
    SDL_LockMutex(app.mutex);
#if USE_DEFAULT_BACKGROUND
    DrawDefaultBackground();
#else
    SDL_RenderCopy(app.ren, load_scene->background, NULL, NULL);
#endif
    SDL_UnlockMutex(app.mutex);

    EffectDraw(&load_scene->loading_effect, 1720, 880, 100, 0.0);
}