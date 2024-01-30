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

static void InitBpmEvent(Event* event, va_list* args) {
    int bpm = va_arg(*args, int);
    event->data = malloc(sizeof(int));
    if (event->data == NULL) {
        fprintf(stderr, "[Event]Failed to malloc data\n");
        app.is_error = 1;
        return;
    }
    memcpy((int*)event->data, &bpm, sizeof(int));
}

static void FreeEffectEvent(void* data) {
    Effect* effect = data + sizeof(int) * 3 + sizeof(double);
    FreeEffect(effect);
}

static void InitEffectEvent(Event* event, va_list* args) {
    int x = va_arg(*args, int);
    int y = va_arg(*args, int);
    int radius = va_arg(*args, int);
    double angle = va_arg(*args, double);
    int type = va_arg(*args, int);
    int repeat_enable = va_arg(*args, int);
    int size = sizeof(int) * 3 + sizeof(double) + sizeof(Effect);
    event->data = malloc(size);
    if (event->data == NULL) {
        fprintf(stderr, "[Event]Failed to malloc data\n");
        app.is_error = 1;
        return;
    }
    memcpy((int*)event->data, &x, sizeof(int));
    memcpy((int*)event->data + 1, &y, sizeof(int));
    memcpy((int*)event->data + 2, &radius, sizeof(int));
    memcpy((int*)event->data + 3, &angle, sizeof(double));
    Effect* effect = event->data + sizeof(int) * 3 + sizeof(double);
    InitEffect(effect, type, repeat_enable);
    effect->is_active = 1;
    event->free = FreeEffectEvent;
}

static void InitBackgroundEvent(Event* event, va_list* args) {
    const char* path = va_arg(*args, const char*);
    int len = strlen(path);
    event->data = malloc(len + 1);
    if (event->data == NULL) {
        fprintf(stderr, "[Event]Failed to malloc data\n");
        app.is_error = 1;
        return;
    }
    memcpy(event->data, path, len + 1);
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
    event->free = NULL;
    switch (type) {
    case TEXT: {
        InitTextEvent(event, args);
        break;
    }
    case BPM: {
        InitBpmEvent(event, args);
        break;
    }
    case EFFECT: {
        InitEffectEvent(event, args);
        break;
    }
    case BACKGROUND: {
        InitBackgroundEvent(event, args);
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
    if (event->free) event->free(event->data);
    free(event->data);
    free(event);
}

static void EffectEventUpdate(Event* event) {
    EffectUpdate((Effect*)(event->data + sizeof(int) * 3 + sizeof(double)));
}

void EventUpdate(Event* event) {
    if (app.timer.relative_time < event->time) return;
    switch (event->type) {
    case EFFECT: {
        EffectEventUpdate(event);
        break;
    }
    default:
        break;
    }
}

static void TextEventDraw(Event* event) {
    DrawText(*((SDL_Rect*)event->data), ((char*)event->data + sizeof(SDL_Rect)), default_colors[0]);
}

static void EffectEventDraw(Event* event) {
    int x = *(int*)event->data;
    int y = *((int*)event->data + 1);
    int radius = *((int*)event->data + 2);
    double angle = *(double*)(event->data + sizeof(int) * 3);
    Effect* effect = event->data + sizeof(int) * 3 + sizeof(double);
    EffectDraw(effect, x, y, radius, angle);
}

void EventDraw(Event* event) {
    if (app.timer.relative_time < event->time) return;
    switch (event->type) {
    case TEXT: {
        TextEventDraw(event);
        break;
    }
    case EFFECT: {
        EffectEventDraw(event);
        break;
    }
    default:
        break;
    }
}