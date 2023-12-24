#include "../inc/Application.h"

Application* CreateApplication() {
    Application* app = malloc(sizeof(Application));
    if (app == NULL) {
        isError = 1;
        printf("[App]Failed to malloc app\n");
        return app;
    }
    app->bgm = NULL;
    app->font = NULL;
    app->game_scene = NULL;
    app->pause_scene = NULL;
    app->renderer = NULL;
    app->select_scene = NULL;
    app->start_scene = NULL;
    app->window = NULL;

    app->window = SDL_CreateWindow(
        GAME_NAME,
        0, 0, SCREEN_WIDTH, SCREEN_HEIGHT,
        /*SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_OPENGL*/
        SDL_WINDOW_SHOWN
    );
    if (app->window == NULL) {
        isError = 1;
        printf("[App]Failed to create window: %s\n", SDL_GetError());
        return app;
    }
    app->renderer = SDL_CreateRenderer(app->window, -1, SDL_RENDERER_ACCELERATED);
    if (app->renderer == NULL) {
        isError = 1;
        printf("[App]Failed to create renderer: %s\n", SDL_GetError());
        return app;
    }

    app->timer.ticks_count = SDL_GetTicks();

    app->bgm = Mix_LoadMUS(DEFAULT_MUSIC_PATH);
    if (app->bgm == NULL) {
        isError = 1;
        printf("[App]Failed to load music: %s\n", Mix_GetError());
        return app;
    }
    app->font = TTF_OpenFont(DEFAULT_FONT_PATH, DEFAULT_FONT_SIZE);
    if (app->font == NULL) {
        isError = 1;
        printf("[App]Failed to open font: %s\n", TTF_GetError());
        return app;
    }

    app->cur_scene = START_SCENE;
    app->start_scene = CreateOptionScene(app->renderer, app->font, START_SCENE, 2, "START", "QUIT");
    if (isError) {
        printf("[App]Failed to create start scene\n");
        return app;
    }
    app->select_scene = CreateSelectScene(app->renderer, app->font);
    if (isError) {
        printf("[App]Failed to create select scene\n");
        return app;
    }
    char buf[APPLICATION_BUFFER_SIZE];
    strcpy(buf, SAVE_PATH);
    strcat(buf, app->select_scene->cur_save_dir->d_name);
    strcat(buf, "/");
    app->game_scene = CreateGameScene(app->renderer, buf);
    if (isError) {
        printf("[App]Failed to create game scene\n");
        return app;
    }
    app->pause_scene = CreateOptionScene(app->renderer, app->font, PAUSE_SCENE, 3, "RESUME", "QUIT TO MENU", "QUIT GAME");
    if (isError) {
        printf("[App]Failed to create pause scene\n");
        return app;
    }

    return app;
}
void StartApplication(Application* app) {
    if (Mix_PlayingMusic() == 0) {
        Mix_PlayMusic(app->bgm, -1);
    }
}
Uint8 EventApplication(Application* app) {
    while (SDL_PollEvent(&app->e)) {
        if (app->e.type == SDL_QUIT) return 0;
        else {
            switch (app->cur_scene) {
            case START_SCENE: {
                break;
            }
            case SELECT_SCENE: {
                break;
            }
            case GAME_SCENE: {
                EventGameScene(&app->e, app->game_scene);
                break;
            }
            case PAUSE_SCENE: {
                break;
            }
            default:
                break;
            }
        }
    }
    return 1;
}
void UpdateApplication(Application* app) {

    SDL_RenderPresent(app->renderer);
    Tick(&app->timer, MAX_FPS);
}
void DrawApplication(Application* app) {
    SDL_RenderClear(app->renderer);
    switch (app->cur_scene) {
    case START_SCENE:
        DrawOptionScene(app->renderer, app->start_scene);
        break;
    case SELECT_SCENE:
        DrawSelectScene(app->renderer, app->select_scene);
        break;
    case GAME_SCENE:
        DrawGameScene(app->renderer, app->game_scene);
        break;
    case PAUSE_SCENE:
        DrawOptionScene(app->renderer, app->pause_scene);
        break;
    default:
        break;
    }
}
void DestroyApplication(Application* app) {
    if (app != NULL) {
        if (app->pause_scene != NULL) DestroyOptionScene(app->pause_scene);
        if (app->game_scene != NULL) DestroyGameScene(app->game_scene);
        if (app->select_scene != NULL) DestroySelectScene(app->select_scene);
        if (app->start_scene != NULL) DestroyOptionScene(app->start_scene);

        if (app->font != NULL) TTF_CloseFont(app->font);
        if (app->bgm != NULL) {
            Mix_HaltMusic();
            Mix_FreeMusic(app->bgm);
        }

        if (app->renderer != NULL) SDL_DestroyRenderer(app->renderer);
        if (app->window != NULL) SDL_DestroyWindow(app->window);
        free(app);
    }
}