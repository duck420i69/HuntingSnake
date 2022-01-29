// Everything relate to screen and visual stuff here

#include <iostream>
#include "Render.h"


using namespace std;

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

    // After finish other things then do the final request
    for (int i = 0; i < SIZE_SNAKE - 1; i++) {
        GotoXY(snake[i].x, snake[i].y);
        cout << "O";
    }
    GotoXY(snake[SIZE_SNAKE - 1].x, snake[SIZE_SNAKE - 1].y);
    cout << "Q";
}

void ClearMenu(int CURSOR) {
    GotoXY(5, CURSOR + 3); cout << " ";
}

void RenderMenu(int CURSOR, bool init) {
    if (init) {
        system("cls");
        GotoXY(0, 3);
        cout << "New Game" << endl;
        cout << "Load Game" << endl;
        cout << "Settings" << endl;
        cout << "Exit" << endl;
    }
    GotoXY(5, CURSOR + 3); cout << "*";
    GotoXY(5, CURSOR + 3);
}


void RenderSettings() {
    system("cls");

}

// Return whether the animation is done or not
bool DeadAnimation() {
    // TO-DO: make dead animation
    return false;
}

// Return whether the animation is done or not
bool PassAnimation() {
    // TO-DO: make passing gate animation
    return false;
}