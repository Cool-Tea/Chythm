#include "../inc/MenuScene.h"

MenuScene* CreateMenuScene(SDL_Renderer* renderer) {
    MenuScene* menu_scene = malloc(sizeof(MenuScene));
    if (menu_scene == NULL) {
        printf("[MenuScene]Failed to malloc menu scene\n");
        is_error = 1;
        return menu_scene;
    }
    menu_scene->background = IMG_LoadTexture(renderer, MENU_SCENE_BACKGROUND);
    if (menu_scene->background == NULL) {
        printf("[MenuScene]Failed to load background: %s\n", IMG_GetError());
        is_error = 1;
        return menu_scene;
    }
    InitButton(&menu_scene->buttons[0], 100, 640, "START", Start);
    InitButton(&menu_scene->buttons[1], 100, 640 + 2 * LETTER_HEIGHT, "QUIT", Quit);
}
void DestroyMenuScene(MenuScene* menu_scene) {
    if (menu_scene != NULL) {
        if (menu_scene->background != NULL) {
            SDL_DestroyTexture(menu_scene->background);
        }
        FreeButton(menu_scene->buttons);
        FreeButton(menu_scene->buttons + 1);
        free(menu_scene);
    }
}
void MenuSceneUpdate(MenuScene* menu_scene, SDL_Event* event) {
    if (event->type == SDL_KEYDOWN) {
        switch (event->key.keysym.sym) {
        case SDLK_w:
        case SDLK_UP: {
            menu_scene->cur_button = (menu_scene->cur_button - 1 + MENU_SCENE_BUTTON_SIZE) % MENU_SCENE_BUTTON_SIZE;
            break;
        }
        case SDLK_s:
        case SDLK_DOWN: {
            menu_scene->cur_button = (menu_scene->cur_button + 1) % MENU_SCENE_BUTTON_SIZE;
            break;
        }
        case SDLK_KP_ENTER: {
            ButtonFunc(&menu_scene->buttons[menu_scene->cur_button]);
            break;
        }
        default:
            break;
        }
    }
}
void MenuSceneDraw(MenuScene* menu_scene, SDL_Renderer* renderer, TTF_Font* font) {
    SDL_RenderCopy(renderer, menu_scene->background, NULL, NULL);
    for (int i = 0; i < MENU_SCENE_BUTTON_SIZE; i++) {
        ButtonDraw(&menu_scene->buttons[i], renderer, font);
    }
    /* TODO: draw cursor */
}
static void Start() {
    cur_scene = SELECT;
}
static void Quit() {
    is_running = 0;
}