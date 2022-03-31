#pragma comment(lib, "winmm.lib")

#include <iostream>
#include <array>
#include <vector>
#include <cstring>
#include <string>
#include <atomic>
#include <condition_variable>
#include <mutex>
#include "Windows.h"
#include "Audio.h"
#include "Render.h"
#include <fstream>

using namespace std;

#define DATA_TYPE short

typedef struct WAV_HEADER {
    uint8_t         RIFF[4];        // RIFF Header Magic header
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
    wav_hdr header;
    WAVEFORMATEX waveFormat;
    const DATA_TYPE* data;
};

struct SoundHandler {
    bool isPlaying;
    unsigned int byteleft;
    const DATA_TYPE* pCurrent;
};

Config DefaultSetting;

vector<Audio> Sounds;                   // Contain all the sounds
vector<wstring> Devices;                // Contain all the devices
vector<SoundHandler> hSounds;           // Contain all the sound need to be played

DATA_TYPE* BlockMemory = nullptr;       // Memory holds all the blocks
WAVEHDR* WaveHeaders = nullptr;         // Memory holds WAVEHDR
HWAVEOUT DeviceOut;                     // Audio Device to send the data

bool thread_exit = false;

const unsigned int BlockCount = 8;      // Number of Blocks
const unsigned int BlockSamples = 512;  // Number of Samples per Block
unsigned int BlockFree = BlockCount;    // Number of free Blocks
unsigned int BlockCurrent = 0;          // Current Block that need to fill and write to the queue

condition_variable cvBlockNotZero;
mutex muxBlockNotZero;
mutex muxFillBlock;


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

    return true;
}

void AudioExit() {
    thread_exit = true;
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
}

void LoadAudio(const char* name) {
    Audio sample;
    FILE* f = nullptr;
    fopen_s(&f, name, "rb");
    if (f == nullptr)
        return;

    fread_s(&sample.header.RIFF, 4, 4, 1, f);
    if (strncmp((char*)sample.header.RIFF, "RIFF", 4) != 0) return;
    fread_s(&sample.header.ChunkSize, 4, 4, 1, f);
    fread_s(&sample.header.WAVE, 4, 4, 1, f);
    if (strncmp((char*)sample.header.WAVE, "WAVE", 4) != 0) return;
    fread_s(&sample.header.fmt, 4, 4, 1, f);
    if (strncmp((char*)sample.header.fmt, "fmt ", 4) != 0) return;
    fread_s(&sample.header.Subchunk1Size, 4, 4, 1, f);
    fread_s(&sample.header.AudioFormat, 2, 2, 1, f);
    fread_s(&sample.header.NumOfChan, 2, 2, 1, f);
    fread_s(&sample.header.SamplesPerSec, 4, 4, 1, f);
    fread_s(&sample.header.bytesPerSec, 4, 4, 1, f);
    fread_s(&sample.header.blockAlign, 2, 2, 1, f);
    fread_s(&sample.header.bitsPerSample, 2, 2, 1, f);
    fread_s(&sample.header.Subchunk2ID, 4, 4, 1, f);
    if (strncmp((char*)sample.header.Subchunk2ID, "data", 4) != 0) return;
    fread_s(&sample.header.Subchunk2Size, 4, 4, 1, f);


    sample.waveFormat.wFormatTag = sample.header.AudioFormat;
    sample.waveFormat.nSamplesPerSec = sample.header.SamplesPerSec;
    sample.waveFormat.wBitsPerSample = sample.header.bitsPerSample;
    sample.waveFormat.nChannels = sample.header.NumOfChan;
    sample.waveFormat.nBlockAlign = sample.header.blockAlign;
    sample.waveFormat.nAvgBytesPerSec = sample.header.SamplesPerSec * sample.header.blockAlign;
    sample.waveFormat.cbSize = 0;
    sample.data = new DATA_TYPE[sample.header.Subchunk2Size / sizeof(DATA_TYPE)];
    fread_s((void*)sample.data, sample.header.Subchunk2Size, sample.header.Subchunk2Size, 1, f);
    sample.id = Sounds.size();
    Sounds.push_back(sample);
    std::cout << "Loading success.";
}



static void CALLBACK waveOutProc(HWAVEOUT hWaveOut, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2) {
    if (uMsg != WOM_DONE) return;
    BlockFree++;
    unique_lock<mutex> lm(muxBlockNotZero);
    cvBlockNotZero.notify_one();
}

bool playSound(int id) {
    unique_lock<mutex> fb(muxFillBlock);
    Audio* buffer = nullptr;
    for (auto& sound : Sounds) {
        if (sound.id == id) {
            buffer = &sound;
        }
    }
    if (buffer == nullptr) return false;


    for (int i = 0; i < BlockCount; i++) {
        WaveHeaders[i].dwBufferLength = BlockSamples * sizeof(DATA_TYPE);
        WaveHeaders[i].lpData = (LPSTR)(BlockMemory + i * BlockSamples);
    }

    if (waveOutOpen(&DeviceOut, WAVE_MAPPER, &buffer->waveFormat, (DWORD_PTR)waveOutProc, (DWORD_PTR)&BlockFree, CALLBACK_FUNCTION) != S_OK) return false;

    SoundHandler a;
    a.isPlaying = true;
    a.byteleft = buffer->header.Subchunk2Size;
    a.pCurrent = buffer->data;
    hSounds.push_back(a);

    return true;
}


void FillAudioBlocks(DATA_TYPE* Blocks, unsigned int Samples) {
    static long int k = pow(2, sizeof(DATA_TYPE) * 8 - 1) - 1;
    long int newSample = 0;

    for (int i = 0; i < Samples; i++) {
        unique_lock<mutex> fb(muxFillBlock);
        newSample = 0;
        for (auto& sound : hSounds) {
            if (sound.isPlaying) {
                if (sound.byteleft > 0) {
                    newSample += *(sound.pCurrent) * DefaultSetting.TotalVolume / 10;
                    sound.pCurrent++;
                    sound.byteleft -= sizeof(DATA_TYPE);
                }
                else {
                    sound.isPlaying = false;
                }
            }
        }
        if (newSample > k) {
            if (newSample > k) newSample = k;
        }
        else {
            if (newSample < -k) newSample = -k;
        }
        Blocks[i] = newSample;
    }

    int i = 0;
    while (i < hSounds.size()) {
        if (!hSounds[i].isPlaying) {
            hSounds.erase(hSounds.begin() + i);
        }
        else i++;
    }
}

void AudioThread() {

    while (!thread_exit) {
        if (BlockFree == 0) {
            unique_lock<mutex> lm(muxBlockNotZero);
            while (BlockFree == 0) {
                cvBlockNotZero.wait(lm);
            }
        }
        BlockFree--;
        if (WaveHeaders[BlockCurrent].dwFlags & WHDR_PREPARED)
            waveOutUnprepareHeader(DeviceOut, &WaveHeaders[BlockCurrent], sizeof(WAVEHDR));

        DATA_TYPE NewSample = 0;
        int CurrentBlock = BlockCurrent * BlockSamples;

        FillAudioBlocks(&BlockMemory[CurrentBlock], BlockSamples);

        waveOutPrepareHeader(DeviceOut, &WaveHeaders[BlockCurrent], sizeof(WAVEHDR));
        waveOutWrite(DeviceOut, &WaveHeaders[BlockCurrent], sizeof(WAVEHDR));

        BlockCurrent++;
        BlockCurrent %= BlockCount;
    }
}
