#ifndef _LANE_H_
#define _LANE_H_

#include "Constants.h"
#include "Note.h"
#include "Event.h"

struct HitPoint {
    int cur_x, cur_y;
    Uint8 isDown;
    SDL_Keycode key;
};
typedef struct HitPoint HitPoint;

struct Lane {
    /* hit point */
    HitPoint hit_point;

    /* note */
    NoteList note_list;

    /* event */
    EventList event_list;
};
typedef struct Lane Lane;
void InitLane(Lane* lane);
void FreeLane(Lane* lane);
void LaneUpdate(Lane* lane, Uint32 relative_time, SDL_Event* event, const char** game_text);
void LaneDraw(Lane* lane, SDL_Renderer* renderer);

#endif