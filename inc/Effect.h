#ifndef _EFFECT_H_
#define _EFFECT_H_

#include <SDL_image.h>
#include "Constants.h"
#include "Globals.h"

struct Particle {
    Uint32 time; // avoid depu update within one frame
    Uint32 cur_frame; // current frame
    Uint32 lasting_frames; // how many frames this particle persist
    SDL_Texture* par_img;
};
typedef struct Particle Particle;

/* by default load png */
static void InitNameParticle(Particle* particle, const char* name, size_t i);
void InitParticle(Particle* particle, EffectType type, size_t i);
void FreeParticle(Particle* particle);
void ParticleReset(Particle* particle);
void ParticleUpdate(Particle* particle);
void ParticleDraw(Particle* particle, int x, int y, int r, double angle);
bool isParticleEnd(Particle* particle);

struct Effect {
    EffectType type;
    bool is_active;
    bool repeat_enable;
    size_t par_size;
    size_t cur_par;
    Particle* particles;
};
typedef struct Effect Effect;

static void InitTypeEffect(Effect* effect, EffectType type);
void InitEffect(Effect* effect, EffectType type, bool repeat_enale);
void FreeEffect(Effect* effect);
void EffectUpdate(Effect* effect);
void EffectDraw(Effect* effect, int x, int y, int r, double angle);
bool isEffectEnd(Effect* effect);

#endif