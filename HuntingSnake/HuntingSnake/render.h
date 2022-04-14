

#pragma once

#include "Windows.h"
#include "winuser.h"
#include <string>


struct Config {
	bool UseAudio = true;	// Audio on or off
	bool BGM = true;		// Background music
	char TotalVolume = 8;	// Game volume
	char MusicVolume = 8;	// Background Volume
	char SFXVolume = 8;	// SFX Volume
	char Difficulty = 2;	// Difficulty
};


void ClearSnake(int SIZE_SNAKE, POINT snake[]);

void ClearFood(POINT food);

void RenderGame(int WIDTH_CONSOLE, int HEIGHT_CONSOLE, POINT food, int SIZE_SNAKE, POINT snake[], POINT wall[], int level, int wall_num, bool gate_state, std::array<POINT, 5> gate);

void RenderGamePause(int WIDTH_CONSOLE, int HEIGHT_CONSOLE, POINT food, int SIZE_SNAKE, POINT snake[], bool init);

void RenderGate(std::array<POINT, 5> gate);

void ClearGate(std::array<POINT, 5> gate);

void ClearMenu(int CURSOR);

void RenderMenu(int CURSOR, bool init = false);

void RenderLoad();

void RenderSave();

void ClearSettings(int CURSOR);

void RenderSettings(Config setting, int CURSOR, bool init);

// ANIMATION
void DeadAnimation(POINT snake);

void AppearAnimation(POINT snake);

void AppearAnimation(POINT snake);

// MAP
void Render_wall(POINT wall[], int wall_num);

void Clear_wall(POINT wall[], int wall_num);

// ETC
void ErrorLog(std::wstring str);

void RenderScore(int score);


void RenderExit();
bool RenderInit();
void Render();

//
void Passing(POINT snake[], int SIZE_SNAKE, int r);
void DrawSnake(POINT snake[], int SIZE_SNAKE);
void ClearSnake(int SIZE_SNAKE, POINT snake[]);
void clearScreen();
//
void renderEnding();

