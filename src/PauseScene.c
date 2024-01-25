#include "../inc/PauseScene.h"

PauseScene* pause_scene = NULL;

PauseScene* CreatePauseScene() {
    pause_scene = malloc(sizeof(PauseScene));
    if (pause_scene == NULL) {
        fprintf(stderr, "[PauseScene]Failed to malloc pause scene\n");
        app.is_error = 1;
        return pause_scene;
    }

#if !USE_DEFAULT_BACKGROUND
    pause_scene->background = IMG_LoadTexture(app.ren, PAUSE_SCENE_BACKGROUND);
    if (pause_scene->background == NULL) {
        fprintf(stderr, "[PauseScene]Failed to load background: %s\n", IMG_GetError());
        app.is_error = 1;
        return pause_scene;
    }
#endif

    InitButton(&pause_scene->buttons[0], SCREEN_WIDTH / 2 - 3 * LETTER_WIDTH, 200, "Resume", Resume);
    InitButton(&pause_scene->buttons[1], SCREEN_WIDTH / 2 - 6 * LETTER_WIDTH, 200 + 2 * LETTER_HEIGHT, "Back To Menu", BackToMenu);
    InitButton(&pause_scene->buttons[2], SCREEN_WIDTH / 2 - 2 * LETTER_WIDTH, 200 + 4 * LETTER_HEIGHT, "Quit", Quit);
    pause_scene->cur_button = 0;
    pause_scene->buttons[pause_scene->cur_button].is_on = 1;
}

void DestroyPauseScene() {
    if (pause_scene != NULL) {

#if !USE_DEFAULT_BACKGROUND
        if (pause_scene->background != NULL) {
            SDL_DestroyTexture(pause_scene->background);
        }
#endif

        for (size_t i = 0; i < PAUSE_SCENE_BUTTON_SIZE; i++) {
            FreeButton(&pause_scene->buttons[i]);
        }
        free(pause_scene);
        pause_scene = NULL;
    }
}

void PauseSceneHandleKey(SDL_Event* event) {
    if (event->type == SDL_KEYDOWN) {
        if (app.key_status[SDL_SCANCODE_E] || app.key_status[SDL_SCANCODE_KP_ENTER]) {
            ButtonFunc(&pause_scene->buttons[pause_scene->cur_button]);
            return;
        }
        if (app.key_status[SDL_SCANCODE_W] || app.key_status[SDL_SCANCODE_UP]) {
            pause_scene->buttons[pause_scene->cur_button].is_on = 0;
            pause_scene->cur_button = (pause_scene->cur_button - 1 + PAUSE_SCENE_BUTTON_SIZE) % PAUSE_SCENE_BUTTON_SIZE;
            pause_scene->buttons[pause_scene->cur_button].is_on = 1;
        }
        if (app.key_status[SDL_SCANCODE_S] || app.key_status[SDL_SCANCODE_DOWN]) {
            pause_scene->buttons[pause_scene->cur_button].is_on = 0;
            pause_scene->cur_button = (pause_scene->cur_button + 1) % PAUSE_SCENE_BUTTON_SIZE;
            pause_scene->buttons[pause_scene->cur_button].is_on = 1;
        }
    }
}

void PauseSceneUpdate() {
}

void PauseSceneDraw() {
#if USE_DEFAULT_BACKGROUND
    DrawDefaultBackgroundPure();
#else
    SDL_RenderCopy(app.ren, pause_scene->background, NULL, NULL);
#endif

    for (size_t i = 0; i < PAUSE_SCENE_BUTTON_SIZE; i++) {
        ButtonDraw(&pause_scene->buttons[i]);
    }
}

static void Resume() {
    GameSceneResume();
    app.cur_scene = GAME;
}
static void BackToMenu() {
    GameScenePause();
    DestroyGameScene();
    app.cur_scene = MENU;
}
static void Quit() {
    app.is_running = 0;
}