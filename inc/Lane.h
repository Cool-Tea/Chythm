#ifndef _LANE_H_
#define _LANE_H_

#include "List.h"
#include "Note.h"
#include "Event.h"
#include "HitPoint.h"

struct Lane {
    HitPoint hit_point;

    List note_list;

    List event_list;
};
typedef struct Lane Lane;

void InitLane(Lane* lane, int x, int y, SDL_Scancode key);
void FreeLane(Lane* lane);
Node* LaneAddNote(Lane* lane,
    NoteType type,
    int start_x, int start_y,
    Uint32 update_time, Uint32 reach_time
);
Node* LaneAddEvent(Lane* lane,
    Uint32 time, Uint32 lasting_time, EventType type, ...
);
void LaneHandleKey(Lane* lane, SDL_Event* event);
void LaneUpdate(Lane* lane);
void LaneDraw(Lane* lane);

#endif