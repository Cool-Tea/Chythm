#ifndef _GAME_SCENE_H_
#define _GAME_SCENE_H_

#include <SDL_image.h>
#include <SDL_mixer.h>
#include "cJSON.h"
#include "Constants.h"
#include "Lane.h"
#include "Drawer.h"

struct GameScene {
    /* timer */
    Uint32 base_time;
    Uint32 relative_time;
    Uint32 cur_time;

    /* image and audio */
    SDL_Texture* background;
    Mix_Music* music;

    /* text */
    const char* text;

    /* event */
    EventList event_list;

    /* lane */
    size_t lane_size;
    Lane* lanes;
};
typedef struct GameScene GameScene;

extern GameScene* game_scene;

/* chart_path sample: ../saves/test/ */
GameScene* CreateGameScene(SDL_Renderer* renderer, const char* chart_path);
void DestroyGameScene();
void GameSceneStart();
void GameScenePause();
void GameSceneResume();
void GameSceneUpdate(SDL_Event* event);
void GameSceneDraw(SDL_Renderer* renderer, TTF_Font* font);

#endif