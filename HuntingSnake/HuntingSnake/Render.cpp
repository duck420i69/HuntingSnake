// Everything relate to screen and visual stuff here

#include <iostream>
#include <cstdio>
#include "Render.h"
#include <array>



using namespace std;

const std::string snakeformat = "2112759721127610";

const int ScreenWidth = 120;
const int ScreenHeight = 40;

static wchar_t* screen = new wchar_t[ScreenWidth * ScreenHeight];
HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
DWORD dwBytesWritten = 0;

CONSOLE_CURSOR_INFO cursor;
SMALL_RECT m_rectWindow = { 0, 0, 1, 1 };


bool RenderInit() {

    SetConsoleWindowInfo(hConsole, TRUE, &m_rectWindow);

    // Set the size of the screen buffer
    COORD coord = { (short)ScreenWidth, (short)ScreenHeight };
    if (!SetConsoleScreenBufferSize(hConsole, coord))
        return false;

    // Assign screen buffer to the console
    if (!SetConsoleActiveScreenBuffer(hConsole))
        return false;

    SetConsoleActiveScreenBuffer(hConsole);

    cursor.bVisible = false;
    SetConsoleCursorInfo(hConsole, &cursor);

    return true;
}

void RenderExit() {
    delete[] screen;
}

void inline changeBuffer(int x, int y, wchar_t c) {
    screen[y * ScreenWidth + x] = c;
}

void changeBuffer(int x, int y, wstring str) {
    if (x < 0 || y >= ScreenHeight || y < 0) return;
    for (int i = x; x < ScreenWidth; i++) {
        if (i - x >= str.size()) break;
        screen[y * ScreenWidth + i] = str[i - x];
    }
}

void changeBuffer(int x, int y, string str) {
    if (x < 0 || y >= ScreenHeight || y < 0) return;
    for (int i = x; x < ScreenWidth; i++) {
        if (i - x >= str.size()) break;
        screen[y * ScreenWidth + i] = str[i - x];
    }
}

void Render() {
    screen[ScreenWidth * ScreenHeight - 1] = '\0';
    WriteConsoleOutputCharacter(hConsole, screen, ScreenWidth * ScreenHeight, { 0,0 }, &dwBytesWritten);
}



void GotoXY(int x, int y) {
    COORD coord;
    coord.X = x * 2;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}


void DrawBoard(int x, int y, int width, int height, int curPosX = 0, int curPosY = 0) {
    GotoXY(x, y); cout << "#";
    for (int i = 1; i < width * 2; i++) cout << "#";
    cout << "#";
    GotoXY(x, height + y); cout << "#";
    for (int i = 1; i < width * 2; i++) cout << "#";
    cout << "#";
    for (int i = y + 1; i < height + y; i++) {
        GotoXY(x, i); cout << "#";
        GotoXY(x + width, i); cout << "#";
    }
    GotoXY(curPosX, curPosY);
}

void ClearSnake(int SIZE_SNAKE, POINT snake[]) {
    for (int i = 0; i < SIZE_SNAKE; i++) {
        GotoXY(snake[i].x, snake[i].y);
        cout << " ";
    }
}

void ClearFood(POINT food) {
    GotoXY(food.x, food.y);
    cout << " ";
}


void RenderGame(int WIDTH_CONSOLE, int HEIGHT_CONSOLE, POINT food, int SIZE_SNAKE, POINT snake[], POINT wall[],  int level, int wall_num, bool gate_state, bool init) {
    if (init) {
        system("cls");
        DrawBoard(0, 0, WIDTH_CONSOLE, HEIGHT_CONSOLE);
    }

    if (!gate_state) {
        GotoXY(food.x, food.y);
        cout << "O";
    }

    if (level != 0) {
        Render_wall(wall,wall_num);
    }

    for (int i = 0; i < SIZE_SNAKE; i++) {
        GotoXY(snake[i].x, snake[i].y);
        cout << snakeformat[(SIZE_SNAKE - i - 1) % 16];
    }

}

