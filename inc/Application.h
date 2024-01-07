#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#include "MenuScene.h"
#include "SelectScene.h"
#include "GameScene.h"
#include "PauseScene.h"

/* TODO: timer and music */

struct Timer {
    Uint32 cur_time;
};
typedef struct Timer Timer;

struct Application {
    SDL_Window* win;
    SDL_Renderer* ren;
    TTF_Font* font;

    MenuScene* menu;
    SelectScene* select;
    GameScene* game;
    PauseScene* pause;

    Timer timer;
};
typedef struct Application Application;

extern Application* app;

Application* CreateApplication();
void DestroyApplication();
void ApplicationStart();
void ApplicationStop();
void ApplicationUpdate();
void ApplicationDraw();
void ApplicationTick();

#endif