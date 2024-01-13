#include "../inc/Lane.h"

void InitLane(Lane* lane) {
    lane->hit_point.isDown = 0;
    lane->hit_point.velo_x = lane->hit_point.velo_y = 0;
    InitNoteList(&lane->note_list);
    InitEventList(&lane->event_list);
    if (is_error) printf("[Lane]Failed to init lane\n");
}
void FreeLane(Lane* lane) {
    FreeNoteList(&lane->note_list);
    FreeEventList(&lane->event_list);
}
void LaneUpdate(Lane* lane, Uint32 relative_time, SDL_Event* event, const char** game_text) {
    /* Key Handling */
    if (event->type == SDL_KEYDOWN && !lane->hit_point.isDown &&
        event->key.keysym.sym == lane->hit_point.key) {
        lane->hit_point.isDown = 1;
        for (; lane->note_list.head != lane->note_list.tail; lane->note_list.head++) {
            if (lane->note_list.head->end_time >= relative_time + MISS_HIT_INTERVAL) break;
            if (lane->note_list.head->end_time - relative_time <= PERFECT_HIT_INTERVAL ||
                relative_time - lane->note_list.head->end_time <= PERFECT_HIT_INTERVAL) {
                /* TODO: give feed back */
                lane->note_list.head->isDown = 1;
                if (lane->note_list.head->type == LONG_HEAD) {
                    lane->note_list.head->start_x = lane->note_list.head->end_x;
                    lane->note_list.head->start_y = lane->note_list.head->end_y;
                    break;
                }
                *game_text = game_scene_texts[0];
                score += PERFECT_HIT_SCORE;
                score += combo * COMBO_EXTRA_SCORE >= COMBO_MAX_SCORE ? COMBO_MAX_SCORE : combo * COMBO_EXTRA_SCORE;
                combo++;
            }
            else if (lane->note_list.head->end_time - relative_time <= GOOD_HIT_INTERVAL ||
                relative_time - lane->note_list.head->end_time <= GOOD_HIT_INTERVAL) {
                /* TODO: give feed back */
                lane->note_list.head->isDown = 1;
                if (lane->note_list.head->type == LONG_HEAD) {
                    lane->note_list.head->start_x = lane->note_list.head->end_x;
                    lane->note_list.head->start_y = lane->note_list.head->end_y;
                    break;
                }
                *game_text = game_scene_texts[1];
                score += GOOD_HIT_SCORE;
                score += combo * COMBO_EXTRA_SCORE >= COMBO_MAX_SCORE ? COMBO_MAX_SCORE : combo * COMBO_EXTRA_SCORE;
                combo++;
            }
            else {
                /* TODO: give feed back */
                *game_text = game_scene_texts[2];
                combo = 0;
            }
        }
    }
    else if (event->type == SDL_KEYUP && lane->hit_point.isDown &&
        event->key.keysym.sym == lane->hit_point.key) {
        /* TODO: give feed back */
        lane->hit_point.isDown = 0;
        for (; lane->note_list.head != lane->note_list.tail; lane->note_list.head++) {
            if (lane->note_list.head->type != LONG_HEAD) break;
            Note* tail = lane->note_list.head + 1;
            if (tail->end_time >= relative_time + MISS_HIT_INTERVAL) {
                *game_text = game_scene_texts[2];
                combo = 0;
                continue;
            }
            if ((tail->end_time - relative_time <= PERFECT_HIT_INTERVAL ||
                relative_time - tail->end_time <= PERFECT_HIT_INTERVAL) &&
                lane->note_list.head->isDown) {
                (lane->note_list.head + 1)->isDown = 1;
                *game_text = game_scene_texts[0];
                score += PERFECT_HIT_SCORE;
                score += combo * COMBO_EXTRA_SCORE >= COMBO_MAX_SCORE ? COMBO_MAX_SCORE : combo * COMBO_EXTRA_SCORE;
                combo++;
            }
            else if ((tail->end_time - relative_time <= GOOD_HIT_INTERVAL ||
                relative_time - tail->end_time <= GOOD_HIT_INTERVAL) &&
                lane->note_list.head->isDown) {
                (lane->note_list.head + 1)->isDown = 1;
                *game_text = game_scene_texts[1];
                score += GOOD_HIT_SCORE;
                score += combo * COMBO_EXTRA_SCORE >= COMBO_MAX_SCORE ? COMBO_MAX_SCORE : combo * COMBO_EXTRA_SCORE;
                combo++;
            }
            else {
                *game_text = game_scene_texts[2];
                combo = 0;
            }
        }
    }
    /* Note Update */
    while (lane->note_list.head->end_time + MISS_HIT_INTERVAL <= relative_time &&
        lane->note_list.head < lane->note_list.tail) {
        if (lane->note_list.head->type == LONG_HEAD && lane->note_list.head->isDown &&
            (lane->note_list.head + 1)->end_time > relative_time) break;
        if (lane->note_list.head->isDown == 0) combo = 0;
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
        switch (lane->event_list.cur_event->type) {
        case MOVE: {
            lane->hit_point.velo_x = *(int*)(lane->event_list.cur_event->data);
            lane->hit_point.velo_y = *((int*)(lane->event_list.cur_event->data) + 1);
            break;
        }
        case STOP: {
            lane->hit_point.velo_x = lane->hit_point.velo_y = 0;
            break;
        }
        default:
            break;
        }
        lane->event_list.cur_event++;
    }
    /* Hit Point Update */
    lane->hit_point.cur_x += lane->hit_point.velo_x;
    lane->hit_point.cur_y += lane->hit_point.velo_y;
}
void LaneDraw(Lane* lane, SDL_Renderer* renderer) {
    DrawHitPoint(renderer, lane->hit_point.cur_x, lane->hit_point.cur_y, hit_point_colors[lane->hit_point.isDown]);
    for (Note* ptr = lane->note_list.head;
        ptr < lane->note_list.tail; ptr++) {
        switch (ptr->type) {
        case SINGLE: {
            NoteDraw(ptr, renderer, NULL);
            break;
        }
        case LONG_HEAD: {
            Note* head = ptr++;
            NoteDraw(head, renderer, ptr);
        }
        default:
            break;
        }
    }
}