void ClearGate(std::array<POINT, 5> gate) {
    for (int i = 0; i < gate.size(); i++) {
        GotoXY(gate[i].x, gate[i].y);
        cout << " ";
    }
}

void RenderGate(std::array<POINT, 5> gate) {
    for (int i = 0; i < gate.size(); i++) {
        GotoXY(gate[i].x, gate[i].y);
        cout << "#";
    }
}

void RenderGamePause(int WIDTH_CONSOLE, int HEIGHT_CONSOLE, POINT food, int SIZE_SNAKE, POINT snake[], bool init) {
    if (init) {
        system("cls");
        DrawBoard(0, 0, WIDTH_CONSOLE, HEIGHT_CONSOLE);
        GotoXY(food.x, food.y);
        cout << "O";

        for (int i = 0; i < SIZE_SNAKE; i++) {
            GotoXY(snake[i].x, snake[i].y);
            cout << snakeformat[(SIZE_SNAKE - i - 1) % 16];
        }

        GotoXY(0, HEIGHT_CONSOLE + 2);
        cout << "Game Paused." << endl;
        cout << "Use W/A/S/D to resume." << endl;
        cout << "L to save, T to load.";
        cout << "Esc to Main Menu.";
    }

}

void ClearMenu(int CURSOR) {
    GotoXY(5, CURSOR + 3); cout << " ";
}

void RenderMenu(int CURSOR, bool init) {
    if (init) {
        system("cls");
        cout << "       HUNTING SNAKE";
        GotoXY(0, 3);
        cout << "New Game" << endl;
        cout << "Load Game" << endl;
        cout << "Settings" << endl;
        cout << "Exit" << endl;
    }
    GotoXY(5, CURSOR + 3); cout << "*";
    GotoXY(5, CURSOR + 3);
}

void ClearSettings(int CURSOR) {
    system("cls");
}

void RenderSettings(Config setting, int CURSOR, bool init = false) {
    if (init) {
        system("cls");
        cout << endl;
        changeBuffer(0, 1, "             SETTINGS:");
        changeBuffer(0, 3, "    DIFFICULTY:  ");

        if (setting.Difficulty == 0)      changeBuffer(18, 3, " < EASY >   | NORMAL |  | HARD | ");
        else if (setting.Difficulty == 1) changeBuffer(18, 3, " | EASY |   < NORMAL >  | HARD | ");
        else if (setting.Difficulty == 2) changeBuffer(18, 3, " | EASY |   | NORMAL |  < HARD > ");
        cout << "    Total Volume:  <";
        for (int i = 0; i < 10; i++) {
            if (i < setting.TotalVolume) cout << "X";
            else cout << "-";
        }
        cout << ">\n";
        cout << "    Music Volume:  <";
        for (int i = 0; i < 10; i++) {
            if (i < setting.MusicVolume) cout << "#";
            else cout << "-";
        }
        cout << ">\n";
        cout << "    Background Music: ["; 
        if (setting.BGM) cout << "X"; 
        else cout << " ";
        cout << "]\n";
        cout << "    Save and Exit\n";
    }
    GotoXY(1, CURSOR + 3); cout << "*";
    GotoXY(1, CURSOR + 3);

}

void RenderLoad() {
    system("cls");
    cout << "Load file: ";
}

void RenderSave() {
    system("cls");
    cout << "Save file name: ";
}

void ErrorLog(std::string str) {
    cout << endl << str << endl;
}

void DeadAnimation(POINT snake) {
    GotoXY(snake.x, snake.y);
    cout << " ";
}

void AppearAnimation(POINT snake) {
    GotoXY(snake.x, snake.y);

}

void Render_wall(POINT wall[],int wall_num) {
    for (int i = 0; i < wall_num; i++) {
        GotoXY(wall[i].x, wall[i].y);
        cout << "#";
    }
}

void Clear_wall(POINT wall[], int wall_num) {
    for (int i = 0; i < wall_num; i++) {
        GotoXY(wall[i].x, wall[i].y); 
        cout << " ";
    }
}

