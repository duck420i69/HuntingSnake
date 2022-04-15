// HuntingSnake.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "Windows.h"
#include "winuser.h"
#include <thread>
#include <conio.h>
#include <deque>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include "Audio.h"
#include "State.h"
#include "Render.h"

using namespace std;

//Global variables
char volatile temp = '.';
char volatile prev_key = '.';
char volatile key = ',';
int k = 0;
int CUR_STATE = 0;
bool volatile TYPING = false;       
bool STOP_THREAD = false;
atomic<bool> GetInput;
char str[20];

mutex muxKeyPress;
condition_variable cvGetKey;

mutex muxInputString;
condition_variable cvGetString;

void FixConsoleWindow() {
    HWND consoleWindow = GetConsoleWindow(); // Console window handler
    LONG style = GetWindowLong(consoleWindow, GWL_STYLE);
    SetConsoleTitle(TEXT("Hunting Snake")); // Set title
    style = style & ~(WS_MAXIMIZEBOX) & ~(WS_THICKFRAME);
    SetWindowLong(consoleWindow, GWL_STYLE, style);
}

void ThreadFunc() {
    while (!STOP_THREAD) {
        {
            GetInput = true;
            unique_lock<mutex> input(muxKeyPress);
            if (prev_key == temp) {
                key = '.';
                temp = '.';
                prev_key = key;
            }
            else {
                prev_key = key;
                key = temp;
                temp = '.';
            }
            GetInput = false;
            cvGetKey.notify_one();
        }
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
                TYPING = true;
                std::chrono::milliseconds(10);
                RenderLoad();
                Render();
                while (TYPING) std::this_thread::sleep_for(std::chrono::milliseconds(50));
                if (LoadFile(str)) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                    CUR_STATE = 0;
                    temp = '.';
                }
                else CUR_STATE = 1;
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
                cvGetKey.notify_one();
                break;
            }
            case 5: {
                // SAVE STATE
                TYPING = true;
                std::chrono::milliseconds(10);
                RenderSave();
                Render();
                while (TYPING) std::this_thread::sleep_for(std::chrono::milliseconds(50));
                SaveFile(str);
                temp = '.';
                CUR_STATE = 1;
                break;
            }
        }
    }
}


int main() {

    FixConsoleWindow();
       
    RenderInit();

    if (AudioInit()) {
        LoadAudio("eat.wav", AudioType::SFX);
        LoadAudio("Bubble heavy 2.wav", AudioType::SFX);
        LoadAudio("BG.wav", AudioType::BGMusic);
        LoadAudio("beep.wav", AudioType::SFX);
        thread tAudio = thread(AudioThread);
        thread t1(ThreadFunc); //Create thread for snake  
        playSound(2, true);
        while (1) {
            if (!TYPING) {
                unique_lock<mutex> input(muxKeyPress);

                if (GetAsyncKeyState('A') & 0x8000) temp = 'A';
                else if (GetAsyncKeyState('S') & 0x8000) temp = 'S';
                else if (GetAsyncKeyState('D') & 0x8000) temp = 'D';
                else if (GetAsyncKeyState('W') & 0x8000) temp = 'W';
                else if (GetAsyncKeyState('P') & 0x8000) temp = 'P';
                else if (GetAsyncKeyState('Y') & 0x8000) temp = 'Y';
                else if (GetAsyncKeyState('L') & 0x8000) temp = 'L';
                else if (GetAsyncKeyState('T') & 0x8000) temp = 'T';
                else if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) temp = 27;
                else if (GetAsyncKeyState(VK_RETURN) & 0x8000) temp = 13;
                if (GetInput == true) {
                    cvGetKey.wait(input);
                }
                if (CUR_STATE == 4) {
                    RenderExit();
                    if (t1.joinable()) t1.join();
                    AudioExit();
                    if (tAudio.joinable()) tAudio.join();
                    break;
                }
            }
            else {
                cin.ignore(INT_MAX, '\n');
                cin >> str;
                TYPING = false;
            }
        }

    }
    return 0;
}

