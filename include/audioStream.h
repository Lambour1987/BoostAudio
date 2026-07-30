//17-7-2026: Klasse die verantwoordelijk wordt voor WASAPI
//AudioStream gaat de stroom van audiogegevens openen

#pragma once

#include <windows.h>
#include <mmdeviceapi.h>
#include <audioclient.h>
#include <endpointvolume.h>
#include <thread>

// Maak een class AudioStream met public een gewone constructor en destructor en bool initialize en void start en stop
class AudioStream
{
    public:
        AudioStream();
        ~AudioStream();
    
        bool initialize();
        void start();
        void stop();

    private:
    //19-7-2026: nieuw (Encapsulation)
        void audioLoop();
        void process();


    // Binnen een class initialiseren heet in-class member initialization. 
    // Private pointer device naar IMMDevice, pointer audioClient naar IAudioClient , pointer capturClient naar object IAudioCaptureClient en een bool running op false
        IMMDevice*device = nullptr;
        IAudioClient*audioClient = nullptr;
        //18-7-2026 wijzig van IAudioCaptureClient*captureClient = nullptr; naar
        IAudioRenderClient* renderClient = nullptr;


        //18-7-2026: Nieuw tbv audiobuffer
        UINT bufferSize = 0;

        WAVEFORMATEX* waveFormat = nullptr;

               
        std::thread audioThread;
        
        bool running = false;
        //20-7-26: Normaal volume 1.0f, 2.0f 200% en 0.5f (zachter)
        float volumeBoost = 2.0f;
};
