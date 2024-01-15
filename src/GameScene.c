#include "../inc/GameScene.h"

GameScene* game_scene = NULL;

static inline void CompletePath(char* buf, const char* path, const char* file) {
    strcpy(buf, path);
    strcat(buf, file);
}

static cJSON* OpenJson(const char* chart_path) {
    char* buffer = malloc(1 << 8);
    if (buffer == NULL) {
        fprintf(stderr, "[GameScene]Failed to malloc buffer\n");
        app.is_error = 1;
        return NULL;
    }
    CompletePath(buffer, chart_path, "chart.json");
    FILE* fp = fopen(buffer, "r");
    if (fp == NULL) {
        fprintf(stderr, "[GameScene]Failed to open chart: %s\n", buffer);
        app.is_error = 1;
        free(buffer);
        return NULL;
    }
    fseek(fp, 0, SEEK_END);
    long len = ftell(fp);
    rewind(fp);
    buffer = realloc(buffer, len + 1);
    if (buffer == NULL) {
        fprintf(stderr, "[GameScene]Failed to malloc buffer\n");
        app.is_error = 1;
        fclose(fp);
        return NULL;
    }
    fread(buffer, 1, len, fp);
    buffer[len] = '\0';
    fclose(fp);

    cJSON* file = cJSON_Parse(buffer);
    free(buffer);
    return file;
}

static int GetAudio(const char* chart_path, const char* audio_path) {
    char buffer[1 << 8];
    CompletePath(buffer, chart_path, audio_path);
    game_scene->music = Mix_LoadMUS(buffer);
    if (game_scene->music == NULL) {
        fprintf(stderr, "[GameScene]Failed to load audio: %s - %s\n", buffer, Mix_GetError());
        app.is_error = 1;
        return -1;
    }
    return 0;
}

static int GetBackground(const char* chart_path, const char* img_path) {
    char buffer[1 << 8];
    CompletePath(buffer, chart_path, img_path);
    game_scene->background = IMG_LoadTexture(app.ren, buffer);
    if (game_scene->background == NULL) {
        fprintf(stderr, "[GameScene]Failed to load background: %s\n", IMG_GetError());
        app.is_error = 1;
        return -1;
    }
    return 0;
}

static SDL_Scancode AllocateKey(int size, int i) {
    int base = KEY_NUM / 2 - size / 2;
    return default_keys[base + i];
}

static int GetLanes(cJSON* hit_points, int size) {
    game_scene->lane_size = size;
    game_scene->lanes = malloc(game_scene->lane_size * sizeof(Lane));
    if (game_scene->lanes == NULL) {
        fprintf(stderr, "[GameScene]Failed to malloc lanes\n");
        app.is_error = 1;
        return -1;
    }
    for (size_t i = 0; i < game_scene->lane_size; i++) {
        InitLane(&game_scene->lanes[i]);
    }
    for (int i = 0; i < game_scene->lane_size; i++) {
        cJSON* hit_point = cJSON_GetArrayItem(hit_points, i);
        if (hit_point == NULL) continue;
        cJSON* lane = cJSON_GetObjectItem(hit_point, "lane");
        cJSON* position = cJSON_GetObjectItem(hit_point, "position");
        cJSON* x = cJSON_GetObjectItem(position, "x");
        cJSON* y = cJSON_GetObjectItem(position, "y");
        game_scene->lanes[lane->valueint].hit_point.cur_x = x->valueint;
        game_scene->lanes[lane->valueint].hit_point.cur_y = y->valueint;
        game_scene->lanes[lane->valueint].hit_point.key = AllocateKey(game_scene->lane_size, lane->valueint);
    }
    return 0;
}

static void GetNotes(cJSON* notes, int bpm) {
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
        LaneAddNote(&game_scene->lanes[lane->valueint],
            type->valueint,
            start_x->valueint, start_y->valueint,
            time->valueint - 60000u / bpm,
            time->valueint
        );
    }
}

