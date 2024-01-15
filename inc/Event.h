#ifndef _EVENT_H_
#define _EVENT_H_

#include <stdarg.h>
#include "Constants.h"
#include "Globals.h"
#include "Drawer.h"

struct Event {
    Uint32 time;
    Uint32 lasting_time;
    EventType type;
    void* data;
};
typedef struct Event Event;
static void InitTextEvent(Event* event, va_list* args);
void FreeEvent(Event* event);
void EventUpdate(Event* event);
static void TextEventDraw(Event* event);
void EventDraw(Event* event);

/* A CPP-vector-like array, designed for this project */
struct EventList {
    size_t size;
    size_t capacity;
    Event* events; // array
    Event* head; // pointer
    Event* tail;
};
typedef struct EventList EventList;

#define EventListFor(event_list) for (Event* ptr = (event_list)->head; ptr != (event_list)->tail; ptr++)

void InitEventList(EventList* event_list);
void FreeEventList(EventList* event_list);
void EventListEmplaceBack(EventList* event_list,
    Uint32 time, Uint32 lasting_time, EventType type, ...
);
void EventListUpdate(EventList* event_list);
void EventListDraw(EventList* event_list);
static void EventListPop(EventList* event_list);
static void EventListPush(EventList* event_list);
static bool isEventListTailEnd(EventList* event_list);

#endif