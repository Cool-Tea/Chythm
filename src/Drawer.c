#include "Drawer.h"

Assets assets;

#if USE_DEFAULT_BACKGROUND
static void GetBackgroundImg() {
    assets.backgrounds[0] = IMG_LoadTexture(app.ren, DEFAULT_BACKGROUND);
    if (assets.backgrounds[0] == NULL) {
        fprintf(stderr, "Failed to load background image (%s): %s\n", DEFAULT_BACKGROUND, IMG_GetError());
        app.is_error = 1;
    }
    assets.backgrounds[1] = IMG_LoadTexture(app.ren, DEFAULT_BACKGROUND_PURE);
    if (assets.backgrounds[0] == NULL) {
        fprintf(stderr, "Failed to load background image (%s): %s\n", DEFAULT_BACKGROUND_PURE, IMG_GetError());
        app.is_error = 1;
    }
}
#endif

static void GetHitPointImg() {
    assets.hit_points[0] = IMG_LoadTexture(app.ren, HIT_POINT_DEFAULT_IMG);
    if (assets.hit_points[0] == NULL) {
        fprintf(stderr, "Failed to load hit point image (%s): %s\n", HIT_POINT_DEFAULT_IMG, IMG_GetError());
        app.is_error = 1;
    }
    assets.hit_points[1] = IMG_LoadTexture(app.ren, HIT_POINT_DOWN_IMG);
    if (assets.hit_points[1] == NULL) {
        fprintf(stderr, "Failed to load hit point image (%s): %s\n", HIT_POINT_DOWN_IMG, IMG_GetError());
        app.is_error = 1;
    }
}

#if !NOTE_ONLY_EFFECT
static void GetNoteImg() {
    /* TODO: other images */
    for (size_t i = 0; i < NOTE_TYPE_NUM; i++) {
        assets.notes[i] = NULL; // this is temporary
    }

    assets.notes[SINGLE] = IMG_LoadTexture(app.ren, SINGLE_NOTE_IMG);
    if (assets.hit_points[1] == NULL) {
        fprintf(stderr, "Failed to load single note image (%s): %s\n", SINGLE_NOTE_IMG, IMG_GetError());
        app.is_error = 1;
    }

    assets.notes[LONG] = IMG_LoadTexture(app.ren, LONG_NOTE_IMG);
    if (assets.hit_points[1] == NULL) {
        fprintf(stderr, "Failed to load single note image (%s): %s\n", SINGLE_NOTE_IMG, IMG_GetError());
        app.is_error = 1;
    }
}
#endif

void InitAssets() {
#if USE_DEFAULT_BACKGROUND
    GetBackgroundImg();
#endif

    GetHitPointImg();

#if !NOTE_ONLY_EFFECT
    GetNoteImg();
#endif

}

void FreeAssets() {
#if USE_DEFAULT_BACKGROUND
    for (size_t i = 0; i < 2; i++) {
        if (assets.backgrounds[i] != NULL) SDL_DestroyTexture(assets.backgrounds[i]);
    }
#endif

    for (size_t i = 0; i < 2; i++) {
        if (assets.hit_points[i] != NULL) SDL_DestroyTexture(assets.hit_points[i]);
    }

#if !NOTE_ONLY_EFFECT
    for (size_t i = 0; i < NOTE_TYPE_NUM; i++) {
        if (assets.notes[i] == NULL) SDL_DestroyTexture(assets.notes[i]);
    }
#endif

}

#if USE_DEFAULT_BACKGROUND
void DrawDefaultBackground() {
    SDL_RenderCopy(app.ren, assets.backgrounds[0], NULL, NULL);
}

void DrawDefaultBackgroundPure() {
    SDL_RenderCopy(app.ren, assets.backgrounds[1], NULL, NULL);
}
#endif

void DrawText(SDL_Rect rect, const char* text, SDL_Color color) {
#if AUTO_RESOLUTION
    rect.x *= app.zoom_rate.w, rect.y *= app.zoom_rate.h;
    rect.w *= app.zoom_rate.w, rect.h *= app.zoom_rate.h;
#endif

    SDL_Surface* sur = TTF_RenderText_Blended(app.font, text, color);
    if (app.cur_scene == LOAD) SDL_LockMutex(app.mutex);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(app.ren, sur);
    SDL_RenderCopy(app.ren, texture, NULL, &rect);
    SDL_DestroyTexture(texture);
    if (app.cur_scene == LOAD) SDL_UnlockMutex(app.mutex);
    SDL_FreeSurface(sur);
}

void DrawCursor(SDL_Rect rect) {
    int center_y = rect.y + (rect.h >> 1);
    thickLineRGBA(
        app.ren,

#if AUTO_RESOLUTION
        (rect.x - CURSOR_LEN) * app.zoom_rate.w, center_y * app.zoom_rate.h,
        rect.x * app.zoom_rate.w, center_y * app.zoom_rate.h,
        CURSOR_WIDTH * app.zoom_rate.w,
#else
        rect.x - CURSOR_LEN, center_y,
        rect.x, center_y,
        CURSOR_WIDTH,
#endif

        cursor_color.r, cursor_color.g, cursor_color.b, cursor_color.a
    );
    thickLineRGBA(
        app.ren,

#if AUTO_RESOLUTION
        (rect.x + rect.w) * app.zoom_rate.w, center_y * app.zoom_rate.w,
        (rect.x + rect.w + CURSOR_LEN) * app.zoom_rate.w, center_y * app.zoom_rate.h,
        CURSOR_WIDTH * app.zoom_rate.w,
#else
        rect.x + rect.w, center_y,
        rect.x + rect.w + CURSOR_LEN, center_y,
        CURSOR_WIDTH,
#endif

        cursor_color.r, cursor_color.g, cursor_color.b, cursor_color.a
    );
}
