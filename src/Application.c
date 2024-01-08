#include "../inc/Application.h"

Application* app = NULL;

Application* CreateApplication() {
    app = malloc(sizeof(Application));
    if (app == NULL) {
        printf("[Application]Failed to malloc app\n");
        is_error = 1;
        return app;
    }
    app->win = NULL, app->ren = NULL, app->font = NULL, app->menu = NULL, app->select = NULL, app->game = NULL, app->pause = NULL;

    /* SDL related */
    app->win = SDL_CreateWindow(
        GAME_TITLE,
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH, SCREEN_HEIGHT,
        SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN
    );
    if (app->win == NULL) {
        printf("[Application]Failed to create window: %s\n", SDL_GetError());
        is_error = 1;
        return app;
    }
    app->ren = SDL_CreateRenderer(app->win, -1, SDL_RENDERER_ACCELERATED);
    if (app->ren == NULL) {
        printf("[Application]Failed to create renderer: %s\n", SDL_GetError());
        is_error = 1;
        return app;
    }
    app->font = TTF_OpenFont(FONT_PATH, FONT_SIZE);
    if (app->font == NULL) {
        printf("[Application]Failed to open font: %s\n", TTF_GetError());
        is_error = 1;
        return app;
    }

    /* Scene related */
    app->menu = CreateMenuScene(app->ren);
    app->select = CreateSelectScene(app->ren);
    app->game = game_scene;
    app->pause = CreatePauseScene(app->ren);

    app->timer.cur_time = SDL_GetTicks();
    app->timer.delta_time = 1;

    return app;
}
void DestroyApplication() {
    if (app != NULL) {
        DestroyPauseScene();
        DestroyGameScene();
        DestroySelectScene();
        DestroyMenuScene();

        if (app->font != NULL) {
            TTF_CloseFont(app->font);
        }
        if (app->ren != NULL) {
            SDL_DestroyRenderer(app->ren);
        }
        if (app->win != NULL) {
            SDL_DestroyWindow(app->win);
        }
        free(app);
    }
}
void ApplicationStart() {
    is_running = 1;
    cur_scene = MENU;
}
void ApplicationStop() {
    Mix_HaltMusic();
}
void ApplicationUpdate() {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            is_running = 0;
            return;
        }
        switch (cur_scene) {
        case MENU: {
            MenuSceneUpdate(&e);
            break;
        }
        case SELECT: {
            SelectSceneUpdate(app->ren, &e);
            break;
        }
        case GAME: {
            if (app->game == NULL && game_scene != NULL) app->game = game_scene;
            GameSceneUpdate(&e);
            break;
        }
        case PAUSE: {
            PauseSceneUpdate(&e);
            break;
        }
        default:
            break;
        }
    }
    switch (cur_scene) {
    case MENU: {
        MenuSceneUpdate(&e);
        break;
    }
    case SELECT: {
        SelectSceneUpdate(app->ren, &e);
        break;
    }
    case GAME: {
        if (app->game == NULL && game_scene != NULL) app->game = game_scene;
        GameSceneUpdate(&e);
        break;
    }
    case PAUSE: {
        PauseSceneUpdate(&e);
        break;
    }
    default:
        break;
    }
}
void ApplicationDraw() {
    SDL_RenderClear(app->ren);
    switch (cur_scene) {
    case MENU: {
        MenuSceneDraw(app->ren, app->font);
        break;
    }
    case SELECT: {
        SelectSceneDraw(app->ren, app->font);
        break;
    }
    case GAME: {
        GameSceneDraw(app->ren, app->font);
        break;
    }
    case PAUSE: {
        PauseSceneDraw(app->ren, app->font);
        break;
    }
    default:
        break;
    }

#ifdef DEV
    char fps[1 << 4];
    int len = sprintf(fps, "fps: %u", 1000u / app->timer.delta_time);
    SDL_Rect rect = { .w = 10 * len, .h = 20, .x = 1800, .y = 0 };
    DrawText(app->ren, rect, fps, app->font, button_colors[0]);
#endif /* dev */

    SDL_RenderPresent(app->ren);
}
void ApplicationTick() {
    Uint32 fps_time = 1000 / MAX_FPS;
    while (SDL_GetTicks() < app->timer.cur_time + fps_time) {
        SDL_Delay(1);
    }
    app->timer.delta_time = SDL_GetTicks() - app->timer.cur_time;
    app->timer.cur_time += app->timer.delta_time;
}