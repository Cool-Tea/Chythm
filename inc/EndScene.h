#ifndef _END_SCENE_H
#define _END_SCENE_H

#include <SDL_image.h>
#include "Button.h"
#include "GameScene.h"

struct EndScene {
    /* image */
    SDL_Texture* background;

    /* rating */
    const char* rating;

    /* button */
    Button buttons[END_SCENE_BUTTON_SIZE];
    size_t cur_button;
};
typedef struct EndScene EndScene;

extern EndScene* end_scene;

EndScene* CreateEndScene(SDL_Renderer* renderer);
void DestroyEndScene();
void EndSceneRate();
void EndSceneUpdate(SDL_Event* event);
void EndSceneDraw(SDL_Renderer* renderer, TTF_Font* font);

/* this is for the button */

static void TryAgain();
static void BackToSelect();
static void BackToMenu();

#endif