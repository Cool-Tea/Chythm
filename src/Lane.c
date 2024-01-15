#include "../inc/Lane.h"

void HitPointUpdate(HitPoint* hit_point) {
    hit_point->cur_x += hit_point->speed_x;
    hit_point->cur_y += hit_point->speed_y;
}

void HitPointDraw(HitPoint* hit_point) {
    DrawHitPoint(app.ren, hit_point->cur_x, hit_point->cur_y, hit_point_colors[hit_point->is_down]);
}

void InitLane(Lane* lane) {
    lane->hit_point.is_down = 0;
    lane->hit_point.speed_x = lane->hit_point.speed_y = 0;
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
        (NOTE_RADIUS << 1);
}

static int isPerfectHit(const HitPoint* hit_point, const Note* note) {
    return
        SDL_abs(hit_point->cur_x - note->cur_x) + SDL_abs(hit_point->cur_y - note->cur_y)
        <=
        (NOTE_RADIUS >> 1);
}

static int isGoodHit(const HitPoint* hit_point, const Note* note) {
    return
        SDL_abs(hit_point->cur_x - note->cur_x) + SDL_abs(hit_point->cur_y - note->cur_y)
        <=
        NOTE_RADIUS;
}

static void LaneHandleKey(Lane* lane) {
    if (app.key_status[lane->hit_point.key]) {
        lane->hit_point.is_down = 1;
        NoteListFor(&lane->note_list) {
            if (isBeyondHit(&lane->hit_point, ptr)) break;
            else if (isPerfectHit(&lane->hit_point, ptr)) {
                /* TODO: when is perfect hit*/
            }
            else if (isGoodHit(&lane->hit_point, ptr)) {

            }
            else {
                ptr->is_missed = 1;
            }
        }
    }
    else {
        lane->hit_point.is_down = 0;
        /* TODO: when hit point is up */
    }
}

static void LaneHandleEvent(Lane* lane) {
    /* TODO: event tackle */
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