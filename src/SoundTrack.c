#include "../inc/SoundTrack.h"

SoundTrack* CreateSTrack(float x, float y, float vx, float vy, SDL_Color color, const char* track_text) {
    SoundTrack* strack = malloc(sizeof(SoundTrack));
    if (strack == NULL) {
        isError = 1;
        printf("[SoundTrack]Failed to malloc sound track\n");
        return strack;
    }
    strack->judge_coord.x = x, strack->judge_coord.y = y;
    strack->base_ray.x = vx, strack->base_ray.y = vy;
    strack->color = color;
    strack->sound_track = strack->cursor = track_text;
    strack->cache_id = 0;
    strack->cache_size = STRACK_DEFAULT_CACHE_SIZE;
    strack->cache = malloc(strack->cache_size * sizeof(Note*));
    if (strack->cache == NULL) {
        isError = 1;
        printf("[SoundTrack]Failed to malloc cache\n");
        return strack;
    }
    for (size_t i = 0; i < strack->cache_size; i++) {
        strack->cache[i] = CreateNote();
        if (isError) {
            printf("[SoundTrack]Failed to create note %zu\n", i);
        }
    }
    return strack;
}
void EventSTrack(SDL_Event* event, SoundTrack* strack) {
    switch (event->type) {
    case SDL_KEYDOWN: {
        strack->color = strack_down;
        break;
    }
    case SDL_KEYUP: {
        strack->color = strack_default;
        /* TODO: judge note */
        break;
    }
    default:
        break;
    }
}
void UpdateSTrack(SoundTrack* strack) {
    if (*strack->cursor != '\0') {
        strack->cursor++;
        if (*strack->cursor != '_') {
            Vector2 init_coord;
            switch (*strack->cursor) {
            case '#':
                init_coord.x = strack->judge_coord.x -
                    (strack->judge_coord.y + NOTE_DEFAULT_RADIUS) / strack->base_ray.y * strack->base_ray.x;
                init_coord.y = NOTE_DEFAULT_RADIUS;
                InitNote(strack->cache[strack->cache_id],
                    init_coord, strack->judge_coord, strack->color, NOTE_NORMAL_TICK);
                break;
            default:
                break;
            }
            strack->cache_id = (strack->cache_id + 1) % strack->cache_size;
        }
        for (size_t i = 0; i < strack->cache_size; i++) {
            MoveNote(strack->cache[i]);
        }
    }
}
void DrawSTrack(SDL_Renderer* renderer, SoundTrack* strack) {
    SDL_SetRenderDrawColor(renderer, strack->color.r, strack->color.g, strack->color.b, strack->color.a);
    DrawCircle(renderer, strack->judge_coord.x, strack->judge_coord.y, STRACK_DEFAULT_RADIUS);
}
void DestroySTrack(SoundTrack* strack) {
    if (strack != NULL) {
        if (strack->cache != NULL) {
            for (size_t i = 0; i < strack->cache_size; i++) {
                if (strack->cache[i] != NULL) DestroyNote(strack->cache[i]);
            }
            free(strack->cache);
        }
        // if (strack->sound_track != NULL) free(strack->sound_track);
        free(strack);
    }
}