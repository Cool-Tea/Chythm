#include "../inc/GameScene.h"

GameScene* game_scene = NULL;
ResetPack reset_pack = { NULL, NULL };

static inline void CompletePath(char* buf, const char* path, const char* file) {
    strcpy(buf, path);
    strcat(buf, file);
}
GameScene* CreateGameScene(SDL_Renderer* renderer, const char* chart_path) {
    game_scene = malloc(sizeof(GameScene));
    if (game_scene == NULL) {
        printf("[GameScene]Failed to malloc game scene\n");
        is_error = 1;
        return game_scene;
    }
    game_scene->background = NULL;
    game_scene->music = NULL;
    game_scene->lanes = NULL;
    game_scene->text = NULL;
    game_scene->hit_status = NULL;

    char* buffer = malloc(1 << 8);
    if (buffer == NULL) {
        printf("[GameScene]Failed to malloc buffer\n");
        is_error = 1;
        return game_scene;
    }
    CompletePath(buffer, chart_path, "./chart.json");
    FILE* fp = fopen(buffer, "r");
    if (fp == NULL) {
        printf("[GameScene]Failed to open chart: %s\n", buffer);
        is_error = 1;
        free(buffer);
        return game_scene;
    }
    fseek(fp, 0, SEEK_END);
    long len = ftell(fp);
    rewind(fp);
    buffer = realloc(buffer, len + 1);
    if (buffer == NULL) {
        printf("[GameScene]Failed to malloc buffer\n");
        is_error = 1;
        fclose(fp);
        return game_scene;
    }
    fread(buffer, 1, len, fp);
    buffer[len] = '\0';
    fclose(fp);

    cJSON* file = cJSON_Parse(buffer);
    if (file == NULL) {
        printf("[GameScene]Failed to parse json: %s\n", cJSON_GetErrorPtr());
        is_error = 1;
        free(buffer);
        return game_scene;
    }

    cJSON* chart = cJSON_GetObjectItem(file, "chart");

    /* Get audio and background */
    cJSON* audio = cJSON_GetObjectItem(chart, "audio");
    CompletePath(buffer, chart_path, audio->valuestring);
    game_scene->music = Mix_LoadMUS(buffer);
    if (game_scene->music == NULL) {
        printf("[GameScene]Failed to load audio: %s - %s\n", buffer, Mix_GetError());
        is_error = 1;
        free(buffer);
        cJSON_Delete(file);
        return game_scene;
    }
    cJSON* background = cJSON_GetObjectItem(chart, "background");
    CompletePath(buffer, chart_path, background->valuestring);
    game_scene->background = IMG_LoadTexture(renderer, buffer);
    if (game_scene->background == NULL) {
        printf("[GameScene]Failed to load background: %s\n", IMG_GetError());
        is_error = 1;
        free(buffer);
        cJSON_Delete(file);
        return game_scene;
    }

    /* Get lanes info */
    cJSON* lane_size = cJSON_GetObjectItem(chart, "lane_size");
    game_scene->lane_size = lane_size->valueint;
    game_scene->lanes = malloc(game_scene->lane_size * sizeof(Lane));
    if (game_scene->lanes == NULL) {
        printf("[GameScene]Failed to malloc lanes\n");
        is_error = 1;
        free(buffer);
        cJSON_Delete(file);
        return game_scene;
    }
    for (size_t i = 0; i < game_scene->lane_size; i++) {
        InitLane(&game_scene->lanes[i]);
    }
    if (is_error) {
        printf("[GameScene]Failed to init lanes\n");
        free(buffer);
        cJSON_Delete(file);
        return game_scene;
    }

    /* Get notes */
    cJSON* notes = cJSON_GetObjectItem(chart, "notes");
    cJSON* bpm = cJSON_GetObjectItem(chart, "bpm");
    int note_size = cJSON_GetArraySize(notes);
    for (int i = 0; i < note_size; i++) {
        cJSON* note = cJSON_GetArrayItem(notes, i);
        cJSON* type = cJSON_GetObjectItem(note, "type");
        cJSON* lane = cJSON_GetObjectItem(note, "lane");
        cJSON* move = cJSON_GetObjectItem(note, "move");
        cJSON* start = cJSON_GetObjectItem(move, "start");
        cJSON* start_x = cJSON_GetObjectItem(start, "x");
        cJSON* start_y = cJSON_GetObjectItem(start, "y");
        cJSON* end = cJSON_GetObjectItem(move, "end");
        cJSON* end_x = cJSON_GetObjectItem(end, "x");
        cJSON* end_y = cJSON_GetObjectItem(end, "y");
        /* TODO: more note */
        cJSON* time = cJSON_GetObjectItem(note, "time");
        NoteListEmplaceBack(&game_scene->lanes[lane->valueint].note_list,
            type->valueint, time->valueint - 60000u / bpm->valueint, time->valueint,
            start_x->valueint, start_y->valueint, end_x->valueint, end_y->valueint
        );
    }

    /* Get hit points */
    cJSON* hit_points = cJSON_GetObjectItem(chart, "hit_points");
    for (int i = 0; i < game_scene->lane_size; i++) {
        cJSON* hit_point = cJSON_GetArrayItem(hit_points, i);
        if (hit_point == NULL) continue;
        cJSON* lane = cJSON_GetObjectItem(hit_point, "lane");
        cJSON* position = cJSON_GetObjectItem(hit_point, "position");
        cJSON* x = cJSON_GetObjectItem(position, "x");
        cJSON* y = cJSON_GetObjectItem(position, "y");
        game_scene->lanes[lane->valueint].hit_point.cur_x = x->valueint;
        game_scene->lanes[lane->valueint].hit_point.cur_y = y->valueint;
        game_scene->lanes[lane->valueint].hit_point.key = default_keys[lane->valueint];
    }

    /* Get events */
    InitEventList(&game_scene->event_list);
    cJSON* events = cJSON_GetObjectItem(chart, "events");
    int event_size = cJSON_GetArraySize(events);
    for (int i = 0; i < event_size; i++) {
        cJSON* event = cJSON_GetArrayItem(events, i);
        cJSON* object = cJSON_GetObjectItem(event, "object");
        cJSON* type = cJSON_GetObjectItem(event, "type");
        cJSON* time = cJSON_GetObjectItem(event, "time");
        cJSON* data = cJSON_GetObjectItem(event, "data");
        switch (object->valueint) {
        case GAME_SCENE: {
            /* TODO: More Type*/
            cJSON* text = cJSON_GetObjectItem(data, "text");
            size_t len = strlen(text->valuestring);
            char* txt = malloc(len + 1);
            strcpy(txt, text->valuestring);
            EventListEmplaceBack(&game_scene->event_list,
                time->valueint, type->valueint, txt);
            break;
        }
        case LANE: {
            /* TODO: More Type*/
            cJSON* lane = cJSON_GetObjectItem(event, "lane");
            switch (type->valueint) {
            case MOVE: {
                cJSON* x = cJSON_GetObjectItem(data, "x");
                cJSON* y = cJSON_GetObjectItem(data, "y");
                int* coord = malloc(2 * sizeof(int));
                coord[0] = x->valueint, coord[1] = y->valueint;
                EventListEmplaceBack(&game_scene->lanes[lane->valueint].event_list,
                    time->valueint, type->valueint, coord);
                break;
            }
            case STOP: {
                EventListEmplaceBack(&game_scene->lanes[lane->valueint].event_list,
                    time->valueint, type->valueint, NULL);
                break;
            }
            default:
                break;
            }
            break;
        }
        default:
            break;
        }
    }

    cJSON_Delete(file);
    free(buffer);

    /* Reset Pack */
    reset_pack.chart_path = chart_path;
    reset_pack.ren = renderer;

    return game_scene;
}
void DestroyGameScene() {
    if (game_scene != NULL) {
        if (game_scene->background != NULL) SDL_DestroyTexture(game_scene->background);
        if (game_scene->music != NULL) Mix_FreeMusic(game_scene->music);
        FreeEventList(&game_scene->event_list);
        if (game_scene->lanes != NULL) free(game_scene->lanes);
        free(game_scene);
        game_scene = NULL;
    }
}
void GameSceneReset() {
    DestroyGameScene();
    CreateGameScene(reset_pack.ren, reset_pack.chart_path);
}
void GameSceneStart() {
    score = 0;
    game_scene->cur_time = game_scene->base_time = SDL_GetTicks();
    game_scene->relative_time = 0;
    Mix_PlayMusic(game_scene->music, 0);
}
extern void EndSceneRate();
void GameSceneEnd() {
    cur_scene = END;
    EndSceneRate();
}
void GameScenePause() {
    Mix_PauseMusic();
    game_scene->cur_time = SDL_GetTicks();
}
void GameSceneResume() {
    game_scene->base_time += SDL_GetTicks() - game_scene->cur_time;
    game_scene->cur_time = SDL_GetTicks();
    game_scene->relative_time = game_scene->cur_time - game_scene->base_time;
    Mix_ResumeMusic();
}
void GameSceneUpdate(SDL_Event* event) {
    /* Music checking */
    if (Mix_PlayingMusic() == 0) {
        GameSceneEnd();
        return;
    }

    /* Key Handling */
    if (event->type == SDL_KEYDOWN) {
        switch (event->key.keysym.scancode) {
        case SDL_SCANCODE_ESCAPE: {
            GameScenePause();
            cur_scene = PAUSE;
            return;
        }
        default:
            break;
        }
    }
    /* Time Update */
    game_scene->cur_time = SDL_GetTicks();
    game_scene->relative_time = game_scene->cur_time - game_scene->base_time;
    /* Lane Update */
    for (size_t i = 0; i < game_scene->lane_size; i++) {
        LaneUpdate(&game_scene->lanes[i], game_scene->relative_time, event, &game_scene->hit_status);
    }
    /* Event Update */
    while (game_scene->event_list.cur_event - game_scene->event_list.events < game_scene->event_list.size &&
        game_scene->event_list.cur_event->time <= game_scene->relative_time) {
        /* TODO: perform more event */
        switch (game_scene->event_list.cur_event->type) {
        case TEXT: {
            game_scene->text = game_scene->event_list.cur_event->data;
            break;
        }
        default:
            break;
        }
        game_scene->event_list.cur_event++;
    }
}
void GameSceneDraw(SDL_Renderer* renderer, TTF_Font* font) {
    SDL_RenderCopy(renderer, game_scene->background, NULL, NULL);

    static int len;
    static char buf[1 << 4];
    static SDL_Rect rect;

#ifdef DEV
    len = sprintf(buf, "time: %us", game_scene->relative_time / 1000);
    rect.w = 10 * len, rect.h = 20, rect.x = 0, rect.y = 800;
    DrawText(renderer, rect, buf, font, default_colors[0]);
#endif /* dev */

    /* TODO: draw event stuff */
    if (game_scene->text != NULL) {
        len = strlen(game_scene->text);
        rect.w = GAME_SCENE_LETTER_WIDTH * len, rect.h = GAME_SCENE_LETTER_HEIGHT,
            rect.x = SCREEN_WIDTH / 2 - GAME_SCENE_LETTER_WIDTH * len / 2, rect.y = 0;
        DrawText(renderer, rect, game_scene->text, font, default_colors[0]);
    }

    if (game_scene->hit_status != NULL) {
        len = strlen(game_scene->hit_status);
        rect.w = LETTER_WIDTH * len, rect.h = LETTER_HEIGHT,
            rect.x = SCREEN_WIDTH - LETTER_WIDTH * len, rect.y = GAME_SCENE_LETTER_HEIGHT;
        DrawText(renderer, rect, game_scene->hit_status, font, default_colors[0]);
    }

    /* Draw Score */
    len = sprintf(buf, "SCORE: %lu", score);
    rect.w = GAME_SCENE_LETTER_WIDTH * len, rect.h = GAME_SCENE_LETTER_HEIGHT, rect.x = 0, rect.y = 0;
    DrawText(renderer, rect, buf, font, default_colors[0]);

    for (size_t i = 0; i < game_scene->lane_size; i++) {
        LaneDraw(&game_scene->lanes[i], renderer);
    }
}