#ifndef _DRAWER_H_
#define _DRAWER_H_

#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL2_gfxPrimitives.h>
#include "Globals.h"
#include "Constants.h"

struct Assets {
    /* audio */

    /* fonts */

    /* images */
    SDL_Texture* backgrounds[2];
    SDL_Texture* hit_points[2];
    SDL_Texture* notes[NOTE_TYPE_NUM];
};
typedef struct Assets Assets;

extern Assets assets;

static void GetBackgroundImg();
static void GetHitPointImg();
static void GetNoteImg();
void InitAssets();
void FreeAssets();

void DrawDefaultBackground();
void DrawDefaultBackgroundPure();
void DrawText(SDL_Rect rect, const char* text, SDL_Color color);
void DrawCursor(SDL_Rect rect);
void DrawHitPoint(int x, int y, bool is_down);
void DrawSingleNote(int x, int y, int dire_x, int dire_y);
void DrawLongNote(int head_x, int head_y, int tail_x, int tail_y, SDL_Color color);

#endif