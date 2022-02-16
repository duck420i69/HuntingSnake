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
POINT fpoint;                       
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
int ANIMATION;                      // Animation to play (0: None, 1: Dead, 2: Passing Gate)
int REMAIN_FRAME;                   // Remaining frame in  animation

bool IsValid(int x, int y) {
    for (int i = 0; i < SIZE_SNAKE; i++)
        if (snake[i].x == x && snake[i].y == y)
            return false;
    return true;
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


void BuildGate() {
    
}

//Function to process the dead of snake
void ProcessDead() {
    ANIMATION = 1;
    REMAIN_FRAME = SIZE_SNAKE;
    IS_PLAYING = false;
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
    if (snake[SIZE_SNAKE - 1].y + 1 == HEIGHT_CONSOLE) {
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

void SaveFile(const char* filename) {
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
            f << food_queue[i].x << " " << food_queue[i].y << " ";
        }

        f.close();
    }
    else {
        ErrorLog("Unable to load file: " + std::string(filename));
    }
}


void LoadFile(const char* filename) {
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
        f.close();
    } 
    else {
        ErrorLog("Unable to load file: " + std::string(filename));
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

    if (IS_PLAYING) {
        if (IS_PAUSE) {
            if (INIT) {
                RenderGamePause(WIDTH_CONSOLE, HEIGHT_CONSOLE, food, SIZE_SNAKE, snake, INIT);
                INIT = false;
            }
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

                for (int i = 0; i < SIZE_SNAKE - 1; i++) {
                    for (int j = i + 1; j < SIZE_SNAKE; j++) {
                        if ((snake[i].x == snake[j].x) && (snake[i].y == snake[j].y)) {
                            ProcessDead();
                        }
                    }
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

                RenderGame(WIDTH_CONSOLE, HEIGHT_CONSOLE, food, SIZE_SNAKE, snake, INIT);
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
                std::this_thread::sleep_for(std::chrono::milliseconds(70));
            }
            else {
                ANIMATION = 0;
            }
            break;
        }
        case 2: {
            // Passing gate animation
            if (REMAIN_FRAME > 0) {
                ANIMATION = 0;
                std::this_thread::sleep_for(std::chrono::milliseconds(70));
            }
            else {
                // After Animation stuff
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
