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
        (HIT_POINT_RADIUS << 1);
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
        ((NOTE_RADIUS + HIT_POINT_RADIUS));
}

static void LaneSingleNoteJudge(Lane* lane, Note* note) {
    switch (note->status) {
    case 1: {
        *update_data.score += PERFECT_HIT_SCORE;
        (*update_data.combo)++;
        lane->hit_point.hit_effect.is_active = 1;
        update_data.hit_status = 0;
        *update_data.score += *update_data.combo * COMBO_EXTRA_SCORE >= COMBO_MAX_SCORE ?
            COMBO_MAX_SCORE : *update_data.combo * COMBO_EXTRA_SCORE;
        break;
    }
    case 2: {
        *update_data.score += GOOD_HIT_SCORE;
        (*update_data.combo)++;
        lane->hit_point.hit_effect.is_active = 1;
        update_data.hit_status = 1;
        *update_data.score += *update_data.combo * COMBO_EXTRA_SCORE >= COMBO_MAX_SCORE ?
            COMBO_MAX_SCORE : *update_data.combo * COMBO_EXTRA_SCORE;
        break;
    }
    case 3: {
        *update_data.combo = 0;
        lane->hit_point.hit_effect.is_active = 0;
        update_data.hit_status = 2;
        break;
    }
    default: break;
    }
}

static void LaneLongNoteJudge(Lane* lane, Note* note) {
    switch (note->status) {
    case 1: {
        lane->hit_point.hit_effect.is_active = 1;
        lane->hit_point.hit_effect.repeat_enable = 1;
        update_data.hit_status = 0;
        (*update_data.combo)++;
        *update_data.score += LONG_HIT_SCORE;
        note->linked_notes[0]->status = 1;
        break;
    }
    case 2: {
        lane->hit_point.hit_effect.is_active = 1;
        lane->hit_point.hit_effect.repeat_enable = 1;
        update_data.hit_status = 1;
        (*update_data.combo)++;
        *update_data.score += LONG_HIT_SCORE;
        note->linked_notes[0]->status = 1;
        break;
    }
    case 3: {
        lane->hit_point.hit_effect.is_active = 0;
        lane->hit_point.hit_effect.repeat_enable = 0;
        update_data.hit_status = 2;
        *update_data.combo = 0;
        note->linked_notes[0]->status = 3;
        break;
    }
    default: break;
    }
}

static void LaneMultiNoteJudge(Lane* lane, Note* note) {
    switch (note->status) {
    case 1: {
        if ((note->linked_notes[0] && note->linked_notes[0]->status == 3) ||
            (note->linked_notes[1] && note->linked_notes[1]->status == 3)) break;;
        *update_data.score += PERFECT_HIT_SCORE;
        (*update_data.combo)++;
        lane->hit_point.hit_effect.is_active = 1;
        lane->hit_point.hit_effect.repeat_enable = 1;
        update_data.hit_status = 0;
        *update_data.score += *update_data.combo * COMBO_EXTRA_SCORE >= COMBO_MAX_SCORE ?
            COMBO_MAX_SCORE : *update_data.combo * COMBO_EXTRA_SCORE;
        break;
    }
    case 2: {
        if ((note->linked_notes[0] && note->linked_notes[0]->status == 3) ||
            (note->linked_notes[1] && note->linked_notes[1]->status == 3)) break;;
        *update_data.score += PERFECT_HIT_SCORE;
        (*update_data.combo)++;
        lane->hit_point.hit_effect.is_active = 1;
        lane->hit_point.hit_effect.repeat_enable = 1;
        update_data.hit_status = 1;
        *update_data.score += *update_data.combo * COMBO_EXTRA_SCORE >= COMBO_MAX_SCORE ?
            COMBO_MAX_SCORE : *update_data.combo * COMBO_EXTRA_SCORE;
        break;
    }
    case 3: {
        *update_data.combo = 0;
        lane->hit_point.hit_effect.is_active = 0;
        lane->hit_point.hit_effect.repeat_enable = 0;
        update_data.hit_status = 2;
        break;
    }
    default: break;
    }
}

