

// Define how state work

#include <deque>
#include <array>
#include "Render.h"
#include "Audio.h"
#include <thread>
#include <chrono>
#include <conio.h>
#include <fstream>
#include <iostream>


bool DEBUG_MODE = 1;

Config setting;

//Constants
#define MAX_SIZE_SNAKE 25  
#define MAX_SIZE_FOOD 4             // Amount of food to get to next level (can add 1 more for gate)
#define MAX_SPEED 20
#define MAX_SIZE_WALL 300
#define MAX_LEVEL 3

bool INIT = true;                   // When enter another state, INIT = true
bool LOADFILE = false;

//***************************************************
POINT snake[MAX_SIZE_SNAKE];        // snake
POINT food;                         // food
POINT epoint;
std::array<POINT, 5> gate;
std::deque<POINT> food_queue;       // food in snake stomach (add snake size when the eaten food reach the snake tail to solve a bug)
char CHAR_LOCK;                     // used to determine the direction my snake cannot move (At a moment, there is one direction my snake cannot move to)
char MOVING;                        // used to determine the direction my snake moves (At a moment, thereare three directions my snake can move)
int SPEED;                          // Standing for level, the higher the level, the quicker the speed
int HEIGHT_CONSOLE, WIDTH_CONSOLE;  // Width and height of console-screen
int FOOD_INDEX;                     // current food-index
int SIZE_SNAKE;                     // size of snake, initially maybe 6 units and maximum size may be 10
bool IS_PLAYING;                    // State of snake: dead or alive
bool IS_PAUSE;                      // pause or not
bool PAUSE_INIT;
bool GATE_OPEN;
int ANIMATION;                      // Animation to play (0: None, 1: Dead, 2: Passing Gate, 3: Appear)
int REMAIN_FRAME;                   // Remaining frame in  animation
int wall_num;                       // number of wall in map
POINT wall[MAX_SIZE_WALL];          // wall 
int level = 0;                        // game level
int score = 0;                      // score of player



bool IsCollision(POINT a, POINT b) {
    return ((a.x == b.x) && (a.y == b.y));
}

bool IsValid(int x, int y) {
    for (int i = 0; i < SIZE_SNAKE; i++)
        if (snake[i].x == x && snake[i].y == y)
            return false;
    for (int i = 0; i < wall_num; i++) {
        if (wall[i].x == x && wall[i].y == y)
            return false;
    }
    return true;
}

bool IsDead(POINT head) {
    if (level != 0) {
        for (int i = 0; i < wall_num; i++) {
            if (IsCollision(head, wall[i]))
                return true;
        }
    }
    if (GATE_OPEN)
        for (int i = 0; i < 5; i++)
            if (IsCollision(head, gate[i]))
                return true;

    for (int i = 0; i < SIZE_SNAKE - 2; i++)
        if (IsCollision(head, snake[i + 1]))
            return true;

    for (int i = 0; i < food_queue.size(); i++)
        if (IsCollision(head, food_queue[i]))
            return true;

    if ((head.x <= 0) || (head.x >= WIDTH_CONSOLE - 1) || (head.y <= 0) || (head.y >= HEIGHT_CONSOLE - 1))
        return true;

    return false;
}

bool CheckGateSpawn(int x, int y) {
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            if (!IsValid(x + i - 2, y + j - 2)) return false;
        }
    }
    return true;
}

void drawSquare(POINT a[], int& n, int x, int y) {
    a[++n] = { x + 1,y };
    a[++n] = { x  ,y + 1 };
    a[++n] = { x ,y };
    a[++n] = { x + 1,y + 1 };
}

