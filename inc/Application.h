#pragma once
#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#include "Timer.h"
#include "SoundTrack.h"
#include "Scene.h"

/* Macros here */
#define APPLICATION_BUFFER_SIZE 1 << 8
/* Macros end */

struct Application {
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Event e;

    Timer timer;

    Mix_Music* bgm;
    TTF_Font* font;

    SceneType cur_scene;
    OptionScene* start_scene;
    SelectScene* select_scene;
    GameScene* game_scene;
    OptionScene* pause_scene;
};
typedef struct Application Application;
Application* CreateApplication();
void StartApplication(Application* app);
Uint8 EventApplication(Application* app);
void UpdateApplication(Application* app);
void DrawApplication(Application* app);
void DestroyApplication(Application* app);

#endif