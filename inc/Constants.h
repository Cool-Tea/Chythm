#ifndef _CONSTANTS_H_
#define _CONSTANTS_H_

#include <SDL.h>

/* Common */
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
extern const SDL_Color NoteColors[];

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

extern const SDL_Color HitPointColors[];

/* Game Scene */
enum ObjectType {
    LANE,
    GAME_SCENE
};
typedef enum ObjectType ObjectType;

/* Button */
extern const SDL_Color ButtonColors[];

#endif