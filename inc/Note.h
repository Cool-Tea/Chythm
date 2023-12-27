#ifndef _NOTE_H_
#define _NOTE_H_

#include "Constants.h"
#include "Globals.h"

struct Note {
    NoteType type;
    Uint32 start_time, end_time; // ms
    int start_x, start_y;
    int cur_x, cur_y;
    int end_x, end_y;
};
typedef struct Note Note;
void NoteUpdate(Note* note, Uint32 relative_time);
void NoteDraw(Note* note, SDL_Renderer* renderer);

/* A CPP-vector-like array, designed specially for this project*/
struct NoteList {
    size_t size;
    size_t capacity;
    Note* notes;
    Note* head;
    Note* tail;
};
typedef struct NoteList NoteList;
void InitNoteList(NoteList* note_list);
void FreeNoteList(NoteList* note_list);
void NoteListEmplaceBack(NoteList* note_list,
    NoteType type,
    Uint32 start_time, Uint32 end_time,
    int start_x, int start_y, int end_x, int end_y
);

#endif