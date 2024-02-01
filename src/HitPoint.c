#include "HitPoint.h"

void InitHitPoint(HitPoint* hit_point, int x, int y, SDL_Scancode key) {
    hit_point->is_down = 0;
    hit_point->cur_x = x, hit_point->cur_y = y;

    hit_point->key = key;
    hit_point->speed.x = hit_point->speed.y = 0;
    hit_point->speed.move_enable = 0;

    hit_point->dest.update_x = x, hit_point->dest.update_y = y;
    hit_point->dest.update_time = hit_point->dest.reach_time = 0;
    hit_point->dest.move_enable = 0;

    InitEffect(&hit_point->down_effect, CIRCLE, 1);
    InitEffect(&hit_point->hit_effect, MAGIC, 0);
}

void FreeHitPoint(HitPoint* hit_point) {
    FreeEffect(&hit_point->down_effect);
}

static void HitPointUpdateSpeed(HitPoint* hit_point) {
    hit_point->cur_x += hit_point->speed.x;
    hit_point->cur_y += hit_point->speed.y;
}

static void HitPointUpdateDest(HitPoint* hit_point) {
    hit_point->cur_x =
        (hit_point->dest.reach_x - hit_point->dest.update_x)
        * (signed)(app.timer.relative_time - hit_point->dest.update_time)
        / (signed)(hit_point->dest.reach_time - hit_point->dest.update_time)
        + hit_point->dest.update_x;
    hit_point->cur_y =
        (hit_point->dest.reach_y - hit_point->dest.update_y)
        * (signed)(app.timer.relative_time - hit_point->dest.update_time)
        / (signed)(hit_point->dest.reach_time - hit_point->dest.update_time)
        + hit_point->dest.update_y;
}

void HitPointUpdate(HitPoint* hit_point) {
    if (hit_point->speed.move_enable) {
        HitPointUpdateSpeed(hit_point);
    }
    if (hit_point->dest.move_enable) {
        HitPointUpdateDest(hit_point);
    }
    EffectUpdate(&hit_point->down_effect);
    EffectUpdate(&hit_point->hit_effect);
}

static void HitPointDrawPrompt(HitPoint* hit_point) {
    static SDL_Rect rect = { .h = HIT_POINT_RADIUS, .w = HIT_POINT_RADIUS >> 1 };

    rect.x = hit_point->cur_x - (rect.w >> 1), rect.y = hit_point->cur_y - (rect.h >> 1);
    DrawText(rect, SDL_GetKeyName(SDL_GetKeyFromScancode(hit_point->key)), default_colors[0]);
}

void HitPointDraw(HitPoint* hit_point) {
    static SDL_Rect rect
#if !AUTO_RESOLUTION
        = { .h = HIT_POINT_RADIUS << 1, .w = HIT_POINT_RADIUS << 1 }
#endif
    ;

    EffectDraw(&hit_point->down_effect, hit_point->cur_x, hit_point->cur_y, HIT_POINT_RADIUS + 20, 0.0);

    rect.x = hit_point->cur_x - HIT_POINT_RADIUS, rect.y = hit_point->cur_y - HIT_POINT_RADIUS;

#if AUTO_RESOLUTION
    rect.h = HIT_POINT_RADIUS << 1, rect.w = HIT_POINT_RADIUS << 1;
    rect.x *= app.zoom_rate.w, rect.y *= app.zoom_rate.h;
    rect.w *= app.zoom_rate.w, rect.h *= app.zoom_rate.h;
#endif

    SDL_RenderCopy(app.ren, assets.hit_points[hit_point->is_down], NULL, &rect);

    EffectDraw(&hit_point->hit_effect, hit_point->cur_x, hit_point->cur_y, HIT_POINT_RADIUS + 40, 0.0);

    HitPointDrawPrompt(hit_point);
}
