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
#if USE_DEFAULT_BACKGROUND
    SDL_Texture* backgrounds[2];
#endif

    SDL_Texture* hit_points[2];

#if !NOTE_ONLY_EFFECT
    SDL_Texture* notes[NOTE_TYPE_NUM];
#endif

};
typedef struct Assets Assets;

extern Assets assets;

#if USE_DEFAULT_BACKGROUND
static void GetBackgroundImg();
static void GetHitPointImg();
#endif

#if !NOTE_ONLY_EFFECT
static void GetNoteImg();
#endif

void InitAssets();
void FreeAssets();

void DrawDefaultBackground();
void DrawDefaultBackgroundPure();
void DrawText(SDL_Rect rect, const char* text, SDL_Color color);
void DrawCursor(SDL_Rect rect);

#endif