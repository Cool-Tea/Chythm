#ifndef _PAUSE_SCENE_H_
#define _PAUSE_SCENE_H_

#include "Button.h"
#include "GameScene.h"

struct PauseScene {
    /* image */
#if !USE_DEFAULT_BACKGROUND
    SDL_Texture* background;
#endif

    /* button */
    Button buttons[PAUSE_SCENE_BUTTON_SIZE];
    size_t cur_button;
};
typedef struct PauseScene PauseScene;

extern PauseScene* pause_scene;

PauseScene* CreatePauseScene();
void DestroyPauseScene();
static void PauseSceneHandleKey();
void PauseSceneUpdate(SDL_Event* event);
void PauseSceneDraw();

/* this is for the button */

static void Resume();
static void BackToMenu();
static void Quit();

#endif