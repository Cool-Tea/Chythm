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
    enum { SINGLE = 0, LONG } cur_mode;
    Uint8 lane_is_down[4];
    Uint32 lane_time[4];
    size_t lane_note_size[4];
} Application;

Application app;
const char* mode_text[2] = {
    "Single", "Long"
};
const int note_x[4] = {
    400, 800, 1200, 1600
};

void InitEditor(char* audio_path);
void QuitEditor();
void InitApplication(char* audio_path);
void FreeApplication();
void ApplicationStart();
void AddNote(cJSON* notes, int type, int lane);
void JsonHandleKey(SDL_Event* event);
void ApplicationHandleKey(SDL_Event* event);
void DrawText(int x, int y, int w, int h, const char* text);
void ApplicationDraw();
void ApplicationTick();

#endif