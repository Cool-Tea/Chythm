#include "../inc/SelectScene.h"

SelectScene* select_scene = NULL;

void InitChartList(ChartList* list, const char* saves_path) {
    list->saves_path = malloc(strlen(saves_path) + 1);
    if (list->saves_path == NULL) {
        printf("[ChartList]Failed to malloc saves path\n");
        is_error = 1;
        return;
    }
    strcpy(list->saves_path, saves_path);
    list->size = 0;
    list->capacity = CHART_LIST_INIT_CAPACITY;
    list->cur_chart = 0;
    list->charts = malloc(list->capacity * sizeof(ChartInfo));
    if (list->charts == NULL) {
        printf("[ChartList]Failed to malloc charts\n");
        is_error = 1;
        return;
    }

    /* Read saves dir and initial the list */
    ChartListRefresh(list);
    if (is_error) {
        printf("[ChartList]Failed to init list\n");
    }
}
void FreeChartList(ChartList* list) {
    if (list->saves_path != NULL) {
        free(list->saves_path);
        list->saves_path = NULL;
    }
    if (list->charts != NULL) {
        for (size_t i = 0; i < list->size; i++) {
            if (list->charts[i].chart_path != NULL) {
                free(list->charts[i].chart_path);
            }
            if (list->charts[i].title != NULL) {
                free(list->charts[i].title);
            }
            if (list->charts[i].artist != NULL) {
                free(list->charts[i].artist);
            }
            if (list->charts[i].preview != NULL) {
                free(list->charts[i].preview);
            }
        }
        free(list->charts);
        list->charts = NULL;
    }
}
void ChartListPushBack(ChartList* list,
    const char* path,
    const char* title,
    const char* artist,
    const char* preview
) {
    size_t len = strlen(path);
    list->charts[list->size].chart_path = malloc(len + 1);
    if (list->charts[list->size].chart_path == NULL) {
        printf("[ChartList]Failed to malloc chart path\n");
        is_error = 1;
    }
    else strcpy(list->charts[list->size].chart_path, path);
    len = strlen(title);
    list->charts[list->size].title = malloc(len + 1);
    if (list->charts[list->size].title == NULL) {
        printf("[ChartList]Failed to malloc name\n");
        is_error = 1;
    }
    else strcpy(list->charts[list->size].title, title);
    len = strlen(artist);
    list->charts[list->size].artist = malloc(len + 1);
    if (list->charts[list->size].artist == NULL) {
        printf("[ChartList]Failed to malloc author\n");
        is_error = 1;
    }
    else strcpy(list->charts[list->size].artist, artist);
    len = strlen(preview);
    list->charts[list->size].preview = malloc(len + 1);
    if (list->charts[list->size].preview == NULL) {
        printf("[ChartList]Failed to malloc preview\n");
        is_error = 1;
    }
    else strcpy(list->charts[list->size].preview, preview);
    if (!is_error) list->size++;
}
void ChartListRefresh(ChartList* list) {
    DIR* saves = opendir(list->saves_path);
    if (saves == NULL) {
        printf("[ChartList]Failed to open saves dir\n");
        is_error = 1;
        return;
    }
    struct dirent* chart;
    char buffer[1 << 8];
    while ((chart = readdir(saves)) != NULL && chart->d_name[0] != '.') {
        strcpy(buffer, list->saves_path);
        strcat(buffer, chart->d_name);
        strcat(buffer, "/chart.json");
        FILE* fp = fopen(buffer, "r");
        if (fp == NULL) {
            printf("[ChartList]Failed to open chart: %s\n", buffer);
            continue;
        }
        fseek(fp, 0, SEEK_END);
        long len = ftell(fp);
        rewind(fp);
        char* file = malloc(len + 1);
        if (file == NULL) {
            printf("[ChartList]Failed to malloc json\n");
            fclose(fp);
            continue;
        }
        fread(file, 1, len, fp);
        file[len] = '\0';
        fclose(fp);

        cJSON* json = cJSON_Parse(file);
        cJSON* info = cJSON_GetObjectItem(json, "info");
        cJSON* title = cJSON_GetObjectItem(info, "title");
        cJSON* artist = cJSON_GetObjectItem(info, "artist");
        cJSON* preview = cJSON_GetObjectItem(info, "preview");
        ChartListPushBack(list,
            buffer,
            title->valuestring,
            artist->valuestring,
            preview->valuestring
        );

        cJSON_Delete(json);
        free(file);
    }

    closedir(saves);
}

