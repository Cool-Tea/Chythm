#include "GameScene.h"

GameScene* game_scene = NULL;
UpdateData update_data = { NULL, NULL, -1 };

/* chart_path sample: ../saves/test/ */
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
    game_scene->audio = Mix_LoadMUS(buffer);
    if (game_scene->audio == NULL) {
        fprintf(stderr, "[GameScene]Failed to load audio: %s - %s\n", buffer, Mix_GetError());
        app.is_error = 1;
        return -1;
    }
    return 0;
}

#if !USE_DEFAULT_BACKGROUND
static int GetBackground(const char* chart_path, const char* img_path) {
    char buffer[1 << 8];
    CompletePath(buffer, chart_path, img_path);
    SDL_LockMutex(app.mutex);
    game_scene->background = IMG_LoadTexture(app.ren, buffer);
    SDL_UnlockMutex(app.mutex);
    if (game_scene->background == NULL) {
        fprintf(stderr, "[GameScene]Failed to load background: %s\n", IMG_GetError());
        app.is_error = 1;
        return -1;
    }
    return 0;
}
#endif

static SDL_Scancode AllocateKey(int size, int i) {
    int base = KEY_NUM / 2 - size / 2;
    return default_keys[base + i];
}

static int GetLanes(cJSON* lanes, int size) {
    game_scene->lane_size = size;
    game_scene->lanes = malloc(game_scene->lane_size * sizeof(Lane));
    if (game_scene->lanes == NULL) {
        fprintf(stderr, "[GameScene]Failed to malloc lanes\n");
        app.is_error = 1;
        return -1;
    }
    for (int i = 0; i < game_scene->lane_size; i++) {
        cJSON* lane = cJSON_GetArrayItem(lanes, i);
        if (lane == NULL) continue;
        cJSON* lane_index = cJSON_GetObjectItem(lane, "lane");
        cJSON* position = cJSON_GetObjectItem(lane, "position");
        cJSON* x = cJSON_GetObjectItem(position, "x");
        cJSON* y = cJSON_GetObjectItem(position, "y");
        cJSON* note_size = cJSON_GetObjectItem(lane, "note_size");
        InitLane(&game_scene->lanes[lane_index->valueint],
            x->valueint, y->valueint,
            AllocateKey(game_scene->lane_size, lane_index->valueint),
            note_size->valueint
        );
    }
    return 0;
}

