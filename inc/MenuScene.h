#ifndef _MENU_SCENE_H_
#define _MENU_SCENE_H_

#include <SDL_image.h>
#include "Button.h"

struct MenuScene {
    /* image */
    SDL_Texture* background;

    /* button */
    Button buttons[MENU_SCENE_BUTTON_SIZE]; // start and quit
    size_t cur_button;
};
typedef struct MenuScene MenuScene;

extern MenuScene* menu_scene;

MenuScene* CreateMenuScene(SDL_Renderer* renderer);
void DestroyMenuScene();
void MenuSceneUpdate(SDL_Event* event);
void MenuSceneDraw(SDL_Renderer* renderer, TTF_Font* font);
/* this is for the button */
static void Start();
static void Quit();

#endif