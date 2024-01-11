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
    PAUSE,
    END
};
typedef enum SceneType SceneType;

extern const SDL_Color default_colors[];

/* Note */
#define NOTE_LIST_INIT_CAPACITY 1 << 4
#define NOTE_RADIUS 30

enum NoteType {
    SINGLE = 0,
    LONG_HEAD = 1,
    LONG_TAIL = 2
};
typedef enum NoteType NoteType;
extern const SDL_Color note_colors[];

/* Event */
#define EVENT_LIST_INIT_CAPACITY 1 << 3

enum EventType {
    TEXT = 0,
    MOVE = 1,
    STOP = 2
};
typedef enum EventType EventType;

/* Lane */
#define PERFECT_HIT_INTERVAL 50 //ms
#define PERFECT_HIT_SCORE 500
#define GOOD_HIT_INTERVAL 100 //ms
#define GOOD_HIT_SCORE 200
#define MISS_HIT_INTERVAL 150 //ms
#define HIT_POINT_RADIUS 35

extern const SDL_Keycode default_keys[];
extern const SDL_Color hit_point_colors[];

/* Game Scene */
#define GAME_SCENE_TEXT_PERSISTENCE 2000
#define GAME_SCENE_LETTER_HEIGHT 40
#define GAME_SCENE_LETTER_WIDTH 20

enum ObjectType {
    GAME_SCENE = 0,
    LANE = 1
};
typedef enum ObjectType ObjectType;
extern const char* game_scene_texts[];

/* Button && Cursor */
#define CURSOR_LEN 100
#define CURSOR_WIDTH 5

extern const SDL_Color button_colors[];
extern const SDL_Color cursor_color;

/* Menu Scene */
#define MENU_SCENE_BACKGROUND "../assets/images/Chythm.png"
#define MENU_SCENE_BUTTON_SIZE 2

/* Select Scene */
#define CHART_LIST_INIT_CAPACITY 1 << 3
#define CHART_LIST_NAME_MAX_SIZE SCREEN_HEIGHT / LETTER_HEIGHT / 2
#define SELECT_SCENE_BACKGROUND "../assets/images/pure.png"

extern const SDL_Rect preview_rect;

/* Pause Scene */
#define PAUSE_SCENE_BUTTON_SIZE 3
#define PAUSE_SCENE_BACKGROUND "../assets/images/pure.png"

/* End Scene */
#define END_SCENE_BUTTON_SIZE 3
#define END_SCENE_BACKGROUND "../assets/images/pure.png"
#define RATING_S_PERCENTAGE 0.9
#define RATING_A_PERCENTAGE 0.8
#define RATING_B_PERCENTAGE 0.7
#define RATING_C_PERCENTAGE 0.5

extern const char* end_scene_ratings[];

#endif