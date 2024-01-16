#include "../inc/Application.h"

void InitApplication() {
    app.win = NULL;
    app.ren = NULL;
    app.font = NULL;

    /* SDL related */
    app.win = SDL_CreateWindow(
        GAME_TITLE,
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH, SCREEN_HEIGHT,
        SDL_WINDOW_OPENGL /*| SDL_WINDOW_FULLSCREEN_DESKTOP*/
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

void ApplicationUpdate() {
    SDL_Event e;
    SDL_PollEvent(&e);
    do {
        switch (app.cur_scene) {
        case MENU: {
            MenuSceneUpdate(&e);
            break;
        }
        case SELECT: {
            SelectSceneUpdate(&e);
            break;
        }
        case GAME: {
            GameSceneUpdate(&e);
            break;
        }
        case PAUSE: {
            PauseSceneUpdate(&e);
            break;
        }
        case END: {
            EndSceneUpdate(&e);
            break;
        }
        default:
            break;
        }
    } while (SDL_PollEvent(&e));
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
    static SDL_Rect rect = { .h = 20, .x = 1800, .y = 0 };
    int len = sprintf(fps, "fps: %u", 1000u / app.timer.delta_time);
    rect.w = 10 * len;
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