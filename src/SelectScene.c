#include "SelectScene.h"

SelectScene* select_scene = NULL;

void InitChartList(ChartList* list, const char* saves_path) {
    list->saves_path = malloc(strlen(saves_path) + 1);
    if (list->saves_path == NULL) {
        fprintf(stderr, "[ChartList]Failed to malloc saves path\n");
        app.is_error = 1;
        return;
    }
    strcpy(list->saves_path, saves_path);
    list->size = 0;
    list->capacity = CHART_LIST_INIT_CAPACITY;
    list->cur_chart = 0;
    list->charts = malloc(list->capacity * sizeof(ChartInfo));
    if (list->charts == NULL) {
        fprintf(stderr, "[ChartList]Failed to malloc charts\n");
        app.is_error = 1;
        return;
    }

    /* Read saves dir and initial the list */
    ChartListRefresh(list);
    if (app.is_error) {
        fprintf(stderr, "[ChartList]Failed to init list\n");
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
        fprintf(stderr, "[ChartList]Failed to malloc chart path\n");
        app.is_error = 1;
    }
    else strcpy(list->charts[list->size].chart_path, path);

    len = strlen(title);
    list->charts[list->size].title = malloc(len + 1);
    if (list->charts[list->size].title == NULL) {
        fprintf(stderr, "[ChartList]Failed to malloc name\n");
        app.is_error = 1;
    }
    else strcpy(list->charts[list->size].title, title);

    len = strlen(artist);
    list->charts[list->size].artist = malloc(len + 1);
    if (list->charts[list->size].artist == NULL) {
        fprintf(stderr, "[ChartList]Failed to malloc author\n");
        app.is_error = 1;
    }
    else strcpy(list->charts[list->size].artist, artist);

    len = strlen(preview);
    list->charts[list->size].preview = malloc(len + 1);
    if (list->charts[list->size].preview == NULL) {
        fprintf(stderr, "[ChartList]Failed to malloc preview\n");
        app.is_error = 1;
    }
    else strcpy(list->charts[list->size].preview, preview);

    if (!app.is_error) list->size++;
}

