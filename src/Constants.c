#include "../inc/Constants.h"

/* Note */
const SDL_Color note_colors[] = {
    {0x00,0x00,0xff,0xff}, // SINGLE
    {0x00,0xff,0xff,0xff} // LONG
};

/* Lane */
const SDL_Color hit_point_colors[] = {
    {0xff,0x00,0xff,0xff}, // key up
    {0xff,0x00,0xff,0x5f} // key down
};

/* Button */
const SDL_Color button_colors[] = {
    {0xff,0xff,0xff,0xff}, // button up
    {0x00,0xff,0x00,0xff} // button down
};

/* Select Scene */
const SDL_Rect preview_rect = {
    SCREEN_WIDTH / 2 - 250, SCREEN_HEIGHT / 2 - 400, 500, 500
};
const SDL_Color title_color = {
    0xff, 0xff, 0xff, 0xff
};