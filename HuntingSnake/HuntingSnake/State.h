#pragma once

#include <string>


int GameLoop(char key);

int Menu_State(char key);

int Setting_State(char key);

int SaveFile(const char* filename);

int LoadFile(const char* filename);
