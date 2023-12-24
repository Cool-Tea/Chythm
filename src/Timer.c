#include "../inc/Timer.h"

void Tick(Timer* timer, Uint32 max_fps) {
    Uint32 fps_time = 1000 / max_fps; //ms
    while (SDL_GetTicks() < timer->ticks_count + fps_time) {
        SDL_Delay(1);
    }
    timer->delta_time = (SDL_GetTicks() - timer->ticks_count) / 1000.0f;
    timer->ticks_count = SDL_GetTicks();
}