void Load_map() {
    int n;
    switch (level) {
    case 1:
        wall_num = 100;
        n = 0;
        for (int i = 4; i < 18; i++) {
            wall[n] = { i ,i };
            n++;
        }
        for (int i = 4; i < 18; i++) {
            wall[n] = { 59 - i, i };
            n++;
        }
        for (int i = 0; i < 24; i++) {
            wall[n] = { 18 + i,25 };
            n++;
        }
        for (int i = 0; i < 10; i++) {
            wall[++n] = { 29,14 - i };
        }
        for (int i = 1; i < 6; i++) {
            wall[++n] = { 29,14 + i };
        }
        for (int i = 1; i < 10; i++) {
            wall[++n] = { 29 + i,14 };
            wall[++n] = { 29 - i,14 };
        }
        wall[++n] = { 35,13 }; wall[++n] = { 23,10 };
        wall[++n] = { 54,26 }; wall[++n] = { 5,26 };
        for (int i = 1; i < 10; i++) {
            wall[++n] = { 54 - i,26 - i };
            wall[++n] = { 5 + i,26 - i };
        }
        break;
    case 2:
        wall_num = 150;
        n = -1;
        for (int i = 0; i < 10; i++) {
            wall[++n] = { 10 + i, 10 };
            wall[++n] = { 10 + i, 11 };
        }
        for (int i = 0; i < 9; i++) {
            wall[++n] = { 19 , 11 - i };
            wall[++n] = { 20 , 11 - i };
        }
        for (int i = 1; i <= 5; i++) {
            wall[++n] = { 20 + i , 11 + i };
        }
        for (int i = 1; i <= 4; i++) {
            wall[++n] = { 20 + 6 + i , 11 + 6 + i };
        }
        for (int i = 1; i < 8; i++) {
            wall[++n] = { 20 + 10 + i , 11 + 10 };
            wall[++n] = { 20 + 10  , 11 + 10 + i };
        }
        for (int i = 0; i < 7; i++) {
            wall[++n] = { 21 + 10 + i , 12 + 10 };
            wall[++n] = { 21 + 10  , 12 + 10 + i };
        }
        drawSquare(wall, n, 30, 4);
        drawSquare(wall, n, 35, 14);
        drawSquare(wall, n, 40, 7);
        drawSquare(wall, n, 42, 25);
        drawSquare(wall, n, 43, 20);
        for (int i = 0; i <= 8; i += 2) {
            drawSquare(wall, n, 6 + i, 24 - i);
        }
        break;
    }
}


void SpawnGate() {
    int x, y, z;
    srand(time(NULL));

    do {
        x = rand() % (WIDTH_CONSOLE - 6) + 3;
        y = rand() % (HEIGHT_CONSOLE - 6) + 3;
    } while (!CheckGateSpawn(x, y));
    epoint.x = x; epoint.y = y;
    z = rand() % 4;
    if (z == 0) {
        gate[0].x = x - 1; gate[0].y = y;
        gate[1].x = x - 1; gate[1].y = y - 1;
        gate[2].x = x; gate[2].y = y - 1;
        gate[3].x = x + 1; gate[3].y = y - 1;
        gate[4].x = x + 1; gate[4].y = y;
    }
    else if (z == 1) {
        gate[0].x = x; gate[0].y = y + 1;
        gate[1].x = x - 1; gate[1].y = y + 1;
        gate[2].x = x - 1; gate[2].y = y;
        gate[3].x = x - 1; gate[3].y = y - 1;
        gate[4].x = x; gate[4].y = y - 1;
    }
    else if (z == 2) {
        gate[0].x = x - 1; gate[0].y = y;
        gate[1].x = x - 1; gate[1].y = y + 1;
        gate[2].x = x; gate[2].y = y + 1;
        gate[3].x = x + 1; gate[3].y = y + 1;
        gate[4].x = x + 1; gate[4].y = y;
    }
    else if (z == 3) {
        gate[0].x = x; gate[0].y = y + 1;
        gate[1].x = x + 1; gate[1].y = y + 1;
        gate[2].x = x + 1; gate[2].y = y;
        gate[3].x = x + 1; gate[3].y = y - 1;
        gate[4].x = x; gate[4].y = y - 1;
    }
}

void PassGate() {
    ANIMATION = 2;
    REMAIN_FRAME = SIZE_SNAKE;
    IS_PLAYING = false;
}

