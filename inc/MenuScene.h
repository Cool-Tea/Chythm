#ifndef _MENU_SCENE_H_
#define _MENU_SCENE_H_

#include "Button.h"

struct MenuScene {
    /* image */
#if !USE_DEFAULT_BACKGROUND
    SDL_Texture* background;
#endif

    /* button */
    Button buttons[MENU_SCENE_BUTTON_SIZE]; // start and quit
    size_t cur_button;
};
typedef struct MenuScene MenuScene;

extern MenuScene* menu_scene;

MenuScene* CreateMenuScene();
void DestroyMenuScene();
void MenuSceneHandleKey(SDL_Event* event);
void MenuSceneUpdate();
void MenuSceneDraw();

/* this is for the button */

static void Start();
static void Quit();

#endif