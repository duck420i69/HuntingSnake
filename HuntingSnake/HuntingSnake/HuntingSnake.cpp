// HuntingSnake.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "Windows.h"
#include "winuser.h"
#include <thread>
#include <conio.h>
#include <deque>

//Constants
#define MAX_SIZE_SNAKE 25
#define MAX_SIZE_FOOD 4
#define MAX_SPEED 9

using namespace std;

//Global variables
// Menu data
int CURSOR = 0;
int STATE = 0;
// Ingame data
POINT snake[MAX_SIZE_SNAKE]; //snake
POINT food; // food
deque<POINT> food_queue;
int CHAR_LOCK;                    // used to determine the direction my snake cannot move (At a moment, there is one direction my snake cannot move to)
int MOVE_BUFFER;                  // store movement of the keyboard for processing
int MOVING;                       // used to determine the direction my snake moves (At a moment, thereare three directions my snake can move)
int SPEED;                        // Standing for level, the higher the level, the quicker the speed
int HEIGH_CONSOLE, WIDTH_CONSOLE; // Width and height of console-screen
int FOOD_INDEX;                   // current food-index
int SIZE_SNAKE;                   // size of snake, initially maybe 6 units and maximum size may be 10
bool IS_PLAYING;                  // State of snake: dead or alive
bool IS_PAUSE;                    // pause or not
bool RENDERING;                   // is in render or not?


void FixConsoleWindow() {
    HWND consoleWindow = GetConsoleWindow(); // Console window handler
    LONG style = GetWindowLong(consoleWindow, GWL_STYLE);
    style = style & ~(WS_MAXIMIZEBOX) & ~(WS_THICKFRAME);
    SetWindowLong(consoleWindow, GWL_STYLE, style);
}


void GotoXY(int x, int y) {
    COORD coord;
    coord.X = x * 2;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}


bool IsValid(int x, int y) {
    for (int i = 0; i < SIZE_SNAKE; i++) 
        if (snake[i].x == x && snake[i].y == y) 
            return false;
    return true;   
}


void DrawBoard(int x, int y, int width, int height, int curPosX = 0, int curPosY = 0) {
    GotoXY(x, y); cout << "# ";
    for (int i = 1; i < width; i++) cout << "# ";
    cout << "# ";
    GotoXY(x, height + y); cout << "# ";
    for (int i = 1; i < width; i++) cout << "# ";
    cout << "# ";
    for (int i = y + 1; i < height + y; i++) {
        GotoXY(x, i); cout << "# ";
        GotoXY(x + width, i); cout << "# ";
    }
    GotoXY(curPosX, curPosY);
}


void GenerateFood() {

    int x, y;
    srand(time(NULL));
    do {
        x = rand() % (WIDTH_CONSOLE - 1) + 1;
        y = rand() % (HEIGH_CONSOLE - 1) + 1;
    } while (IsValid(x, y) == false);
    food.x = x;
    food.y = y;
}


void ResetData() {
    //Initialize the global values
    CHAR_LOCK = 'A', MOVING = 'D', MOVE_BUFFER = 'D', SPEED = 5; FOOD_INDEX = 0, SIZE_SNAKE = 6,
        WIDTH_CONSOLE = 40, HEIGH_CONSOLE = 20;
    // Initialize default values for snake
    snake[0] = { 10, 5 }; snake[1] = { 11, 5 };
    snake[2] = { 12, 5 }; snake[3] = { 13, 5 };
    snake[4] = { 14, 5 }; snake[5] = { 15, 5 };
    food_queue.clear();
    GenerateFood(); //Create food

}

//Function to update global data
void Eat() {
    food_queue.push_back(food);
    GenerateFood();
    if (FOOD_INDEX == MAX_SIZE_FOOD - 1) {
        FOOD_INDEX = 0;
        if (SPEED == MAX_SPEED) ResetData();
        else SPEED++;
    }
    else {
        FOOD_INDEX++;
    }
}

