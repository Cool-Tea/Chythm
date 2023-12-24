#pragma once
#ifndef _SCENE_H_
#define _SCENE_H_

#include "Button.h"
#include "SoundTrack.h"

/* Macros here */
#define START_SCENE_BUTTON_X 100
#define START_SCENE_TOP_BUTTON_Y 640
#define START_SCENE_BUTTON_INTERVAL 20

#define SELECT_SCENE_BUFFER_SIZE 1 << 8

#define PAUSE_SCENE_TOP_BUTTON_Y 200
#define PAUSE_SCENE_BUTTON_INTERVAL 100

#define GAME_SCENE_BUFFER_SIZE 1 << 15
/* Macros end */

enum SceneType {
    START_SCENE,
    SELECT_SCENE,
    GAME_SCENE,
    PAUSE_SCENE
};
typedef enum SceneType SceneType;

struct OptionScene {
    SDL_Texture* bkground;
    TTF_Font* font;
    size_t buttons_size;
    Button** buttons;
    Button** cursor;
};
typedef struct OptionScene OptionScene;
/**
 * Create option scene with b_size args const char* text
*/
OptionScene* CreateOptionScene(SDL_Renderer* renderer, TTF_Font* font, SceneType scene_type, size_t b_size, const char* text, ...);
void EventOptionScene(SDL_Event* event, OptionScene* option_scene);
void DrawOptionScene(SDL_Renderer* renderer, OptionScene* option_scene);
void DestroyOptionScene(OptionScene* opption_scene);

struct SelectScene {
    SDL_Texture* bkground;
    TTF_Font* font;

    /* select scrolls */
    DIR* saves_dir;
    struct dirent* cur_save_dir;
    size_t name_size;
    char* name;
    SDL_Texture* preview;
};
typedef struct SelectScene SelectScene;
SelectScene* CreateSelectScene(SDL_Renderer* renderer, TTF_Font* font);
void EventSelectScene(SDL_Event* event, SelectScene* select_scene);
void DrawSelectScene(SDL_Renderer* renderer, SelectScene* select_scene);
void DestroySelectScene(SelectScene* select_scene);

struct GameScene {
    SDL_Texture* bkground;
    Mix_Music* bgm;
    size_t stracks_size;
    SoundTrack** stracks;
};
typedef struct GameScene GameScene;
GameScene* CreateGameScene(SDL_Renderer* renderer, const char* save_path);
void StartGameScene(GameScene* game_scene);
void EventGameScene(SDL_Event* event, GameScene* game_scene);
void UpdateGameScene(GameScene* game_scene);
void DrawGameScene(SDL_Renderer* renderer, GameScene* game_scene);
void DestroyGameScene(GameScene* game_scene);

#endif