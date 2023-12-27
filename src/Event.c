#include "../inc/Event.h"

void InitEventList(EventList* event_list) {
    event_list->size = 0;
    event_list->capacity = EVENT_LIST_INIT_CAPACITY;
    event_list->events = malloc(event_list->capacity * sizeof(Event));
    if (event_list->events == NULL) {
        printf("[EventList]Failed to malloc event list\n");
        is_error = 1;
    }
    event_list->cur_event = event_list->events;
}
void FreeEventList(EventList* event_list) {
    for (size_t i = 0; i < event_list->size; i++) {
        if (event_list->events[i].data != NULL) {
            free(event_list->events[i].data);
        }
    }
    free(event_list->events);
}
void EventListEmplaceBack(EventList* event_list,
    Uint32 time, EventType type, void* data
) {
    event_list->events[event_list->size++] = (Event){
        .time = time,
        .type = type,
        .data = data
    };
    if (event_list->size == event_list->capacity) {
        event_list->capacity <<= 1;
        event_list->events = realloc(event_list->events, event_list->capacity * sizeof(Event));
    }
}