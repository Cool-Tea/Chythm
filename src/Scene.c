#include "../inc/Scene.h"

OptionScene* CreateOptionScene(SDL_Renderer* renderer, TTF_Font* font, SceneType scene_type, size_t b_size, const char* text, ...) {
    OptionScene* opscene = malloc(sizeof(OptionScene));
    if (opscene == NULL) {
        isError = 1;
        printf("[OptionScene]Failed to malloc option scene\n");
        return opscene;
    }
    opscene->bkground = NULL;
    opscene->buttons = NULL;
    opscene->font = NULL;

    opscene->bkground = IMG_LoadTexture(renderer, DEFAULT_BKG_PATH);
    if (opscene->bkground == NULL) {
        isError = 1;
        printf("[OptionScene]Failed to load image: %s\n", IMG_GetError());
        return opscene;
    }
    opscene->font = font;
    opscene->buttons_size = b_size;
    opscene->buttons = malloc(b_size * sizeof(Button*));
    if (opscene->buttons == NULL) {
        isError = 1;
        printf("[OptionScene]Failed to malloc buttons\n");
        return opscene;
    }

    va_list arg_list;
    va_start(arg_list, text);
    for (size_t i = 0; i < b_size; i++) {
        opscene->buttons[i] = CreateButton(0.0f, 0.0f, text);
        if (isError) {
            printf("[OptionScene]Failed to create button %zu\n", i);
        }
        switch (scene_type) {
        case START_SCENE:
            opscene->buttons[i]->rect.x = START_SCENE_BUTTON_X;
            opscene->buttons[i]->rect.y = START_SCENE_TOP_BUTTON_Y + i * START_SCENE_BUTTON_INTERVAL + i * HEIGHT_PER_LETTER;
            break;
        case PAUSE_SCENE:
            opscene->buttons[i]->rect.x = SCREEN_WIDTH / 2 - opscene->buttons[i]->rect.w / 2;
            opscene->buttons[i]->rect.y = PAUSE_SCENE_TOP_BUTTON_Y + i * PAUSE_SCENE_BUTTON_INTERVAL + i * HEIGHT_PER_LETTER;
            break;
        default:
            break;
        }
        text = va_arg(arg_list, const char*);
    }
    return opscene;
}
void DrawOptionScene(SDL_Renderer* renderer, OptionScene* opscene) {
    SDL_RenderCopy(renderer, opscene->bkground, NULL, NULL);
    for (size_t i = 0; i < opscene->buttons_size; i++) {
        DrawButton(renderer, opscene->buttons[i], opscene->font);
    }
}
void DestroyOptionScene(OptionScene* opscene) {
    if (opscene != NULL) {
        if (opscene->buttons != NULL) {
            for (size_t i = 0; i < opscene->buttons_size; i++) {
                if (opscene->buttons[i] != NULL) DestroyButton(opscene->buttons[i]);
            }
            free(opscene->buttons);
        }
        opscene->font = NULL;
        if (opscene->bkground != NULL) SDL_DestroyTexture(opscene->bkground);
        free(opscene);
    }
}

