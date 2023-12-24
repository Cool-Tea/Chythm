#ifndef _CONSTANTS_H_
#define _CONSTANTS_H_

#include <SDL.h>

/* Common */

/* Note */
enum NoteType {
    SINGLE,
    LONG
};
typedef enum NoteType NoteType;
extern const SDL_Color NoteColors[];

/* Event */
enum EventType {
    MOVE,
    ROTATE,
    STOP,
    TEXT
};
typedef enum EventType EventType;

#endif