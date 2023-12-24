#ifndef _EVENT_H_
#define _EVENT_H_

#include "Constants.h"

struct Event {
    Uint32 time;
    EventType type;
    void* data;
};
typedef struct Event Event;

/* A CPP-vector-like array, designed for this project */
struct EventList {
    size_t size;
    Event* events; // array
    Event* cur_event; // pointer
};
typedef struct EventList EventList;
void EventListInit(EventList* event_list);
void EventListFree(EventList* event_list);
void EventListEmplaceBack(EventList* event_list,
    Uint32 time, EventType type, void* data
);

#endif