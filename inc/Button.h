#ifndef _BUTTON_H_
#define _BUTTON_H_

#include "Constants.h"

struct Button {
    SDL_Rect rect;
    char* text;
};
typedef struct Button Button;

#endif