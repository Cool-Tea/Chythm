#pragma once
#ifndef _SOUND_TRACK_H_
#define _SOUND_TRACK_H_

#include "Note.h"

/* Macros here */
#define STRACK_DEFAULT_CACHE_SIZE 20
#define STRACK_DEFAULT_RADIUS 30
/* Macros end */

struct SoundTrack {
    /* position */
    Vector2 base_ray;
    Coordinate judge_coord;

    /* attr */
    SDL_Color color;

    /* note storage */
    const char* sound_track;
    const char* cursor;
    size_t cache_id;
    size_t cache_size;
    Note** cache;
};
typedef struct SoundTrack SoundTrack;
SoundTrack* CreateSTrack(float x, float y, float vx, float vy, SDL_Color color, const char* track_text);
void EventSTrack(SDL_Event* event, SoundTrack* strack);
/**
 * every tick this will be called
 * move cursor to the next position, if cursor encounters note, initalize one note for display
 * at the meanwhile, move all the note that is in track
 *
 * \param strack the sound track
*/
void UpdateSTrack(SoundTrack* strack);
void DrawSTrack(SDL_Renderer* renderer, SoundTrack* strack);
void DestroySTrack(SoundTrack* strack);

#endif