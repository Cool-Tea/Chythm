#include "MenuScene.h"

MenuScene* menu_scene = NULL;

MenuScene* CreateMenuScene() {
    menu_scene = malloc(sizeof(MenuScene));
    if (menu_scene == NULL) {
        fprintf(stderr, "[MenuScene]Failed to malloc menu scene\n");
        app.is_error = 1;
        return menu_scene;
    }

#if !USE_DEFAULT_BACKGROUND
    menu_scene->background = IMG_LoadTexture(app.ren, MENU_SCENE_BACKGROUND);
    if (menu_scene->background == NULL) {
        fprintf(stderr, "[MenuScene]Failed to load background: %s\n", IMG_GetError());
        app.is_error = 1;
        return menu_scene;
    }
#endif

    InitButton(&menu_scene->buttons[0], 100, 640, "START", Start);
    InitButton(&menu_scene->buttons[1], 100, 640 + 2 * LETTER_HEIGHT, "QUIT", Quit);
    menu_scene->cur_button = 0;
    menu_scene->buttons[menu_scene->cur_button].is_on = 1;
    return menu_scene;
}

void DestroyMenuScene() {
    if (menu_scene != NULL) {

#if !USE_DEFAULT_BACKGROUND
        if (menu_scene->background != NULL) {
            SDL_DestroyTexture(menu_scene->background);
        }
#endif

        FreeButton(menu_scene->buttons);
        FreeButton(menu_scene->buttons + 1);
        free(menu_scene);
        menu_scene = NULL;
    }
}

static void MenuSceneHandleKeyDown(SDL_Scancode key) {
    switch (key) {
    case SDL_SCANCODE_E:
    case SDL_SCANCODE_KP_ENTER: {
        ButtonFunc(&menu_scene->buttons[menu_scene->cur_button]);
        break;
    }
    case SDL_SCANCODE_W:
    case SDL_SCANCODE_UP: {
        menu_scene->buttons[menu_scene->cur_button].is_on = 0;
        menu_scene->cur_button = (menu_scene->cur_button - 1 + MENU_SCENE_BUTTON_SIZE) % MENU_SCENE_BUTTON_SIZE;
        menu_scene->buttons[menu_scene->cur_button].is_on = 1;
        break;
    }
    case SDL_SCANCODE_S:
    case SDL_SCANCODE_DOWN: {
        menu_scene->buttons[menu_scene->cur_button].is_on = 0;
        menu_scene->cur_button = (menu_scene->cur_button + 1) % MENU_SCENE_BUTTON_SIZE;
        menu_scene->buttons[menu_scene->cur_button].is_on = 1;
        break;
    }
    default:
        break;
    }
}

void MenuSceneHandleKey(SDL_Event* event) {
    if (event->type == SDL_KEYDOWN) {
        MenuSceneHandleKeyDown(event->key.keysym.scancode);
    }
}

void MenuSceneUpdate() {
}

void MenuSceneDraw() {
#if USE_DEFAULT_BACKGROUND
    DrawDefaultBackground();
#else
    SDL_RenderCopy(app.ren, menu_scene->background, NULL, NULL);
#endif
    for (int i = 0; i < MENU_SCENE_BUTTON_SIZE; i++) {
        ButtonDraw(&menu_scene->buttons[i]);
    }
    DrawCursor(menu_scene->buttons[menu_scene->cur_button].rect);
}

static void Start() {
    app.cur_scene = SELECT;
}

static void Quit() {
    app.is_running = 0;
}