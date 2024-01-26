#ifndef _CONSTANTS_H_
#define _CONSTANTS_H_

#include <SDL.h>

/* Common */
#define GAME_TITLE "Chythm"

#define AUTO_RESOLUTION 0

#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080

#define LETTER_WIDTH 50
#define LETTER_HEIGHT 100

#define MAX_FPS 60; // this should not exceed 1000

#define SAVES_PATH "../saves/"

#define USE_DEFAULT_BACKGROUND 0 // if is ture, all the scene will use default background

#define DEFAULT_BACKGROUND "../assets/images/Chythm.png"
#define DEFAULT_BACKGROUND_PURE "../assets/images/pure.png"

#define BGM_PATH
#define FONT_PATH "../assets/fonts/font.TTF"
#define FONT_SIZE 128

#define PI 3.141592653589793

enum SceneType {
    MENU,
    SELECT,
    GAME,
    PAUSE,
    END,
    LOAD
};
typedef enum SceneType SceneType;

extern const SDL_Color default_colors[];

/* Particle and Effect */
#define PARTICLE_IMG_DIR_PATH "../assets/images/effects/"

enum EffectType {
    SCORCH,
    CIRCLE,
    MAGIC,
    MUZZLE,
    STAR,
    TWIRL
};
typedef enum EffectType EffectType;

/* Note */
#define NOTE_ONLY_EFFECT 1 // if enabled, note will only draw effect

#if !NOTE_ONLY_EFFECT
#define SINGLE_NOTE_IMG "../assets/images/sprites/single_note.png"
#define LONG_NOTE_IMG "../assets/images/sprites/long_note.png"
#endif

#define NOTE_LIST_INIT_CAPACITY 1 << 4
#define NOTE_RADIUS 30

enum NoteType {
    SINGLE = 0,
    LONG = 1,

    NOTE_TYPE_NUM
};
typedef enum NoteType NoteType;

/* Event */
#define EVENT_LIST_INIT_CAPACITY 1 << 3

enum EventType {
    /* Reserved */
    NONE = 0x00, // do nothing

    /* Game Scene */
    TEXT = 0x10, // show text on the screen

    /* Lane */
    MOVE = 0x20, // move hit point for some miliseconds
    MOVETO,       // move hit point to somewhere within some miliseconds
    STOP        // stop hit point movement
};
typedef enum EventType EventType;

/* Lane */
#define HIT_POINT_DEFAULT_IMG "../assets/images/sprites/hit_point_default.png"
#define HIT_POINT_DOWN_IMG "../assets/images/sprites/hit_point_down.png"

#define PERFECT_HIT_SCORE 500
#define GOOD_HIT_SCORE 200
#define LONG_HIT_SCORE 100 // for continuing long hit
#define HIT_POINT_RADIUS 35
#define COMBO_EXTRA_SCORE 50
#define COMBO_MAX_SCORE 400
#define KEY_NUM 8

extern const SDL_Scancode default_keys[];

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
#if !USE_DEFAULT_BACKGROUND
/* if USE_DEFAULT_BACKGROUND is false, this will change the background of the menu */
#define MENU_SCENE_BACKGROUND DEFAULT_BACKGROUND // the path to the background
#endif

#define MENU_SCENE_BUTTON_SIZE 2

/* Select Scene */
#define CHART_LIST_INIT_CAPACITY 1 << 3
#define CHART_LIST_NAME_MAX_SIZE SCREEN_HEIGHT / LETTER_HEIGHT / 2

#if !USE_DEFAULT_BACKGROUND
/* if USE_DEFAULT_BACKGROUND is false, this will change the background of the select */
#define SELECT_SCENE_BACKGROUND DEFAULT_BACKGROUND_PURE // the path to the background
#endif

extern const SDL_Rect preview_rect;

/* Pause Scene */
#define PAUSE_SCENE_BUTTON_SIZE 3

#if !USE_DEFAULT_BACKGROUND
/* if USE_DEFAULT_BACKGROUND is false, this will change the background of the pause */
#define PAUSE_SCENE_BACKGROUND DEFAULT_BACKGROUND_PURE // the path to the background
#endif

/* End Scene */
#define END_SCENE_BUTTON_SIZE 3

#if !USE_DEFAULT_BACKGROUND
/* if USE_DEFAULT_BACKGROUND is false, this will change the background of the end scene */
#define END_SCENE_BACKGROUND DEFAULT_BACKGROUND_PURE // the path to the background
#endif

#define RATING_S_PERCENTAGE 0.9
#define RATING_A_PERCENTAGE 0.8
#define RATING_B_PERCENTAGE 0.7
#define RATING_C_PERCENTAGE 0.5

extern const char* end_scene_ratings[];

/* Load Scene */
#if !USE_DEFAULT_BACKGROUND
#define LOAD_SCENE_BACKGROUND "../assets/images/loading.png"
#endif

#endif