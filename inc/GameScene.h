#ifndef _GAME_SCENE_H_
#define _GAME_SCENE_H_

#include <SDL_mixer.h>
#include "cJSON.h"
#include "Lane.h"

struct GameScene {
    /* file path*/
    const char* chart_path;

    /* image and audio */
#if !USE_DEFAULT_BACKGROUND
    SDL_Texture* background;
#endif

    Mix_Music* audio;

    /* event */
    EventList event_list;

    /* lane */
    size_t lane_size;
    Lane* lanes;

    /* scores */
    int combo;
    Uint64 score;
    Uint64 history_score; // the best score in history
};
typedef struct GameScene GameScene;

extern GameScene* game_scene;

GameScene* CreateGameScene(const char* chart_path);
int ThreadCreateGameScene(void* chart_path);
void DestroyGameScene();

void GameSceneReset();
void GameSceneStart();
void GameSceneEnd();
void GameScenePause();
void GameSceneResume();

void GameSceneHandleKey(SDL_Event* event);
void GameSceneUpdate();

void GameSceneDraw();

#endif