#include "../inc/Note.h"

void NoteLink(Note* lnote, Note* rnote) {
    lnote->linked_notes[lnote->linked_notes[0] != NULL] = rnote;
    rnote->linked_notes[rnote->linked_notes[0] != NULL] = lnote;
}

void NoteUpdate(Note* note, int target_x, int target_y) {
    if (note->update_enable == 0) return;
    note->cur_x =
        (target_x - note->update_x)
        * (signed)(app.timer.relative_time - note->update_time)
        / (signed)(note->reach_time - note->update_time)
        + note->update_x;
    note->cur_y = (target_y - note->update_y)
        * (signed)(app.timer.relative_time - note->update_time)
        / (signed)(note->reach_time - note->update_time)
        + note->update_y;
    EffectUpdate(&note->effect);
}

#if !NOTE_ONLY_EFFECT
static void TypeNoteDraw(Note* note) {
    static SDL_Rect rect = { .h = NOTE_RADIUS << 1, .w = NOTE_RADIUS << 1 };
    rect.x = note->cur_x - NOTE_RADIUS, rect.y = note->cur_y - NOTE_RADIUS;
    double angle =
        SDL_atan2(note->cur_x - note->update_x, note->cur_y - note->update_y)
        * 180.0 / PI;
    if (note->cur_x > note->update_x) angle = -angle;
    SDL_RenderCopyEx(app.ren, assets.notes[note->type], NULL, &rect, angle, NULL, SDL_FLIP_NONE);
}
#endif

void NoteDraw(Note* note) {
    double angle =
        SDL_atan2(note->cur_x - note->update_x, note->cur_y - note->update_y)
        * 180.0 / PI;
    if (note->cur_x > note->update_x) angle = -angle;
    EffectDraw(&note->effect, note->cur_x, note->cur_y, NOTE_RADIUS << 1, angle);

#if !NOTE_ONLY_EFFECT
    TypeNoteDraw(note);
#endif

    /* TODO: draw different note */
    switch (note->type) {
    case LONG: {
        lineRGBA(
            app.ren,
            note->cur_x, note->cur_y,
            note->linked_notes[0]->cur_x, note->linked_notes[0]->cur_y,
            default_colors[0].r, default_colors[0].g, default_colors[0].b, default_colors[0].a
        );
        break;
    }
    default:
        break;
    }
}

void InitNoteList(NoteList* note_list) {
    note_list->size = 0;
    note_list->capacity = NOTE_LIST_INIT_CAPACITY;
    note_list->notes = malloc(note_list->capacity * sizeof(Note));
    if (note_list->notes == NULL) {
        fprintf(stderr, "[NoteList]Failed to malloc note list\n");
        app.is_error = 1;
    }
    note_list->head = note_list->tail = note_list->notes;
}

void FreeNoteList(NoteList* note_list) {
    for (size_t i = 0; i < note_list->size; i++) {
        FreeEffect(&note_list->notes[i].effect);
    }
    free(note_list->notes);
}

void NoteListEmplaceBack(NoteList* note_list,
    NoteType type,
    int start_x, int start_y,
    Uint32 update_time, Uint32 reach_time
) {
    note_list->notes[note_list->size] = (Note){
        .type = type,
        .linked_notes = { NULL, NULL },
        .cur_x = start_x,
        .cur_y = start_y,
        .update_enable = 1,
        .update_x = start_x,
        .update_y = start_y,
        .update_time = update_time,
        .reach_time = reach_time,
        .is_missed = 0,
        .is_hit = 0
    };
    switch (type) {
    case SINGLE: {
        InitEffect(&note_list->notes[note_list->size].effect, MUZZLE, 1);
        break;
    }
    case LONG: {
        InitEffect(&note_list->notes[note_list->size].effect, STAR, 1);
        break;
    }
    default:
        break;
    }
    note_list->size++;
    if (note_list->size >= note_list->capacity) {
        note_list->capacity <<= 1;
        note_list->notes = realloc(note_list->notes, note_list->capacity * sizeof(Note));
    }
}

void NoteListUpdate(NoteList* note_list, int target_x, int target_y) {
    NoteListFor(note_list) {
        if (ptr->reach_time > app.timer.relative_time) break;
        else if (ptr->reach_time + 150 <= app.timer.relative_time) continue;
        ptr->update_enable = 0;
    }
    while (note_list->head < note_list->tail && note_list->head->reach_time + 150 <= app.timer.relative_time) {
        switch (note_list->head->type) {
        case SINGLE: {
            if (note_list->head->is_hit == 0) {
                *update_data.combo = 0;
                update_data.hit_status = 2;
            }
            NoteListPop(note_list);
            break;
        }
        case LONG: {
            if (note_list->head->is_hit == 0) {
                *update_data.combo = 0;
                update_data.hit_status = 2;
            }
            if (note_list->head->linked_notes[0]->reach_time <= app.timer.relative_time) {
                NoteListPop(note_list);
                NoteListPop(note_list);
            }
            else goto out; // jump out of this loop (out is exactly below this loop)
            break;
        }
        default:
            break;
        }
    }
out:
    while (!isNoteListTailEnd(note_list) && note_list->tail->update_time <= app.timer.relative_time) {
        NoteListPush(note_list);
    }
    NoteListFor(note_list) {
        NoteUpdate(ptr, target_x, target_y);
    }
}

void NoteListDraw(NoteList* note_list) {
    NoteListFor(note_list) {
        NoteDraw(ptr);
    }
}

static void NoteListPop(NoteList* note_list) {
    note_list->head->effect.is_active = 0;
    note_list->head++;
}

static void NoteListPush(NoteList* note_list) {
    note_list->tail->effect.is_active = 1;
    note_list->tail++;
}

static bool isNoteListTailEnd(NoteList* note_list) {
    return note_list->tail - note_list->notes >= note_list->size;
}