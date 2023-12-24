#pragma once
#ifndef _NOTE_H_
#define _NOTE_H_

#include "Dependency.h"
#include "Utility.h"
#include "Drawer.h"
#include "Global.h"
#include "Const.h"

/* Macro here */
// #define NOTE_WIDTH 250.0f
// #define NOTE_HEIGHT 10.0f
#define NOTE_DEFAULT_RADIUS 25
#define NOTE_NORMAL_TICK 60
/* Macro end */

struct Note {
    Coordinate coord;
    Coordinate destination;
    Vector2 speed;
    Uint8 isVisible;
    SDL_Color color;
};
typedef struct Note Note;
Note* CreateNote();
/**
 * initalize a note which will reach the destination in ticks tick
*/
void InitNote(Note* note, Coordinate coord, Coordinate destination, SDL_Color color, size_t ticks);
void MoveNote(Note* note);
void DrawNote(SDL_Renderer* renderer, Note* note);
void DestroyNote(Note* note);

#endif