#include "../inc/Lane.h"

void InitLane(Lane* lane) {
    lane->hit_point.isDown = 0;
    InitNoteList(&lane->note_list);
    InitEventList(&lane->event_list);
    if (is_error) printf("[Lane]Failed to init lane\n");
}
void FreeLane(Lane* lane) {
    FreeNoteList(&lane->note_list);
    FreeEventList(&lane->event_list);
}
void LaneUpdate(Lane* lane, Uint32 relative_time, SDL_Event* event) {
    /* Key Handling */
    if (event != NULL) {
        if (event->type == SDL_KEYDOWN &&
            !lane->hit_point.isDown &&
            event->key.keysym.sym == lane->hit_point.key) {
            /* TODO: give feed back */
            for (; lane->note_list.head != lane->note_list.tail; lane->note_list.head++) {
                if (lane->note_list.head->end_time >= relative_time + MISS_HIT_INTERVAL) break;
                if (abs(lane->note_list.head->end_time - relative_time) <= PERFECT_HIT_INTERVAL) {
                    /* TODO: give feed back */
                }
                else if (abs(lane->note_list.head->end_time - relative_time) <= GOOD_HIT_INTERVAL) {
                    /* TODO: */
                }
                else {
                    /* TODO: */
                }
            }
        }
        else if (event->type == SDL_KEYUP) {
            /* TODO: give feed back */
        }
    }
    /* Note Update */
    while (lane->note_list.head->end_time <= relative_time - MISS_HIT_INTERVAL &&
        lane->note_list.head != lane->note_list.tail) {
        lane->note_list.head++;
    }
    while (lane->note_list.tail->start_time <= relative_time + MISS_HIT_INTERVAL &&
        lane->note_list.tail - lane->note_list.notes < lane->note_list.size) {
        lane->note_list.tail++;
    }
    for (Note* ptr = lane->note_list.head;
        ptr != lane->note_list.tail; ptr++) {
        NoteUpdate(ptr, relative_time);
    }
    /* Event Update */
    while (lane->event_list.cur_event - lane->event_list.events < lane->event_list.size &&
        lane->event_list.cur_event->time <= relative_time) {
        /* TODO: perform event */
        lane->event_list.cur_event++;
    }
}
void LaneDraw(Lane* lane, SDL_Renderer* renderer) {
    /* TODO: draw hitpoint */
    for (Note* ptr = lane->note_list.head;
        ptr != lane->note_list.tail; ptr++) {
        NoteDraw(ptr, renderer);
    }
}