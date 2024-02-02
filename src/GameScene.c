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
        app.error_level = app.error_level > 2 ? app.error_level : 2;
        return NULL;
    }
    CompletePath(buffer, game_scene->chart_path, "chart.json");
    FILE* fp = fopen(buffer, "r");
    if (fp == NULL) {
        fprintf(stderr, "[GameScene]Failed to open chart: %s\n", buffer);
        app.error_level = app.error_level > 2 ? app.error_level : 2;
        free(buffer);
        return NULL;
    }
    fseek(fp, 0, SEEK_END);
    long len = ftell(fp);
    rewind(fp);
    buffer = realloc(buffer, len + 1);
    if (buffer == NULL) {
        fprintf(stderr, "[GameScene]Failed to malloc buffer\n");
        app.error_level = app.error_level > 2 ? app.error_level : 2;
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
        app.error_level = app.error_level > 2 ? app.error_level : 2;
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
        app.error_level = app.error_level > 2 ? app.error_level : 2;
        return -1;
    }
    return 0;
}
#endif

static SDL_Scancode AllocateKey(int size, int i) {
    int base = (KEY_NUM - size) >> 1;
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
        app.error_level = app.error_level > 2 ? app.error_level : 2;
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
    for (cJSON* note = notes ? notes->child : NULL, * next = NULL; note != NULL; note = next) {
        next = note->next;
        cJSON* type = cJSON_GetObjectItem(note, "type");
        cJSON* move = cJSON_GetObjectItem(note, "move");
        cJSON* start = NULL;
        size_t tot = 0;
        Node* nodes[4] = { NULL };
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
        note = cJSON_DetachItemViaPointer(notes, note);
        cJSON_Delete(note);
    }
}

static void GetGameSceneEvents(cJSON* event, cJSON* time, cJSON* type, cJSON* data) {
    switch (type->valueint) {
    case TEXT: {
        cJSON* lasting_time = cJSON_GetObjectItem(event, "lasting_time");
        cJSON* x = cJSON_GetObjectItem(data, "x");
        cJSON* y = cJSON_GetObjectItem(data, "y");
        cJSON* text = cJSON_GetObjectItem(data, "text");
        if (x->type == cJSON_NULL) {
            int len = strlen(text->valuestring);
            cJSON_SetNumberValue(x, ((STD_SCREEN_WIDTH - len * GAME_SCENE_LETTER_WIDTH) >> 1));
        }
        if (y->type == cJSON_NULL) {
            cJSON_SetNumberValue(y, 0);
        }
        Event* new_event = CreateEvent(time->valueint, lasting_time->valueint,
            TEXT, x->valueint, y->valueint, text->valuestring);
        ListEmplaceTail(&game_scene->event_list, new_event);
        break;
    }
    case BPM: {
        cJSON* bpm = cJSON_GetObjectItem(data, "bpm");
        Event* new_event = CreateEvent(time->valueint, time->valueint + 1000,
            BPM, bpm->valueint);
        ListEmplaceTail(&game_scene->event_list, new_event);
        break;
    }
    case EFFECT: {
        cJSON* lasting_time = cJSON_GetObjectItem(event, "lasting_time");
        cJSON* x = cJSON_GetObjectItem(data, "x");
        cJSON* y = cJSON_GetObjectItem(data, "y");
        cJSON* radius = cJSON_GetObjectItem(data, "radius");
        cJSON* angle = cJSON_GetObjectItem(data, "angle");
        cJSON* effect_type = cJSON_GetObjectItem(data, "type");
        cJSON* repeat_en = cJSON_GetObjectItem(data, "repeat");
        Event* new_event = CreateEvent(time->valueint, lasting_time->valueint,
            EFFECT, x->valueint, y->valueint, radius->valueint, angle->valuedouble, effect_type->valueint, repeat_en->valueint);
        ListEmplaceTail(&game_scene->event_list, new_event);
        break;
    }
    case BACKGROUND: {
        cJSON* background = cJSON_GetObjectItem(data, "background");
        Event* new_event = CreateEvent(time->valueint, time->valueint + 1000,
            BACKGROUND, background->valuestring);
        ListEmplaceTail(&game_scene->event_list, new_event);
    }
    default: break;
    }
}

static void GetLaneEvents(cJSON* event, cJSON* time, cJSON* type, cJSON* data) {
    cJSON* lane = cJSON_GetObjectItem(event, "lane");
    switch (type->valueint) {
    case MOVE:
    case MOVETO: {
        cJSON* lasting_time = cJSON_GetObjectItem(event, "lasting_time");
        cJSON* x = cJSON_GetObjectItem(data, "x");
        cJSON* y = cJSON_GetObjectItem(data, "y");
        LaneAddEvent(&game_scene->lanes[lane->valueint],
            time->valueint, lasting_time->valueint, type->valueint, x->valueint, y->valueint);
        LaneAddEvent(&game_scene->lanes[lane->valueint],
            time->valueint + lasting_time->valueint, 1000, STOP, NULL);
        break;
    }
    default: break;
    }
}

