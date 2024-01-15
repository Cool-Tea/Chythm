#ifndef _NOTE_H_
#define _NOTE_H_

#include "Constants.h"
#include "Globals.h"
#include "Drawer.h"

struct Note {
    NoteType type;
    int cur_x, cur_y;
    int speed_x, speed_y;
    Uint32 update_time; // when the note starts to be update
    Uint32 reach_time; // when the note reached the hit point
    bool is_missed; // whether the note is missed or not
};
typedef struct Note Note;

void NoteUpdate(Note* note, int target_x, int target_y);
void NoteDraw(Note* note);

/* A CPP-vector-like array, designed specially for this project*/
struct NoteList {
    size_t size;
    size_t capacity;
    Note* notes;
    Note* head;
    Note* tail;
};
typedef struct NoteList NoteList;

#define NoteListFor(note_list) for (Note* ptr = (note_list)->head; ptr != (note_list)->tail; ptr++)

void InitNoteList(NoteList* note_list);
void FreeNoteList(NoteList* note_list);
void NoteListEmplaceBack(NoteList* note_list,
    NoteType type,
    int start_x, int start_y,
    Uint32 update_time, Uint32 reach_time
);
void NoteListUpdate(NoteList* note_list, int target_x, int target_y);
void NoteListDraw(NoteList* note_list);
static void NoteListPop(NoteList* note_list);
static void NoteListPush(NoteList* note_list);
static bool isNoteListTailEnd(NoteList* note_list);

#endif