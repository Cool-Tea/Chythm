#include "Note.h"

Note* CreateNote(
    NoteType type,
    int update_x, int update_y,
    Uint32 update_time, Uint32 reach_time
) {
    Note* note = malloc(sizeof(Note));
    if (note == NULL) {
        fprintf(stderr, "[Note]Failed to malloc note\n");
        app.error_level = app.error_level > 1 ? app.error_level : 1;
        return note;
    }
    note->type = type;
    note->linked_notes[0] = note->linked_notes[1] = NULL;
    note->cur_x = note->update_x = update_x;
    note->cur_y = note->update_y = update_y;
    note->update_time = update_time;
    note->reach_time = reach_time;
    note->update_enable = 1;
    note->status = 0;
    switch (type) {
    case SINGLE: {
        InitEffect(&note->effect, MUZZLE, 1);
        break;
    }
    case LONG: {
        InitEffect(&note->effect, STAR, 1);
        break;
    }
    case MULTI: {
        InitEffect(&note->effect, SPARK, 1);
        break;
    }
    default:
        break;
    }
    note->effect.is_active = 1;
    return note;
}

void DestroyNote(Note* note) {
    if (note == NULL) return;
    FreeEffect(&note->effect);
    free(note);
}

void NoteLink(Note* lnote, Note* rnote) {
    if (!lnote || !rnote) return;
    lnote->linked_notes[lnote->linked_notes[0] != NULL] = rnote;
    rnote->linked_notes[rnote->linked_notes[0] != NULL] = lnote;
}

void NoteUnlink(Note* lnote, Note* rnote) {
    if (!lnote || !rnote) return;
    lnote->linked_notes[lnote->linked_notes[0] != rnote] = NULL;
    rnote->linked_notes[rnote->linked_notes[0] != lnote] = NULL;
}

void NoteUpdate(Note* note, int target_x, int target_y) {
    if (!note->update_enable || app.timer.relative_time < note->update_time) return;
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
    static SDL_Rect rect;
    rect.x = ZoomWidth(note->cur_x - NOTE_RADIUS), rect.y = ZoomHeight(note->cur_y - NOTE_RADIUS);
    rect.w = ZoomWidth(NOTE_RADIUS << 1), rect.h = ZoomHeight(NOTE_RADIUS << 1);
    double angle =
        SDL_atan2(note->update_x - note->cur_x, note->cur_y - note->update_y)
        * 180.0 / PI;
    SDL_RenderCopyEx(app.ren, assets.notes[note->type], NULL, &rect, angle, NULL, SDL_FLIP_NONE);
}
#endif

static void LongNoteDraw(Note* note) {
    if (note->linked_notes[0])
        lineRGBA(
            app.ren,
            ZoomWidth(note->cur_x), ZoomHeight(note->cur_y),
            ZoomWidth(note->linked_notes[0]->cur_x), ZoomHeight(note->linked_notes[0]->cur_y),
            default_colors[0].r, default_colors[0].g, default_colors[0].b, default_colors[0].a
        );
}

static void MultiNoteDraw(Note* note) {
    if (note->linked_notes[0])
        lineRGBA(
            app.ren,
            ZoomWidth(note->cur_x), ZoomHeight(note->cur_y),
            ZoomWidth(note->linked_notes[0]->cur_x), ZoomHeight(note->linked_notes[0]->cur_y),
            default_colors[2].r, default_colors[2].g, default_colors[2].b, default_colors[2].a
        );

    if (note->linked_notes[1])
        lineRGBA(
            app.ren,
            ZoomWidth(note->cur_x), ZoomHeight(note->cur_y),
            ZoomWidth(note->linked_notes[1]->cur_x), ZoomHeight(note->linked_notes[1]->cur_y),
            default_colors[2].r, default_colors[2].g, default_colors[2].b, default_colors[2].a
        );
}

void NoteDraw(Note* note) {
    if (app.timer.relative_time < note->update_time) return;
    double angle =
        SDL_atan2(note->update_x - note->cur_x, note->cur_y - note->update_y)
        * 180.0 / PI;
    EffectDraw(&note->effect, note->cur_x, note->cur_y, NOTE_RADIUS, angle);

#if !NOTE_ONLY_EFFECT
    TypeNoteDraw(note);
#endif

    /* TODO: draw different note */
    switch (note->type) {
    case LONG: {
        LongNoteDraw(note);
        break;
    }
    case MULTI: {
        MultiNoteDraw(note);
        break;
    }
    default:
        break;
    }
}