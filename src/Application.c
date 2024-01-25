#include "../inc/Application.h"

void InitApplication() {
    app.win = NULL;
    app.ren = NULL;
    app.font = NULL;

    /* SDL related */
#if AUTO_RESOLUTION
    SDL_DisplayMode dm;
    if (SDL_GetCurrentDisplayMode(0, &dm) < 0) {
        fprintf(stderr, "[Application]Failed to get current resolution: %s\n", SDL_GetError());
        app.is_error = 1;
        return;
    }
    app.zoom_rate.w = (double)dm.w / SCREEN_WIDTH;
    app.zoom_rate.h = (double)dm.h / SCREEN_HEIGHT;
#endif

    app.win = SDL_CreateWindow(
        GAME_TITLE,
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,

#if AUTO_RESOLUTION
        SCREEN_WIDTH * app.zoom_rate.w, SCREEN_HEIGHT * app.zoom_rate.h,
#else
        SCREEN_WIDTH, SCREEN_HEIGHT,
#endif

        SDL_WINDOW_OPENGL /* TODO: | SDL_WINDOW_FULLSCREEN_DESKTOP*/
    );
    if (app.win == NULL) {
        fprintf(stderr, "[Application]Failed to create window: %s\n", SDL_GetError());
        app.is_error = 1;
        return;
    }

    app.ren = SDL_CreateRenderer(app.win, -1, SDL_RENDERER_ACCELERATED);
    if (app.ren == NULL) {
        fprintf(stderr, "[Application]Failed to create renderer: %s\n", SDL_GetError());
        app.is_error = 1;
        return;
    }

    app.font = TTF_OpenFont(FONT_PATH, FONT_SIZE);
    if (app.font == NULL) {
        fprintf(stderr, "[Application]Failed to open font: %s\n", TTF_GetError());
        app.is_error = 1;
        return;
    }

    /* Scene related */
    CreateMenuScene(app.ren);
    CreateSelectScene(app.ren);
    CreatePauseScene(app.ren);
    CreateEndScene(app.ren);

    app.timer.real_time = SDL_GetTicks();
    app.timer.delta_time = 1;
    app.timer.base_time = app.timer.real_time;
    app.timer.relative_time = 0;

    /* keyboard status */
    app.key_status = SDL_GetKeyboardState(NULL);

    /* assets */
    InitAssets();
}

void DestroyApplication() {
    FreeAssets();

    DestroyEndScene();
    DestroyPauseScene();
    DestroyGameScene();
    DestroySelectScene();
    DestroyMenuScene();

    if (app.font != NULL) {
        TTF_CloseFont(app.font);
    }
    if (app.ren != NULL) {
        SDL_DestroyRenderer(app.ren);
    }
    if (app.win != NULL) {
        SDL_DestroyWindow(app.win);
    }
}

void ApplicationStart() {
    app.is_running = 1;
    app.cur_scene = MENU;
}

void ApplicationStop() {
    Mix_HaltMusic();
}

void ApplicationHandleKey() {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        switch (app.cur_scene) {
        case MENU: {
            MenuSceneHandleKey(&e);
            break;
        }
        case SELECT: {
            SelectSceneHandleKey(&e);
            break;
        }
        case GAME: {
            GameSceneHandleKey(&e);
            break;
        }
        case PAUSE: {
            PauseSceneHandleKey(&e);
            break;
        }
        case END: {
            EndSceneHandleKey(&e);
            break;
        }
        default:
            break;
        }
    }
}

void ApplicationUpdate() {
    switch (app.cur_scene) {
    case MENU: {
        MenuSceneUpdate();
        break;
    }
    case SELECT: {
        SelectSceneUpdate();
        break;
    }
    case GAME: {
        GameSceneUpdate();
        break;
    }
    case PAUSE: {
        PauseSceneUpdate();
        break;
    }
    case END: {
        EndSceneUpdate();
        break;
    }
    default:
        break;
    }
}

void ApplicationDraw() {
    SDL_RenderClear(app.ren);
    switch (app.cur_scene) {
    case MENU: {
        MenuSceneDraw(app.ren, app.font);
        break;
    }
    case SELECT: {
        SelectSceneDraw(app.ren, app.font);
        break;
    }
    case GAME: {
        GameSceneDraw(app.ren, app.font);
        break;
    }
    case PAUSE: {
        PauseSceneDraw(app.ren, app.font);
        break;
    }
    case END: {
        EndSceneDraw(app.ren, app.font);
        break;
    }
    default:
        break;
    }

#ifdef DEV
    static char fps[1 << 4];
    static SDL_Rect rect
#if !AUTO_RESOLUTION
        = { .h = 20, .x = 1800, .y = 0 };
#endif
    ;
    int len = sprintf(fps, "fps: %u", 1000u / app.timer.delta_time);
    rect.w = 10 * len;

#if AUTO_RESOLUTION
    rect.h = 20, rect.x = 1800, rect.y = 0;
    rect.x *= app.zoom_rate.w, rect.y *= app.zoom_rate.h;
    rect.w *= app.zoom_rate.w, rect.h *= app.zoom_rate.h;
#endif

    DrawText(rect, fps, default_colors[0]);
#endif /* dev */

    SDL_RenderPresent(app.ren);
}

void ApplicationTick() {
    static Uint32 fps_time = 1000 / MAX_FPS;
    while (SDL_GetTicks() < app.timer.real_time + fps_time) {
        SDL_Delay(1);
    }
    app.timer.delta_time = SDL_GetTicks() - app.timer.real_time;
    app.timer.real_time += app.timer.delta_time;
    app.timer.relative_time = app.timer.real_time - app.timer.base_time;
}