//29-7-2026: Lijkt sterk op AudioStream, maar Audiostream gebruik IAudioRenderClient om audio NAAR windows te sturen.
// AudioCapture gebruikt IAudioCapture Client om audio UIT windows te halen
//30-7-2026: Compositie toepassen: AudioCapture is samengesteld uit meerdere objecten
//Probleem: RingBuffer heeft een contstructor: Dus we moeten in de constructor van AudioCapture de ringbuffer initialiseren


#pragma once

#include <windows.h>
#include <mmdeviceapi.h>
#include <audioclient.h>

//Multithreading & Concurrency
#include <thread>
#include <atomic>

//30-7-2026: Toegevoegd
#include "ringbuffer.h"

class AudioCapture
{
    public:
        AudioCapture();
        ~AudioCapture();

        bool initialize();
        void start();
        void stop();

    private:
        void captureLoop();
        void process();

        IMMDevice* device = nullptr;
        IAudioClient* audioClient = nullptr;
        IAudioCaptureClient* captureClient = nullptr;
        
        WAVEFORMATEX* waveFormat = nullptr;

        UINT32 bufferSize = 0;

        std::atomic<bool> running = false;

        std::thread captureThread;
        
        //30-7-26: Tijdelijk
        float volumeBoost = 2.0f;

        //RingBuffer
        RingBuffer ringBuffer;
};