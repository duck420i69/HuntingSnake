// HuntingSnake.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "Windows.h"
#include "winuser.h"
#include <thread>
#include <conio.h>
#include <deque>
#include "State.h"

using namespace std;

//Global variables
char temp = '.';
char key = ',';
int PRE_STATE = -1;
int CUR_STATE = 0;
bool TYPING = false;         // Whether using key to interact or typing a string for save file or load file (TO-DO: find some way to pass the string to the running thread)
bool STOP_THREAD = false;


void FixConsoleWindow() {
    HWND consoleWindow = GetConsoleWindow(); // Console window handler
    LONG style = GetWindowLong(consoleWindow, GWL_STYLE);
    style = style & ~(WS_MAXIMIZEBOX) & ~(WS_THICKFRAME);
    SetWindowLong(consoleWindow, GWL_STYLE, style);
}

void ThreadFunc() {
    
    while (1) {
        key = ',';
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
            break;
        }
        case 3: {
            // SETTINGS STATE
            break;
        }
        case 4: {
            // EXIT
            STOP_THREAD = true;
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
             // ADVANCE STUFF: some how make communicate to the process thread smoother
             temp = '.';
             temp = toupper(_getch());
             while (key != temp) {
                 std::this_thread::sleep_for(std::chrono::milliseconds(10));
             }
             if (CUR_STATE == 4) {
                 t1.join();
                 break;
             }
         }
         else {

         }
     }
     return 0;
 }

