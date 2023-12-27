#ifndef _BUTTON_H_
#define _BUTTON_H_

#include <SDL_ttf.h>
#include "Constants.h"
#include "Globals.h"

typedef void (*OnHit)();

struct Button {
    SDL_Rect rect;
    char* text;
    OnHit func;
};
typedef struct Button Button;
void InitButton(Button* button, int x, int y, const char* text, OnHit func);
void FreeButton(Button* button);
void ButtonFunc(Button* button);
void ButtonDraw(Button* button, SDL_Renderer* renderer, TTF_Font* font);

#endif