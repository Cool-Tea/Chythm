#ifndef _END_SCENE_H
#define _END_SCENE_H

#include "Button.h"
#include "GameScene.h"

struct EndScene {
    /* image */
#if !USE_DEFAULT_BACKGROUND
    SDL_Texture* background;
#endif

    /* rating */
    const char* rating;

    /* button */
    Button buttons[END_SCENE_BUTTON_SIZE];
    size_t cur_button;
};
typedef struct EndScene EndScene;

extern EndScene* end_scene;

EndScene* CreateEndScene();
void DestroyEndScene();
void EndSceneRate();
static void EndSceneHandleKeyDown(SDL_Scancode key);
void EndSceneHandleKey(SDL_Event* event);
void EndSceneUpdate();
static void EndSceneDrawRating();
static void EndSceneDrawScore();
void EndSceneDraw();

/* this is for the button */

static void TryAgain();
static void BackToSelect();
static void BackToMenu();

#endif