static void GetEvents(cJSON* events) {
    InitEventList(&game_scene->event_list);
    int event_size = cJSON_GetArraySize(events);
    for (int i = 0; i < event_size; i++) {
        cJSON* event = cJSON_GetArrayItem(events, i);
        cJSON* object = cJSON_GetObjectItem(event, "object");
        cJSON* type = cJSON_GetObjectItem(event, "type");
        cJSON* time = cJSON_GetObjectItem(event, "time");
        cJSON* lasting_time = cJSON_GetObjectItem(event, "lasting_time");
        cJSON* data = cJSON_GetObjectItem(event, "data");
        switch (object->valueint) {
        case GAME_SCENE: {
            /* TODO: More Type*/
            cJSON* text = cJSON_GetObjectItem(data, "text");
            size_t len = strlen(text->valuestring);
            char* txt = malloc(len + 1);
            strcpy(txt, text->valuestring);
            EventListEmplaceBack(&game_scene->event_list,
                time->valueint, lasting_time->valueint, type->valueint, txt);
            break;
        }
        case LANE: {
            /* TODO: More Type*/
            cJSON* lane = cJSON_GetObjectItem(event, "lane");
            switch (type->valueint) {
            case MOVE: {
                cJSON* x = cJSON_GetObjectItem(data, "x");
                cJSON* y = cJSON_GetObjectItem(data, "y");
                EventListEmplaceBack(&game_scene->lanes[lane->valueint].event_list,
                    time->valueint, lasting_time->valueint, type->valueint, x->valueint, y->valueint);
                break;
            }
            case STOP: {
                EventListEmplaceBack(&game_scene->lanes[lane->valueint].event_list,
                    time->valueint, lasting_time->valueint, type->valueint, NULL);
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
}

static int GetScore(const char* chart_path) {
    char buffer[1 << 8];
    CompletePath(buffer, chart_path, "score.txt");
    FILE* fp = fopen(buffer, "r");
    if (fp == NULL) {
        fprintf(stderr, "[GameScene]Failed to read histroy score: %s\n", buffer);
        app.is_error = 1;
        return -1;
    }
    int len = fscanf(fp, "HistoryBestScore = %lu", &game_scene->history_score);
    if (len == 0) game_scene->history_score = 0;
    game_scene->score = 0;
    fclose(fp);
    return 0;
}

GameScene* CreateGameScene(const char* chart_path) {
    game_scene = malloc(sizeof(GameScene));
    if (game_scene == NULL) {
        fprintf(stderr, "[GameScene]Failed to malloc game scene\n");
        app.is_error = 1;
        return game_scene;
    }
    game_scene->chart_path = chart_path;
    game_scene->background = NULL;
    game_scene->music = NULL;
    game_scene->lanes = NULL;

    cJSON* file = OpenJson(chart_path);
    if (file == NULL) {
        return game_scene;
    }
    cJSON* chart = cJSON_GetObjectItem(file, "chart");

    cJSON* audio = cJSON_GetObjectItem(chart, "audio");
    if (GetAudio(chart_path, audio->valuestring) < 0) {
        cJSON_Delete(file);
        return game_scene;
    }

    cJSON* background = cJSON_GetObjectItem(chart, "background");
    if (GetBackground(chart_path, background->valuestring) < 0) {
        cJSON_Delete(file);
        return game_scene;
    }

    cJSON* lane_size = cJSON_GetObjectItem(chart, "lane_size");
    cJSON* hit_points = cJSON_GetObjectItem(chart, "hit_points");
    if (GetLanes(hit_points, lane_size->valueint) < 0) {
        cJSON_Delete(file);
        return game_scene;
    }

    cJSON* notes = cJSON_GetObjectItem(chart, "notes");
    cJSON* bpm = cJSON_GetObjectItem(chart, "bpm");
    GetNotes(notes, bpm->valueint);

    cJSON* events = cJSON_GetObjectItem(chart, "events");
    GetEvents(events);

    cJSON_Delete(file);

    GetScore(chart_path);
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
    const char* chart_path = game_scene->chart_path;
    DestroyGameScene();
    CreateGameScene(chart_path);
}

void GameSceneStart() {
    game_scene->score = 0;
    game_scene->combo = 0;
    app.timer.base_time = app.timer.real_time = SDL_GetTicks();
    app.timer.relative_time = 0;
    Mix_PlayMusic(game_scene->music, 0);
}

void GameSceneEnd() {
    /* TODO: do something */
    app.cur_scene = END;
}

void GameScenePause() {
    Mix_PauseMusic();
    app.timer.real_time = SDL_GetTicks();
}

void GameSceneResume() {
    app.timer.base_time += SDL_GetTicks() - app.timer.real_time;
    app.timer.real_time = SDL_GetTicks();
    app.timer.relative_time = app.timer.real_time - app.timer.base_time;
    Mix_ResumeMusic();
}

static void GameSceneCheckEnd() {
    if (Mix_PlayingMusic() == 0) {
        GameSceneEnd();
    }
}

static void GameSceneHandleKey() {
    if (app.key_status[SDL_SCANCODE_ESCAPE]) {
        GameScenePause();
        app.cur_scene = PAUSE;
    }
}

static void GameSceneHandleEvent() {
    /* TODO: tackle event */
    EventListUpdate(&game_scene->event_list);
}

static void GameSceneUpdateTime() {
    app.timer.real_time = SDL_GetTicks();
    app.timer.relative_time = app.timer.real_time - app.timer.base_time;
}

void GameSceneUpdate(SDL_Event* event) {
    GameSceneCheckEnd();
    //GameSceneUpdateTime(); TODO: really need this ?
    if (event->type == SDL_KEYDOWN)
        GameSceneHandleKey();
    for (size_t i = 0; i < game_scene->lane_size; i++) {
        LaneUpdate(&game_scene->lanes[i], event);
    }
    GameSceneHandleEvent();
}

void GameSceneDraw() {
    SDL_RenderCopy(app.ren, game_scene->background, NULL, NULL);

    static int len;
    static char buf[1 << 4];
    static SDL_Rect rect;

#ifdef DEV
    len = sprintf(buf, "time: %us", app.timer.relative_time / 1000);
    rect.w = 10 * len, rect.h = 20, rect.x = 0, rect.y = 800;
    DrawText(app.ren, rect, buf, app.font, default_colors[0]);
#endif /* dev */

    /* TODO: draw event stuff */

    /* Draw Score */
    len = sprintf(buf, "SCORE: %lu", game_scene->score);
    rect.w = GAME_SCENE_LETTER_WIDTH * len, rect.h = GAME_SCENE_LETTER_HEIGHT, rect.x = 0, rect.y = 0;
    DrawText(app.ren, rect, buf, app.font, default_colors[0]);

    /* Draw Combo */
    len = sprintf(buf, "COMBO x %u", game_scene->combo);
    rect.w = GAME_SCENE_LETTER_WIDTH * len, rect.h = GAME_SCENE_LETTER_HEIGHT, rect.x = 0, rect.y = rect.h;
    DrawText(app.ren, rect, buf, app.font, default_colors[0]);

    for (size_t i = 0; i < game_scene->lane_size; i++) {
        LaneDraw(&game_scene->lanes[i]);
    }

    EventListDraw(&game_scene->event_list);
}