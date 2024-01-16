#include "../inc/Drawer.h"

Assets assets;

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
}

void InitAssets() {
    GetBackgroundImg();
    GetHitPointImg();
    GetNoteImg();
}

void FreeAssets() {
    for (size_t i = 0; i < 2; i++) {
        if (assets.backgrounds[i] != NULL) SDL_DestroyTexture(assets.backgrounds[i]);
    }
    for (size_t i = 0; i < 2; i++) {
        if (assets.hit_points[i] != NULL) SDL_DestroyTexture(assets.hit_points[i]);
    }
    for (size_t i = 0; i < NOTE_TYPE_NUM; i++) {
        if (assets.notes[i] == NULL) SDL_DestroyTexture(assets.notes[i]);
    }
}

void DrawDefaultBackground() {
    SDL_RenderCopy(app.ren, assets.backgrounds[0], NULL, NULL);
}

void DrawDefaultBackgroundPure() {
    SDL_RenderCopy(app.ren, assets.backgrounds[1], NULL, NULL);
}

void DrawText(SDL_Rect rect, const char* text, SDL_Color color) {
    SDL_Surface* sur = TTF_RenderText_Blended(app.font, text, color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(app.ren, sur);
    SDL_RenderCopy(app.ren, texture, NULL, &rect);
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(sur);
}

void DrawCursor(SDL_Rect rect) {
    int center_y = rect.y + (rect.h >> 1);
    thickLineRGBA(
        app.ren,
        rect.x - CURSOR_LEN, center_y,
        rect.x, center_y,
        CURSOR_WIDTH,
        cursor_color.r, cursor_color.g, cursor_color.b, cursor_color.a
    );
    thickLineRGBA(
        app.ren,
        rect.x + rect.w, center_y,
        rect.x + rect.w + CURSOR_LEN, center_y,
        CURSOR_WIDTH,
        cursor_color.r, cursor_color.g, cursor_color.b, cursor_color.a
    );
}

void DrawHitPoint(int x, int y, bool is_down) {
    static SDL_Rect rect = { .h = HIT_POINT_RADIUS << 1, .w = HIT_POINT_RADIUS << 1 };
    rect.x = x - HIT_POINT_RADIUS, rect.y = y - HIT_POINT_RADIUS;
    SDL_RenderCopy(app.ren, assets.hit_points[is_down], NULL, &rect);
}

void DrawSingleNote(int x, int y, int dire_x, int dire_y) {
    static SDL_Rect rect = { .h = NOTE_RADIUS << 1, .w = NOTE_RADIUS << 1 };
    rect.x = x - NOTE_RADIUS, rect.y = y - NOTE_RADIUS;
    double angle = SDL_acos(dire_y / SDL_sqrt(dire_x * dire_x + dire_y * dire_y));
    SDL_RenderCopyEx(app.ren, assets.notes[SINGLE], NULL, &rect, angle, NULL, SDL_FLIP_NONE);
}

void DrawLongNote(int head_x, int head_y, int tail_x, int tail_y, SDL_Color color) {
    filledCircleRGBA(app.ren, head_x, head_y, NOTE_RADIUS, color.r, color.g, color.b, color.a);
    filledCircleRGBA(app.ren, tail_x, tail_y, NOTE_RADIUS, color.r, color.g, color.b, color.a);
    /* TODO: figure out a better way to present */
    // this could vastly decreased the fps
    // thickLineRGBA(renderer, head_x, head_y, tail_x, tail_y, 2 * NOTE_RADIUS, color.r, color.g, color.b, color.a);
    lineRGBA(app.ren, head_x, head_y, tail_x, tail_y, color.r, color.g, color.b, color.a);
}