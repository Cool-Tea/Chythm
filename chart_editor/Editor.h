#ifndef _EDITOR_H_
#define _EDITOR_H_

#include <SDL2/SDL.h>
// #include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include "../inc/cJSON.h"

#define SCREEN_WIDTH 600
#define SCREEN_HEIGHT 300
// #define BACKGROUND "../assets/images/pure.png"
#define FONT "../assets/fonts/font.TTF"
#define STEP_INTERVAL 0.5 // second
#define MAX_KEY_NUM 8

typedef enum NoteType {
    SINGLE = 0,
    LONG
} NoteType;

typedef struct Application {
    SDL_Window* win;
    SDL_Renderer* ren;
    // SDL_Texture* background;
    Mix_Music* audio;
    TTF_Font* font;
    Uint32 cur_time;
    Uint8 is_running;
    Uint8 is_error;
    const Uint8* key_state;

    cJSON* json;
    cJSON* notes;
    size_t lane_size;
    size_t* notes_size;
    Uint8* down_states;
    Uint32* times;
    cJSON** insert_poses;
} Application;

Application app;
const SDL_Scancode default_keys[MAX_KEY_NUM] = {
    SDL_SCANCODE_S,
    SDL_SCANCODE_D,
    SDL_SCANCODE_F,
    SDL_SCANCODE_G,
    SDL_SCANCODE_H,
    SDL_SCANCODE_J,
    SDL_SCANCODE_K,
    SDL_SCANCODE_L
};

void InitEditor(size_t lane_size, char* audio_path);
void QuitEditor();
void InitApplication(size_t lane_size, char* audio_path);
void FreeApplication();
void ApplicationStart();
void AddNote(cJSON* notes, int type, int lane);
void JsonHandleKey(SDL_Event* event);
void ApplicationHandleKey(SDL_Event* event);
void DrawText(int x, int y, int w, int h, const char* text);
void ApplicationDraw();
void ApplicationTick();

#endif