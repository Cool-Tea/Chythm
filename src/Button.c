#include "../inc/Button.h"

void InitButton(Button* button, int x, int y, const char* text, OnHit func) {
    size_t len = strlen(text);
    button->rect.x = x, button->rect.y = y;
    button->rect.w = len * LETTER_WIDTH, button->rect.h = LETTER_HEIGHT;
    button->text = malloc(len + 1);
    if (button->text == NULL) {
        printf("[Button]Failed to malloc button\n");
        is_error = 1;
    }
    strcpy(button->text, text);
    button->func = func;
}
void FreeButton(Button* button) {
    button->func = NULL;
    free(button->text);
}
void ButtonFunc(Button* button) {
    if (button->func != NULL) {
        button->func();
    }
}
void ButtonDraw(Button* button, SDL_Renderer* renderer, TTF_Font* font) {
    SDL_Surface* sur = TTF_RenderText_Blended(font, button->text, button_colors[0]);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, sur);
    SDL_RenderCopy(renderer, texture, &button->rect, NULL);
    SDL_FreeSurface(sur);
    SDL_DestroyTexture(texture);
}