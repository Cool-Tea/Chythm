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
void ButtonDraw(Button* button, SDL_Renderer* renderer, TTF_Font* font, Uint8 status) {
    DrawText(renderer, button->rect, button->text, font, button_colors[status]);
}