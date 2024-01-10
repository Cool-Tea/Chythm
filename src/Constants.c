#include "../inc/Constants.h"

/* Common */
const SDL_Color default_colors[] = {
    {0xff,0xff,0xff,0xff} // white
};

/* Note */
const SDL_Color note_colors[] = {
    {0x00,0x00,0xff,0xff}, // SINGLE
    {0x00,0xff,0xff,0xff} // LONG
};

/* Lane */
const SDL_Keycode default_keys[] = {
    SDLK_j, SDLK_f, SDLK_k, SDLK_d, SDLK_l, SDLK_s
};
const SDL_Color hit_point_colors[] = {
    {0xff,0x00,0xff,0xff}, // key up
    {0xff,0x00,0xff,0x5f} // key down
};

/* Game Scene */
const char* game_scene_texts[] = {
    "PERFECT", "GOOD", "MISS"
};

/* Button && Cursor */
const SDL_Color button_colors[] = {
    {0xff,0xff,0xff,0xff}, // button up
    {0x00,0xff,0x00,0xff} // button down
};
const SDL_Color cursor_color = {
    0xff,0xff,0xff,0xff
};

/* Select Scene */
const SDL_Rect preview_rect = {
    SCREEN_WIDTH / 2 - 250, SCREEN_HEIGHT / 2 - 400, 500, 500
};
const SDL_Color title_color = {
    0xff, 0xff, 0xff, 0xff
};