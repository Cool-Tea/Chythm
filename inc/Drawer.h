#ifndef _DRAWER_H_
#define _DRAWER_H_

#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL2_gfxPrimitives.h>
#include "Constants.h"

void DrawText(SDL_Renderer* renderer, SDL_Rect rect, const char* text, TTF_Font* font, SDL_Color color);
void DrawCursor(SDL_Renderer* renderer, SDL_Rect rect);
void DrawSingleNote(SDL_Renderer* renderer, int x, int y, int dire_x, int dire_y, SDL_Color color);
void DrawHitPoint(SDL_Renderer* renderer, int x, int y, SDL_Color color);

#endif