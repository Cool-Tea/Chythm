#ifndef _LOAD_SCENE_H_
#define _LOAD_SCENE_H_

#include "GameScene.h"

struct LoadScene {
#if !USE_DEFAULT_BACKGROUND
    SDL_Texture* background;
#endif

    Effect loading_effect;
};
typedef struct LoadScene LoadScene;

extern LoadScene* load_scene;

LoadScene* CreateLoadScene();
void DestroyLoadScene();
void LoadSceneUpdate();
void LoadSceneDraw();

#endif