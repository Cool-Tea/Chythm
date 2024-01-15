#ifndef _GAME_SCENE_H_
#define _GAME_SCENE_H_

#include <SDL_image.h>
#include <SDL_mixer.h>
#include "cJSON.h"
#include "Constants.h"
#include "Lane.h"
#include "Drawer.h"

struct GameScene {
    /* file path*/
    const char* chart_path;

    /* image and audio */
    SDL_Texture* background;
    Mix_Music* music;

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

/* chart_path sample: ../saves/test/ */
static inline void CompletePath(char* buf, const char* path, const char* file);
static cJSON* OpenJson(const char* chart_path);
static int GetAudio(const char* chart_path, const char* audio_path);
static int GetBackground(const char* chart_path, const char* img_path);
static SDL_Scancode AllocateKey(int size, int i);
static int GetLanes(cJSON* hit_points, int size);
static void GetNotes(cJSON* notes, int bpm);
static void GetEvents(cJSON* events);
static int GetScore(const char* chart_path);
GameScene* CreateGameScene(const char* chart_path);
void DestroyGameScene();
void GameSceneReset();
void GameSceneStart();
void GameSceneEnd();
void GameScenePause();
void GameSceneResume();
static void GameSceneCheckEnd();
static void GameSceneHandleKey();
static void GameSceneHandleEvent();
static void GameSceneUpdateTime();
void GameSceneUpdate(SDL_Event* event);
void GameSceneDraw();

#endif