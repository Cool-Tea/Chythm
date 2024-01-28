#include "EndScene.h"

EndScene* end_scene = NULL;

EndScene* CreateEndScene() {
    end_scene = malloc(sizeof(EndScene));
    if (end_scene == NULL) {
        fprintf(stderr, "[EndScene]Failed to malloc end scene\n");
        app.is_error = 1;
        return end_scene;
    }
    end_scene->rating = NULL;

#if !USE_DEFAULT_BACKGROUND
    end_scene->background = IMG_LoadTexture(app.ren, END_SCENE_BACKGROUND);
    if (end_scene->background == NULL) {
        fprintf(stderr, "[EndScene]Failed to load background: %s\n", IMG_GetError());
        app.is_error = 1;
        return end_scene;
    }
#endif

    InitButton(&end_scene->buttons[0], 100, 480, "Try Again", TryAgain);
    InitButton(&end_scene->buttons[1], 100, 480 + 2 * LETTER_HEIGHT, "Back To Select", BackToSelect);
    InitButton(&end_scene->buttons[2], 100, 480 + 4 * LETTER_HEIGHT, "Back To Menu", BackToMenu);
    end_scene->cur_button = 0;
    end_scene->buttons[end_scene->cur_button].is_on = 1;
    return end_scene;
}

void DestroyEndScene() {
    if (end_scene != NULL) {
        for (end_scene->cur_button = 0; end_scene->cur_button < END_SCENE_BUTTON_SIZE; end_scene->cur_button++) {
            FreeButton(&end_scene->buttons[end_scene->cur_button]);
        }

#if !USE_DEFAULT_BACKGROUND
        if (end_scene->background != NULL) {
            SDL_DestroyTexture(end_scene->background);
        }
#endif

        free(end_scene);
        end_scene = NULL;
    }
}

void EndSceneRate() {
    if (game_scene == NULL) {
        fprintf(stderr, "[EndScene]game scene doesn't exsit\n");
        end_scene->rating = NULL;
        return;
    }
    size_t note_size = 0;
    for (size_t i = 0; i < game_scene->lane_size; i++) {
        note_size += game_scene->lanes[i].note_list.size;
    }
    double rate = (double)game_scene->score / note_size / PERFECT_HIT_SCORE;
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

static void EndSceneHandleKeyDown(SDL_Scancode key) {
    switch (key) {
    case SDL_SCANCODE_E:
    case SDL_SCANCODE_KP_ENTER: {
        ButtonFunc(&end_scene->buttons[end_scene->cur_button]);
        break;
    }
    case SDL_SCANCODE_W:
    case SDL_SCANCODE_UP: {
        end_scene->buttons[end_scene->cur_button].is_on = 0;
        end_scene->cur_button = (end_scene->cur_button - 1 + END_SCENE_BUTTON_SIZE) % END_SCENE_BUTTON_SIZE;
        end_scene->buttons[end_scene->cur_button].is_on = 1;
        break;
    }
    case SDL_SCANCODE_S:
    case SDL_SCANCODE_DOWN: {
        end_scene->buttons[end_scene->cur_button].is_on = 0;
        end_scene->cur_button = (end_scene->cur_button + 1) % END_SCENE_BUTTON_SIZE;
        end_scene->buttons[end_scene->cur_button].is_on = 1;
        break;
    }
    default:
        break;
    }
}

void EndSceneHandleKey(SDL_Event* event) {
    if (event->type == SDL_KEYDOWN) {
        EndSceneHandleKeyDown(event->key.keysym.scancode);
    }
}

void EndSceneUpdate() {
}

static void EndSceneDrawRating() {
    static int len;
    static char buf[1 << 6];
    static SDL_Rect rect = { .y = 100, .h = LETTER_HEIGHT };
    if (end_scene->rating != NULL) {
        len = strlen(end_scene->rating);
        rect.w = len * LETTER_WIDTH;
        rect.x = SCREEN_WIDTH / 2 - rect.w / 2;
        DrawText(rect, end_scene->rating, default_colors[0]);
    }
}

static void EndSceneDrawScore() {
    static int len;
    static char buf[1 << 6];
    static SDL_Rect rect = { .h = LETTER_HEIGHT };

    len = sprintf(buf, "SCORE: %lu", game_scene->score);
    rect.y = 100 + rect.h;
    rect.w = len * LETTER_WIDTH;
    rect.x = SCREEN_WIDTH / 2 - rect.w / 2;
    DrawText(rect, buf, default_colors[0]);

    len = sprintf(buf, "HISTORY BEST: %lu", game_scene->history_score);
    rect.y += rect.h;
    rect.w = len * LETTER_WIDTH;
    rect.x = SCREEN_WIDTH / 2 - rect.w / 2;
    DrawText(rect, buf, default_colors[0]);
}

void EndSceneDraw() {
#if USE_DEFAULT_BACKGROUND
    DrawDefaultBackgroundPure();
#else
    SDL_RenderCopy(app.ren, end_scene->background, NULL, NULL);
#endif
    EndSceneDrawRating();
    EndSceneDrawScore();
    for (size_t i = 0; i < END_SCENE_BUTTON_SIZE; i++) {
        ButtonDraw(&end_scene->buttons[i]);
    }
}

static void TryAgain() {
    GameSceneReload();
}
static void BackToSelect() {
    DestroyGameScene();
    app.cur_scene = SELECT;
}
static void BackToMenu() {
    DestroyGameScene();
    app.cur_scene = MENU;
}