void Appear() {
    ANIMATION = 3;
    REMAIN_FRAME = SIZE_SNAKE;
    IS_PLAYING = false;
}

void DeleteGate() {
    GATE_OPEN = false;
    FOOD_INDEX = 0;
    ClearGate(gate);
}

void GenerateFood() {

    int x, y;
    srand(time(NULL));
    do {
        x = rand() % (WIDTH_CONSOLE - 2) + 1;
        y = rand() % (HEIGHT_CONSOLE - 2) + 1;
    } while (IsValid(x, y) == false);
    food.x = x;
    food.y = y;
}



void ResetData() {
    switch (level) {
    case 0:
        switch (setting.Difficulty) {
        case 0: SPEED = 3; break;
        case 1: SPEED = 5; break;
        case 2: SPEED = 10; break;
        }
        //Initialize the global values
        CHAR_LOCK = 'A', MOVING = 'D'; FOOD_INDEX = 0, SIZE_SNAKE = 6,
            WIDTH_CONSOLE = 60, HEIGHT_CONSOLE = 30, score = 0;
        // Initialize default values for snake
        snake[0] = { 10, 5 }; snake[1] = { 11, 5 };
        snake[2] = { 12, 5 }; snake[3] = { 13, 5 };
        snake[4] = { 14, 5 }; snake[5] = { 15, 5 };
        DeleteGate();
        food_queue.clear();
        GenerateFood(); //Create food
        break;
    case 1:
        switch (setting.Difficulty) {
        case 0: SPEED = 4; break;
        case 1: SPEED = 7; break;
        case 2: SPEED = 8; break;
        }
        CHAR_LOCK = 'A', MOVING = 'D'; FOOD_INDEX = 0, SIZE_SNAKE = 6,
            WIDTH_CONSOLE = 60, HEIGHT_CONSOLE = 30;
        snake[0] = { 10, 5 }; snake[1] = { 11, 5 };
        snake[2] = { 12, 5 }; snake[3] = { 13, 5 };
        snake[4] = { 14, 5 }; snake[5] = { 15, 5 };
        DeleteGate();
        food_queue.clear();
        GenerateFood();
        break;
    case 2:
        switch (setting.Difficulty) {
        case 0: SPEED = 5; break;
        case 1: SPEED = 7; break;
        case 2: SPEED = 8; break;
        }
        CHAR_LOCK = 'D', MOVING = 'A'; FOOD_INDEX = 0, SIZE_SNAKE = 6,
            WIDTH_CONSOLE = 60, HEIGHT_CONSOLE = 30;
        snake[5] = { 49, 27 }; snake[4] = { 50, 27 };
        snake[3] = { 51, 27 }; snake[2] = { 52, 27 };
        snake[1] = { 53, 27 }; snake[0] = { 54, 27 };
        DeleteGate();
        food_queue.clear();
        GenerateFood();
        break;
    }

}

//Function to update global data
void Eat() {

    if (GATE_OPEN == false) {
        score += (setting.Difficulty + 1) * 10;
        if (FOOD_INDEX == MAX_SIZE_FOOD - 1) {
            SpawnGate();
            GATE_OPEN = true;
            RenderGate(gate);

            if (SPEED == MAX_SPEED) {
                ResetData();
            }
            else SPEED++;
        }
        else {
            food_queue.push_back(food);
            FOOD_INDEX++;
            GenerateFood();
        }
    }
}


//Function to process the dead of snake
void ProcessDead() {
    ANIMATION = 1;
    REMAIN_FRAME = SIZE_SNAKE;
    IS_PLAYING = false;
}

