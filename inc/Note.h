#ifndef _NOTE_H_
#define _NOTE_H_

#include "Constants.h"
#include "Globals.h"
#include "Drawer.h"
#include "Effect.h"

/**
 * This is specially desigin to pass parameters from game_scene to lane or note
*/
struct UpdateData {
    int* combo;
    Uint64* score;
    int hit_status;
};
typedef struct UpdateData UpdateData;

extern UpdateData update_data;

struct Note {
    NoteType type;
    struct Note* linked_notes[2]; // for special linked note like LONG or sth else
    int cur_x, cur_y;
    int update_x, update_y; // where when the note starts to be updated
    Uint32 update_time; // when the note starts to be updated
    Uint32 reach_time; // when the note reached the hit point
    bool update_enable; // whether update is enabled
    bool is_missed; // whether the note is missed or not
    bool is_hit; // whether the note is hit or not
    Effect effect; // the effect note pocessed
};
typedef struct Note Note;

void NoteLink(Note* lnote, Note* rnote);
void NoteUpdate(Note* note, int target_x, int target_y);

#if !NOTE_ONLY_EFFECT
static void TypeNoteDraw(Note* note);
#endif

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

#define NoteListFor(note_list) for (Note* ptr = (note_list)->head; ptr < (note_list)->tail; ptr++)
#define NoteListBack(note_list) ((note_list)->notes + (note_list)->size)

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