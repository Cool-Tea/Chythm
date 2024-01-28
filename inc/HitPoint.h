#ifndef _HIT_POINT_H_
#define _HIT_POINT_H_

#include "Drawer.h"
#include "Effect.h"

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
void HitPointUpdate(HitPoint* hit_point);
void HitPointDraw(HitPoint* hit_point);

#endif