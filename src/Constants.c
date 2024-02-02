#include "Constants.h"

/* Common */
int SCREEN_WIDTH, SCREEN_HEIGHT;

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

/* Lane */
const SDL_Scancode default_keys[] = {
    SDL_SCANCODE_S,
    SDL_SCANCODE_D,
    SDL_SCANCODE_F,
    SDL_SCANCODE_G,
    SDL_SCANCODE_H,
    SDL_SCANCODE_J,
    SDL_SCANCODE_K,
    SDL_SCANCODE_L
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

/* End Scene */
const char* end_scene_ratings[] = {
    "RATING: S",
    "RATING: A",
    "RATING: B",
    "RATING: C",
    "RATING: D"
};

void InitConstants() {
    SDL_DisplayMode dm;
    SDL_assert(SDL_GetCurrentDisplayMode(0, &dm) == 0);
    SCREEN_WIDTH = dm.w, SCREEN_HEIGHT = dm.h;
}