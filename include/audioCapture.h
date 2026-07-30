//29-7-2026: Lijkt sterk op AudioStream, maar Audiostream gebruik IAudioRenderClient om audio NAAR windows te sturen.
// AudioCapture gebruikt IAudioCapture Client om audio UIT windows te halen


#pragma once

#include <windows.h>
#include <mmdeviceapi.h>
#include <audioclient.h>

//Multithreading & Concurrency
#include <thread>
#include <atomic>

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
};