static void GetEvents() {
    cJSON* events = cJSON_GetObjectItem(game_scene->chart, "events");
    for (cJSON* event = events ? events->child : NULL, * next; event != NULL; event = next) {
        next = event->next;
        cJSON* time = cJSON_GetObjectItem(event, "time");
        // check if the event is within preload time
        if (app.timer.relative_time + GAME_SCENE_PRELOAD_OFFSET < time->valueint) return;
        cJSON* type = cJSON_GetObjectItem(event, "type");
        cJSON* data = cJSON_GetObjectItem(event, "data");
        /* TODO: More Type*/
        switch (type->valueint & 0xf0) {
        case 0x10: {
            GetGameSceneEvents(event, time, type, data);
            break;
        }
        case 0x20: {
            GetLaneEvents(event, time, type, data);
            break;
        }
        default: break;
        }
        event = cJSON_DetachItemViaPointer(events, event);
        cJSON_Delete(event);
    }
}

static int GetScore(const char* chart_path) {
    char buffer[1 << 8];
    CompletePath(buffer, chart_path, "score.txt");
    FILE* fp = fopen(buffer, "r");
    if (fp == NULL) {
        fprintf(stderr, "[GameScene]Failed to read histroy score: %s\n", buffer);
        app.error_level = app.error_level > 1 ? app.error_level : 1;
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
        app.error_level = app.error_level > 2 ? app.error_level : 2;
        return game_scene;
    }
    game_scene->status = 0;
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
    game_scene->status = 1;
    game_scene->score = 0;
    game_scene->combo = 0;
    app.timer.base_time = app.timer.real_time = SDL_GetTicks();
    app.timer.relative_time = 1;
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
    game_scene->status = 0;
    Mix_HaltMusic();
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
    game_scene->status = 2;
    Mix_PauseMusic();
    GameSceneTimeMark();
}

void GameSceneResume() {
    game_scene->status = 1;
    app.timer.base_time += SDL_GetTicks() - GameSceneTimeMark();
    app.timer.real_time = SDL_GetTicks();
    app.timer.relative_time = app.timer.real_time - app.timer.base_time;
    Mix_ResumeMusic();
}

static void GameSceneHandleKeyDown(SDL_Scancode key) {
    switch (key) {
    case SDL_SCANCODE_ESCAPE: {
        GameScenePause();
        app.cur_scene = PAUSE;
        break;
    }
    case SDL_SCANCODE_SPACE: {
        if (game_scene->status == 2) {
            GameSceneResume();
        }
        else if (game_scene->status == 0) {
            GameSceneStart();
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
    if (game_scene->status != 1) return;
    for (size_t i = 0; i < game_scene->lane_size; i++) {
        LaneHandleKey(&game_scene->lanes[i], event);
    }
}

static void GameSceneExecBpmEvent(Event* event) {
    cJSON* bpm = cJSON_GetObjectItem(game_scene->chart, "bpm");
    cJSON_SetNumberValue(bpm, (*(int*)event->data));
}

static void GameSceneExecBackgroundEvent(Event* event) {
    char path[1 << 8];
    CompletePath(path, game_scene->chart_path, event->data);
    SDL_Texture* background = IMG_LoadTexture(app.ren, path);
    if (background == NULL) {
        fprintf(stderr, "[GameScene]Failed to change background (%s): %s\n", path, IMG_GetError());
        app.error_level = app.error_level > 1 ? app.error_level : 1;
        return;
    }
    SDL_DestroyTexture(game_scene->background);
    game_scene->background = background;
}

static void GameSceneUpdateEvents() {
    for (Node* ptr = game_scene->event_list.head, *next = NULL; ptr != NULL; ptr = next) {
        next = ptr->next;
        Event* event = ptr->value;
        if (app.timer.relative_time < event->time) break;
        else if (app.timer.relative_time < event->time + event->lasting_time) {
            switch (event->type) {
            case BPM: {
                GameSceneExecBpmEvent(event);
                ListErase(&game_scene->event_list, ptr);
                break;
            }
            case BACKGROUND: {
                GameSceneExecBackgroundEvent(event);
                ListErase(&game_scene->event_list, ptr);
                break;
            }
            default: break;
            }
            EventUpdate(event);
        }
        else {
            ListErase(&game_scene->event_list, ptr);
        }
    }
}

void GameSceneUpdate() {
    if (game_scene->status != 1) return;
    else if (!Mix_PlayingMusic() && app.timer.relative_time) {
        GameSceneEnd();
        return;
    }
    for (size_t i = 0; i < game_scene->lane_size; i++) {
        LaneUpdate(&game_scene->lanes[i]);
    }
    GameSceneUpdateEvents();
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
    static SDL_Rect rect = { .h = LETTER_HEIGHT, .y = (STD_SCREEN_HEIGHT - LETTER_HEIGHT) >> 1 };

    len = strlen(prompt);
    rect.w = len * LETTER_WIDTH;
    rect.x = (STD_SCREEN_WIDTH - rect.w) >> 1;

    DrawText(rect, prompt, default_colors[0]);
}

static void GameSceneDrawEvent() {
    Node* ptr = NULL;
    ListForEach(ptr, &game_scene->event_list) {
        EventDraw(ptr->value);
    }
}

void GameSceneDraw() {
#if USE_DEFAULT_BACKGROUND
    DrawDefaultBackgroundPure();
#else
    SDL_RenderCopy(app.ren, game_scene->background, NULL, NULL);
#endif

    if (game_scene->status != 1) {
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