#ifndef _PAUSE_SCENE_H_
#define _PAUSE_SCENE_H_

#include <SDL_image.h>
#include "Button.h"
#include "GameScene.h"

struct PauseScene {
    /* image */
    SDL_Texture* background;

    /* button */
    Button buttons[PAUSE_SCENE_BUTTON_SIZE];
    size_t cur_button;
};
typedef struct PauseScene PauseScene;

extern PauseScene* pause_scene;

PauseScene* CreatePauseScene(SDL_Renderer* renderer);
void DestroyPauseScene();
void PauseSceneUpdate(SDL_Event* event);
void PauseSceneDraw(SDL_Renderer* renderer, TTF_Font* font);

/* this is for the button */

static void Resume();
static void BackToMenu();
static void Quit();

#endif