//Function to process the dead of snake
void ProcessDead() {
    IS_PLAYING = 0;

    GotoXY(0, HEIGH_CONSOLE + 2);
    cout << "Dead, type y to continue or ESC to exit";
    for (int i = SIZE_SNAKE; i > 0; i--) {
        if (IS_PLAYING == 1) break;
        GotoXY(food.x, food.y);
        cout << "O";
        GotoXY(snake[i - 1].x, snake[i - 1].y);
        cout << " ";
        for (int j = 0; j < i - 1; j++) {
            GotoXY(snake[j].x, snake[j].y);
            cout << "O";
        }
        GotoXY(0, 0);
        Sleep(2000);
    }
}

//Function to draw
void DrawSnakeAndFood(char* str) {
    GotoXY(food.x, food.y);
    cout << str;
    for (int i = 0; i < SIZE_SNAKE; i++) {
        GotoXY(snake[i].x, snake[i].y);
        cout << str;
    }
}


void MoveRight() {
        if (snake[SIZE_SNAKE - 1].x + 1 == WIDTH_CONSOLE) {
            ProcessDead();
        }
        else {
                if (snake[SIZE_SNAKE - 1].x + 1 == food.x && snake[SIZE_SNAKE - 1].y == food.y) {
                    Eat();
                }
                for (int i = 0; i < SIZE_SNAKE - 1; i++) {
                    snake[i].x = snake[i + 1].x;
                    snake[i].y = snake[i + 1].y;
                }
            snake[SIZE_SNAKE - 1].x++;
        }
}

void MoveLeft() {
    if (snake[SIZE_SNAKE - 1].x - 1 == 0) {
        ProcessDead();
    }
    else {
        if (snake[SIZE_SNAKE - 1].x - 1 == food.x && snake[SIZE_SNAKE - 1].y == food.y) {
            Eat();
        }
        for (int i = 0; i < SIZE_SNAKE - 1; i++) {
            snake[i].x = snake[i + 1].x;
            snake[i].y = snake[i + 1].y;
        }
        snake[SIZE_SNAKE - 1].x--;
    }
}

