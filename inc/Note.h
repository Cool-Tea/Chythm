#ifndef _NOTE_H_
#define _NOTE_H_

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

Note* CreateNote(
    NoteType type,
    int update_x, int update_y,
    Uint32 update_time, Uint32 reach_time
);
void DestroyNote(Note* note);
void NoteLink(Note* lnote, Note* rnote);
void NoteUpdate(Note* note, int target_x, int target_y);
void NoteDraw(Note* note);

#endif