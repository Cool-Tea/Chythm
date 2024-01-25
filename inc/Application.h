#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#include "MenuScene.h"
#include "SelectScene.h"
#include "GameScene.h"
#include "PauseScene.h"
#include "EndScene.h"

void InitApplication();
void DestroyApplication();
void ApplicationStart();
void ApplicationStop();
void ApplicationHandleKey();
void ApplicationUpdate();
void ApplicationDraw();
void ApplicationTick();

#endif