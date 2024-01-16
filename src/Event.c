#include "../inc/Event.h"

static void InitTextEvent(Event* event, va_list* args) {
    int x = va_arg(*args, int);
    int y = va_arg(*args, int);
    const char* text = va_arg(*args, char*);
    int len = strlen(text);
    int size = len + 1 + sizeof(SDL_Rect);
    event->data = malloc(size);
    if (event->data == NULL) {
        fprintf(stderr, "[Event]Failed to malloc data\n");
        app.is_error = 1;
        return;
    }
    SDL_Rect rect = {
        .h = GAME_SCENE_LETTER_HEIGHT,
        .w = len * GAME_SCENE_LETTER_WIDTH,
        .x = x,
        .y = y
    };
    memcpy((SDL_Rect*)event->data, &rect, sizeof(SDL_Rect));
    memcpy((char*)event->data + sizeof(SDL_Rect), text, len);
    *((char*)event->data + sizeof(SDL_Rect) + len) = '\0';
}

static void InitMoveEvent(Event* event, va_list* args) {
    int x = va_arg(*args, int);
    int y = va_arg(*args, int);
    event->data = malloc(2 * sizeof(int));
    if (event->data == NULL) {
        fprintf(stderr, "[Event]Failed to malloc data\n");
        app.is_error = 1;
        return;
    }
    memcpy((int*)event->data, &x, sizeof(int));
    memcpy((int*)event->data + 1, &y, sizeof(int));
}

static void InitMoveToEvent(Event* event, va_list* args) {
    InitMoveEvent(event, args);
}

static void InitStopEvent(Event* event) {
    event->data = NULL;
}

void FreeEvent(Event* event) {
    if (event != NULL && event->data != NULL) {
        free(event->data);
    }
}

void EventUpdate(Event* event) {
    /* TODO: do something */
}

static void TextEventDraw(Event* event) {
    DrawText(*((SDL_Rect*)event->data), ((char*)event->data + sizeof(SDL_Rect)), default_colors[0]);
}

void EventDraw(Event* event) {
    switch (event->type) {
    case TEXT: {
        TextEventDraw(event);
        break;
    }
    default:
        break;
    }
}

void InitEventList(EventList* event_list) {
    event_list->size = 0;
    event_list->capacity = EVENT_LIST_INIT_CAPACITY;
    event_list->events = malloc(event_list->capacity * sizeof(Event));
    if (event_list->events == NULL) {
        fprintf(stderr, "[EventList]Failed to malloc event list\n");
        app.is_error = 1;
    }
    event_list->head = event_list->events;
    event_list->tail = event_list->head;
}

void FreeEventList(EventList* event_list) {
    EventListFor(event_list) {
        FreeEvent(ptr);
    }
    free(event_list->events);
}

void EventListEmplaceBack(EventList* event_list,
    Uint32 time, Uint32 lasting_time, EventType type, ...
) {
    event_list->events[event_list->size].type = type;
    event_list->events[event_list->size].time = time;
    event_list->events[event_list->size].lasting_time = lasting_time;
    va_list args;
    va_start(args, type);
    switch (type) {
    case TEXT: {
        InitTextEvent(&event_list->events[event_list->size], &args);
        break;
    }
    case MOVE: {
        InitMoveEvent(&event_list->events[event_list->size], &args);
        break;
    }
    case MOVETO: {
        InitMoveToEvent(&event_list->events[event_list->size], &args);
        break;
    }
    case STOP: {
        InitStopEvent(&event_list->events[event_list->size]);
        break;
    }
    default:
        break;
    }
    va_end(args);
    event_list->size++;
    if (event_list->size == event_list->capacity) {
        event_list->capacity <<= 1;
        event_list->events = realloc(event_list->events, event_list->capacity * sizeof(Event));
    }
}

void EventListUpdate(EventList* event_list) {
    while (event_list->head != event_list->tail &&
        event_list->head->time + event_list->head->lasting_time < app.timer.relative_time) {
        EventListPop(event_list);
    }
    while (!isEventListTailEnd(event_list) && event_list->tail->time < app.timer.relative_time) {
        EventListPush(event_list);
    }
    EventListFor(event_list) {
        EventUpdate(ptr);
    }
}

void EventListDraw(EventList* event_list) {
    EventListFor(event_list) {
        switch (ptr->type) {
        case TEXT: {
            TextEventDraw(ptr);
            break;
        }
        case MOVE:
        default:
            break;
        }
    }
}

static void EventListPop(EventList* event_list) {
    event_list->head++;
}

static void EventListPush(EventList* event_list) {
    event_list->tail++;
}

static bool isEventListTailEnd(EventList* event_list) {
    return event_list->tail - event_list->events
        >=
        event_list->size;
}