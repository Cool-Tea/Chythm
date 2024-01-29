#include "Lane.h"

void InitLane(Lane* lane, int x, int y, SDL_Scancode key) {
    InitHitPoint(&lane->hit_point, x, y, key);
    InitList(&lane->note_list);
    ListSetFreeMethod(&lane->note_list, (FreeFunc)DestroyNote);
    InitList(&lane->event_list);
    ListSetFreeMethod(&lane->event_list, (FreeFunc)DestroyEvent);
    if (app.is_error) fprintf(stderr, "[Lane]Failed to init lane\n");
}

void FreeLane(Lane* lane) {
    FreeHitPoint(&lane->hit_point);
    FreeList(&lane->note_list);
    FreeList(&lane->event_list);
}

Node* LaneAddNote(Lane* lane,
    NoteType type,
    int start_x, int start_y,
    Uint32 update_time, Uint32 reach_time
) {
    Note* note = CreateNote(type, start_x, start_y, update_time, reach_time);
    if (note == NULL) return NULL;
    return ListEmplaceTail(&lane->note_list, note);
}

Node* LaneAddEvent(Lane* lane,
    Uint32 time, Uint32 lasting_time, EventType type, ...
) {
    va_list args;
    va_start(args, type);
    Event* event = CreateEventVA(time, lasting_time, type, &args);
    va_end(args);
    if (event == NULL) return NULL;
    return ListEmplaceTail(&lane->event_list, event);
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
    Node* ptr = NULL;
    ListForEach(ptr, &lane->note_list) {
        Note* note = ptr->value;
        if (isBeyondHit(&lane->hit_point, note)) break;
        if (note->is_missed) continue;
        if (isPerfectHit(&lane->hit_point, note)) {
            lane->hit_point.hit_effect.is_active = 1;
            update_data.hit_status = 0;
            note->is_hit = 1;
            (*update_data.combo)++;
            switch (note->type) {
            case SINGLE: {
                *update_data.score += PERFECT_HIT_SCORE;
                *update_data.score += *update_data.combo * COMBO_EXTRA_SCORE >= COMBO_MAX_SCORE ?
                    COMBO_MAX_SCORE : *update_data.combo * COMBO_EXTRA_SCORE;
                break;
            }
            case LONG: {
                *update_data.score += LONG_HIT_SCORE;
                note->linked_notes[0]->is_hit = 1;
                break;
            }
            default:
                break;
            }
        }
        else if (isGoodHit(&lane->hit_point, note)) {
            lane->hit_point.hit_effect.is_active = 1;
            update_data.hit_status = 1;
            note->is_hit = 1;
            (*update_data.combo)++;
            switch (note->type) {
            case SINGLE: {
                *update_data.score += GOOD_HIT_SCORE;
                *update_data.score += *update_data.combo * COMBO_EXTRA_SCORE >= COMBO_MAX_SCORE ?
                    COMBO_MAX_SCORE : *update_data.combo * COMBO_EXTRA_SCORE;
                break;
            }
            case LONG: {
                *update_data.score += LONG_HIT_SCORE;
                note->linked_notes[0]->is_hit = 1;
                break;
            }
            default:
                break;
            }
        }
        else if (!note->is_hit) {
            lane->hit_point.hit_effect.is_active = 0;
            update_data.hit_status = 2;
            note->is_missed = 1;
            *update_data.combo = 0;
        }
    }
}

static void LaneHandleKeyUp(Lane* lane, SDL_Scancode key) {
    if (key != lane->hit_point.key) return;
    lane->hit_point.is_down = 0;
    lane->hit_point.down_effect.is_active = 0;
    /* TODO: better check */
    Node* ptr = NULL;
    ListForEach(ptr, &lane->note_list) {
        Note* note = ptr->value;
        if (note->type != LONG || note->is_missed || !note->is_hit) break;
        note->is_missed = 1;
        note->is_hit = 0;
        note->linked_notes[0]->is_missed = 1;
        note->linked_notes[0]->is_hit = 0;
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
    if (!hit_point->speed.move_enable) return;
    hit_point->speed.x = *((int*)event->data);
    hit_point->speed.y = *((int*)event->data + 1);
    hit_point->speed.move_enable = 1;
}

static void LaneHandleEventMoveTo(HitPoint* hit_point, const Event* event) {
    if (!hit_point->dest.move_enable) return;
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

static void LaneUpdateEvents(Lane* lane) {
    for (Node* ptr = lane->event_list.head, *next = NULL; ptr != NULL; ptr = next) {
        next = ptr->next;
        Event* event = ptr->value;
        if (app.timer.relative_time < event->time) break;
        else if (app.timer.relative_time < event->time + event->lasting_time) {
            switch (event->type) {
            case MOVE: {
                LaneHandleEventMove(&lane->hit_point, event);
                break;
            }
            case MOVETO: {
                LaneHandleEventMoveTo(&lane->hit_point, event);
                break;
            }
            case STOP: {
                LaneHandleEventStop(&lane->hit_point);
                break;
            }
            default:
                break;
            }
            EventUpdate(event);
        }
        else {
            ListErase(&lane->event_list, ptr);
        }
    }
}

static void LaneUpdateNotes(Lane* lane) {
    if (isListEmpty(&lane->note_list)) return;
    for (Node* ptr = lane->note_list.head, *next = NULL; ptr != NULL; ptr = next) {
        next = ptr->next;
        Note* note = ptr->value;
        // check Long notes and set inactive notes between reach time and reach time plus 100
        if (app.timer.relative_time < note->reach_time) break;
        else if (app.timer.relative_time < note->reach_time + 100) note->update_enable = 0;
        else {
            // check whether the note should be destroyed
            switch (note->type) {
            case SINGLE: {
                if (!note->is_hit) {
                    *update_data.combo = 0;
                    update_data.hit_status = 2;
                }
                ListErase(&lane->note_list, ptr);
                break;
            }
            case LONG: {
                if (!note->is_hit) {
                    *update_data.combo = 0;
                    update_data.hit_status = 2;
                }
                if (note->linked_notes[0]->reach_time + 100 < app.timer.relative_time) {
                    note->linked_notes[0]->linked_notes[0] = note->linked_notes[0];
                    ListErase(&lane->note_list, ptr);
                }
                break;
            }
            default:
                break;
            }
        }
    }
    Node* ptr = NULL;
    ListForEach(ptr, &lane->note_list) {
        Note* note = ptr->value;
        NoteUpdate(note, lane->hit_point.cur_x, lane->hit_point.cur_y);
    }
}

void LaneUpdate(Lane* lane) {
    LaneUpdateEvents(lane);
    LaneUpdateNotes(lane);
    HitPointUpdate(&lane->hit_point);
}

static void LaneDrawNotes(Lane* lane) {
    Node* ptr = NULL;
    ListForEach(ptr, &lane->note_list) {
        NoteDraw(ptr->value);
    }
}

static void LaneDrawEvents(Lane* lane) {
    Node* ptr = NULL;
    ListForEach(ptr, &lane->event_list) {
        EventDraw(ptr->value);
    }
}

void LaneDraw(Lane* lane) {
    LaneDrawNotes(lane);
    HitPointDraw(&lane->hit_point);
    LaneDrawEvents(lane);
}