SelectScene* CreateSelectScene(SDL_Renderer* renderer) {
    select_scene = malloc(sizeof(SelectScene));
    if (select_scene == NULL) {
        printf("[SelectScene]Failed to malloc select scene\n");
        is_error = 1;
        return select_scene;
    }
    select_scene->background = select_scene->preview = select_scene->title = select_scene->artist = NULL;
    for (size_t i = 0; i < CHART_LIST_NAME_MAX_SIZE; i++) select_scene->list[i] = NULL;
    InitChartList(&select_scene->chart_list, SAVES_PATH);
    if (is_error) {
        printf("[SelectScene]Failed to init chart list\n");
    }
    select_scene->background = IMG_LoadTexture(renderer, SELECT_SCENE_BACKGROUND);
    if (select_scene->background == NULL) {
        printf("[SelectScene]Failed to load background: %s\n", IMG_GetError());
        is_error = 1;
    }
    return select_scene;
}
void DestroySelectScene() {
    if (select_scene != NULL) {
        if (select_scene->background != NULL) {
            SDL_DestroyTexture(select_scene->background);
        }
        if (select_scene->preview != NULL) {
            SDL_DestroyTexture(select_scene->preview);
        }
        if (select_scene->background != NULL) {
            SDL_DestroyTexture(select_scene->preview);
        }
        if (select_scene->artist != NULL) {
            SDL_DestroyTexture(select_scene->artist);
        }
        for (size_t i = 0; i < CHART_LIST_NAME_MAX_SIZE; i++) {
            if (select_scene->list[i] != NULL) SDL_DestroyTexture(select_scene->list[i]);
        }
        FreeChartList(&select_scene->chart_list);
        free(select_scene);
    }
}
void SelectSceneUpdate(SDL_Renderer* renderer, SDL_Event* event) {
    if (event->type == SDL_KEYDOWN) {
        switch (event->key.keysym.sym) {
        case SDLK_KP_ENTER: {
            CreateGameScene(renderer, select_scene->chart_list.charts[select_scene->chart_list.cur_chart].chart_path);
            break;
        }
        case SDLK_ESCAPE: {
            cur_scene = MENU;
            break;
        }
        case SDLK_w:
        case SDLK_UP: {
            select_scene->chart_list.cur_chart = (select_scene->chart_list.cur_chart + 1) % select_scene->chart_list.size;
            SDL_DestroyTexture(select_scene->preview);
            SDL_DestroyTexture(select_scene->title);
            SDL_DestroyTexture(select_scene->artist);
            SDL_DestroyTexture(select_scene->list[CHART_LIST_NAME_MAX_SIZE - 1]);
            for (size_t i = CHART_LIST_NAME_MAX_SIZE - 1; i > 0; i--) {
                select_scene->list[i] = select_scene->list[i - 1];
            }
            select_scene->list[0] = NULL;
            select_scene->preview = select_scene->title = select_scene->artist = NULL;
            break;
        }
        case SDLK_d:
        case SDLK_DOWN: {
            select_scene->chart_list.cur_chart = (select_scene->chart_list.cur_chart - 1 + select_scene->chart_list.size) % select_scene->chart_list.size;
            SDL_DestroyTexture(select_scene->preview);
            SDL_DestroyTexture(select_scene->title);
            SDL_DestroyTexture(select_scene->artist);
            SDL_DestroyTexture(select_scene->list[0]);
            for (size_t i = 0; i < CHART_LIST_NAME_MAX_SIZE - 1; i++) {
                select_scene->list[i] = select_scene->list[i + 1];
            }
            select_scene->list[CHART_LIST_NAME_MAX_SIZE - 1] = NULL;
            select_scene->preview = select_scene->title = select_scene->artist = NULL;
            break;
        }
        case SDLK_r: {
            ChartListRefresh(&select_scene->chart_list);
            break;
        }
        default:
            break;
        }
    }
}
void SelectSceneDraw(SDL_Renderer* renderer, TTF_Font* font) {
    SDL_RenderCopy(renderer, select_scene->background, NULL, NULL);
    if (select_scene->preview == NULL)
        select_scene->preview = IMG_LoadTexture(renderer, select_scene->chart_list.charts[select_scene->chart_list.cur_chart].preview);
    SDL_RenderCopy(renderer, select_scene->preview, NULL, &preview_rect);
    if (select_scene->title == NULL) {
        SDL_Surface* sur = TTF_RenderText_Blended(font, select_scene->chart_list.charts[select_scene->chart_list.cur_chart].title, title_color);
        select_scene->title = SDL_CreateTextureFromSurface(renderer, sur);
        SDL_FreeSurface(sur);
    }
    SDL_Rect rect = { .h = LETTER_HEIGHT, .y = preview_rect.y + preview_rect.h + LETTER_HEIGHT };
    size_t len = strlen(select_scene->chart_list.charts[select_scene->chart_list.cur_chart].title);
    rect.w = len * LETTER_WIDTH;
    rect.x = (preview_rect.x + preview_rect.w >> 1) - rect.w;
    SDL_RenderCopy(renderer, select_scene->title, NULL, &rect);
    if (select_scene->artist == NULL) {
        SDL_Surface* sur = TTF_RenderText_Blended(font, select_scene->chart_list.charts[select_scene->chart_list.cur_chart].artist, title_color);
        select_scene->artist = SDL_CreateTextureFromSurface(renderer, sur);
        SDL_FreeSurface(sur);
    }
    len = strlen(select_scene->chart_list.charts[select_scene->chart_list.cur_chart].artist);
    rect.y += 2 * LETTER_HEIGHT;
    rect.w = len * LETTER_WIDTH;
    rect.x = (preview_rect.x + preview_rect.w >> 1) - rect.w;
    SDL_RenderCopy(renderer, select_scene->artist, NULL, &rect);
    /* TODO: draw list */
    for (size_t i = 0, cur = (select_scene->chart_list.cur_chart - CHART_LIST_NAME_MAX_SIZE >> 1 + select_scene->chart_list.size) % select_scene->chart_list.size;
        i < CHART_LIST_NAME_MAX_SIZE;
        i++, cur = (cur + 1) % select_scene->chart_list.size) {
        if (select_scene->list[i] == NULL) {
            SDL_Surface* sur = TTF_RenderText_Blended(font, select_scene->chart_list.charts[cur].title, title_color);
            select_scene->list[i] = SDL_CreateTextureFromSurface(renderer, sur);
            SDL_FreeSurface(sur);
        }
        len = strlen(select_scene->chart_list.charts[cur].title);
        rect.y = (LETTER_HEIGHT + 50) * i + 100;
        rect.w = len * LETTER_WIDTH;
        rect.x = 100;
        SDL_RenderCopy(renderer, select_scene->list[i], NULL, &rect);
    }
}