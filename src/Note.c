#include "../inc/Note.h"

void NoteUpdate(Note* note, Uint32 relative_time) {
    note->cur_x = (
        (note->end_x - note->start_x) * (relative_time - note->start_time)
        + note->start_x * (note->end_time - relative_time)
        ) / (note->end_time - note->start_time);
    note->cur_y = (
        (note->end_y - note->start_y) * (relative_time - note->start_time)
        + note->start_y * (note->end_time - relative_time)
        ) / (note->end_time - note->start_time);
}
void NoteDraw(Note* note, SDL_Renderer* renderer) {
    /* TODO: get the image of the note and draw */
}

void InitNoteList(NoteList* note_list) {
    note_list->size = 0;
    note_list->capacity = NOTE_LIST_INIT_CAPACITY;
    note_list->notes = malloc(note_list->capacity * sizeof(Note));
    if (note_list->notes == NULL) {
        printf("[NoteList]Failed to malloc note list\n");
        is_error = 1;
    }
    note_list->head = note_list->tail = note_list->notes;
}
void FreeNoteList(NoteList* note_list) {
    free(note_list->notes);
}
void NoteListEmplaceBack(NoteList* note_list,
    NoteType type,
    Uint32 start_time, Uint32 end_time,
    int start_x, int start_y, int end_x, int end_y
) {
    note_list->notes[note_list->size++] = (Note){
        .type = type,
        .start_time = start_time,
        .end_time = end_time,
        .start_x = start_x,
        .start_y = start_y,
        .cur_x = start_x,
        .cur_y = start_y,
        .end_x = end_x,
        .end_y = end_y
    };
    if (note_list->size >= note_list->capacity) {
        note_list->capacity <<= 1;
        note_list->notes = realloc(note_list->notes, note_list->capacity * sizeof(Note));
    }
}