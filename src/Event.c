#include "Event.h"

static void InitTextEvent(Event* event, va_list* args) {
    SDL_Rect rect;
    rect.x = va_arg(*args, int);
    rect.y = va_arg(*args, int);
    const char* text = va_arg(*args, char*);
    int len = strlen(text);
    rect.w = len * GAME_SCENE_LETTER_WIDTH, rect.h = GAME_SCENE_LETTER_HEIGHT;
    event->data = malloc(sizeof(SDL_Rect) + len + 1);
    if (event->data == NULL) {
        fprintf(stderr, "[Event]Failed to malloc data\n");
        app.error_level = app.error_level > 1 ? app.error_level : 1;
        return;
    }
    char* text_cp = event->data + sizeof(SDL_Rect);
    memcpy(event->data, &rect, sizeof(SDL_Rect));
    memcpy(text_cp, text, len + 1);
}

static void InitBpmEvent(Event* event, va_list* args) {
    int bpm = va_arg(*args, int);
    event->data = malloc(sizeof(int));
    if (event->data == NULL) {
        fprintf(stderr, "[Event]Failed to malloc data\n");
        app.error_level = app.error_level > 1 ? app.error_level : 1;
        return;
    }
    memcpy((int*)event->data, &bpm, sizeof(int));
}

static void FreeEffectEvent(void* data) {
    int size = sizeof(struct { int x, y, r; double angle; });
    Effect* effect = data + size;
    FreeEffect(effect);
}

static void InitEffectEvent(Event* event, va_list* args) {
    struct {
        int x, y, r;
        double angle;
    } data;
    data.x = va_arg(*args, int);
    data.y = va_arg(*args, int);
    data.r = va_arg(*args, int);
    data.angle = va_arg(*args, double);
    int type = va_arg(*args, int);
    int repeat_enable = va_arg(*args, int);
    event->data = malloc(sizeof(data) + sizeof(Effect));
    if (event->data == NULL) {
        fprintf(stderr, "[Event]Failed to malloc data\n");
        app.error_level = app.error_level > 1 ? app.error_level : 1;
        return;
    }
    memcpy(event->data, &data, sizeof(data));
    Effect* effect = event->data + sizeof(data);
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
        app.error_level = app.error_level > 1 ? app.error_level : 1;
        return;
    }
    memcpy(event->data, path, len + 1);
}

static void InitMoveEvent(Event* event, va_list* args) {
    struct {
        int x, y;
    } data;
    data.x = va_arg(*args, int);
    data.y = va_arg(*args, int);
    event->data = malloc(sizeof(data));
    if (event->data == NULL) {
        fprintf(stderr, "[Event]Failed to malloc data\n");
        app.error_level = app.error_level > 1 ? app.error_level : 1;
        return;
    }
    memcpy(event->data, &data, sizeof(data));
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
        app.error_level = app.error_level > 1 ? app.error_level : 1;
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
    Effect* effect = event->data + sizeof(struct { int x, y, r; double angle; });
    EffectUpdate(effect);
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
    SDL_Rect* rect = event->data;
    char* text = event->data + sizeof(SDL_Rect);
    DrawText(*rect, text, default_colors[0]);
}

static void EffectEventDraw(Event* event) {
    struct Data {
        int x, y, r;
        double angle;
    }*data = event->data;
    Effect* effect = event->data + sizeof(struct Data);
    EffectDraw(effect, data->x, data->y, data->r, data->angle);
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