void MoveRight() {
    POINT head;
    head.x = snake[SIZE_SNAKE - 1].x + 1;
    head.y = snake[SIZE_SNAKE - 1].y;
    if (IsDead(head)) {
        ProcessDead();
    }
    else {
        if (!GATE_OPEN) {
            if (snake[SIZE_SNAKE - 1].x + 1 == food.x && snake[SIZE_SNAKE - 1].y == food.y) {
                Eat();
            }
        }
        else {
            if (snake[SIZE_SNAKE - 1].x + 1 == epoint.x && snake[SIZE_SNAKE - 1].y == epoint.y) {
                PassGate();
            }
        }
        for (int i = 0; i < SIZE_SNAKE - 1; i++) {
            snake[i].x = snake[i + 1].x;
            snake[i].y = snake[i + 1].y;
        }
        snake[SIZE_SNAKE - 1].x++;
    }
}

void MoveLeft() {
    POINT head;
    head.x = snake[SIZE_SNAKE - 1].x - 1;
    head.y = snake[SIZE_SNAKE - 1].y;
    if (IsDead(head)) {
        ProcessDead();
    }
    else {
        if (!GATE_OPEN) {
            if (snake[SIZE_SNAKE - 1].x - 1 == food.x && snake[SIZE_SNAKE - 1].y == food.y) {
                Eat();
            }
        }
        else {
            if (snake[SIZE_SNAKE - 1].x - 1 == epoint.x && snake[SIZE_SNAKE - 1].y == epoint.y) {
                PassGate();
            }
        }
        for (int i = 0; i < SIZE_SNAKE - 1; i++) {
            snake[i].x = snake[i + 1].x;
            snake[i].y = snake[i + 1].y;
        }
        snake[SIZE_SNAKE - 1].x--;
    }
}

void MoveDown() {
    POINT head;
    head.x = snake[SIZE_SNAKE - 1].x;
    head.y = snake[SIZE_SNAKE - 1].y + 1;
    if (IsDead(head)) {
        ProcessDead();
    }
    else {
        if (!GATE_OPEN) {
            if (snake[SIZE_SNAKE - 1].x == food.x && snake[SIZE_SNAKE - 1].y + 1 == food.y) {
                Eat();
            }
        }
        else {
            if (snake[SIZE_SNAKE - 1].x == epoint.x && snake[SIZE_SNAKE - 1].y + 1 == epoint.y) {
                PassGate();
            }
        }
        for (int i = 0; i < SIZE_SNAKE - 1; i++) {
            snake[i].x = snake[i + 1].x;
            snake[i].y = snake[i + 1].y;
        }
        snake[SIZE_SNAKE - 1].y++;
    }
}

void MoveUp() {
    POINT head;
    head.x = snake[SIZE_SNAKE - 1].x;
    head.y = snake[SIZE_SNAKE - 1].y - 1;
    if (IsDead(head)) {
        ProcessDead();
    }
    else {
        if (!GATE_OPEN) {
            if (snake[SIZE_SNAKE - 1].x == food.x && snake[SIZE_SNAKE - 1].y - 1 == food.y) {
                Eat();
            }
        }
        else {
            if (snake[SIZE_SNAKE - 1].x == epoint.x && snake[SIZE_SNAKE - 1].y - 1 == epoint.y) {
                PassGate();
            }
        }
        for (int i = 0; i < SIZE_SNAKE - 1; i++) {
            snake[i].x = snake[i + 1].x;
            snake[i].y = snake[i + 1].y;
        }
        snake[SIZE_SNAKE - 1].y--;
    }
}

std::wstring s2ws(const std::string& str)
{
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
    std::wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
    return wstrTo;
}

int SaveFile(const char* filename) {
    std::ofstream f;
    f.open(filename);

    if (f.is_open()) {
        // SNAKE
        f << SIZE_SNAKE << std::endl;
        for (int i = 0; i < SIZE_SNAKE; i++) {
            f << snake[i].x << " " << snake[i].y << " ";
        }
        f << std::endl;

        // MOVING
        f << (char)MOVING << " " << (char)CHAR_LOCK << " " << SPEED << std::endl;

        // FOOD
        f << FOOD_INDEX << " " << food.x << " " << food.y << std::endl;
        f << food_queue.size() << std::endl;
        for (int i = 0; i < food_queue.size(); i++) {
            f << food_queue[i].x << " " << food_queue[i].y << " " << std::endl;
        }
        f << GATE_OPEN << std::endl;
        if (GATE_OPEN) {
            for (int i = 0; i < 5; i++)
                f << gate[i].x << " " << gate[i].y << " ";
        }

        f.close();
        return 0;
    }
    else {
        ErrorLog(L"Unable to load file: " + s2ws(std::string(filename)));
        return 1;
    }
}


