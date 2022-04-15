#pragma once

#include "Windows.h"
#include "Render.h"

enum class AudioType {
    BGMusic,
    SFX,
    COUNT
};

// Start Audio Engine
bool AudioInit();

// Exit Audio Engine
void AudioExit();

void ChangeAudioConfig(Config setting);

// Load Sound
bool LoadAudio(const char* name, AudioType type);

// Play music with id
bool playSound(int id, bool loop = false);

void AudioThread();