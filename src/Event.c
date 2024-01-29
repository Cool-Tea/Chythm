#include "Event.h"

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

Event* CreateEvent(
    Uint32 time, Uint32 lasting_time, EventType type, ...
) {
    va_list args;
    va_start(args, type);
    Event* event = CreateEventVA(time, lasting_time, type, &args);
    va_end(args);
    return event;
}

Event* CreateEventVA(
    Uint32 time, Uint32 lasting_time, EventType type, va_list* args
) {
    Event* event = malloc(sizeof(Event));
    if (event == NULL) {
        fprintf(stderr, "[Event]Failed to malloc event\n");
        app.is_error = 1;
        return event;
    }
    event->time = time;
    event->lasting_time = lasting_time;
    event->type = type;
    switch (type) {
    case TEXT: {
        InitTextEvent(event, args);
        break;
    }
    case MOVE: {
        InitMoveEvent(event, args);
        break;
    }
    case MOVETO: {
        InitMoveToEvent(event, args);
        break;
    }
    case STOP: {
        InitStopEvent(event);
        break;
    }
    default:
        break;
    }

    return event;
}

void DestroyEvent(Event* event) {
    if (event == NULL) return;
    free(event->data);
    free(event);
}

void EventUpdate(Event* event) {
    /* TODO: do something */
}

static void TextEventDraw(Event* event) {
    DrawText(*((SDL_Rect*)event->data), ((char*)event->data + sizeof(SDL_Rect)), default_colors[0]);
}

void EventDraw(Event* event) {
    if (app.timer.relative_time < event->time) return;
    switch (event->type) {
    case TEXT: {
        TextEventDraw(event);
        break;
    }
    default:
        break;
    }
}