int LoadFile(const char* filename) {
    std::ifstream f;
    f.open(filename);

    int queue_size = 0;
    POINT food;

    ResetData();

    if (f.is_open()) {
        // SNAKE
        LOADFILE = true;
        f >> SIZE_SNAKE;
        for (int i = 0; i < SIZE_SNAKE; i++) {
            f >> snake[i].x >> snake[i].y;
        }

        // MOVING
        f >> MOVING >> CHAR_LOCK >> SPEED;

        // FOOD
        f >> FOOD_INDEX >> food.x >> food.y;
        f >> queue_size;
        for (int i = 0; i < queue_size; i++) {
            f >> food.x >> food.y;
            food_queue.push_back(food);
        }
        f >> GATE_OPEN;
        if (GATE_OPEN) {
            for (int i = 0; i < 5; i++)
                f >> gate[i].x >> gate[i].y;
        }
        f.close();
        return 0;
    }
    else {
        ErrorLog(L"Unable to load file: " + s2ws(std::string(filename)));
        return 1;
    }
}

int GameLoop(char key) {
    // Actually there are a lot of mini state here
    if (INIT) {
        IS_PLAYING = true;
        IS_PAUSE = true;
        if (!LOADFILE) {
            level = 0;
            ResetData();
            Load_map();
            IS_PAUSE = false;
        }
    }
    LOADFILE = false;

    if (IS_PLAYING) {
        if (IS_PAUSE) {

            RenderGamePause(WIDTH_CONSOLE, HEIGHT_CONSOLE, food, SIZE_SNAKE, snake, INIT);
            INIT = false;

            if ((key == 'W') || (key == 'A') || (key == 'S') || (key == 'D')) {
                // Can add some delay for the player to be ready here
                IS_PAUSE = false;
            }
            else if (key == 'L') {
                INIT = true;
                return 5;
            }
            else if (key == 'T') {
                INIT = true;
                return 2;
            }
            else if (key == 27) {
                INIT = true;
                return 0;
            }
        }
        else {
            if ((key == 'P') || (key == 27)) {
                IS_PAUSE = true;
            }
            else {
                if ((key == 'W') || (key == 'A') || (key == 'S') || (key == 'D')) {
                    MOVING = key;
                }
                ClearSnake(SIZE_SNAKE, snake);

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

                RenderGame(WIDTH_CONSOLE, HEIGHT_CONSOLE, food, SIZE_SNAKE, snake, wall, level, wall_num, GATE_OPEN, gate);
                RenderScore(score);
                INIT = false;
                Render();
                std::this_thread::sleep_for(std::chrono::milliseconds(1000 / SPEED)); // Pause thread for x ms
            }
        }
    }
    else {
        switch (ANIMATION) {
        case 0: {
            if (key == 'Y') {
                ClearSnake(SIZE_SNAKE, snake);
                ClearFood(food);
                Clear_wall(wall, wall_num);
                level = 0;
                ResetData();
                RenderGame(WIDTH_CONSOLE, HEIGHT_CONSOLE, food, SIZE_SNAKE, snake, wall, level, wall_num, GATE_OPEN, gate);
                IS_PLAYING = true;
            }
            else if (key == 'T') {
                INIT = true;
                return 2;
            }
            else if (key == 27) {
                INIT = true;
                return 0;
            }
            break;
        }
        case 1: {
            // Dead animation
            if (REMAIN_FRAME > 0) {

                DeadAnimation(snake[REMAIN_FRAME - 1]);
                REMAIN_FRAME--;
                Render();
                std::this_thread::sleep_for(std::chrono::milliseconds(90));
            }
            else {

                ANIMATION = 0;
                level = 0;
            }
            break;
        }
        case 2: {
            // Passing gate animation
            if (REMAIN_FRAME > 0) {
                Passing(snake, SIZE_SNAKE, REMAIN_FRAME);
                Render();
                REMAIN_FRAME--;
                std::this_thread::sleep_for(std::chrono::milliseconds(1000 / SPEED));
            }
            else {
                // After Animation stuff
                ClearSnake(SIZE_SNAKE, snake);
                Render();
                std::this_thread::sleep_for(std::chrono::milliseconds(1000 / SPEED));
                DeleteGate();
                Clear_wall(wall, wall_num);
                level++;
                if (level != MAX_LEVEL) {
                    IS_PLAYING = true;
                    Load_map();
                    ResetData();
                }
                else {
                    ANIMATION = 4;
                }
            }
            break;
        }
        case 3:
            // after passing gate and appearing in new map.
            if (REMAIN_FRAME > 0) {

            }
            break;
        case 4: {
            clearScreen();
            renderEnding();
            Render();
            if (key == 'Y') {
                ClearSnake(SIZE_SNAKE, snake);
                ClearFood(food);
                Clear_wall(wall, wall_num);
                level = 0;
                ResetData();
                RenderGame(WIDTH_CONSOLE, HEIGHT_CONSOLE, food, SIZE_SNAKE, snake, wall, level, wall_num, GATE_OPEN, gate);
                IS_PLAYING = true;
            }
            else if (key == 27) {
                INIT = true;
                return 0;
            }
            break;
        }
        }
    }
    return 1;
}