void MoveDown() {
    if (snake[SIZE_SNAKE - 1].y + 1 == HEIGH_CONSOLE) {
        ProcessDead();
    }
    else {
        if (snake[SIZE_SNAKE - 1].x == food.x && snake[SIZE_SNAKE - 1].y + 1 == food.y) {
            Eat();
        }
        for (int i = 0; i < SIZE_SNAKE - 1; i++) {
            snake[i].x = snake[i + 1].x;
            snake[i].y = snake[i + 1].y;
        }
        snake[SIZE_SNAKE - 1].y++;
    }
}

 void MoveUp() {
    if (snake[SIZE_SNAKE - 1].y - 1 == 0) {
        ProcessDead();
    }
    else {
        if (snake[SIZE_SNAKE - 1].x == food.x && snake[SIZE_SNAKE - 1].y - 1 == food.y) {
             Eat();        
        }
        for (int i = 0; i < SIZE_SNAKE - 1; i++) {
            snake[i].x = snake[i + 1].x;      
            snake[i].y = snake[i + 1].y;           
        }
         snake[SIZE_SNAKE - 1].y--;
    }
}


 void RenderMenu() {
     system("cls");
     GotoXY(0, 3); cout << "New Game";
     GotoXY(0, 4); cout << "Load Game";
     GotoXY(0, 5); cout << "Settings";
     GotoXY(0, 6); cout << "Exit";
     GotoXY(5, CURSOR + 3); cout << "*";
     GotoXY(5, CURSOR + 3);
 }


 void RenderSettings() {
     system("cls");

 }

 void RenderGame() {
     system("cls");

 }


 void StartGame() {
     char A[] = " ";
     system("cls");
     DrawSnakeAndFood(A);
     ResetData(); // Intialize original data
     DrawBoard(0, 0, WIDTH_CONSOLE, HEIGH_CONSOLE); // Draw game

     IS_PLAYING = 1;//Start running Thread
 }

 //Function exit game
 void ExitGame(HANDLE t) {
     system("cls");
     TerminateThread(t, 0); // Dangerous
 }

 //Function pause game
 void PauseGame(HANDLE t) {
     SuspendThread(t);
     GotoXY(0, HEIGH_CONSOLE + 2);
     cout << "Paused";
 }


 //Subfunction for thread
 void ThreadFunc() {
     while (1) {
         switch (STATE) {
         case 0: {
             RenderMenu();
             Sleep(50);
             break;
         }
         case 1: {
             if (IS_PLAYING == 1) {
                 //If my snake is alive
                 char A[] = " ";
                 char B[] = "O";

                 // Clear snake
                 DrawSnakeAndFood(A);
                 GotoXY(0, HEIGH_CONSOLE + 2);
                 cout << "      ";

                 // Debug
                 GotoXY(0, HEIGH_CONSOLE + 4);
                 cout << "BUFFER: " << (char) MOVE_BUFFER << ", MOVE: " << (char) MOVING << ", LOCK: " << (char) CHAR_LOCK;
                 MOVING = MOVE_BUFFER;

                 // Logic
                 if (MOVING == CHAR_LOCK) {
                     switch (MOVING) {
                     case 'A':
                         MOVING = 'D';
                         break;
                     case 'D':
                         MOVING = 'A';
                         break;
                     case 'W':
                         MOVING = 'S';
                         break;
                     case 'S':
                         MOVING = 'W';
                         break;
                     }
                 }
                 switch (MOVING) {
                 case 'A':
                     MoveLeft();
                     CHAR_LOCK = 'D';
                     break;
                 case 'D':
                     MoveRight();
                     CHAR_LOCK = 'A';
                     break;
                 case 'W':
                     MoveUp();
                     CHAR_LOCK = 'S';
                     break;
                 case 'S':
                     MoveDown();
                     CHAR_LOCK = 'W';
                     break;
                 }

                 if (food_queue.size() > 0) {
                     if (IsValid(food_queue.front().x, food_queue.front().y)) {
                         for (int i = SIZE_SNAKE - 1; i >= 0; i--)
                             snake[i + 1] = snake[i];
                         snake[0].x = food_queue.front().x;
                         snake[0].y = food_queue.front().y;
                         SIZE_SNAKE++;
                         food_queue.pop_front();
                     }
                 }

                 for (int i = 0; i < SIZE_SNAKE - 1; i++)
                     for (int j = i + 1; j < SIZE_SNAKE; j++)
                         if ((snake[i].x == snake[j].x) && (snake[i].y == snake[j].y)) ProcessDead();

                 // Debug
                 GotoXY(0, HEIGH_CONSOLE + 5);
                 cout << "BUFFER: " << (char) MOVE_BUFFER << ", MOVE: " << (char) MOVING << ", LOCK: " << (char) CHAR_LOCK;
                 MOVING = MOVE_BUFFER;

                 // Render
                 if (IS_PLAYING == 1) DrawSnakeAndFood(B);
                 GotoXY(0, 0);

                 Sleep(1000 / SPEED); //Sleep function with SPEED variable
             }
             break;
         }
         case 2: {
             break;
         }
         case 3: {
             break;
         }

         }
     }
 }

 int main() {
     int temp;
     FixConsoleWindow();
     /*
     * 
     * 
     * 
     * 
     * 
     * 
     * 
     * 
     */
     StartGame();
     thread t1(ThreadFunc); //Create thread for snake  
     HANDLE handle_t1 = t1.native_handle(); //Take handle of thread
     while (1) {
         temp = toupper(_getch());
         switch (STATE) {
         case 0: {
             if (temp == 'W') {
                 CURSOR -= 1;
             }
             else if (temp == 'S') {
                 CURSOR += 1;
             }
             else if (temp == 13) {
                 STATE = CURSOR + 1;
             }
             CURSOR = CURSOR % 4;
             if (CURSOR < 0) CURSOR += 4;
             break;
         }
         case 1: {

             if (IS_PLAYING == 1) {
                 if (temp == 'P') {
                     PauseGame(handle_t1);

                 }
                 else if (temp == 27) {
                     STATE = 0;
                     return 0;

                 }
                 else if ((temp == 'A') || (temp == 'W') || (temp == 'S') || (temp == 'D')) {
                     ResumeThread(handle_t1);
                     MOVE_BUFFER = temp;
                 }

             }
             else if (IS_PLAYING == 0) {
                 if (temp == 'Y') {
                     StartGame();
                 }
                 else if (temp == 27) {
                     STATE = 0;
                     return 0;

                 }
             }
             break;
         }
         case 2: {
             break;
         }
         case 3: {
             break;
         }
         }

     }
 }

