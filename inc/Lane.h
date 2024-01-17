#ifndef _LANE_H_
#define _LANE_H_

#include "Constants.h"
#include "Note.h"
#include "Event.h"

struct HitPoint {
    int cur_x, cur_y;
    struct {
        int x, y;
        bool move_enable;
    } speed; // this should not collided with dest
    struct {
        int update_x, update_y;
        int reach_x, reach_y;
        Uint32 update_time, reach_time;
        bool move_enable;
    } dest; // this should not collided with speed
    SDL_Scancode key; // the key for this point
    bool is_down;
    Effect down_effect;
    Effect hit_effect;
};
typedef struct HitPoint HitPoint;
void InitHitPoint(HitPoint* hit_point, int x, int y, SDL_Scancode key);
void FreeHitPoint(HitPoint* hit_point);
static void HitPointUpdateSpeed(HitPoint* hit_point);
static void HitPointUpdateDest(HitPoint* hit_point);
void HitPointUpdate(HitPoint* hit_point);
void HitPointDraw(HitPoint* hit_point);

struct Lane {
    HitPoint hit_point;

    NoteList note_list;

    EventList event_list;
};
typedef struct Lane Lane;

void InitLane(Lane* lane);
void FreeLane(Lane* lane);
void LaneAddNote(Lane* lane,
    NoteType type,
    int start_x, int start_y,
    Uint32 update_time, Uint32 reach_time
);

static int isBeyondHit(const HitPoint* hit_point, const Note* note);
static int isPerfectHit(const HitPoint* hit_point, const Note* note);
static int isGoodHit(const HitPoint* hit_point, const Note* note);
static void LaneHandleKey(Lane* lane);

static void LaneHandleEventMove(HitPoint* hit_point, const Event* event);
static void LaneHandleEventMoveTo(HitPoint* hit_point, const Event* event);
static void LaneHandleEventStop(HitPoint* hit_point);
static void LaneHandleEvent(Lane* lane);

void LaneUpdate(Lane* lane, SDL_Event* event);
void LaneDraw(Lane* lane);

#endif