void ChartListRefresh(ChartList* list) {
    DIR* saves = opendir(list->saves_path);
    if (saves == NULL) {
        fprintf(stderr, "[ChartList]Failed to open saves dir\n");
        app.is_error = 1;
        return;
    }
    struct dirent* chart;
    char buffer[1 << 8];
    while ((chart = readdir(saves)) != NULL) {
        if (chart->d_name[0] == '.') continue;
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

        len = strlen(buffer);
        buffer[len - 10] = '\0';

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

SelectScene* CreateSelectScene() {
    select_scene = malloc(sizeof(SelectScene));
    if (select_scene == NULL) {
        fprintf(stderr, "[SelectScene]Failed to malloc select scene\n");
        app.is_error = 1;
        return select_scene;
    }

#if !USE_DEFAULT_BACKGROUND
    select_scene->background = NULL;
#endif

    select_scene->preview = NULL;

    InitChartList(&select_scene->chart_list, SAVES_PATH);
    if (app.is_error) {
        fprintf(stderr, "[SelectScene]Failed to init chart list\n");
    }

#if !USE_DEFAULT_BACKGROUND
    select_scene->background = IMG_LoadTexture(app.ren, SELECT_SCENE_BACKGROUND);
    if (select_scene->background == NULL) {
        fprintf(stderr, "[SelectScene]Failed to load background: %s\n", IMG_GetError());
        app.is_error = 1;
    }
#endif

    return select_scene;
}

void DestroySelectScene() {
    if (select_scene != NULL) {

#if !USE_DEFAULT_BACKGROUND
        if (select_scene->background != NULL) {
            SDL_DestroyTexture(select_scene->background);
        }
#endif

        if (select_scene->preview != NULL) {
            SDL_DestroyTexture(select_scene->preview);
        }
        FreeChartList(&select_scene->chart_list);
        free(select_scene);
        select_scene = NULL;
    }
}

static void SelectSceneHandleKeyDown(SDL_Scancode key) {
    switch (key) {
    case SDL_SCANCODE_E:
    case SDL_SCANCODE_KP_ENTER: {
        app.is_loaded = 0;
        app.cur_scene = LOAD;
        SDL_Thread* th =
            SDL_CreateThread(
                ThreadCreateGameScene,
                "CreateGameScene",
                select_scene->chart_list.charts[select_scene->chart_list.cur_chart].chart_path
            );
        SDL_DetachThread(th);
        break;
    }
    case SDL_SCANCODE_W:
    case SDL_SCANCODE_UP: {
        select_scene->chart_list.cur_chart = (select_scene->chart_list.cur_chart - 1 + select_scene->chart_list.size) % select_scene->chart_list.size;
        SDL_DestroyTexture(select_scene->preview);
        select_scene->preview = NULL;
        break;
    }
    case SDL_SCANCODE_S:
    case SDL_SCANCODE_DOWN: {
        select_scene->chart_list.cur_chart = (select_scene->chart_list.cur_chart + 1) % select_scene->chart_list.size;
        SDL_DestroyTexture(select_scene->preview);
        select_scene->preview = NULL;
        break;
    }
    case SDL_SCANCODE_ESCAPE: {
        app.cur_scene = MENU;
        break;
    }
    case SDL_SCANCODE_R: {
        ChartListRefresh(&select_scene->chart_list);
        break;
    }
    default:
        break;
    }
}

void SelectSceneHandleKey(SDL_Event* event) {
    if (event->type == SDL_KEYDOWN) {
        SelectSceneHandleKeyDown(event->key.keysym.scancode);
    }
}

void SelectSceneUpdate() {
}

static void SelectSceneDrawPreview(SDL_Rect rect) {
#if AUTO_RESOLUTION
    rect.x *= app.zoom_rate.w, rect.y *= app.zoom_rate.h;
    rect.w *= app.zoom_rate.w, rect.h *= app.zoom_rate.h;
#endif

    if (select_scene->preview == NULL) {
        char buffer[1 << 8] = { 0 };
        strcat(buffer, select_scene->chart_list.charts[select_scene->chart_list.cur_chart].chart_path);
        strcat(buffer, select_scene->chart_list.charts[select_scene->chart_list.cur_chart].preview);
        select_scene->preview = IMG_LoadTexture(app.ren, buffer);
    }
    if (select_scene->preview == NULL)
        fprintf(stderr, "[SelectScene]Failed to load preview: %s\n", IMG_GetError());
    else
        SDL_RenderCopy(app.ren, select_scene->preview, NULL, &rect);
}

static void SelectSceneDrawInfo() {
    static SDL_Rect rect = { .h = LETTER_HEIGHT };
    static int len;

    /* Draw preview */
    SelectSceneDrawPreview(preview_rect);

    /* Draw title */
    len = strlen(select_scene->chart_list.charts[select_scene->chart_list.cur_chart].title);
    rect.y = preview_rect.y + preview_rect.h + LETTER_HEIGHT;
    rect.w = len * LETTER_WIDTH;
    rect.x = (preview_rect.x + (preview_rect.w >> 1)) - (rect.w >> 1);
    DrawText(rect, select_scene->chart_list.charts[select_scene->chart_list.cur_chart].title, default_colors[0]);

    /* Draw artist */
    len = strlen(select_scene->chart_list.charts[select_scene->chart_list.cur_chart].artist);
    rect.y += LETTER_HEIGHT;
    rect.w = len * LETTER_WIDTH;
    rect.x = (preview_rect.x + (preview_rect.w >> 1)) - (rect.w >> 1);
    DrawText(rect, select_scene->chart_list.charts[select_scene->chart_list.cur_chart].artist, default_colors[0]);
}

static void SelectSceneDrawList() {
    static int len;
    static SDL_Rect rect = { .h = LETTER_HEIGHT };

    for (size_t i = 0,

        cur =
        (select_scene->chart_list.cur_chart - CHART_LIST_NAME_MAX_SIZE / 2 + select_scene->chart_list.size)
        % select_scene->chart_list.size;

        i < CHART_LIST_NAME_MAX_SIZE;

        i++, cur = (cur + 1) % select_scene->chart_list.size) {

        len = strlen(select_scene->chart_list.charts[cur].title);
        rect.y = LETTER_HEIGHT * i * 2 + LETTER_HEIGHT;
        rect.w = len * LETTER_WIDTH;
        rect.x = 100;
        DrawText(rect, select_scene->chart_list.charts[cur].title, default_colors[0]);
        if (i == (CHART_LIST_NAME_MAX_SIZE - 1) / 2) {
            DrawCursor(rect);
        }
    }
}

void SelectSceneDraw() {
    /* Draw background */
#if USE_DEFAULT_BACKGROUND
    DrawDefaultBackgroundPure();
#else
    SDL_RenderCopy(app.ren, select_scene->background, NULL, NULL);
#endif

    /* Draw Info */
    SelectSceneDrawInfo();
    /* Draw list */
    SelectSceneDrawList();
}