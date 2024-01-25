#ifndef _SELECT_SCENE_H_
#define _SELECT_SCENE_H_

#include <dirent.h>
#include <SDL_ttf.h>
#include "GameScene.h"

struct ChartInfo {
    char* chart_path;
    char* title;
    char* artist;
    char* preview;
};
typedef struct ChartInfo ChartInfo;

struct ChartList {
    char* saves_path; // end with '/'
    size_t size;
    size_t capacity;
    size_t cur_chart;
    ChartInfo* charts;
};
typedef struct ChartList ChartList;

void InitChartList(ChartList* list, const char* saves_path);
void FreeChartList(ChartList* list);
void ChartListPushBack(ChartList* list,
    const char* path,
    const char* title,
    const char* artist,
    const char* preview
);
void ChartListRefresh(ChartList* list);

struct SelectScene {

#if !USE_DEFAULT_BACKGROUND
    SDL_Texture* background;
#endif

    /* list */
    ChartList chart_list;
    SDL_Texture* preview;
};
typedef struct SelectScene SelectScene;

extern SelectScene* select_scene;

SelectScene* CreateSelectScene();
void DestroySelectScene();
void SelectSceneHandleKey(SDL_Event* event);
void SelectSceneUpdate();
static void SelectSceneDrawPreview(SDL_Rect rect);
static void SelectSceneDrawInfo();
static void SelectSceneDrawList();
void SelectSceneDraw();

#endif