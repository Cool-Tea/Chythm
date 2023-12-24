#pragma once
#ifndef _BUTTON_H_
#define _BUTTON_H_

#include "Dependency.h"
#include "Utility.h"
#include "Drawer.h"
#include "Global.h"
#include "Const.h"

/* Macros start */
#define WIDTH_PER_LETTER 50
#define HEIGHT_PER_LETTER 100
/* Macros end */

struct Button {
    SDL_FRect rect;
    SDL_Color color;
    const char* text;
};
typedef struct Button Button;
Button* CreateButton(float x, float y, const char* text);
void EventButton(SDL_Event* event, Button* button);
void DrawButton(SDL_Renderer* renderer, Button* button, TTF_Font* font);
void DestroyButton(Button* button);

#endif