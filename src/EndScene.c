#include "../inc/EndScene.h"

EndScene* end_scene = NULL;

EndScene* CreateEndScene(SDL_Renderer* renderer) {
    end_scene = malloc(sizeof(EndScene));
    if (end_scene == NULL) {
        printf("[EndScene]Failed to malloc end scene\n");
        is_error = 1;
        return end_scene;
    }
    end_scene->rating = NULL;

    end_scene->background = IMG_LoadTexture(renderer, END_SCENE_BACKGROUND);
    if (end_scene->background == NULL) {
        printf("[EndScene]Failed to load background: %s\n", IMG_GetError());
        is_error = 1;
        return end_scene;
    }

    end_scene->cur_button = 0;
    InitButton(&end_scene->buttons[0], 100, 480, "Try Again", TryAgain);
    InitButton(&end_scene->buttons[1], 100, 480 + 2 * LETTER_HEIGHT, "Back To Select", BackToSelect);
    InitButton(&end_scene->buttons[2], 100, 480 + 4 * LETTER_HEIGHT, "Back To Menu", BackToMenu);
    return end_scene;
}
void DestroyEndScene() {
    if (end_scene != NULL) {
        for (end_scene->cur_button = 0; end_scene->cur_button < END_SCENE_BUTTON_SIZE; end_scene->cur_button++) {
            FreeButton(&end_scene->buttons[end_scene->cur_button]);
        }
        if (end_scene->background != NULL) {
            SDL_DestroyTexture(end_scene->background);
        }
        free(end_scene);
        end_scene = NULL;
    }
}
void EndSceneRate() {
    if (game_scene == NULL) {
        printf("[EndScene]game scene doesn't exsit\n");
        end_scene->rating = NULL;
        return;
    }
    size_t note_size = 0;
    for (size_t i = 0; i < game_scene->lane_size; i++) {
        note_size += game_scene->lanes[i].note_list.size;
    }
    double rate = (double)score / note_size / PERFECT_HIT_SCORE;
    if (rate >= RATING_S_PERCENTAGE) {
        end_scene->rating = end_scene_ratings[0];
    }
    else if (rate >= RATING_A_PERCENTAGE) {
        end_scene->rating = end_scene_ratings[1];
    }
    else if (rate >= RATING_B_PERCENTAGE) {
        end_scene->rating = end_scene_ratings[2];
    }
    else if (rate >= RATING_C_PERCENTAGE) {
        end_scene->rating = end_scene_ratings[3];
    }
    else {
        end_scene->rating = end_scene_ratings[4];
    }
}
void EndSceneUpdate(SDL_Event* event) {
    if (event->type == SDL_KEYDOWN) {
        switch (event->key.keysym.scancode) {
        case SDL_SCANCODE_W:
        case SDL_SCANCODE_UP: {
            end_scene->cur_button = (end_scene->cur_button - 1 + END_SCENE_BUTTON_SIZE) % END_SCENE_BUTTON_SIZE;
            break;
        }
        case SDL_SCANCODE_S:
        case SDL_SCANCODE_DOWN: {
            end_scene->cur_button = (end_scene->cur_button + 1) % END_SCENE_BUTTON_SIZE;
            break;
        }
        case SDL_SCANCODE_E:
        case SDL_SCANCODE_KP_ENTER: {
            ButtonFunc(&end_scene->buttons[end_scene->cur_button]);
            break;
        }
        default:
            break;
        }
    }
}
void EndSceneDraw(SDL_Renderer* renderer, TTF_Font* font) {
    static int len;
    static char buf[1 << 6];
    static SDL_Rect rect = { .y = 100, .h = LETTER_HEIGHT };

    SDL_RenderCopy(renderer, end_scene->background, NULL, NULL);

    /* Draw Rating */
    if (end_scene->rating != NULL) {
        len = strlen(end_scene->rating);
        rect.y = 100, rect.h = LETTER_HEIGHT;
        rect.w = len * LETTER_WIDTH;
        rect.x = SCREEN_WIDTH / 2 - rect.w / 2;
        DrawText(renderer, rect, end_scene->rating, font, default_colors[0]);
    }

    /* Draw Score */
    len = sprintf(buf, "SCORE: %lu", score);
    rect.y += rect.h;
    rect.w = len * LETTER_WIDTH;
    rect.x = SCREEN_WIDTH / 2 - rect.w / 2;
    DrawText(renderer, rect, buf, font, default_colors[0]);

    len = sprintf(buf, "HISTORY BEST: %lu", history_best);
    rect.y += rect.h;
    rect.w = len * LETTER_WIDTH;
    rect.x = SCREEN_WIDTH / 2 - rect.w / 2;
    DrawText(renderer, rect, buf, font, default_colors[0]);

    for (size_t i = 0; i < END_SCENE_BUTTON_SIZE; i++) {
        ButtonDraw(&end_scene->buttons[i], renderer, font, i == end_scene->cur_button);
    }
}

static void TryAgain() {
    GameSceneReset();
    cur_scene = GAME;
    GameSceneStart();
}
static void BackToSelect() {
    DestroyGameScene();
    cur_scene = SELECT;
}
static void BackToMenu() {
    DestroyGameScene();
    cur_scene = MENU;
}