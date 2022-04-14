// Everything relate to screen and visual stuff here

#include <iostream>
#include <cstdio>
#include "Render.h"
#include <array>
#include<Windows.h>



using namespace std;

const std::string snakeformat = "2112759721127610";

const int ScreenWidth = 120;
const int ScreenHeight = 40;

static CHAR_INFO* screen;
HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
DWORD dwBytesWritten = 0;
CONSOLE_CURSOR_INFO cursor;
SMALL_RECT rectWindow = { 0, 0, 1, 1 };


bool RenderInit() {

    screen = new CHAR_INFO[ScreenWidth * ScreenHeight];
    memset(screen, 0, sizeof(CHAR_INFO) * ScreenWidth * ScreenHeight);

    if (hConsole == INVALID_HANDLE_VALUE)
        return false;

    SetConsoleWindowInfo(hConsole, TRUE, &rectWindow);

    // Set the size of the screen buffer
    COORD coord = { (short)ScreenWidth, (short)ScreenHeight };
    if (!SetConsoleScreenBufferSize(hConsole, coord))
        return false;

    // Assign screen buffer to the console
    if (!SetConsoleActiveScreenBuffer(hConsole))
        return false;

    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (!GetConsoleScreenBufferInfo(hConsole, &csbi))
        return false;
    if (ScreenHeight > csbi.dwMaximumWindowSize.Y)
        return false;
    if (ScreenWidth > csbi.dwMaximumWindowSize.X)
        return false;

    // Set Physical Console Window Size
    rectWindow = { 0, 0, (short)ScreenWidth - 1, (short)ScreenHeight - 1 };

    if (!SetConsoleWindowInfo(hConsole, TRUE, &rectWindow))
        return false;

    GetConsoleCursorInfo(hConsole, &cursor);
    cursor.bVisible = false;
    SetConsoleCursorInfo(hConsole, &cursor);

    SetConsoleActiveScreenBuffer(hConsole);

    return true;
}


void RenderExit() {
    delete[] screen;
}

void inline changeBuffer(int x, int y, wchar_t c, short col = 0x000F) {
    screen[y * ScreenWidth + x].Char.UnicodeChar = c;
    screen[y * ScreenWidth + x].Attributes = col;
}

void changeBuffer(int x, int y, wstring str, short col = 0x000F) {
    if (x < 0 || y >= ScreenHeight || y < 0) return;
    for (int i = x; x < ScreenWidth; i++) {
        if (i - x >= str.size()) break;
        if (str[i - x] == L' ') continue;
        screen[y * ScreenWidth + i].Char.UnicodeChar = str[i - x];
        screen[y * ScreenWidth + i].Attributes = col;
    }
}

void Render() {
    screen[ScreenWidth * ScreenHeight - 1].Char.UnicodeChar = '\0';
    for (int i = 0; i < ScreenHeight; i++) {
        for (int j = 0; j < ScreenWidth; j++) {
            switch (screen[i * ScreenWidth + j].Char.UnicodeChar) {
            case L'#': {
                screen[i * ScreenWidth + j].Attributes = 0x000F; 
                screen[i * ScreenWidth + j].Char.UnicodeChar = 0x2588;
                screen[i * ScreenWidth + j + 1].Attributes = 0x000F;
                screen[i * ScreenWidth + j + 1].Char.UnicodeChar = 0x2588;
            }
            case L'\0': {
                screen[i * ScreenWidth + j].Attributes = 0x00F0;
            }
            default: {
                screen[i * ScreenWidth + j].Attributes = 0x00F0;
            }
            }
        }
    }
    WriteConsoleOutput(hConsole, screen, { ScreenWidth, ScreenHeight }, { 0,0 }, &rectWindow);
}

