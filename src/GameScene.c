#include "../inc/GameScene.h"

static inline void CompletePath(char* buf, const char* path, const char* file) {
    strcpy(buf, path);
    strcat(buf, file);
}
GameScene* CreateGameScene(SDL_Renderer* renderer, const char* chart_path) {
    GameScene* game_scene = malloc(sizeof(GameScene));
    if (game_scene == NULL) {
        printf("[GameScene]Failed to malloc game scene\n");
        is_error = 1;
        return game_scene;
    }
    game_scene->background = NULL;
    game_scene->music = NULL;
    game_scene->lanes = NULL;

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
    buffer = realloc(buffer, len + 1);
    if (buffer == NULL) {
        printf("[GameScene]Failed to malloc buffer\n");
        is_error = 1;
        fclose(fp);
        return game_scene;
    }
    fread(buffer, len + 1, 1, fp);
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
        printf("[GameScene]Failed to load audio: %s\n", Mix_GetError());
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
        /* TODO: allocate key for each lane */
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
        switch (type->valueint) {
            /* TODO: more note */
        case SINGLE: {
            cJSON* time = cJSON_GetObjectItem(note, "time");
            NoteListEmplaceBack(&game_scene->lanes[lane->valueint].note_list,
                type->valueint, time->valueint, time->valueint + 60000u / bpm->valueint,
                start_x->valueint, start_y->valueint, end_x->valueint, end_y->valueint
            );
            break;
        }
        case LONG: {
            cJSON* start_time = cJSON_GetObjectItem(note, "start_time");
            cJSON* end_time = cJSON_GetObjectItem(note, "end_time");
            NoteListEmplaceBack(&game_scene->lanes[lane->valueint].note_list,
                type->valueint, start_time->valueint, end_time->valueint,
                start_x->valueint, start_y->valueint, end_x->valueint, end_y->valueint
            );
            break;
        }
        default:
            break;
        }
    }

    /* Get hit points */
    cJSON* hit_points = cJSON_GetObjectItem(chart, "hit_points");
    for (int i = 0; i < game_scene->lane_size; i++) {
        cJSON* hit_point = cJSON_GetArrayItem(hit_points, i);
        cJSON* lane = cJSON_GetObjectItem(hit_point, "lane");
        cJSON* position = cJSON_GetObjectItem(hit_point, "position");
        cJSON* x = cJSON_GetObjectItem(position, "x");
        cJSON* y = cJSON_GetObjectItem(position, "y");
        game_scene->lanes[lane->valueint].hit_point.cur_x = x->valueint;
        game_scene->lanes[lane->valueint].hit_point.cur_y = y->valueint;
    }

    /* Get events */
    cJSON* events = cJSON_GetObjectItem(chart, "events");
    int event_size = cJSON_GetArraySize(events);
    for (int i = 0; i < event_size; i++) {
        cJSON* event = cJSON_GetArrayItem(events, i);
        cJSON* object = cJSON_GetObjectItem(event, "object");
        cJSON* type = cJSON_GetObjectItem(event, "type");
        cJSON* time = cJSON_GetObjectItem(event, "time");
        cJSON* data = cJSON_GetObjectItem(event, "data");
        switch (object->valueint) {
        case LANE: {
            /* TODO: More Type*/
            cJSON* lane = cJSON_GetObjectItem(event, "lane");
            cJSON* x = cJSON_GetObjectItem(data, "x");
            cJSON* y = cJSON_GetObjectItem(data, "y");
            int* coord = malloc(2 * sizeof(int));
            coord[0] = x->valueint, coord[1] = x->valueint;
            EventListEmplaceBack(&game_scene->lanes[lane->valueint].event_list,
                time->valueint, type->valueint, coord);
        }
        case GAME_SCENE: {
            /* TODO: More Type*/
            cJSON* text = cJSON_GetObjectItem(data, "text");
            size_t len = strlen(text->valuestring);
            char* txt = malloc(len + 1);
            strcpy(txt, text->valuestring);
            EventListEmplaceBack(&game_scene->event_list,
                time->valueint, type->valueint, txt);
        }
        default:
            break;
        }
    }

    cJSON_Delete(file);
    free(buffer);
    return game_scene;
}
void DestroyGameScene(GameScene* game_scene) {
    if (game_scene != NULL) {
        if (game_scene->background != NULL) SDL_DestroyTexture(game_scene->background);
        if (game_scene->music != NULL) Mix_FreeMusic(game_scene->music);
        FreeEventList(&game_scene->event_list);
        if (game_scene->lanes != NULL) free(game_scene->lanes);
        free(game_scene);
    }
}
void GameSceneStart(GameScene* game_scene) {
    game_scene->cur_time = game_scene->base_time = SDL_GetTicks();
    game_scene->relative_time = 0;
    Mix_PlayMusic(game_scene->music, 0);
}
void GameScenePause(GameScene* game_scene) {
    game_scene->cur_time = SDL_GetTicks();
    Mix_PauseMusic();
}
void GameSceneResume(GameScene* game_scene) {
    game_scene->base_time += SDL_GetTicks() - game_scene->cur_time;
    game_scene->cur_time = SDL_GetTicks();
    game_scene->relative_time = game_scene->cur_time - game_scene->base_time;
    Mix_ResumeMusic();
}
void GameSceneUpdate(GameScene* game_scene, SDL_Event* event) {
    game_scene->cur_time = SDL_GetTicks();
    game_scene->relative_time = game_scene->cur_time - game_scene->base_time;
    for (size_t i = 0; i < game_scene->lane_size; i++) {
        LaneUpdate(&game_scene->lanes[i], game_scene->relative_time, event);
    }
    /* Event Update */
    while (game_scene->event_list.cur_event - game_scene->event_list.events < game_scene->event_list.size &&
        game_scene->event_list.cur_event->time <= game_scene->relative_time) {
        /* TODO: perform event */
        game_scene->event_list.cur_event++;
    }
}
void GameSceneDraw(GameScene* game_scene, SDL_Renderer* renderer) {
    SDL_RenderCopy(renderer, game_scene->background, NULL, NULL);
    /* TODO: draw event stuff */
    for (size_t i = 0; i < game_scene->lane_size; i++) {
        LaneDraw(&game_scene->lanes[i], renderer);
    }
}