#pragma once

#include <string>


int GameLoop(char key);

int Menu_State(char key);

int Setting_State(char key);

void SaveFile(const char* filename);

void LoadFile(const char* filename);
