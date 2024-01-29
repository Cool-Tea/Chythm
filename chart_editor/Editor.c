#include "Editor.h"

int main(int argc, char** args) {
    if (argc != 3) {
        fprintf(stderr, "[Editor]2 parameter needed, but there is %d\n", argc - 1);
        return 0;
    }

    InitEditor(atoi(args[1]), args[2]);
    if (app.is_error) return 0;
    ApplicationStart();
    SDL_Event e;
    while (app.is_running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) app.is_running = 0;
            ApplicationHandleKey(&e);
        }
        ApplicationDraw();
        ApplicationTick();
    }

    QuitEditor();

    return 0;
}

void InitEditor(size_t lane_size, char* audio_path) {
    if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0) {
        fprintf(stderr, "[Editor]Failed to init SDL: %s\n", SDL_GetError());
        app.is_error = 1;
        return;
    }
    // else if (!(IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG) & (IMG_INIT_JPG | IMG_INIT_PNG))) {
    //     fprintf(stderr, "[Editor]Failed to init IMG: %s\n", IMG_GetError());
    //     app.is_error = 1;
    //     return;
    // }
    else if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 4096) < 0) {
        fprintf(stderr, "[Editor]Failed to init MIX: %s\n", Mix_GetError());
        app.is_error = 1;
        return;
    }
    else if (TTF_Init() < 0) {
        fprintf(stderr, "[Editor]Failed to init TTF: %s\n", TTF_GetError());
        app.is_error = 1;
        return;
    }
    InitApplication(lane_size, audio_path);
}

void QuitEditor() {
    FreeApplication();
    TTF_Quit();
    Mix_CloseAudio();
    // IMG_Quit();
    SDL_Quit();
}

void InitApplication(size_t lane_size, char* audio_path) {
    app.win = NULL;
    app.ren = NULL;
    app.audio = NULL;
    app.font = NULL;
    app.json = app.notes = NULL;

    app.win = SDL_CreateWindow(
        "Chart Editor",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH, SCREEN_HEIGHT,
        SDL_WINDOW_OPENGL
    );
    if (app.win == NULL) {
        fprintf(stderr, "[Editor]Failed to create window: %s\n", SDL_GetError());
        app.is_error = 1;
        return;
    }

    app.ren = SDL_CreateRenderer(app.win, -1, SDL_RENDERER_ACCELERATED);
    if (app.ren == NULL) {
        fprintf(stderr, "[Editor]Failed to create renderer: %s\n", SDL_GetError());
        app.is_error = 1;
        return;
    }

    // app.background = IMG_LoadTexture(app.ren, BACKGROUND);
    // if (app.background == NULL) {
    //     fprintf(stderr, "[Editor]Failed to load background: %s\n", IMG_GetError());
    //     app.is_error = 1;
    //     return;
    // }

    app.audio = Mix_LoadMUS(audio_path);
    if (app.audio == NULL) {
        fprintf(stderr, "[Editor]Failed to load music: %s\n", Mix_GetError());
        app.is_error = 1;
        return;
    }

    app.font = TTF_OpenFont(FONT, 128);
    if (app.font == NULL) {
        fprintf(stderr, "[Editor]Failed to load font: %s\n", TTF_GetError());
        app.is_error = 1;
        return;
    }

    app.cur_time = SDL_GetTicks();
    app.key_state = SDL_GetKeyboardState(NULL);
    app.is_error = 0;

    app.json = cJSON_CreateObject();
    app.notes = cJSON_AddArrayToObject(app.json, "notes");
    app.lane_size = lane_size;
    app.notes_size = calloc(app.lane_size, sizeof(size_t));
    app.down_states = calloc(app.lane_size, sizeof(Uint8));
    app.times = calloc(app.lane_size, sizeof(Uint32));
    if (!app.notes_size || !app.down_states || !app.times) {
        fprintf(stderr, "[Editor]Failed to calloc notes/downs/times\n");
        app.is_error = 1;
        return;
    }
}

