#pragma once
#ifndef _TIMER_H_
#define _TIMER_H_

#include "Dependency.h"

struct Timer {
    float delta_time;
    Uint32 ticks_count;
};
typedef struct Timer Timer;
/**
 * restrict the lowest frame-update-time.
 * notice that max_fps should never exceed 1000.
 *
 * \param timer the timer used to handle ticks
 * \param max_fps the maximun of fps
*/
void Tick(Timer* timer, Uint32 max_fps);

#endif