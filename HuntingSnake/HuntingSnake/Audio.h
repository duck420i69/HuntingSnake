#pragma once

#include "Windows.h"
#include "Render.h"

// Start Audio Engine
bool AudioInit();

// Exit Audio Engine
void AudioExit();

void ChangeAudioConfig(Config setting);

// Load Sound
void LoadAudio(const char* name);

// Play music with id
bool playSound(int id);

void AudioThread();