void FreeApplication() {
    if (app.times) free(app.times);
    if (app.down_states) free(app.down_states);
    if (app.notes_size) free(app.notes_size);
    if (app.json) {
        cJSON_Delete(app.json);
    }
    if (app.font) {
        TTF_CloseFont(app.font);
    }
    if (app.audio) {
        Mix_FreeMusic(app.audio);
    }
    // if (app.background) {
    //     SDL_DestroyTexture(app.background);
    // }
    if (app.ren) {
        SDL_DestroyRenderer(app.ren);
    }
    if (app.win) {
        SDL_DestroyWindow(app.win);
    }
}

void ApplicationStart() {
    app.is_running = 1;
    Mix_PlayMusic(app.audio, 0);
}

static inline int PosX(int index) {
    return 1920u * (index + 1) / (app.lane_size + 1);
}

static void AddSingle(cJSON* notes, Uint32 reach_time, int lane) {
    app.notes_size[lane]++;
    cJSON* note = cJSON_CreateObject();
    cJSON_AddNumberToObject(note, "type", SINGLE);
    cJSON* moves = cJSON_AddArrayToObject(note, "move");
    cJSON* move = cJSON_CreateObject();
    cJSON_AddNumberToObject(move, "lane", lane);
    cJSON_AddNumberToObject(move, "x", PosX(lane));
    cJSON_AddNumberToObject(move, "y", -40);
    cJSON_AddNumberToObject(move, "reach_time", reach_time);
    cJSON_AddItemToArray(moves, move);
    cJSON_AddItemToArray(notes, note);
}

static void AddLong(cJSON* notes, Uint32 reach_time, Uint32 lasting_time, int lane) {
    app.notes_size[lane] += 2;
    cJSON* note = cJSON_CreateObject();
    cJSON_AddNumberToObject(note, "type", LONG);
    cJSON* moves = cJSON_AddArrayToObject(note, "move");
    for (int i = 0; i < 2; i++, reach_time += lasting_time) {
        cJSON* move = cJSON_CreateObject();
        cJSON_AddNumberToObject(move, "lane", lane);
        cJSON_AddNumberToObject(move, "x", PosX(lane));
        cJSON_AddNumberToObject(move, "y", -40);
        cJSON_AddNumberToObject(move, "reach_time", reach_time);
        cJSON_AddItemToArray(moves, move);
    }
    cJSON_AddItemToArray(notes, note);
}

static int KeyToLane(SDL_Scancode key) {
    int index = -1;
    switch (key) {
    case SDL_SCANCODE_S: index = 0; break;
    case SDL_SCANCODE_D: index = 1; break;
    case SDL_SCANCODE_F: index = 2; break;
    case SDL_SCANCODE_G: index = 3; break;
    case SDL_SCANCODE_H: index = 4; break;
    case SDL_SCANCODE_J: index = 5; break;
    case SDL_SCANCODE_K: index = 6; break;
    case SDL_SCANCODE_L: index = 7; break;
    default: return -1;
    }
    return index - ((MAX_KEY_NUM - app.lane_size) >> 1);
}

