#pragma once

#include "Windows.h"
#include "winuser.h"
#include <string>

/*
struct Config {
	bool TurnAudio = true;
	bool BGM = true;
	char TotalVolume = 8;
	char MusicVolume = 8;
	char Difficulty = 2;
};
*/


void GotoXY(int x, int y);

void ClearSnake(int SIZE_SNAKE, POINT snake[]);

void ClearFood(POINT food);

void RenderGame(int WIDTH_CONSOLE, int HEIGHT_CONSOLE, POINT food, int SIZE_SNAKE, POINT snake[], bool gate_state, bool init = false);

void RenderGamePause(int WIDTH_CONSOLE, int HEIGHT_CONSOLE, POINT food, int SIZE_SNAKE, POINT snake[], bool init = false);

void RenderGate(std::array<POINT, 5> gate);

void ClearGate(std::array<POINT, 5> gate);

void ClearMenu(int CURSOR);

void RenderMenu(int CURSOR, bool init = false);

void RenderLoad();

void RenderSave();

void ClearSettings(int CURSOR);

void RenderSettings(int CURSOR, bool init = false);

// ANIMATION
void DeadAnimation(POINT snake);

void PassAnimation(POINT snake);

void AppearAnimation(POINT snake);

// ETC
void ErrorLog(std::string str);