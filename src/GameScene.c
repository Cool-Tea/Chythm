#include "GameScene.h"

GameScene* game_scene = NULL;
UpdateData update_data = { NULL, NULL, -1 };

/* chart_path sample: ../saves/test/ */
static inline void CompletePath(char* buf, const char* path, const char* file) {
    strcpy(buf, path);
    strcat(buf, file);
}

static cJSON* OpenJson() {
    char* buffer = malloc(1 << 8);
    if (buffer == NULL) {
        fprintf(stderr, "[GameScene]Failed to malloc buffer\n");
        app.is_error = 1;
        return NULL;
    }
    CompletePath(buffer, game_scene->chart_path, "chart.json");
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
    cJSON* chart = cJSON_DetachItemFromObject(file, "chart");
    cJSON_Delete(file);
    return chart;
}

static int GetAudio() {
    char buffer[1 << 8];
    cJSON* audio = cJSON_DetachItemFromObject(game_scene->chart, "audio");
    CompletePath(buffer, game_scene->chart_path, audio->valuestring);
    cJSON_Delete(audio);
    game_scene->audio = Mix_LoadMUS(buffer);
    if (game_scene->audio == NULL) {
        fprintf(stderr, "[GameScene]Failed to load audio: %s - %s\n", buffer, Mix_GetError());
        app.is_error = 1;
        return -1;
    }
    return 0;
}

