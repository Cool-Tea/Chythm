#ifndef _GAME_SCENE_H_
#define _GAME_SCENE_H_

#include <SDL_image.h>
#include <SDL_mixer.h>
#include "Constants.h"
#include "Lane.h"

struct GameScene {
    /* timer */
    Uint32 base_time;
    Uint32 relative_time;
    Uint32 cur_time;

    /* image and audio */
    SDL_Texture* background;
    Mix_Music* music;

    /* event */
    EventList event_list;

    /* lane */
    size_t lane_size;
    Lane* lanes;
};
typedef struct GameScene GameScene;
GameScene* CreateGameScene(SDL_Renderer* renderer, const char* chart_path);
void DestroyGameScene(GameScene* game_scene);
void GameSceneStart(GameScene* game_scene);
void GameScenePause(GameScene* game_scene);
void GameSceneResume(GameScene* game_scene);
void GameSceneUpdate(GameScene* game_scene, SDL_Event* event);
void GameSceneDraw(GameScene* GameScene, SDL_Renderer* renderer);

#endif