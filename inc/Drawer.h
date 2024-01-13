#ifndef _DRAWER_H_
#define _DRAWER_H_

#include <SDL_ttf.h>
#include <SDL2_gfxPrimitives.h>
#include "Constants.h"

void DrawText(SDL_Renderer* renderer, SDL_Rect rect, const char* text, TTF_Font* font, SDL_Color color);
void DrawCursor(SDL_Renderer* renderer, SDL_Rect rect);
void DrawHitPoint(SDL_Renderer* renderer, int x, int y, SDL_Color color);
void DrawSingleNote(SDL_Renderer* renderer, int x, int y, SDL_Color color);
void DrawLongNote(SDL_Renderer* renderer, int head_x, int head_y, int tail_x, int tail_y, SDL_Color color);

#endif