void DrawBoard(int x, int y, int width, int height, int curPosX = 0, int curPosY = 0) {
    for (int i = 0; i < width; i++) {
        changeBuffer(i * 2, 0, '#');
        changeBuffer(i * 2, height - 1, '#');
    }
    for (int i = 1; i < height - 1; i++) {
        changeBuffer(0, i, '#');
        changeBuffer((width - 1) * 2, i, '#');
    }
}

void ClearSnake(int SIZE_SNAKE, POINT snake[]) {
    for (int i = 0; i < SIZE_SNAKE; i++) {
        changeBuffer(snake[i].x * 2, snake[i].y, ' ');
    }
}

void clearScreen() {
    memset(screen, 0, sizeof(CHAR_INFO) * ScreenWidth * ScreenHeight);
}

void ClearFood(POINT food) {
    changeBuffer(food.x * 2, food.y, ' ');
}

void DrawSnake(POINT snake[], int SIZE_SNAKE) {
    for (int i = 0; i < SIZE_SNAKE; i++) {
        changeBuffer(snake[i].x * 2, snake[i].y, snakeformat[(SIZE_SNAKE - i - 1) % 16]);
    }
}

void RenderGame(int WIDTH_CONSOLE, int HEIGHT_CONSOLE, POINT food, int SIZE_SNAKE, POINT snake[], POINT wall[], int level, int wall_num, bool gate_state, std::array<POINT, 5> gate) {
     clearScreen();
     DrawBoard(0, 0, WIDTH_CONSOLE, HEIGHT_CONSOLE);
    if (!gate_state) {
        changeBuffer(food.x * 2, food.y, L'0');
    }
    else {
        RenderGate(gate);
    }
    if (level != 0) {
        Render_wall(wall, wall_num);
    }
    DrawSnake(snake, SIZE_SNAKE);

}

void ClearGate(std::array<POINT, 5> gate) {
    for (int i = 0; i < gate.size(); i++) {
        changeBuffer(gate[i].x * 2, gate[i].y, ' ');
    }
}

void RenderGate(std::array<POINT, 5> gate) {
    for (int i = 0; i < gate.size(); i++) {
        changeBuffer(gate[i].x * 2, gate[i].y, '#');
    }
}

void RenderGamePause(int WIDTH_CONSOLE, int HEIGHT_CONSOLE, POINT food, int SIZE_SNAKE, POINT snake[], bool init) {
    if (init) {
        system("cls");
        DrawBoard(0, 0, WIDTH_CONSOLE, HEIGHT_CONSOLE);
        changeBuffer(food.x, food.y, '0');
        DrawSnake(snake, SIZE_SNAKE);
        changeBuffer(HEIGHT_CONSOLE + 2, 0, L"Game Paused.");
        changeBuffer(HEIGHT_CONSOLE + 3, 0, L"Use W/A/S/D to resume.");
        changeBuffer(HEIGHT_CONSOLE + 4, 0, L"L to save, T to load.");
        changeBuffer(HEIGHT_CONSOLE + 5, 0, L"Esc to Main Menu.");
    }

}

void ClearMenu(int CURSOR) {
    changeBuffer(10, CURSOR + 3, ' ');
}

void RenderMenu(int CURSOR, bool init) {
    if (init) {
        clearScreen();

        changeBuffer(0, 0, L"======== HUNTING SNAKE ========");
        changeBuffer(0, 3, L"New Game");
        changeBuffer(0, 4, L"Load Game");
        changeBuffer(0, 5, L"Settings");
        changeBuffer(0, 6, L"Exit");

    }
    changeBuffer(10, CURSOR + 3, '*');
}

void ClearSettings(int CURSOR) {
    clearScreen();
}

