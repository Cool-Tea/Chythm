#include "../inc/MenuScene.h"

MenuScene* menu_scene = NULL;

MenuScene* CreateMenuScene(SDL_Renderer* renderer) {
    menu_scene = malloc(sizeof(MenuScene));
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
    menu_scene->cur_button = 0;
}
void DestroyMenuScene() {
    if (menu_scene != NULL) {
        if (menu_scene->background != NULL) {
            SDL_DestroyTexture(menu_scene->background);
        }
        FreeButton(menu_scene->buttons);
        FreeButton(menu_scene->buttons + 1);
        free(menu_scene);
    }
}
void MenuSceneUpdate(SDL_Event* event) {
    if (event->type == SDL_KEYDOWN) {
        switch (event->key.keysym.scancode) {
        case SDL_SCANCODE_W:
        case SDL_SCANCODE_UP: {
            menu_scene->cur_button = (menu_scene->cur_button - 1 + MENU_SCENE_BUTTON_SIZE) % MENU_SCENE_BUTTON_SIZE;
            break;
        }
        case SDL_SCANCODE_S:
        case SDL_SCANCODE_DOWN: {
            menu_scene->cur_button = (menu_scene->cur_button + 1) % MENU_SCENE_BUTTON_SIZE;
            break;
        }
        case SDL_SCANCODE_E:
        case SDL_SCANCODE_KP_ENTER: {
            ButtonFunc(&menu_scene->buttons[menu_scene->cur_button]);
            break;
        }
        default:
            break;
        }
    }
}
void MenuSceneDraw(SDL_Renderer* renderer, TTF_Font* font) {
    SDL_RenderCopy(renderer, menu_scene->background, NULL, NULL);
    for (int i = 0; i < MENU_SCENE_BUTTON_SIZE; i++) {
        ButtonDraw(&menu_scene->buttons[i], renderer, font, i == menu_scene->cur_button);
    }
    DrawCursor(renderer, menu_scene->buttons[menu_scene->cur_button].rect);
}
static void Start() {
    cur_scene = SELECT;
}
static void Quit() {
    is_running = 0;
}