SelectScene* CreateSelectScene(SDL_Renderer* renderer, TTF_Font* font) {
    SelectScene* select_scene = malloc(sizeof(SelectScene));
    if (select_scene == NULL) {
        isError = 1;
        printf("[SelectScene]Failed to malloc select scene\n");
        return select_scene;
    }
    select_scene->bkground = NULL;
    select_scene->cur_save_dir = NULL;
    select_scene->font = NULL;
    select_scene->name = NULL;
    select_scene->preview = NULL;
    select_scene->saves_dir = NULL;

    select_scene->bkground = IMG_LoadTexture(renderer, DEFAULT_BKG_PATH);
    if (select_scene->bkground == NULL) {
        isError = 1;
        printf("[SelectScene]Failed to load image: %s\n", IMG_GetError());
        printf("[SelectScene]Failed at: %s\n", DEFAULT_BKG_PATH);
        return select_scene;
    }
    select_scene->font = font;

    select_scene->saves_dir = opendir(SAVE_PATH);
    if (select_scene->saves_dir == NULL) {
        isError = 1;
        printf("[SelectScene]Failed to open save path\n");
        return select_scene;
    }
    select_scene->cur_save_dir = readdir(select_scene->saves_dir);
    if (select_scene->cur_save_dir == NULL) {
        isError = 1;
        printf("[SelectScene]No music is found!\n");
        return select_scene;
    }

    char* path = malloc(SELECT_SCENE_BUFFER_SIZE);
    if (path == NULL) {
        isError = 1;
        printf("[SelectScene]Failed to malloc path\n");
        return select_scene;
    }
    strcpy(path, SAVE_PATH);
    strcat(path, select_scene->cur_save_dir->d_name);
    strcat(path, CONFIG_PATH);

    FILE* fp = fopen(path, "r"); // TODO:
    if (fp == NULL) {
        isError = 1;
        printf("[SelectScene]Failed to open file: %s\n", path);
        return select_scene;
    }
    select_scene->name = malloc(SELECT_SCENE_BUFFER_SIZE);
    if (select_scene->name == NULL) {
        isError = 1;
        printf("[SelectScene]Failed to malloc name\n");
        return select_scene;
    }
    fscanf(fp, "name=%s", select_scene->name);
    select_scene->name_size = strlen(select_scene->name);

    strcpy(path, SAVE_PATH);
    strcat(path, select_scene->cur_save_dir->d_name);
    size_t len = strlen(path);
    path[len] = '/';
    fscanf(fp, " preview=%s", path + len + 1);
    select_scene->preview = IMG_LoadTexture(renderer, path);
    if (select_scene->preview == NULL) {
        isError = 1;
        printf("[SelectScene]Failed to load image: %s\n", IMG_GetError());
        printf("[SelectScene]Failed at: %s\n", path);
        return select_scene;
    }

    fclose(fp);
    free(path);
    return select_scene;
}
void DrawSelectScene(SDL_Renderer* renderer, SelectScene* select_scene) {
    SDL_RenderCopy(renderer, select_scene->bkground, NULL, NULL);
    if (select_scene->cur_save_dir != NULL) {
        /* Draw preview */
        SDL_RenderCopyF(renderer, select_scene->preview, NULL, &preview_rect);
        /* Draw name */
        SDL_FRect name_rect = { preview_rect.x, preview_rect.y + 100.0f,
            select_scene->name_size * WIDTH_PER_LETTER, HEIGHT_PER_LETTER };
        SDL_Surface* sur = TTF_RenderText_Blended(select_scene->font, select_scene->name, button_default);
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, sur);
        SDL_RenderCopyF(renderer, texture, NULL, &name_rect);
        SDL_DestroyTexture(texture);
        SDL_FreeSurface(sur);
    }
}
void DestroySelectScene(SelectScene* select_scene) {
    if (select_scene != NULL) {
        if (select_scene->preview != NULL) SDL_DestroyTexture(select_scene->preview);
        if (select_scene->name != NULL) free(select_scene->name);
        select_scene->cur_save_dir = NULL;
        if (select_scene->saves_dir != NULL) closedir(select_scene->saves_dir);
        select_scene->font = NULL;
        if (select_scene->bkground != NULL) SDL_DestroyTexture(select_scene->bkground);
        free(select_scene);
    }
}