static void LaneNotesJudge(Lane* lane, Note* note) {
    switch (note->type) {
    case SINGLE: {
        LaneSingleNoteJudge(lane, note);
        break;
    }
    case LONG: {
        LaneLongNoteJudge(lane, note);
        break;
    }
    case MULTI: {
        LaneMultiNoteJudge(lane, note);
        break;
    }
    default: break;
    }
}

static void LaneHandleKeyDown(Lane* lane, SDL_Scancode key) {
    if (key != lane->hit_point.key) return;
    lane->hit_point.is_down = 1;
    lane->hit_point.down_effect.is_active = 1;
    Node* ptr = NULL;
    ListForEach(ptr, &lane->note_list) {
        Note* note = ptr->value;
        if (isBeyondHit(&lane->hit_point, note)) break;
        if (note->status) continue;
        if (isPerfectHit(&lane->hit_point, note)) {
            note->status = 1;
        }
        else if (isGoodHit(&lane->hit_point, note)) {
            note->status = 2;
        }
        else if (!note->status) {
            note->status = 3;
        }
        LaneNotesJudge(lane, note);
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
        if (note->status == 0 || note->status == 3) break;
        if (note->type != LONG && note->type != MULTI) break;
        note->status = 3;
        if (note->linked_notes[0]) note->linked_notes[0]->status = 3;
        if (note->linked_notes[1]) note->linked_notes[1]->status = 3;
        LaneNotesJudge(lane, note);
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

static void LaneExecMoveEvent(HitPoint* hit_point, const Event* event) {
    hit_point->speed.x = *((int*)event->data);
    hit_point->speed.y = *((int*)event->data + 1);
    hit_point->speed.move_enable = 1;
}

static void LaneExecMoveToEvent(HitPoint* hit_point, const Event* event) {
    hit_point->dest.update_time = event->time;
    hit_point->dest.reach_time = event->time + event->lasting_time;
    /* This will generate momentum*/
    hit_point->dest.update_x = hit_point->cur_x;
    hit_point->dest.update_y = hit_point->cur_y;
    hit_point->dest.reach_x = *((int*)event->data);
    hit_point->dest.reach_y = *((int*)event->data + 1);
    hit_point->dest.move_enable = 1;
}

static void LaneExecStopEvent(HitPoint* hit_point) {
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
                LaneExecMoveEvent(&lane->hit_point, event);
                ListErase(&lane->event_list, ptr);
                break;
            }
            case MOVETO: {
                LaneExecMoveToEvent(&lane->hit_point, event);
                break;
            }
            case STOP: {
                LaneExecStopEvent(&lane->hit_point);
                ListErase(&lane->event_list, ptr);
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
            if (!note->status) {
                note->status = 3;
                *update_data.combo = 0;
                update_data.hit_status = 2;
            }
            switch (note->type) {
            case SINGLE: {
                ListErase(&lane->note_list, ptr);
                break;
            }
            case LONG: {
                if (note->linked_notes[0] && note->linked_notes[0]->reach_time + 100 >= app.timer.relative_time) break;
                NoteUnlink(note, note->linked_notes[0]);
                ListErase(&lane->note_list, ptr);
                lane->hit_point.hit_effect.repeat_enable = 0;
                break;
            }
            case MULTI: {
                if ((note->linked_notes[0] && note->linked_notes[0]->reach_time + 100 >= app.timer.relative_time) ||
                    (note->linked_notes[1] && note->linked_notes[1]->reach_time + 100 >= app.timer.relative_time))
                    break;
                if (note->linked_notes[0]) NoteUnlink(note, note->linked_notes[0]);
                if (note->linked_notes[1]) NoteUnlink(note, note->linked_notes[1]);
                ListErase(&lane->note_list, ptr);
                lane->hit_point.hit_effect.repeat_enable = 0;
                break;
            }
            default: break;
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