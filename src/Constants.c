#include "../inc/Constants.h"

/* Common */
const SDL_Color default_colors[] = {
    {0xff,0xff,0xff,0xff}, // white
    {0x51,0xb1,0x4d,0xff}, // light green
    {0xfd,0xa2,0x1d,0xff}, // orange
    {0xdd,0x00,0x16,0xff}, // red
    {0x00,0xb1,0x94,0xff}, // cray
    {0x47,0xa5,0xeb,0xff}, // blue
    {0xfd,0xd6,0x00,0xff}  // yellow
};

/* Note */
const SDL_Color note_colors[] = {
    {0x00,0xb1,0x94,0xff}, // SINGLE
    {0xfd,0xd6,0x00,0xff}  // LONG
};

/* Lane */
const SDL_Keycode default_keys[] = {
    SDLK_j, SDLK_f, SDLK_k, SDLK_d, SDLK_l, SDLK_s
};
const SDL_Color hit_point_colors[] = {
    {0x47,0xa5,0xeb,0xff}, // key up
    {0x47,0xa5,0xeb,0x9f} // key down
};

/* Game Scene */
const char* game_scene_texts[] = {
    "PERFECT", "GOOD", "MISS"
};

/* Button && Cursor */
const SDL_Color button_colors[] = {
    {0xff,0xff,0xff,0xff}, // button up
    {0x51,0xb1,0x4d,0xff} // button hover
};
const SDL_Color cursor_color = {
    0xff,0xff,0xff,0xff
};

/* Select Scene */
const SDL_Rect preview_rect = {
    SCREEN_WIDTH / 2 - 250, SCREEN_HEIGHT / 2 - 400, 500, 500
};

/* End Scene */
const char* end_scene_ratings[] = {
    "RATING: S",
    "RATING: A",
    "RATING: B",
    "RATING: C",
    "RATING: D"
};