void RenderSettings(Config setting, int CURSOR, bool init = false) {
    if (init) {
        clearScreen();
        changeBuffer(0, 1, L"SETTINGS:");
        changeBuffer(0, 3, L"    DIFFICULTY:  ");
        if (setting.Difficulty == 0)      changeBuffer(19, 3, L" < EASY >   | NORMAL |  | HARD | ");
        else if (setting.Difficulty == 1) changeBuffer(19, 3, L" | EASY |   < NORMAL >  | HARD | ");
        else if (setting.Difficulty == 2) changeBuffer(19, 3, L" | EASY |   | NORMAL |  < HARD > ");
        changeBuffer(0, 4, L"    Total Volume:  <");
        for (int i = 0; i < 10; i++) {
            if (i < setting.TotalVolume) changeBuffer(20 + i, 4, L"X");
            else changeBuffer(20 + i, 4, L"-");
        }
        changeBuffer(30, 4, L">");
        changeBuffer(0, 5, L"    Music Volume:  <");
        for (int i = 0; i < 10; i++) {
            if (i < setting.MusicVolume) changeBuffer(20 + i, 5, L"X");
            else changeBuffer(20 + i, 5, L"-");
        }
        changeBuffer(30, 5, L">");
        changeBuffer(0, 6, L"    SFX Volume:    <");
        for (int i = 0; i < 10; i++) {
            if (i < setting.SFXVolume) changeBuffer(20 + i, 6, L"X");
            else changeBuffer(20 + i, 6, L"-");
        }
        changeBuffer(30, 6, L">");
        if (setting.BGM) changeBuffer(0, 7, L"    Background Music: [X]");
        else  changeBuffer(0, 7, L"    Background Music: [ ]");
        changeBuffer(0, 8, L"    Save and Exit");
        changeBuffer(1, CURSOR + 3, '*');
    }
}

void RenderLoad() {
    clearScreen();
    changeBuffer(0, 0, L"Load file:");
}

void RenderSave() {
    clearScreen();
    changeBuffer(0, 0, L"Save file name: ");
}

void RenderScore(int score) {
    wstring strScore = L"Score: ";
    wstring temp(1, 0);
    if (score == 0) {
        changeBuffer(0, ScreenHeight - 7, strScore + L'0');
    }
    while (score != 0) {
        temp[0] = L'0' + score % 10;
        strScore.insert(7, temp);
        score /= 10;
    }
    changeBuffer(0, ScreenHeight - 7, strScore);
}

void ErrorLog(std::wstring str) {
    changeBuffer(0, ScreenHeight - 3, str);
}

void DeadAnimation(POINT snake) {
    changeBuffer(snake.x * 2, snake.y, ' ');
}


void AppearAnimation(POINT snake) {

}

void Passing(POINT snake[], int SIZE_SNAKE) {
    for (int i = 0; i < SIZE_SNAKE; i++) {
        changeBuffer(snake[i].x * 2, snake[i].y, snakeformat[(SIZE_SNAKE - i - 1) % 16]);
    }
}

void Render_wall(POINT wall[], int wall_num) {
    for (int i = 0; i < wall_num; i++) {
        changeBuffer(wall[i].x * 2, wall[i].y, '#');
    }
}

void Clear_wall(POINT wall[], int wall_num) {
    clearScreen();
    for (int i = 0; i < wall_num; i++) {
        changeBuffer(wall[i].x * 2, wall[i].y, ' ');
    }
}

void DrawSnakeFromHead(POINT snake[], int sz, int poped_head) {
    int j = sz - 1;
    for (int i = 0; i < sz; i++) {
        changeBuffer(snake[j].x * 2, snake[j].y, snakeformat[i + poped_head % 16]);
        j--;
    }
    for (int i = 0; i < poped_head; i++) {
        changeBuffer(snake[i].x * 2, snake[i].y, L' ');
    }
}

void Passing(POINT snake[], int SIZE_SNAKE, int r) {
    DrawSnakeFromHead(snake, SIZE_SNAKE, SIZE_SNAKE - r);
}

void renderEnding() {
    changeBuffer(20, 0, L"            Congratulation!!");
    changeBuffer(20, 2, L"     You have finished the game.");
    changeBuffer(20, 5, L"     Press Y to play again.");
    changeBuffer(20, 6, L"     Press Esc to exit.");
}