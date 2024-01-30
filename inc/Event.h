#ifndef _EVENT_H_
#define _EVENT_H_

#include <stdarg.h>
#include "Effect.h"
#include "Drawer.h"

struct Event {
    Uint32 time;
    Uint32 lasting_time;
    EventType type;
    void* data;
    void (*free)(void*);
};
typedef struct Event Event;

Event* CreateEvent(
    Uint32 time, Uint32 lasting_time, EventType type, ...
);
Event* CreateEventVA(
    Uint32 time, Uint32 lasting_time, EventType type, va_list* args
);
void DestroyEvent(Event* event);
void EventUpdate(Event* event);
void EventDraw(Event* event);

#endif