GameScene* CreateGameScene(SDL_Renderer* renderer, const char* save_path) {
    GameScene* game_scene = malloc(sizeof(GameScene));
    if (game_scene == NULL) {
        isError = 1;
        printf("[GameScene]Failed to malloc game scene\n");
        return game_scene;
    }
    game_scene->bgm = NULL;
    game_scene->bkground = NULL;
    game_scene->stracks = NULL;

    char path[SELECT_SCENE_BUFFER_SIZE];
    strcpy(path, save_path);
    char name[SELECT_SCENE_BUFFER_SIZE] = "config.conf";
    char* buf = malloc(GAME_SCENE_BUFFER_SIZE * sizeof(char));
    if (buf == NULL) {
        isError = 1;
        printf("[GameScene]Failed to malloc buffer\n");
        return game_scene;
    }
    strcat(path, name);
    FILE* fp = fopen(path, "r");
    if (fp == NULL) {
        isError = 1;
        printf("[GameScene]Failed to open file: %s\n", path);
        return game_scene;
    }
    fgets(buf, GAME_SCENE_BUFFER_SIZE, fp); // name=
    fgets(buf, GAME_SCENE_BUFFER_SIZE, fp); // preview=

    /* back_ground image */
    strcpy(path, save_path);
    fscanf(fp, " back_ground=%s", name);
    strcat(path, name);
    game_scene->bkground = IMG_LoadTexture(renderer, path);
    if (game_scene->bkground == NULL) {
        isError = 1;
        printf("[GameScene]Failed to load image: %s\n", IMG_GetError());
        return game_scene;
    }

    /* music */
    strcpy(path, save_path);
    fscanf(fp, " game_music=%s", name);
    strcat(path, name);
    game_scene->bgm = Mix_LoadMUS(path);
    if (game_scene->bgm == NULL) {
        isError = 1;
        printf("[GameScene]Failed to load music: %s\n", Mix_GetError());
        return game_scene;
    }

    /* track size */
    fscanf(fp, " track_size=%zu", &game_scene->stracks_size);
    game_scene->stracks = malloc(game_scene->stracks_size * sizeof(SoundTrack*));
    if (game_scene->stracks == NULL) {
        isError = 1;
        printf("[GameScene]Failed to malloc sound tracks\n");
        return game_scene;
    }

    /* sound_tracks */
    strcpy(path, save_path);
    fscanf(fp, " sound_tracks=%s", name);
    strcat(path, name);
    fclose(fp);

    /* read sound tracks */
    fp = fopen(path, "r");
    SDL_Color tem = { 0xff, 0xff, 0xff, 0x00 };
    for (size_t i = 0; i < game_scene->stracks_size; i++) {
        fgets(buf, GAME_SCENE_BUFFER_SIZE, fp);

        /* TODO: read the pos of the strack */
        game_scene->stracks[i] = CreateSTrack(SCREEN_WIDTH / game_scene->stracks_size * i + START_SCENE_BUTTON_X, SCREEN_HEIGHT / 2, 0.0f, 0.0f, tem, buf);
        if (isError) {
            printf("[GameScene]Failed to create sound track %zu\n", i);
            return game_scene;
        }

        buf = malloc(GAME_SCENE_BUFFER_SIZE * sizeof(char));
        if (buf == NULL) {
            isError = 1;
            printf("[GameScene]Failed to malloc buffer %zu\n", i);
            return game_scene;
        }
    }
    fclose(fp);
    free(buf);
    return game_scene;
}
void StartGameScene(GameScene* game_scene) {
    if (Mix_PlayingMusic() == 0) {
        Mix_PlayMusic(game_scene->bgm, -1);
    }
}
void EventGameScene(SDL_Event* event, GameScene* game_scene) {
    switch (event->type) {
    case SDL_KEYDOWN:
    case SDL_KEYUP: {
        for (size_t i = 0; i < game_scene->stracks_size; i++) {
            if (event->key.keysym.sym == default_keys[i]) {
                EventSTrack(event, game_scene->stracks[i]);
                break;
            }
        }
        break;
    }
    default:
        break;
    }
}
void UpdateGameScene(GameScene* game_scene) {

}
void DrawGameScene(SDL_Renderer* renderer, GameScene* game_scene) {
    SDL_RenderCopy(renderer, game_scene->bkground, NULL, NULL);
    for (size_t i = 0; i < game_scene->stracks_size; i++) {
        DrawSTrack(renderer, game_scene->stracks[i]);
    }
}
void DestroyGameScene(GameScene* game_scene) {
    if (game_scene != NULL) {
        if (game_scene->stracks != NULL) {
            for (size_t i = 0; i < game_scene->stracks_size; i++) {
                if (game_scene->stracks[i] != NULL) DestroySTrack(game_scene->stracks[i]);
            }
            free(game_scene->stracks);
        }
        if (game_scene->bgm != NULL) Mix_FreeMusic(game_scene->bgm);
        if (game_scene->bkground != NULL) SDL_DestroyTexture(game_scene->bkground);
        free(game_scene);
    }
}