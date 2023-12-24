#include "../inc/Note.h"

Note* CreateNote() {
    Note* note = malloc(sizeof(Note));
    if (note == NULL) {
        isError = 1;
        printf("[Note]Failed to malloc note\n");
        return note;
    }
    note->coord.x = note->coord.y = -1.0f;
    note->destination.x = note->destination.y = -1.0f;
    note->speed.x = note->speed.y = 0.0f;
    note->color.r = note->color.g = note->color.b = note->color.a = 0xff;
    note->isVisible = 0;
    return note;
}
void InitNote(Note* note, Coordinate coord, Coordinate destination, SDL_Color color, size_t ticks) {
    note->coord = coord, note->destination = destination;
    note->color = color;
    note->speed.x = (destination.x - coord.x) / ticks, note->speed.y = (destination.y - coord.y) / ticks;
    note->isVisible = 1;
}
void MoveNote(Note* note) {
    if (note->isVisible) {
        note->coord.x += note->speed.x;
        note->coord.y += note->speed.y;
        if ((note->destination.x - note->coord.x) / note->speed.x < 0 &&
            (note->destination.y - note->coord.y) / note->speed.y < 0) {
            note->isVisible = 0;
            note->coord.x = note->coord.y = -1.0f;
        }
    }
}
void DrawNote(SDL_Renderer* renderer, Note* note) {
    if (note->isVisible) {
        SDL_SetRenderDrawColor(renderer, note->color.r, note->color.g, note->color.b, note->color.a);
        // SDL_FRect rect = { note->coord.x, note->coord.y, NOTE_WIDTH, NOTE_HEIGHT };
        // SDL_RenderFillRectF(renderer, &rect);
        DrawCircle(renderer, note->coord.x, note->coord.y, NOTE_DEFAULT_RADIUS);
    }
}
void DestroyNote(Note* note) {
    if (note != NULL) {
        free(note);
    }
}