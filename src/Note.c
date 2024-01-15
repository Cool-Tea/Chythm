#include "../inc/Note.h"

void NoteUpdate(Note* note, int target_x, int target_y) {
    note->speed_x = (target_x - note->cur_x) / (note->reach_time - app.timer.relative_time);
    note->speed_y = (target_y - note->cur_y) / (note->reach_time - app.timer.relative_time);
    note->cur_x += note->speed_x;
    note->cur_y += note->speed_y;
}

void NoteDraw(Note* note) {
    /* TODO: draw different note */
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
    free(note_list->notes);
}

void NoteListEmplaceBack(NoteList* note_list,
    NoteType type,
    int start_x, int start_y,
    Uint32 update_time, Uint32 reach_time
) {
    note_list->notes[note_list->size++] = (Note){
        .type = type,
        .cur_x = start_x,
        .cur_y = start_y,
        .speed_x = 0,
        .speed_y = 0,
        .update_time = update_time,
        .reach_time = reach_time,
        .is_missed = 0
    };
    if (note_list->size >= note_list->capacity) {
        note_list->capacity <<= 1;
        note_list->notes = realloc(note_list->notes, note_list->capacity * sizeof(Note));
    }
}

void NoteListUpdate(NoteList* note_list, int target_x, int target_y) {
    while (note_list->head != note_list->tail && note_list->head->reach_time <= app.timer.relative_time) {
        NoteListPop(note_list);
    }
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
    note_list->head++;
}

static void NoteListPush(NoteList* note_list) {
    note_list->tail++;
}

static bool isNoteListTailEnd(NoteList* note_list) {
    return note_list->tail - note_list->notes >= note_list->size;
}