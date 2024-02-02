#include "PauseScene.h"

PauseScene* pause_scene = NULL;

PauseScene* CreatePauseScene() {
    pause_scene = malloc(sizeof(PauseScene));
    if (pause_scene == NULL) {
        fprintf(stderr, "[PauseScene]Failed to malloc pause scene\n");
        app.error_level = app.error_level > 2 ? app.error_level : 2;
        return pause_scene;
    }

#if !USE_DEFAULT_BACKGROUND
    pause_scene->background = IMG_LoadTexture(app.ren, PAUSE_SCENE_BACKGROUND);
    if (pause_scene->background == NULL) {
        fprintf(stderr, "[PauseScene]Failed to load background: %s\n", IMG_GetError());
        app.error_level = app.error_level > 2 ? app.error_level : 2;
        return pause_scene;
    }
#endif

    InitButton(&pause_scene->buttons[0], STD_SCREEN_WIDTH / 2 - 3 * LETTER_WIDTH, 200, "Resume", Resume);
    InitButton(&pause_scene->buttons[1], STD_SCREEN_WIDTH / 2 - 6 * LETTER_WIDTH, 200 + 2 * LETTER_HEIGHT, "Back To Menu", BackToMenu);
    InitButton(&pause_scene->buttons[2], STD_SCREEN_WIDTH / 2 - 2 * LETTER_WIDTH, 200 + 4 * LETTER_HEIGHT, "Quit", Quit);
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

static void PauseSceneHandleKeyDown(SDL_Scancode key) {
    switch (key) {
    case SDL_SCANCODE_E:
    case SDL_SCANCODE_KP_ENTER: {
        ButtonFunc(&pause_scene->buttons[pause_scene->cur_button]);
        break;
    }
    case SDL_SCANCODE_W:
    case SDL_SCANCODE_UP: {
        pause_scene->buttons[pause_scene->cur_button].is_on = 0;
        pause_scene->cur_button = (pause_scene->cur_button - 1 + PAUSE_SCENE_BUTTON_SIZE) % PAUSE_SCENE_BUTTON_SIZE;
        pause_scene->buttons[pause_scene->cur_button].is_on = 1;
        break;
    }
    case SDL_SCANCODE_S:
    case SDL_SCANCODE_DOWN: {
        pause_scene->buttons[pause_scene->cur_button].is_on = 0;
        pause_scene->cur_button = (pause_scene->cur_button + 1) % PAUSE_SCENE_BUTTON_SIZE;
        pause_scene->buttons[pause_scene->cur_button].is_on = 1;
        break;
    }
    default:
        break;
    }
}

void PauseSceneHandleKey(SDL_Event* event) {
    if (event->type == SDL_KEYDOWN) {
        PauseSceneHandleKeyDown(event->key.keysym.scancode);
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