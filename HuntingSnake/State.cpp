// Define how state work

#include <deque>
#include <array>
#include "Render.h"
#include <thread>
#include <chrono>
#include <conio.h>
#include <fstream>
#include <iostream>


bool DEBUG_MODE = 1;

//Constants
#define MAX_SIZE_SNAKE 25  
#define MAX_SIZE_FOOD 4             // Amount of food to get to next level (can add 1 more for gate)
#define MAX_SPEED 9

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
int ANIMATION;                      // Animation to play (0: None, 1: Dead, 2: Passing Gate)
int REMAIN_FRAME;                   // Remaining frame in  animation

bool IsValid(int x, int y) {
    for (int i = 0; i < SIZE_SNAKE; i++)
        if (snake[i].x == x && snake[i].y == y)
            return false;
    return true;
}


bool IsCollision(POINT a, POINT b) {
    return ((a.x == b.x) && (a.y == b.y));
}

bool IsDead(POINT head) {
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

    if ((head.x <= 0) || (head.x >= WIDTH_CONSOLE) || (head.y <= 0) || (head.y >= HEIGHT_CONSOLE))
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
        gate[2].x = x    ; gate[2].y = y - 1;
        gate[3].x = x + 1; gate[3].y = y - 1;
        gate[4].x = x + 1; gate[4].y = y;
    }
    else if (z == 1) {
        gate[0].x = x    ; gate[0].y = y + 1;
        gate[1].x = x - 1; gate[1].y = y + 1;
        gate[2].x = x - 1; gate[2].y = y;
        gate[3].x = x - 1; gate[3].y = y - 1;
        gate[4].x = x    ; gate[4].y = y - 1;
    }
    else if (z == 2) {
        gate[0].x = x - 1; gate[0].y = y;
        gate[1].x = x - 1; gate[1].y = y + 1;
        gate[2].x = x    ; gate[2].y = y + 1;
        gate[3].x = x + 1; gate[3].y = y + 1;
        gate[4].x = x + 1; gate[4].y = y;
    }
    else if (z == 3) {
        gate[0].x = x    ; gate[0].y = y + 1;
        gate[1].x = x + 1; gate[1].y = y + 1;
        gate[2].x = x + 1; gate[2].y = y;
        gate[3].x = x + 1; gate[3].y = y - 1;
        gate[4].x = x    ; gate[4].y = y - 1;
    }
}

void PassGate() {
    ANIMATION = 2;
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
        x = rand() % (WIDTH_CONSOLE - 1) + 1;
        y = rand() % (HEIGHT_CONSOLE - 1) + 1;
    } while (IsValid(x, y) == false);
    food.x = x;
    food.y = y;
}


void ResetData() {
    //Initialize the global values
    CHAR_LOCK = 'A', MOVING = 'D', SPEED = 5; FOOD_INDEX = 0, SIZE_SNAKE = 6,
        WIDTH_CONSOLE = 40, HEIGHT_CONSOLE = 20;
    // Initialize default values for snake
    snake[0] = { 10, 5 }; snake[1] = { 11, 5 };
    snake[2] = { 12, 5 }; snake[3] = { 13, 5 };
    snake[4] = { 14, 5 }; snake[5] = { 15, 5 };
    DeleteGate();
    food_queue.clear();
    GenerateFood(); //Create food

}

//Function to update global data
void Eat() {

    if (GATE_OPEN == false) {
        if (FOOD_INDEX == MAX_SIZE_FOOD - 1) {
            SpawnGate();
            GATE_OPEN = true;
            RenderGate(gate);

            if (SPEED == MAX_SPEED) ResetData();
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
        f << (char) MOVING << " " << (char) CHAR_LOCK << " " << SPEED << std::endl;

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
        ErrorLog("Unable to load file: " + std::string(filename));
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
        ErrorLog("Unable to load file: " + std::string(filename));
        return 1;
    }
}

int GameLoop(char key) {
    // Actually there are a lot of mini state here
    if (INIT) {
        IS_PLAYING = true;
        IS_PAUSE = true;
        if (!LOADFILE) {
            ResetData();
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

                RenderGame(WIDTH_CONSOLE, HEIGHT_CONSOLE, food, SIZE_SNAKE, snake, GATE_OPEN, INIT);
                INIT = false;

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
                ResetData();
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
                std::this_thread::sleep_for(std::chrono::milliseconds(90));
            }
            else {
                ANIMATION = 0;
            }
            break;
        }
        case 2: {
            // Passing gate animation
            if (REMAIN_FRAME > 0) {
                DeadAnimation(snake[SIZE_SNAKE - REMAIN_FRAME]);
                REMAIN_FRAME--;
                std::this_thread::sleep_for(std::chrono::milliseconds(90));
            }
            else {
                // After Animation stuff
                ANIMATION = 0;
                DeleteGate();
                IS_PLAYING = true;
            }
            break;
        }
        }
    }
    return 1;
}

int Menu_State(char key) {
    static int CURSOR;
    ClearMenu(CURSOR);
    if (key == 'W') {
        CURSOR -= 1;
    }
    else if (key == 'S') {
        CURSOR += 1;
    }
    else if (key == 13) {
        INIT = true;
        return CURSOR + 1;
    }
    CURSOR = CURSOR % 4;
    if (CURSOR < 0) CURSOR += 4;
    RenderMenu(CURSOR, INIT);
    INIT = false;
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    return 0;
}

int Setting_State(char key) {
    static int CURSOR = 0;
    ClearSettings(CURSOR);
    if (key == 'W') {
        CURSOR -= 1;
    }
    else if (key == 'S') {
        CURSOR += 1;
    }
    else if (key == 13) {
        if (CURSOR == 4) {
            INIT = true;
            return 0;
        }
    }
    CURSOR = CURSOR % 5;
    if (CURSOR < 0) CURSOR += 5;
    RenderSettings(CURSOR, INIT);
    INIT = false;
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    return 3;
}
