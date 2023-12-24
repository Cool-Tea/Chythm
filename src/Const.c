#include "../inc/Const.h"

Uint8 isError = 0;

const SDL_Color button_default = { 0xff,0xff,0xff,0xff };
const SDL_Color button_on_hover = {};
const SDL_Color button_down = {};

const SDL_FRect preview_rect = { 600.0f,300.0f,600.0f,600.0f };

const SDL_Keycode default_keys[] = { SDLK_j, SDLK_f, SDLK_k, SDLK_d, SDLK_l, SDLK_s };
const SDL_Color strack_default = { 0xff,0xff,0xff,0xff };
const SDL_Color strack_down = { 0x00,0x00,0x00,0x00 };