int Menu_State(char key) {
    static int CURSOR = 0;
    ClearMenu(CURSOR);
    if (key == 'W') {
        CURSOR -= 1;
    }
    else if (key == 'S') {
        CURSOR += 1;
    }
    else if (key == 13) {
        INIT = true;
        Render();
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        return CURSOR + 1;
    }
    CURSOR = CURSOR % 4;
    if (CURSOR < 0) CURSOR += 4;
    RenderMenu(CURSOR, INIT);
    INIT = false;
    Render();
    std::this_thread::sleep_for(std::chrono::milliseconds(150));

    return 0;
}

int Setting_State(char key) {
    static int CURSOR = 0;
    // ClearSettings(CURSOR);
    if (key == 'W') {
        CURSOR -= 1;
    }
    else if (key == 'S') {
        CURSOR += 1;
    }
    else if ((key == 13) || (key == 27)) {
        if (CURSOR == 5) {
            Render();
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            INIT = true;
            return 0;
        }
    }
    else if (key == 'A') {
        switch (CURSOR) {
        case 0: {
            if (setting.Difficulty > 0) setting.Difficulty--;
            break;
        }
        case 1: {
            if (setting.TotalVolume > 0) setting.TotalVolume--;
            break;
        }
        case 2: {
            if (setting.MusicVolume > 0) setting.MusicVolume--;
            break;
        }
        case 3: {
            if (setting.SFXVolume > 0) setting.SFXVolume--;
            break;
        }
        case 4: {
            setting.BGM = !setting.BGM;
            break;
        }
        }
    }
    else if (key == 'D') {
        switch (CURSOR) {
        case 0: {
            if (setting.Difficulty < 2) setting.Difficulty++;
            break;
        }
        case 1: {
            if (setting.TotalVolume < 10) setting.TotalVolume++;
            break;
        }
        case 2: {
            if (setting.MusicVolume < 10) setting.MusicVolume++;
            break;
        }
        case 3: {
            if (setting.SFXVolume < 10) setting.SFXVolume++;
            break;
        }
        case 4: {
            setting.BGM = !setting.BGM;
            break;
        }
        }
    }
    ChangeAudioConfig(setting);
    CURSOR = CURSOR % 6;
    if (CURSOR < 0) CURSOR += 6;
    RenderSettings(setting, CURSOR, true);
    INIT = false;
    Render();
    std::this_thread::sleep_for(std::chrono::milliseconds(150));

    return 3;
}

