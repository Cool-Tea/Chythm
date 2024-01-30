#ifndef _GAME_SCENE_H_
#define _GAME_SCENE_H_

#include <SDL_mixer.h>
#include "cJSON.h"
#include "Lane.h"

struct GameScene {
    /* status */
    Uint8 status; // 0: not started 1: started 2: paused

    /* file path*/
    const char* chart_path;

    /* json file */
    cJSON* chart;

    /* image and audio */
#if !USE_DEFAULT_BACKGROUND
    SDL_Texture* background;
#endif

    Mix_Music* audio;

    /* event */
    List event_list;

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
void GameSceneReload();
void GameSceneStart();
void GameSceneEnd();
void GameScenePause();
void GameSceneResume();
void GameSceneHandleKey(SDL_Event* event);
void GameSceneUpdate();
void GameSceneDraw();

#endif