void JsonHandleKey(SDL_Event* event) {
    if (event->type == SDL_KEYDOWN) {
        switch (event->key.keysym.scancode) {
        case SDL_SCANCODE_P: {
            char* str = cJSON_Print(app.json);
            printf("%s\n", str);
            cJSON_free(str);
            for (size_t i = 0; i < app.lane_size; i++) {
                printf("Lane %lu: %lu notes\n", i, app.notes_size[i]);
            }
            break;
        }
        case SDL_SCANCODE_O: {
            char* str = cJSON_Print(app.json);
            FILE* fp = fopen("notes.json", "w");
            fprintf(fp, "%s\n", str);
            cJSON_free(str);
            fclose(fp);
            for (size_t i = 0; i < app.lane_size; i++) {
                printf("Lane %lu: %lu notes\n", i, app.notes_size[i]);
            }
            break;
        }
        case SDL_SCANCODE_S:
        case SDL_SCANCODE_D:
        case SDL_SCANCODE_F:
        case SDL_SCANCODE_G:
        case SDL_SCANCODE_H:
        case SDL_SCANCODE_J:
        case SDL_SCANCODE_K:
        case SDL_SCANCODE_L: {
            int lane = KeyToLane(event->key.keysym.scancode);
            if (0 <= lane && lane < app.lane_size &&
                !app.down_states[lane]) {
                //printf("key: %s down\n", SDL_GetKeyName(event->key.keysym.sym));
                app.times[lane] = Mix_GetMusicPosition(app.audio) * 1000.0;
                app.down_states[lane] = 1;
            }
            break;
        }
        default:
            break;
        }
    }
    else if (event->type == SDL_KEYUP) {
        switch (event->key.keysym.scancode) {
        case SDL_SCANCODE_S:
        case SDL_SCANCODE_D:
        case SDL_SCANCODE_F:
        case SDL_SCANCODE_G:
        case SDL_SCANCODE_H:
        case SDL_SCANCODE_J:
        case SDL_SCANCODE_K:
        case SDL_SCANCODE_L: {
            int lane = KeyToLane(event->key.keysym.scancode);
            if (app.down_states[lane]) {
                //printf("key: %s up\n", SDL_GetKeyName(event->key.keysym.sym));
                Uint32 time = Mix_GetMusicPosition(app.audio) * 1000.0;
                time -= app.times[lane];
                if (time >= 300) {
                    AddLong(app.notes, app.times[lane], time, lane);
                }
                else {
                    AddSingle(app.notes, app.times[lane], lane);
                }
                app.down_states[lane] = 0;
            }
            break;
        }
        default:
            break;
        }
    }
}

void ApplicationHandleKey(SDL_Event* event) {
    if (event->type == SDL_KEYDOWN) {
        switch (event->key.keysym.scancode) {
        case SDL_SCANCODE_ESCAPE:
        case SDL_SCANCODE_Q: {
            app.is_running = 0;
            break;
        }
        case SDL_SCANCODE_SPACE: {
            if (Mix_PausedMusic()) {
                Mix_ResumeMusic();
            }
            else {
                Mix_PauseMusic();
            }
            break;
        }
        case SDL_SCANCODE_Z: {
            Mix_PauseMusic();
            break;
        }
        case SDL_SCANCODE_X: {
            Mix_ResumeMusic();
            break;
        }
        case SDL_SCANCODE_C: {
            Mix_HaltMusic();
            Mix_PlayMusic(app.audio, 0);
            break;
        }
        case SDL_SCANCODE_LEFT: {
            Mix_SetMusicPosition(Mix_GetMusicPosition(app.audio) - STEP_INTERVAL);
            break;
        }
        case SDL_SCANCODE_RIGHT: {
            Mix_SetMusicPosition(Mix_GetMusicPosition(app.audio) + STEP_INTERVAL);
            break;
        }
        default:
            break;
        }
    }
    JsonHandleKey(event);
}

void DrawText(int x, int y, int w, int h, const char* text) {
    static const SDL_Color white = { 0xff, 0xff, 0xff, 0xff };
    SDL_Rect rect = { .x = x, .y = y, .w = w, .h = h };

    SDL_Surface* sur = TTF_RenderText_Blended(app.font, text, white);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(app.ren, sur);
    SDL_RenderCopy(app.ren, texture, NULL, &rect);
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(sur);
}

void ApplicationDraw() {
    static int len;
    static char buf[1 << 6];

    SDL_RenderClear(app.ren);

    len = sprintf(buf, "%.3lfs", Mix_GetMusicPosition(app.audio));
    DrawText(0, 0, 25 * len, 50, buf);

    SDL_RenderPresent(app.ren);
}

void ApplicationTick() {
    static int fps = 1000 / 240;
    while (!SDL_TICKS_PASSED(SDL_GetTicks(), app.cur_time + fps)) {
        SDL_Delay(1);
    }
    app.cur_time = SDL_GetTicks();
}