#if !USE_DEFAULT_BACKGROUND
static int GetBackground() {
    char buffer[1 << 8];
    cJSON* background = cJSON_DetachItemFromObject(game_scene->chart, "background");
    CompletePath(buffer, game_scene->chart_path, background->valuestring);
    cJSON_Delete(background);
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

static int GetLanes() {
    cJSON* size = cJSON_DetachItemFromObject(game_scene->chart, "lane_size");
    cJSON* lanes = cJSON_DetachItemFromObject(game_scene->chart, "lanes");
    game_scene->lane_size = size->valueint;
    cJSON_Delete(size);
    game_scene->lanes = malloc(game_scene->lane_size * sizeof(Lane));
    if (game_scene->lanes == NULL) {
        fprintf(stderr, "[GameScene]Failed to malloc lanes\n");
        app.is_error = 1;
        cJSON_Delete(lanes);
        return -1;
    }
    cJSON* lane = NULL;
    cJSON_ArrayForEach(lane, lanes) {
        cJSON* lane_index = cJSON_GetObjectItem(lane, "lane");
        cJSON* position = cJSON_GetObjectItem(lane, "position");
        cJSON* x = cJSON_GetObjectItem(position, "x");
        cJSON* y = cJSON_GetObjectItem(position, "y");
        cJSON* note_size = cJSON_GetObjectItem(lane, "note_size");
        InitLane(&game_scene->lanes[lane_index->valueint],
            x->valueint, y->valueint,
            AllocateKey(game_scene->lane_size, lane_index->valueint)
        );
    }
    cJSON_Delete(lanes);
    return 0;
}

static void GetNotes() {
    cJSON* bpm = cJSON_GetObjectItem(game_scene->chart, "bpm");
    cJSON* notes = cJSON_GetObjectItem(game_scene->chart, "notes");
    /* TODO: more note */
    for (cJSON* note = notes ? notes->child : NULL; note != NULL; ) {
        cJSON* type = cJSON_GetObjectItem(note, "type");
        cJSON* move = cJSON_GetObjectItem(note, "move");
        cJSON* start = NULL;
        size_t tot = 0;
        Node* nodes[3] = { NULL };
        cJSON_ArrayForEach(start, move) {
            cJSON* reach_time = cJSON_GetObjectItem(start, "reach_time");
            // check if the note is within preload time
            if (!tot && app.timer.relative_time + 60000 / bpm->valueint + GAME_SCENE_PRELOAD_OFFSET < reach_time->valueint) return;
            cJSON* lane = cJSON_GetObjectItem(start, "lane");
            cJSON* start_x = cJSON_GetObjectItem(start, "x");
            cJSON* start_y = cJSON_GetObjectItem(start, "y");
            nodes[tot] = LaneAddNote(&game_scene->lanes[lane->valueint],
                type->valueint,
                start_x->valueint, start_y->valueint,
                reach_time->valueint < 60000 / bpm->valueint ? 0 : reach_time->valueint - 60000 / bpm->valueint,
                reach_time->valueint
            );
            if (tot) NoteLink((Note*)nodes[tot - 1]->value, (Note*)nodes[tot]->value);
            tot++;
        }
        cJSON* next = note->next;
        note = cJSON_DetachItemViaPointer(notes, note);
        cJSON_Delete(note);
        note = next;
    }
}

static void GetEvents() {
    cJSON* events = cJSON_GetObjectItem(game_scene->chart, "events");
    for (cJSON* event = events ? events->child : NULL; event != NULL; ) {
        cJSON* time = cJSON_GetObjectItem(event, "time");
        // check if the event is within preload time
        if (app.timer.relative_time + GAME_SCENE_PRELOAD_OFFSET < time->valueint) return;
        cJSON* object = cJSON_GetObjectItem(event, "object");
        cJSON* type = cJSON_GetObjectItem(event, "type");
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
                Event* new_event = CreateEvent(time->valueint, lasting_time->valueint, TEXT, x->valueint, y->valueint, txt);
                ListEmplaceTail(&game_scene->event_list, new_event);
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
            case MOVE:
            case MOVETO: {
                cJSON* x = cJSON_GetObjectItem(data, "x");
                cJSON* y = cJSON_GetObjectItem(data, "y");
                LaneAddEvent(&game_scene->lanes[lane->valueint],
                    time->valueint, lasting_time->valueint, type->valueint, x->valueint, y->valueint);
                LaneAddEvent(&game_scene->lanes[lane->valueint],
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
        cJSON* next = event->next;
        event = cJSON_DetachItemViaPointer(events, event);
        cJSON_Delete(event);
        event = next;
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
    SDL_LockMutex(app.mutex);
    app.timer.base_time = app.timer.real_time = SDL_GetTicks();
    app.timer.relative_time = 0;
    SDL_UnlockMutex(app.mutex);
    game_scene->chart_path = chart_path;
    game_scene->chart = NULL;

#if !USE_DEFAULT_BACKGROUND
    game_scene->background = NULL;
#endif

    game_scene->audio = NULL;
    game_scene->lanes = NULL;

    game_scene->chart = OpenJson(game_scene->chart_path);
    if (game_scene->chart == NULL) {
        return game_scene;
    }

    if (GetAudio() < 0) {
        return game_scene;
    }

#if !USE_DEFAULT_BACKGROUND
    if (GetBackground() < 0) {
        return game_scene;
    }
#endif

    if (GetLanes() < 0) {
        return game_scene;
    }
    GetNotes();
    InitList(&game_scene->event_list);
    ListSetFreeMethod(&game_scene->event_list, (FreeFunc)DestroyEvent);
    GetEvents();
    GetScore(game_scene->chart_path);
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
        FreeList(&game_scene->event_list);
        for (size_t i = 0; i < game_scene->lane_size; i++) {
            FreeLane(&game_scene->lanes[i]);
        }
        if (game_scene->lanes != NULL) free(game_scene->lanes);
        cJSON_Delete(game_scene->chart);
        free(game_scene);
        game_scene = NULL;
    }
}

void GameSceneReload() {
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

static void GameSceneUpdateEvent() {
    Node* ptr = NULL;
    ListForEach(ptr, &game_scene->event_list) {
        Event* event = ptr->value;
        if (app.timer.relative_time < event->time) break;
        else if (app.timer.relative_time < event->time + event->lasting_time) {
            EventUpdate(event);
        }
        else {
            ListErase(&game_scene->event_list, ptr);
        }
    }
}

void GameSceneUpdate() {
    if (!game_scene->is_started) return;
    if (GameSceneCheckEnd() < 0) return;
    for (size_t i = 0; i < game_scene->lane_size; i++) {
        LaneUpdate(&game_scene->lanes[i]);
    }
    GameSceneUpdateEvent();
    GetNotes();
    GetEvents();
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

static void GameSceneDrawEvent() {
    Node* ptr = NULL;
    ListForEach(ptr, &game_scene->event_list) {
        Event* event = ptr->value;
        EventDraw(event);
    }
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
    GameSceneDrawEvent();
}