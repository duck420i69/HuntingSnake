#pragma comment(lib, "winmm.lib")

#include <iostream>
#include <array>
#include <vector>
#include <cstring>
#include <string>
#include <chrono>
#include <thread>
#include <condition_variable>
#include <mutex>
#include "Windows.h"
#include "Audio.h"
#include "Render.h"
#include <fstream>

using namespace std;

#define DATA_TYPE short



typedef struct WAV_HEADER {
    uint8_t         RIFF[4];        // RIFF Header
    uint32_t        ChunkSize;      // RIFF Chunk Size
    uint8_t         WAVE[4];        // WAVE Header
    uint8_t         fmt[4];         // FMT header
    uint32_t        Subchunk1Size;  // Size of the fmt chunk
    uint16_t        AudioFormat;    // Audio format 1=PCM,6=mulaw,7=alaw,     257=IBM Mu-Law, 258=IBM A-Law, 259=ADPCM
    uint16_t        NumOfChan;      // Number of channels 1=Mono 2=Sterio
    uint32_t        SamplesPerSec;  // Sampling Frequency in Hz
    uint32_t        bytesPerSec;    // bytes per second
    uint16_t        blockAlign;     // 2=16-bit mono, 4=16-bit stereo
    uint16_t        bitsPerSample;  // Number of bits per sample
    uint8_t         Subchunk2ID[4]; // "data"  string
    uint32_t        Subchunk2Size;  // Sampled data length   
}wav_hdr;

struct Audio {
    int id;
    AudioType type;
    wav_hdr header;
    WAVEFORMATEX waveFormat;
    const DATA_TYPE* data;
};

struct SoundHandler {
    bool isPlaying;
    AudioType type;
    bool Loop;
    unsigned int byteleft;
    unsigned int size;
    const DATA_TYPE* pCurrent;
    const DATA_TYPE* pData;
};

Config DefaultSetting;

vector<Audio> Sounds;                   // Contain all the sounds
vector<wstring> Devices;                // Contain all the devices
vector<SoundHandler> hSounds;           // Contain all the sound need to be played

DATA_TYPE* BlockMemory = nullptr;       // Memory holds all the blocks
WAVEHDR* WaveHeaders = nullptr;         // Memory holds WAVEHDR
HWAVEOUT DeviceOut = nullptr;                     // Audio Device to send the data

bool thread_exit = false;

const unsigned int BlockCount = 8;      // Number of Blocks
const unsigned int BlockSamples = 512;  // Number of Samples per Block
unsigned int BlockFree = BlockCount;    // Number of free Blocks
unsigned int BlockCurrent = 0;          // Current Block that need to fill and write to the queue

condition_variable cvBlockNotZero;
condition_variable cvPause;
condition_variable cvActive;
mutex muxBlockNotZero;
mutex muxFillBlock;
mutex muxPause;
mutex muxActive;


bool AudioInit() {
    int DeviceCount = waveOutGetNumDevs();

    for (int i = 0; i < DeviceCount; i++) {
        WAVEOUTCAPS woc;
        if (waveOutGetDevCaps(i, &woc, sizeof(woc)) == S_OK) Devices.push_back(woc.szPname);
    }

    BlockMemory = new DATA_TYPE[BlockCount * BlockSamples];
    if (BlockMemory == nullptr) return false;
    ZeroMemory(BlockMemory, sizeof(DATA_TYPE) * BlockCount * BlockSamples);

    WaveHeaders = new WAVEHDR[BlockCount];
    if (WaveHeaders == nullptr) {
        delete[] BlockMemory;
        return false;
    }
    ZeroMemory(WaveHeaders, sizeof(WAVEHDR) * BlockCount);

    for (int i = 0; i < BlockCount; i++) {
        WaveHeaders[i].dwBufferLength = BlockSamples * sizeof(DATA_TYPE);
        WaveHeaders[i].lpData = (LPSTR)(BlockMemory + i * BlockSamples);
    }

    return true;
}

void AudioExit() {
    thread_exit = true;
    cvBlockNotZero.notify_one();
    for (auto& sound : Sounds) {
        delete[] sound.data;
    }

    delete[] BlockMemory;
    delete[] WaveHeaders;

    hSounds.clear();
    Sounds.clear();
    Devices.clear();
}

