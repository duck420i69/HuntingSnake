#pragma once

#include "Windows.h"
#include "winuser.h"


void ClearSnake(int SIZE_SNAKE, POINT snake[]);

void ClearFood(POINT food);

void RenderGame(int WIDTH_CONSOLE, int HEIGHT_CONSOLE, POINT food, int SIZE_SNAKE, POINT snake[], bool init = false);

void ClearMenu(int CURSOR);

void RenderMenu(int CURSOR, bool init = false);