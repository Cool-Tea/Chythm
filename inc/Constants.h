#ifndef _CONSTANTS_H_
#define _CONSTANTS_H_

#include <SDL.h>

/* Common */
#define GAME_TITLE "Chythm"

#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080

#define LETTER_WIDTH 50
#define LETTER_HEIGHT 100

#define MAX_FPS 60; // this should not exceed 1000

#define SAVES_PATH "../saves/"
#define BGM_PATH
#define FONT_PATH "../assets/fonts/font.TTF"
#define FONT_SIZE 128

enum SceneType {
    MENU,
    SELECT,
    GAME,
    PAUSE
};
typedef enum SceneType SceneType;

/* Note */
#define NOTE_LIST_INIT_CAPACITY 1 << 4

enum NoteType {
    SINGLE,
    LONG
};
typedef enum NoteType NoteType;
extern const SDL_Color note_colors[];

/* Event */
#define EVENT_LIST_INIT_CAPACITY 1 << 3

enum EventType {
    MOVE,
    ROTATE,
    STOP,
    TEXT
};
typedef enum EventType EventType;

/* Lane */
#define PERFECT_HIT_INTERVAL 100 //ms
#define GOOD_HIT_INTERVAL 200 //ms
#define MISS_HIT_INTERVAL 400 //ms

extern const SDL_Color hit_point_colors[];

/* Game Scene */
enum ObjectType {
    GAME_SCENE = 0,
    LANE = 1
};
typedef enum ObjectType ObjectType;

/* Button */
extern const SDL_Color button_colors[];

/* Menu Scene */
#define MENU_SCENE_BACKGROUND "../assets/images/ background.png"
#define MENU_SCENE_BUTTON_SIZE 2

/* Select Scene */
#define CHART_LIST_INIT_CAPACITY 1 << 3
#define CHART_LIST_NAME_MAX_SIZE SCREEN_HEIGHT / (2 * LETTER_HEIGHT)
#define SELECT_SCENE_BACKGROUND "../assets/images/ background.png"

extern const SDL_Rect preview_rect;
extern const SDL_Color title_color;

/* Pause Scene */
#define PAUSE_SCENE_BUTTON_SIZE 3
#define PAUSE_SCENE_BACKGROUND "../assets/images/ background.png"

#endif