static void GetNotes(cJSON* notes, int bpm) {
    int note_size = cJSON_GetArraySize(notes);
    cJSON* note = NULL;
    cJSON_ArrayForEach(note, notes) {
        cJSON* type = cJSON_GetObjectItem(note, "type");
        cJSON* lane = cJSON_GetObjectItem(note, "lane");
        /* TODO: more note */
        cJSON* move = cJSON_GetObjectItem(note, "move");
        switch (type->valueint) {
        case SINGLE: {
            cJSON* start = cJSON_GetArrayItem(move, 0);
            cJSON* start_x = cJSON_GetObjectItem(start, "x");
            cJSON* start_y = cJSON_GetObjectItem(start, "y");
            cJSON* reach_time = cJSON_GetObjectItem(start, "reach_time");
            LaneAddNote(&game_scene->lanes[lane->valueint],
                type->valueint,
                start_x->valueint, start_y->valueint,
                reach_time->valueint < 60000 / bpm ? 0 : reach_time->valueint - 60000 / bpm,
                reach_time->valueint
            );
            break;
        }
        case LONG: {
            for (int i = 0; i < 2; i++) {
                cJSON* start = cJSON_GetArrayItem(move, i);
                cJSON* start_x = cJSON_GetObjectItem(start, "x");
                cJSON* start_y = cJSON_GetObjectItem(start, "y");
                cJSON* reach_time = cJSON_GetObjectItem(start, "reach_time");
                LaneAddNote(&game_scene->lanes[lane->valueint],
                    type->valueint,
                    start_x->valueint, start_y->valueint,
                    reach_time->valueint < 60000 / bpm ? 0 : reach_time->valueint - 60000 / bpm,
                    reach_time->valueint
                );
            }
            NoteLink(
                NoteListBack(&game_scene->lanes[lane->valueint].note_list) - 2,
                NoteListBack(&game_scene->lanes[lane->valueint].note_list) - 1
            );
            break;
        }
        default:
            break;
        }
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
            switch (type->valueint) {
            case TEXT: {
                cJSON* x = cJSON_GetObjectItem(data, "x");
                cJSON* y = cJSON_GetObjectItem(data, "y");
                cJSON* text = cJSON_GetObjectItem(data, "text");
                size_t len = strlen(text->valuestring);
                char* txt = malloc(len + 1);
                strcpy(txt, text->valuestring);
                EventListEmplaceBack(&game_scene->event_list,
                    time->valueint, lasting_time->valueint, TEXT, x->valueint, y->valueint, txt);
                break;
            }
            default:
                break;
            }
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
                    time->valueint, lasting_time->valueint, MOVE, x->valueint, y->valueint);
                EventListEmplaceBack(&game_scene->lanes[lane->valueint].event_list,
                    time->valueint + lasting_time->valueint, 1000, STOP, NULL);
                break;
            }
            case MOVETO: {
                cJSON* x = cJSON_GetObjectItem(data, "x");
                cJSON* y = cJSON_GetObjectItem(data, "y");
                EventListEmplaceBack(&game_scene->lanes[lane->valueint].event_list,
                    time->valueint, lasting_time->valueint, MOVETO, x->valueint, y->valueint);
                EventListEmplaceBack(&game_scene->lanes[lane->valueint].event_list,
                    time->valueint + lasting_time->valueint, 1000, STOP, NULL);
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

static void InitUpdateData() {
    update_data.combo = &game_scene->combo;
    update_data.score = &game_scene->score;
}

GameScene* CreateGameScene(const char* chart_path) {
    game_scene = malloc(sizeof(GameScene));
    if (game_scene == NULL) {
        fprintf(stderr, "[GameScene]Failed to malloc game scene\n");
        app.is_error = 1;
        return game_scene;
    }
    game_scene->is_started = 0;
    game_scene->chart_path = chart_path;

#if !USE_DEFAULT_BACKGROUND
    game_scene->background = NULL;
#endif

    game_scene->audio = NULL;
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

#if !USE_DEFAULT_BACKGROUND
    cJSON* background = cJSON_GetObjectItem(chart, "background");
    if (GetBackground(chart_path, background->valuestring) < 0) {
        cJSON_Delete(file);
        return game_scene;
    }
#endif

    cJSON* lane_size = cJSON_GetObjectItem(chart, "lane_size");
    cJSON* lanes = cJSON_GetObjectItem(chart, "lanes");
    if (GetLanes(lanes, lane_size->valueint) < 0) {
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
    InitUpdateData();
    return game_scene;
}

int ThreadCreateGameScene(void* chart_path) {
    const char* path = chart_path;
    CreateGameScene(path);
    SDL_LockMutex(app.mutex);
    app.is_loaded = 1;
    SDL_UnlockMutex(app.mutex);
    return 0;
}

void DestroyGameScene() {
    if (game_scene != NULL) {

#if !USE_DEFAULT_BACKGROUND
        if (game_scene->background != NULL) SDL_DestroyTexture(game_scene->background);
#endif

        if (game_scene->audio != NULL) Mix_FreeMusic(game_scene->audio);
        FreeEventList(&game_scene->event_list);
        for (size_t i = 0; i < game_scene->lane_size; i++) {
            FreeLane(&game_scene->lanes[i]);
        }
        if (game_scene->lanes != NULL) free(game_scene->lanes);
        free(game_scene);
        game_scene = NULL;
    }
}

void GameSceneReset() {
    const char* chart_path = game_scene->chart_path;
    DestroyGameScene();
    app.is_loaded = 0;
    app.cur_scene = LOAD;
    SDL_Thread* th =
        SDL_CreateThread(
            ThreadCreateGameScene,
            "CreateGameScene",
            (void*)chart_path
        );
    SDL_DetachThread(th);
}

void GameSceneStart() {
    game_scene->score = 0;
    game_scene->combo = 0;
    app.timer.base_time = app.timer.real_time = SDL_GetTicks();
    app.timer.relative_time = 0;
    Mix_PlayMusic(game_scene->audio, 0);
}

static void SaveScore() {
    if (game_scene->score > game_scene->history_score) {
        game_scene->history_score = game_scene->score;
        char buffer[1 << 8];
        CompletePath(buffer, game_scene->chart_path, "score.txt");
        FILE* fp = fopen(buffer, "w+");
        fprintf(fp, "HistoryBestScore = %lu\n", game_scene->score);
        fclose(fp);
    }
}

extern void EndSceneRate();
void GameSceneEnd() {
    /* TODO: do something */
    game_scene->is_started = 0;
    SaveScore();
    EndSceneRate();
    app.cur_scene = END;
}

/**
 * On calling record the current time, return the mark which was recorded last time
*/
static Uint32 GameSceneTimeMark() {
    static Uint32 time = 0;
    Uint32 tem = time;
    time = SDL_GetTicks();
    return tem;
}

void GameScenePause() {
    game_scene->is_started = 0;
    Mix_PauseMusic();
    GameSceneTimeMark();
}

void GameSceneResume() {
    app.timer.base_time += SDL_GetTicks() - GameSceneTimeMark();
    app.timer.real_time = SDL_GetTicks();
    app.timer.relative_time = app.timer.real_time - app.timer.base_time;
    Mix_ResumeMusic();
}

static int GameSceneCheckEnd() {
    if (Mix_PlayingMusic() == 0 && game_scene->is_started) {
        GameSceneEnd();
        return -1;
    }
    return 0;
}

static void GameSceneHandleKeyDown(SDL_Scancode key) {
    switch (key) {
    case SDL_SCANCODE_ESCAPE: {
        GameScenePause();
        app.cur_scene = PAUSE;
        break;
    }
    case SDL_SCANCODE_SPACE: {
        if (!game_scene->is_started) {
            game_scene->is_started = 1;
            if (Mix_PausedMusic()) {
                GameSceneResume();
            }
            else {
                GameSceneStart();
            }
        }
        break;
    }
    default:
        break;
    }
}

void GameSceneHandleKey(SDL_Event* event) {
    if (event->type == SDL_KEYDOWN) {
        GameSceneHandleKeyDown(event->key.keysym.scancode);
    }
    if (!game_scene->is_started) return;
    for (size_t i = 0; i < game_scene->lane_size; i++) {
        LaneHandleKey(&game_scene->lanes[i], event);
    }
}

static void GameSceneHandleEvent() {
    /* TODO: tackle event */
    EventListUpdate(&game_scene->event_list);
}

void GameSceneUpdate() {
    if (!game_scene->is_started) return;
    if (GameSceneCheckEnd() < 0) return;
    for (size_t i = 0; i < game_scene->lane_size; i++) {
        LaneUpdate(&game_scene->lanes[i]);
    }
    GameSceneHandleEvent();
}

static void GameSceneDrawTime() {
    static int len;
    static char buf[1 << 4];
    static SDL_Rect rect = { .h = 20, .y = 800, .x = 0 };
    len = sprintf(buf, "time: %.3lfs", Mix_GetMusicPosition(game_scene->audio));
    rect.w = 10 * len;
    DrawText(rect, buf, default_colors[0]);
}

static void GameSceneDrawScore() {
    static int len;
    static char buf[1 << 4];
    static SDL_Rect rect = { .h = GAME_SCENE_LETTER_HEIGHT, .x = 0 };

    /* Draw Score */
    len = sprintf(buf, "SCORE: %lu", game_scene->score);
    rect.w = GAME_SCENE_LETTER_WIDTH * len, rect.y = 0;
    DrawText(rect, buf, default_colors[0]);

    /* Draw Combo */
    len = sprintf(buf, "COMBO x %u", game_scene->combo);
    rect.w = GAME_SCENE_LETTER_WIDTH * len, rect.y = rect.h;
    DrawText(rect, buf, default_colors[0]);
}

static void GameSceneDrawHitText() {
    static int len, hit_status = -1;
    static Uint32 time = 0;
    static SDL_Rect rect = { .h = LETTER_HEIGHT, .y = 100 };
    if (hit_status != -1 && update_data.hit_status == hit_status) {
        if (time + GAME_SCENE_TEXT_PERSISTENCE < app.timer.relative_time) {
            hit_status = -1;
            update_data.hit_status = -1;
            return;
        }
        len = strlen(game_scene_texts[hit_status]);
        rect.w = len * LETTER_WIDTH;
        rect.x = 1920 - rect.w;
        DrawText(rect, game_scene_texts[hit_status], default_colors[0]);
    }
    else {
        time = app.timer.relative_time;
        hit_status = update_data.hit_status;
    }
}

static void GameSceneDrawPrompt() {
    static int len;
    static char* prompt = "Press <Space> to continue";
    static SDL_Rect rect = { .h = LETTER_HEIGHT, .y = (SCREEN_HEIGHT - LETTER_HEIGHT) >> 1 };

    len = strlen(prompt);
    rect.w = len * LETTER_WIDTH;
    rect.x = (SCREEN_WIDTH - rect.w) >> 1;

    DrawText(rect, prompt, default_colors[0]);
}

void GameSceneDraw() {
#if USE_DEFAULT_BACKGROUND
    DrawDefaultBackgroundPure();
#else
    SDL_RenderCopy(app.ren, game_scene->background, NULL, NULL);
#endif

    if (!game_scene->is_started) {
        GameSceneDrawPrompt();
        return;
    }

#ifdef DEV
    GameSceneDrawTime();
#endif /* dev */

    GameSceneDrawScore();
    GameSceneDrawHitText();
    for (size_t i = 0; i < game_scene->lane_size; i++) {
        LaneDraw(&game_scene->lanes[i]);
    }
    EventListDraw(&game_scene->event_list);
}