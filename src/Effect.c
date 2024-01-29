#include "Effect.h"

static void InitNameParticle(Particle* particle, const char* name, size_t i) {
    char buf[1 << 8];
    sprintf(buf, "%s%s_%.2lu.png", PARTICLE_IMG_DIR_PATH, name, i + 1);
    if (app.cur_scene == LOAD) SDL_LockMutex(app.mutex);
    particle->par_img = IMG_LoadTexture(app.ren, buf);
    if (app.cur_scene == LOAD) SDL_UnlockMutex(app.mutex);
    if (particle->par_img == NULL) {
        fprintf(stderr, "[Particle]Failed to load particle image (%s): %s\n", buf, IMG_GetError());
        app.is_error = 1;
    }
}

void InitParticle(Particle* particle, EffectType type, size_t i) {
    particle->time = app.timer.relative_time;
    particle->cur_frame = 0;
    switch (type) {
    case SCORCH: {
        particle->lasting_frames = 5;
        InitNameParticle(particle, "scorch", i);
        break;
    }
    case CIRCLE: {
        particle->lasting_frames = 3;
        InitNameParticle(particle, "circle", i);
        break;
    }
    case MAGIC: {
        particle->lasting_frames = 3;
        InitNameParticle(particle, "magic", i);
        break;
    }
    case MUZZLE: {
        particle->lasting_frames = 3;
        InitNameParticle(particle, "muzzle", i);
        break;
    }
    case STAR: {
        particle->lasting_frames = 3;
        InitNameParticle(particle, "star", i);
        break;
    }
    case TWIRL: {
        particle->lasting_frames = 5;
        InitNameParticle(particle, "twirl", i);
        break;
    }
    case SPARK: {
        particle->lasting_frames = 3;
        InitNameParticle(particle, "spark", i);
        break;
    }
    default:
        break;
    }
}

void FreeParticle(Particle* particle) {
    if (particle->par_img != NULL)
        SDL_DestroyTexture(particle->par_img);
}

void ParticleReset(Particle* particle) {
    particle->cur_frame = 0;
    particle->time = app.timer.relative_time;
}

void ParticleUpdate(Particle* particle) {
    if (app.timer.relative_time != particle->time) {
        particle->cur_frame++;
        particle->time = app.timer.relative_time;
    }
}

void ParticleDraw(Particle* particle, int x, int y, int r, double angle) {
    static SDL_Rect rect;
    rect.x = x - r, rect.y = y - r;
    rect.h = rect.w = r << 1;

#if AUTO_RESOLUTION
    rect.x *= app.zoom_rate.w, rect.y *= app.zoom_rate.h;
    rect.w *= app.zoom_rate.w, rect.h *= app.zoom_rate.h;
#endif

    if (app.cur_scene == LOAD) SDL_LockMutex(app.mutex);
    if (angle == 0.0)
        SDL_RenderCopy(app.ren, particle->par_img, NULL, &rect);
    else
        SDL_RenderCopyEx(app.ren, particle->par_img, NULL, &rect, angle, NULL, SDL_FLIP_NONE);
    if (app.cur_scene == LOAD) SDL_UnlockMutex(app.mutex);
}

bool isParticleEnd(Particle* particle) {
    return particle->cur_frame >= particle->lasting_frames;
}

static void InitTypeEffect(Effect* effect, EffectType type) {
    effect->particles = malloc(effect->par_size * sizeof(Particle));
    if (effect->particles == NULL) {
        fprintf(stderr, "[Effect]Failed to malloc particle\n");
        app.is_error = 1;
        return;
    }
    for (size_t i = 0; i < effect->par_size; i++) {
        InitParticle(&effect->particles[i], type, i);
    }
}

void InitEffect(Effect* effect, EffectType type, bool repeat_enale) {
    effect->type = type;
    effect->is_active = 0;
    effect->repeat_enable = repeat_enale;
    effect->cur_par = 0;
    switch (type) {
    case SCORCH: {
        effect->par_size = 3;
        break;
    }
    case CIRCLE: {
        effect->par_size = 5;
        break;
    }
    case MAGIC: {
        effect->par_size = 5;
        break;
    }
    case MUZZLE: {
        effect->par_size = 5;
        break;
    }
    case STAR: {
        effect->par_size = 9;
        break;
    }
    case TWIRL: {
        effect->par_size = 3;
        break;
    }
    case SPARK: {
        effect->par_size = 5;
        break;
    }
    default:
        break;
    }
    InitTypeEffect(effect, type);
}

void FreeEffect(Effect* effect) {
    if (effect != NULL && effect->particles != NULL) {
        for (size_t i = 0; i < effect->par_size; i++) {
            FreeParticle(&effect->particles[i]);
        }
        free(effect->particles);
    }
}

void EffectUpdate(Effect* effect) {
    if (effect->is_active == 0) return;
    ParticleUpdate(&effect->particles[effect->cur_par]);
    if (isParticleEnd(&effect->particles[effect->cur_par])) {
        ParticleReset(&effect->particles[effect->cur_par++]);
    }
    if (isEffectEnd(effect)) {
        effect->is_active = effect->repeat_enable;
        effect->cur_par = 0;
    }
}

void EffectDraw(Effect* effect, int x, int y, int r, double angle) {
    if (effect->is_active)
        ParticleDraw(&effect->particles[effect->cur_par], x, y, r, angle);
}

bool isEffectEnd(Effect* effect) {
    return effect->cur_par >= effect->par_size;
}