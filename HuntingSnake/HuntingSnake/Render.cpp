// Everything relate to screen and visual stuff here

#include <iostream>
#include "Render.h"
#include <array>


using namespace std;

const std::string snakeformat = "21127597";


// std::array<std::string, 20> prev_frame; 

/*

void Render(std::array<std::string, 20>& frame_buffer) {

    // 1st way (maybe for init)
    system("cls");
    for (int i = 0; i < H; i++) {
        cout << frame_buffer[i];
    }

    // 2nd way
    for (int j = 0; j < H; j++) {
        for (int i = 0; i < W; i++) {
            if (frame_buffer[j][i] != prev_frame[j][i]) {
                GotoXY(i, j);
                cout << frame_buffer[j][i];
            }
        }
        prev_frame[i] = frame_buffer[i];
    }

    // 3rd way: just draw snake and stuff

}
*/


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

void RenderGame(int WIDTH_CONSOLE, int HEIGHT_CONSOLE, POINT food, int SIZE_SNAKE, POINT snake[], bool init) {
    if (init) {
        system("cls");
        DrawBoard(0, 0, WIDTH_CONSOLE, HEIGHT_CONSOLE);
    }
    GotoXY(food.x, food.y);
    cout << "O";

    for (int i = 0; i < SIZE_SNAKE - 1; i++) {
        GotoXY(snake[i].x, snake[i].y);
        cout << "O";
    }
    GotoXY(snake[SIZE_SNAKE - 1].x, snake[SIZE_SNAKE - 1].y);
    cout << "Q";
}

void RenderGamePause(int WIDTH_CONSOLE, int HEIGHT_CONSOLE, POINT food, int SIZE_SNAKE, POINT snake[], bool init) {
    if (init) {
        system("cls");
        DrawBoard(0, 0, WIDTH_CONSOLE, HEIGHT_CONSOLE);
        GotoXY(food.x, food.y);
        cout << "O";

        for (int i = 0; i < SIZE_SNAKE - 1; i++) {
            GotoXY(snake[i].x, snake[i].y);
            cout << "O";
        }
        GotoXY(snake[SIZE_SNAKE - 1].x, snake[SIZE_SNAKE - 1].y);
        cout << "Q";
    }

    GotoXY(0, HEIGHT_CONSOLE + 2);
    cout << "Game Paused." << endl;
    cout << "Use W/A/S/D to resume." << endl;
    cout << "L to save, T to load.";
    cout << "Esc to Main Menu.";

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
    GotoXY(1, CURSOR + 3); cout << " ";
}

void RenderSettings(int CURSOR, bool init) {
    if (init) {
        system("cls");
        cout << endl;
        cout << "             SETTINGS:";
        cout << endl;
        cout << endl;
        cout << "    DIFFICULTY:  < EASY >  <NORMAL>  < HARD >" << endl;
        cout << "    Total Volume:  <XXXXXXXX-->" << endl;
        cout << "    Music Volume:  <########-->" << endl;
        cout << "    Background Music: [x]" << endl;
        cout << "    Save and Exit" << endl;
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
    // TO-DO: make dead animation
    GotoXY(snake.x, snake.y);
    cout << " ";
}


void PassAnimation(POINT snake) {
    // TO-DO: make passing gate animation

}

void AppearAnimation(POINT snake) {


}