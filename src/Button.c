#include "../inc/Button.h"

Button* CreateButton(float x, float y, const char* text) {
    Button* button = malloc(sizeof(Button));
    if (button == NULL) {
        isError = 1;
        printf("[Button]Failed to malloc button\n");
        return button;
    }
    button->color = button_default;
    button->rect.x = x, button->rect.y = y;
    button->text = text;
    size_t len = strlen(text);
    button->rect.w = len * WIDTH_PER_LETTER, button->rect.h = HEIGHT_PER_LETTER;
    return button;
}
void DrawButton(SDL_Renderer* renderer, Button* button, TTF_Font* font) {
    SDL_Surface* sur = TTF_RenderText_Blended(font, button->text, button->color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, sur);
    SDL_RenderCopyF(renderer, texture, NULL, &button->rect);
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(sur);
}
void DestroyButton(Button* button) {
    if (button != NULL) {
        free(button);
    }
}