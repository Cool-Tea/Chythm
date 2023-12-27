#ifndef _EVENT_H_
#define _EVENT_H_

#include "Constants.h"
#include "Globals.h"

struct Event {
    Uint32 time;
    EventType type;
    void* data;
};
typedef struct Event Event;

/* A CPP-vector-like array, designed for this project */
struct EventList {
    size_t size;
    size_t capacity;
    Event* events; // array
    Event* cur_event; // pointer
};
typedef struct EventList EventList;
void InitEventList(EventList* event_list);
void FreeEventList(EventList* event_list);
void EventListEmplaceBack(EventList* event_list,
    Uint32 time, EventType type, void* data
);

#endif