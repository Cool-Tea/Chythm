#include "../inc/Drawer.h"

void DrawText(SDL_Renderer* renderer, SDL_Rect rect, const char* text, TTF_Font* font, SDL_Color color) {
    SDL_Surface* sur = TTF_RenderText_Blended(font, text, color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, sur);
    SDL_RenderCopy(renderer, texture, NULL, &rect);
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(sur);
}
void DrawCursor(SDL_Renderer* renderer, SDL_Rect rect) {
    int center_y = rect.y + (rect.h >> 1);
    thickLineRGBA(
        renderer,
        rect.x - CURSOR_LEN, center_y,
        rect.x, center_y,
        CURSOR_WIDTH,
        cursor_color.r, cursor_color.g, cursor_color.b, cursor_color.a
    );
    thickLineRGBA(
        renderer,
        rect.x + rect.w, center_y,
        rect.x + rect.w + CURSOR_LEN, center_y,
        CURSOR_WIDTH,
        cursor_color.r, cursor_color.g, cursor_color.b, cursor_color.a
    );
}
void DrawSingleNote(SDL_Renderer* renderer, int x, int y, int dire_x, int dire_y, SDL_Color color) {
    filledCircleRGBA(renderer, x, y, NOTE_RADIUS, color.r, color.g, color.b, color.a);
}
void DrawHitPoint(SDL_Renderer* renderer, int x, int y, SDL_Color color) {
    filledCircleRGBA(renderer, x, y, HIT_POINT_RADIUS, color.r, color.g, color.b, color.a);
}