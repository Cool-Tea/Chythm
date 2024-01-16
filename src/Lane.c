#include "../inc/Lane.h"

void InitHitPoint(HitPoint* hit_point, int x, int y, SDL_Scancode key) {
    hit_point->is_down = 0;
    hit_point->cur_x = x, hit_point->cur_y = y;

    hit_point->key = key;
    hit_point->speed.x = hit_point->speed.y = 0;
    hit_point->speed.move_enable = 0;

    hit_point->dest.update_x = x, hit_point->dest.update_y = y;
    hit_point->dest.update_time = hit_point->dest.reach_time = 0;
    hit_point->dest.move_enable = 0;
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
}

void HitPointDraw(HitPoint* hit_point) {
    DrawHitPoint(hit_point->cur_x, hit_point->cur_y, hit_point->is_down);
}

void InitLane(Lane* lane) {
    InitNoteList(&lane->note_list);
    InitEventList(&lane->event_list);
    if (app.is_error) fprintf(stderr, "[Lane]Failed to init lane\n");
}

void FreeLane(Lane* lane) {
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

static void LaneHandleKey(Lane* lane) {
    if (app.key_status[lane->hit_point.key]) {
        lane->hit_point.is_down = 1;
        NoteListFor(&lane->note_list) {
            if (isBeyondHit(&lane->hit_point, ptr)) break;
            else if (!ptr->is_missed && isPerfectHit(&lane->hit_point, ptr)) {
                /* TODO: hit effect */
                update_data.hit_status = 0;
                *update_data.score += PERFECT_HIT_SCORE;
                *update_data.score += *update_data.combo * COMBO_EXTRA_SCORE >= COMBO_MAX_SCORE ?
                    COMBO_MAX_SCORE : *update_data.combo * COMBO_EXTRA_SCORE;
                (*update_data.combo)++;
            }
            else if (!ptr->is_missed && isGoodHit(&lane->hit_point, ptr)) {
                update_data.hit_status = 1;
                *update_data.score += GOOD_HIT_SCORE;
                *update_data.score += *update_data.combo * COMBO_EXTRA_SCORE >= COMBO_MAX_SCORE ?
                    COMBO_MAX_SCORE : *update_data.combo * COMBO_EXTRA_SCORE;
                (*update_data.combo)++;
            }
            else {
                update_data.hit_status = 2;
                ptr->is_missed = 1;
                *update_data.combo = 0;
            }
        }
    }
    else {
        lane->hit_point.is_down = 0;
        /* TODO: when hit point is up */
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

void LaneUpdate(Lane* lane, SDL_Event* event) {
    if (event->type == SDL_KEYDOWN || event->type == SDL_KEYUP)
        LaneHandleKey(lane);
    LaneHandleEvent(lane);
    NoteListUpdate(&lane->note_list, lane->hit_point.cur_x, lane->hit_point.cur_y);
    HitPointUpdate(&lane->hit_point);
}

void LaneDraw(Lane* lane) {
    NoteListDraw(&lane->note_list);
    HitPointDraw(&lane->hit_point);
    EventListDraw(&lane->event_list);
}