#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include "Constants.h"
#include <SDL_ttf.h>

typedef unsigned char bool;

struct Timer {
    Uint32 real_time; // the milisceonds since Game init
    Uint32 delta_time; // the update time between each frame

    Uint32 base_time; // the milisceconds since Game start
    Uint32 relative_time; // the milisceconds relative to base time
};
typedef struct Timer Timer;

struct Application {
    /* SDL related */
    struct ZoomRate {
        double w, h;
    } zoom_rate; // automatically change resolution
    SDL_Window* win;
    SDL_Renderer* ren;
    TTF_Font* font;

    /* timer */
    Timer timer;

    /* app status */
    bool is_running;
    bool is_error;
    bool is_loaded;
    SceneType cur_scene;

    /* key status */
    // const Uint8* key_status; // SDL built in keyboard status
    //bool key_status[SDL_NUM_SCANCODES];

    /* thread */
    SDL_mutex* mutex;
};
typedef struct Application Application;

extern Application app;

#endif