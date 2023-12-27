#ifndef _CONSTANTS_H_
#define _CONSTANTS_H_

#include <SDL.h>

/* Common */
#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080

#define LETTER_WIDTH 50
#define LETTER_HEIGHT 100

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
    LANE,
    GAME_SCENE
};
typedef enum ObjectType ObjectType;

/* Button */
extern const SDL_Color button_colors[];

/* Menu Scene */
#define MENU_SCENE_BACKGROUND "../assets/images/background.jpg"
#define MENU_SCENE_BUTTON_SIZE 2

#endif