void ChangeAudioConfig(Config setting) {
    DefaultSetting = setting;
    if (DefaultSetting.UseAudio != setting.UseAudio)
    if (setting.UseAudio) {
        unique_lock<mutex> pause(muxPause);
        cvPause.notify_one();
    }
    else {

    }
}

bool LoadAudio(const char* name, AudioType type) {
    Audio sample;
    FILE* f = nullptr;
    fopen_s(&f, name, "rb");
    if (f == nullptr)
        return false;

    // Read file
    fread_s(&sample.header.RIFF, 4, 4, 1, f);
    if (strncmp((char*)sample.header.RIFF, "RIFF", 4) != 0) return false;
    fread_s(&sample.header.ChunkSize, 4, 4, 1, f);
    fread_s(&sample.header.WAVE, 4, 4, 1, f);
    if (strncmp((char*)sample.header.WAVE, "WAVE", 4) != 0) return false;
    fread_s(&sample.header.fmt, 4, 4, 1, f);
    if (strncmp((char*)sample.header.fmt, "JUNK", 4) == 0) {
        uint32_t n;
        uint16_t buffer;
        fread_s(&n, 4, 4, 1, f);
        if (n % 2 == 1) n++;
        for (int i = 0; i < n / 2; i++) {
            fread_s(&buffer, 2, 2, 1, f);
        }
        fread_s(&sample.header.fmt, 4, 4, 1, f);
    }
    if (strncmp((char*)sample.header.fmt, "fmt ", 4) != 0) return false;
    fread_s(&sample.header.Subchunk1Size, 4, 4, 1, f);
    fread_s(&sample.header.AudioFormat, 2, 2, 1, f);
    fread_s(&sample.header.NumOfChan, 2, 2, 1, f);
    fread_s(&sample.header.SamplesPerSec, 4, 4, 1, f);
    fread_s(&sample.header.bytesPerSec, 4, 4, 1, f);
    fread_s(&sample.header.blockAlign, 2, 2, 1, f);
    fread_s(&sample.header.bitsPerSample, 2, 2, 1, f);
    fread_s(&sample.header.Subchunk2ID, 4, 4, 1, f);
    if (strncmp((char*)sample.header.Subchunk2ID, "data", 4) != 0) return false;
    fread_s(&sample.header.Subchunk2Size, 4, 4, 1, f);

    // Fill WAVEFORMATEX
    sample.waveFormat.wFormatTag = sample.header.AudioFormat;
    sample.waveFormat.nSamplesPerSec = sample.header.SamplesPerSec;
    sample.waveFormat.wBitsPerSample = sample.header.bitsPerSample;
    sample.waveFormat.nChannels = sample.header.NumOfChan;
    sample.waveFormat.nBlockAlign = sample.header.blockAlign;
    sample.waveFormat.nAvgBytesPerSec = sample.header.SamplesPerSec * sample.header.blockAlign;
    sample.waveFormat.cbSize = 0;

    // Test
    if (sample.waveFormat.wFormatTag != sample.header.AudioFormat) return false;
    if (sample.waveFormat.nSamplesPerSec != sample.header.SamplesPerSec) return false;
    if (sample.waveFormat.wBitsPerSample != sample.header.bitsPerSample) return false;
    if (sample.waveFormat.nChannels != sample.header.NumOfChan) return false;
    if (sample.waveFormat.nBlockAlign != sample.header.blockAlign) return false;
    if (sample.waveFormat.nAvgBytesPerSec != sample.header.SamplesPerSec * sample.header.blockAlign) return false;
    sample.waveFormat.cbSize = 0;

    // Read data
    sample.data = new DATA_TYPE[sample.header.Subchunk2Size / sizeof(DATA_TYPE)];
    fread_s((void*)sample.data, sample.header.Subchunk2Size, sample.header.Subchunk2Size, 1, f);
    
    sample.type = type;
    sample.id = Sounds.size();
    Sounds.push_back(sample);
    return true;
}


// CALLBACK function for Windows (CALL for every event Windows sent)
static void CALLBACK waveOutProc(HWAVEOUT hWaveOut, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2) {
    if (uMsg != WOM_DONE) return;
    BlockFree++;
    unique_lock<mutex> lm(muxBlockNotZero);
    cvBlockNotZero.notify_one();
}

