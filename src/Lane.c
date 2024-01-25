#include "Lane.h"

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
}

void InitLane(Lane* lane, int x, int y, SDL_Scancode key, size_t note_size) {
    InitHitPoint(&lane->hit_point, x, y, key);
    InitNoteList(&lane->note_list, note_size);
    InitEventList(&lane->event_list);
    if (app.is_error) fprintf(stderr, "[Lane]Failed to init lane\n");
}

void FreeLane(Lane* lane) {
    FreeHitPoint(&lane->hit_point);
    FreeNoteList(&lane->note_list);
    FreeEventList(&lane->event_list);
}

void LaneAddNote(Lane* lane,
    NoteType type,
    int start_x, int start_y,
    Uint32 update_time, Uint32 reach_time
) {
    NoteListEmplaceBack(&lane->note_list, type, start_x, start_y, update_time, reach_time);
}

static int isBeyondHit(const HitPoint* hit_point, const Note* note) {
    return
        SDL_abs(hit_point->cur_x - note->cur_x) + SDL_abs(hit_point->cur_y - note->cur_y)
        >=
        (NOTE_RADIUS << 2);
}

static int isPerfectHit(const HitPoint* hit_point, const Note* note) {
    return
        SDL_abs(hit_point->cur_x - note->cur_x) + SDL_abs(hit_point->cur_y - note->cur_y)
        <=
        (NOTE_RADIUS);
}

static int isGoodHit(const HitPoint* hit_point, const Note* note) {
    return
        SDL_abs(hit_point->cur_x - note->cur_x) + SDL_abs(hit_point->cur_y - note->cur_y)
        <=
        (NOTE_RADIUS << 1);
}

static void LaneHandleKeyDown(Lane* lane, SDL_Scancode key) {
    if (key != lane->hit_point.key) return;
    lane->hit_point.is_down = 1;
    lane->hit_point.down_effect.is_active = 1;
    NoteListFor(&lane->note_list) {
        if (isBeyondHit(&lane->hit_point, ptr)) break;
        if (ptr->is_missed) continue;
        if (isPerfectHit(&lane->hit_point, ptr)) {
            lane->hit_point.hit_effect.is_active = 1;
            update_data.hit_status = 0;
            ptr->is_hit = 1;
            (*update_data.combo)++;
            switch (ptr->type) {
            case SINGLE: {
                *update_data.score += PERFECT_HIT_SCORE;
                *update_data.score += *update_data.combo * COMBO_EXTRA_SCORE >= COMBO_MAX_SCORE ?
                    COMBO_MAX_SCORE : *update_data.combo * COMBO_EXTRA_SCORE;
                break;
            }
            case LONG: {
                *update_data.score += LONG_HIT_SCORE;
                ptr->linked_notes[0]->is_hit = 1;
                break;
            }
            default:
                break;
            }
        }
        else if (isGoodHit(&lane->hit_point, ptr)) {
            lane->hit_point.hit_effect.is_active = 1;
            update_data.hit_status = 1;
            ptr->is_hit = 1;
            (*update_data.combo)++;
            switch (ptr->type) {
            case SINGLE: {
                *update_data.score += GOOD_HIT_SCORE;
                *update_data.score += *update_data.combo * COMBO_EXTRA_SCORE >= COMBO_MAX_SCORE ?
                    COMBO_MAX_SCORE : *update_data.combo * COMBO_EXTRA_SCORE;
                break;
            }
            case LONG: {
                *update_data.score += LONG_HIT_SCORE;
                ptr->linked_notes[0]->is_hit = 1;
                break;
            }
            default:
                break;
            }
        }
        else if (!ptr->is_hit) {
            lane->hit_point.hit_effect.is_active = 0;
            update_data.hit_status = 2;
            ptr->is_missed = 1;
            *update_data.combo = 0;
        }
    }
}

static void LaneHandleKeyUp(Lane* lane, SDL_Scancode key) {
    if (key != lane->hit_point.key) return;
    lane->hit_point.is_down = 0;
    lane->hit_point.down_effect.is_active = 0;
    /* TODO: better check */
    NoteListFor(&lane->note_list) {
        if (ptr->type != LONG || ptr->is_missed || !ptr->is_hit) break;
        ptr->is_missed = 1;
        ptr->is_hit = 0;
        ptr->linked_notes[0]->is_missed = 1;
        ptr->linked_notes[0]->is_hit = 0;
        update_data.hit_status = 2;
        *update_data.combo = 0;
    }
}

void LaneHandleKey(Lane* lane, SDL_Event* event) {
    if (event->type == SDL_KEYDOWN) {
        LaneHandleKeyDown(lane, event->key.keysym.scancode);
    }
    else if (event->type == SDL_KEYUP) {
        LaneHandleKeyUp(lane, event->key.keysym.scancode);
    }
}

static void LaneHandleEventMove(HitPoint* hit_point, const Event* event) {
    hit_point->speed.x = *((int*)event->data);
    hit_point->speed.y = *((int*)event->data + 1);
    hit_point->speed.move_enable = 1;
}

static void LaneHandleEventMoveTo(HitPoint* hit_point, const Event* event) {
    hit_point->dest.update_time = event->time;
    hit_point->dest.reach_time = event->time + event->lasting_time;
    hit_point->dest.update_x = hit_point->cur_x;
    hit_point->dest.update_y = hit_point->cur_y;
    hit_point->dest.reach_x = *((int*)event->data);
    hit_point->dest.reach_y = *((int*)event->data + 1);
    hit_point->dest.move_enable = 1;
}

static void LaneHandleEventStop(HitPoint* hit_point) {
    hit_point->speed.x = 0;
    hit_point->speed.y = 0;
    hit_point->dest.reach_x = hit_point->cur_x;
    hit_point->dest.reach_y = hit_point->cur_y;
    hit_point->dest.update_x = hit_point->cur_x;
    hit_point->dest.update_y = hit_point->cur_y;
    hit_point->speed.move_enable = 0;
    hit_point->dest.move_enable = 0;
}

static void LaneHandleEvent(Lane* lane) {
    /* TODO: event tackle */
    EventListFor(&lane->event_list) {
        switch (ptr->type) {
        case MOVE: {
            if (lane->hit_point.speed.move_enable == 0) {
                LaneHandleEventMove(&lane->hit_point, ptr);
            }
            break;
        }
        case MOVETO: {
            if (lane->hit_point.dest.move_enable == 0) {
                LaneHandleEventMoveTo(&lane->hit_point, ptr);
            }
            break;
        }
        case STOP: {
            LaneHandleEventStop(&lane->hit_point);
            break;
        }
        default:
            break;
        }
    }
    EventListUpdate(&lane->event_list);
}

void LaneUpdate(Lane* lane) {
    LaneHandleEvent(lane);
    NoteListUpdate(&lane->note_list, lane->hit_point.cur_x, lane->hit_point.cur_y);
    HitPointUpdate(&lane->hit_point);
}

void LaneDraw(Lane* lane) {
    NoteListDraw(&lane->note_list);
    HitPointDraw(&lane->hit_point);
    EventListDraw(&lane->event_list);
}