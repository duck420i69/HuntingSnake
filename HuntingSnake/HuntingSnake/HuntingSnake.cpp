// HuntingSnake.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "Windows.h"
#include "winuser.h"
#include <thread>
#include <conio.h>
#include <deque>
#include "State.h"
#include "Render.h"

using namespace std;

//Global variables
char temp = '.';
char key = ',';
int PRE_STATE = -1;
int CUR_STATE = 0;
bool TYPING = false;       
bool STOP_THREAD = false;
char str[20];



void FixConsoleWindow() {
    HWND consoleWindow = GetConsoleWindow(); // Console window handler
    LONG style = GetWindowLong(consoleWindow, GWL_STYLE);
    SetConsoleTitle(TEXT("Hunting Snake")); // Set title
    style = style & ~(WS_MAXIMIZEBOX) & ~(WS_THICKFRAME);
    SetWindowLong(consoleWindow, GWL_STYLE, style);
}

void ThreadFunc() {
    
    while (1) {
        key = temp;
        if (STOP_THREAD) break;
        switch (CUR_STATE) {
        case 0: {
            // MENU STATE
            CUR_STATE = Menu_State(key);
            break;
        }
        case 1: {
            // IN GAME STATE
            CUR_STATE = GameLoop(key);
            break;
        }
        case 2: {
            // LOAD STATE
            RenderLoad();
            TYPING = true;
            while (TYPING) std::this_thread::sleep_for(std::chrono::milliseconds(50));
            LoadFile(str);
            CUR_STATE = 1;
            break;
        }
        case 3: {
            // SETTINGS STATE
            CUR_STATE = Setting_State(key);
            break;
        }
        case 4: {
            // EXIT
            STOP_THREAD = true;
            break;
        }
        case 5: {
            // SAVE STATE
            RenderSave();
            TYPING = true;
            while (TYPING) std::this_thread::sleep_for(std::chrono::milliseconds(50));
            SaveFile(str);
            CUR_STATE = 0;
            break;
        }
        }

    }
}


 int main() {

     FixConsoleWindow();


     thread t1(ThreadFunc); //Create thread for snake  
     HANDLE handle_t1 = t1.native_handle(); //Take handle of thread
     while (1) {
         if (!TYPING) {
             temp = '.';
             temp = toupper(_getch());
             while (key != temp) {
                 if (STOP_THREAD || TYPING) break;
                 std::this_thread::sleep_for(std::chrono::milliseconds(2));
             }
             if (CUR_STATE == 4) {
                 if (t1.joinable()) t1.join();
                 break;
             }
         }
         else {
             cin >> str;
             TYPING = false;
         }
     }
     return 0;
 }