// Play sound by id
bool playSound(int id, bool loop) {
    Audio* buffer = nullptr;
    for (auto& sound : Sounds) {
        if (sound.id == id) {
            buffer = &sound;
        }
    }
    if (buffer == nullptr) return false;

    // Open playing Device (if not open yet)
    if (DeviceOut == nullptr)
        if (waveOutOpen(&DeviceOut, WAVE_MAPPER, &buffer->waveFormat, (DWORD_PTR)waveOutProc, (DWORD_PTR)&BlockFree, CALLBACK_FUNCTION) != S_OK) return false;

    SoundHandler a;
    a.isPlaying = true;
    a.byteleft = buffer->header.Subchunk2Size;
    a.type = buffer->type;
    a.size = buffer->header.Subchunk2Size;
    a.pCurrent = buffer->data;
    a.pData = buffer->data;
    a.Loop = loop;

    unique_lock<mutex> fb(muxFillBlock);
    hSounds.push_back(a);

    cvActive.notify_one();

    return true;
}

// Mixer to make final wave
void FillAudioBlocks(DATA_TYPE* Blocks, unsigned int Samples) {
    static long int k = pow(2, sizeof(DATA_TYPE) * 8 - 1) - 1;
    long int newSample = 0;

    for (int i = 0; i < Samples; i++) {
        unique_lock<mutex> fb(muxFillBlock);
        newSample = 0;
        for (auto& sound : hSounds) {
            if (sound.isPlaying) {
                if (sound.byteleft > 0) {
                    switch (sound.type) {
                    case AudioType::BGMusic: {
                        if (DefaultSetting.BGM) {
                            newSample += *(sound.pCurrent) * DefaultSetting.TotalVolume * DefaultSetting.MusicVolume / 100;
                            sound.pCurrent++;
                            sound.byteleft -= sizeof(DATA_TYPE);
                        }
                        break;
                    }
                    case AudioType::SFX: {
                        newSample += *(sound.pCurrent) * DefaultSetting.TotalVolume * DefaultSetting.SFXVolume / 100;
                        sound.pCurrent++;
                        sound.byteleft -= sizeof(DATA_TYPE);
                        break;
                    }
                    }

                }
                else {
                    sound.isPlaying = false;
                }
            }
        }

        // Clipping
        if (newSample > k) {
            if (newSample > k) newSample = k;
        }
        else {
            if (newSample < -k) newSample = -k;
        }
        Blocks[i] = newSample;
    }

    // Remove sound that done playing
    int i = 0;
    while (i < hSounds.size()) {
        if (!hSounds[i].isPlaying) {
            if (hSounds[i].Loop) {
                hSounds[i].isPlaying = true;
                hSounds[i].pCurrent = hSounds[i].pData;
                hSounds[i].byteleft = hSounds[i].size;
                i++;
            }
            else hSounds.erase(hSounds.begin() + i);
        }
        else i++;
    }

}

void AudioThread() {

    while (!thread_exit) {

        if (DefaultSetting.UseAudio) {
            if (hSounds.size() == 0) {
                unique_lock<mutex> pl(muxActive);
                while (hSounds.size() == 0) {
                    // Sleep until there is sound to play
                    cvActive.wait(pl);
                    if (thread_exit) break;
                }
            }
            if (BlockFree == 0) {
                unique_lock<mutex> lm(muxBlockNotZero);
                while (BlockFree == 0) {
                    // Sleep until block available
                    cvBlockNotZero.wait(lm);
                    if (thread_exit) break;
                }
            }
            if (thread_exit) break;


            BlockFree--;

            if (WaveHeaders[BlockCurrent].dwFlags & WHDR_PREPARED)
                waveOutUnprepareHeader(DeviceOut, &WaveHeaders[BlockCurrent], sizeof(WAVEHDR));

            int CurrentBlock = BlockCurrent * BlockSamples; // iter point to the current block

            FillAudioBlocks(&BlockMemory[CurrentBlock], BlockSamples);

            waveOutPrepareHeader(DeviceOut, &WaveHeaders[BlockCurrent], sizeof(WAVEHDR));
            waveOutWrite(DeviceOut, &WaveHeaders[BlockCurrent], sizeof(WAVEHDR));

            BlockCurrent++;
            BlockCurrent %= BlockCount;
        }
        else {
            unique_lock<mutex> pause(muxPause);
            cvPause.wait(pause);
        }
    }
}
