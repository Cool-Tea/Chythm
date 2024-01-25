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
static void InitUpdateData();

GameScene* CreateGameScene(const char* chart_path);
void DestroyGameScene();

void GameSceneReset();
void GameSceneStart();
static void SaveScore();
void GameSceneEnd();
static Uint32 GameSceneTimeMark();
void GameScenePause();
void GameSceneResume();

static int GameSceneCheckEnd();
void GameSceneHandleKey(SDL_Event* event);
static void GameSceneHandleEvent();
void GameSceneUpdate();

static void GameSceneDrawTime();
static void GameSceneDrawScore();
static void GameSceneDrawHitText();
void GameSceneDraw();

#endif