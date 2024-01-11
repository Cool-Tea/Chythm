#include "../inc/PauseScene.h"

PauseScene* pause_scene = NULL;

PauseScene* CreatePauseScene(SDL_Renderer* renderer) {
    pause_scene = malloc(sizeof(PauseScene));
    if (pause_scene == NULL) {
        printf("[PauseScene]Failed to malloc pause scene\n");
        is_error = 1;
        return pause_scene;
    }
    pause_scene->background = IMG_LoadTexture(renderer, PAUSE_SCENE_BACKGROUND);
    if (pause_scene->background == NULL) {
        printf("[PauseScene]Failed to load background: %s\n", IMG_GetError());
        is_error = 1;
        return pause_scene;
    }
    InitButton(&pause_scene->buttons[0], SCREEN_WIDTH / 2 - 3 * LETTER_WIDTH, 200, "Resume", Resume);
    InitButton(&pause_scene->buttons[1], SCREEN_WIDTH / 2 - 6 * LETTER_WIDTH, 200 + 2 * LETTER_HEIGHT, "Back To Menu", BackToMenu);
    InitButton(&pause_scene->buttons[2], SCREEN_WIDTH / 2 - 2 * LETTER_WIDTH, 200 + 4 * LETTER_HEIGHT, "Quit", Quit);
    pause_scene->cur_button = 0;
}
void DestroyPauseScene() {
    if (pause_scene != NULL) {
        if (pause_scene->background != NULL) {
            SDL_DestroyTexture(pause_scene->background);
        }
        for (size_t i = 0; i < PAUSE_SCENE_BUTTON_SIZE; i++) {
            FreeButton(&pause_scene->buttons[i]);
        }
        free(pause_scene);
        pause_scene = NULL;
    }
}
void PauseSceneUpdate(SDL_Event* event) {
    if (event->type == SDL_KEYDOWN) {
        switch (event->key.keysym.scancode) {
        case SDL_SCANCODE_W:
        case SDL_SCANCODE_UP: {
            pause_scene->cur_button = (pause_scene->cur_button - 1 + PAUSE_SCENE_BUTTON_SIZE) % PAUSE_SCENE_BUTTON_SIZE;
            break;
        }
        case SDL_SCANCODE_S:
        case SDL_SCANCODE_DOWN: {
            pause_scene->cur_button = (pause_scene->cur_button + 1) % PAUSE_SCENE_BUTTON_SIZE;
            break;
        }
        case SDL_SCANCODE_E: {
            ButtonFunc(&pause_scene->buttons[pause_scene->cur_button]);
            break;
        }
        default:
            break;
        }
    }
}
void PauseSceneDraw(SDL_Renderer* renderer, TTF_Font* font) {
    SDL_RenderCopy(renderer, pause_scene->background, NULL, NULL);
    for (size_t i = 0; i < PAUSE_SCENE_BUTTON_SIZE; i++) {
        ButtonDraw(&pause_scene->buttons[i], renderer, font, i == pause_scene->cur_button);
    }
}

static void Resume() {
    GameSceneResume();
    cur_scene = GAME;
}
static void BackToMenu() {
    GameScenePause();
    DestroyGameScene();
    cur_scene = MENU;
}
static void Quit() {
    is_running = 0;
}