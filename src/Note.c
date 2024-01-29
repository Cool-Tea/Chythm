#include "Note.h"

Note* CreateNote(
    NoteType type,
    int update_x, int update_y,
    Uint32 update_time, Uint32 reach_time
) {
    Note* note = malloc(sizeof(Note));
    if (note == NULL) {
        fprintf(stderr, "[Note]Failed to malloc note\n");
        app.is_error = 1;
        return note;
    }
    note->type = type;
    note->linked_notes[0] = note->linked_notes[1] = NULL;
    note->cur_x = note->update_x = update_x;
    note->cur_y = note->update_y = update_y;
    note->update_time = update_time;
    note->reach_time = reach_time;
    note->update_enable = 1;
    note->is_hit = note->is_missed = 0;
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
    lnote->linked_notes[lnote->linked_notes[0] != NULL] = rnote;
    rnote->linked_notes[rnote->linked_notes[0] != NULL] = lnote;
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
    static SDL_Rect rect
#if !AUTO_RESOLUTION
        = { .h = NOTE_RADIUS << 1, .w NOTE_RADIUS << 1 }
#endif
    ;

    rect.x = note->cur_x - NOTE_RADIUS, rect.y = note->cur_y - NOTE_RADIUS;

#if AUTO_RESOLUTION
    rect.h = NOTE_RADIUS << 1, rect.w = NOTE_RADIUS << 1;
    rect.x *= app.zoom_rate.w, rect.y *= app.zoom_rate.h;
    rect.w *= app.zoom_rate.w, rect.h *= app.zoom_rate.h;
#endif

    double angle =
        SDL_atan2(note->cur_x - note->update_x, note->cur_y - note->update_y)
        * 180.0 / PI;
    if (note->cur_x > note->update_x) angle = -angle;
    SDL_RenderCopyEx(app.ren, assets.notes[note->type], NULL, &rect, angle, NULL, SDL_FLIP_NONE);
}
#endif

void NoteDraw(Note* note) {
    if (app.timer.relative_time < note->update_time) return;
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

#if AUTO_RESOLUTION
            note->cur_x * app.zoom_rate.w, note->cur_y * app.zoom_rate.h,
            note->linked_notes[0]->cur_x * app.zoom_rate.w, note->linked_notes[0]->cur_y * app.zoom_rate.h,
#else
            note->cur_x, note->cur_y,
            note->linked_notes[0]->cur_x, note->linked_notes[0]->cur_y,
#endif

            default_colors[0].r, default_colors[0].g, default_colors[0].b, default_colors[0].a
        );
        break;
    }
    default:
        break;
    }
}