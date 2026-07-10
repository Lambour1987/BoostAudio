//9-7-2026: Audio Volume Controller
//9-7-26: Kan blijkbaar gewoon weg
// Voorkom dat headers meer dan 1x geinclude worden
// # pragma once

// #include <endpointvolume.h>

// // klasse AudioVolumeController
// class AudioVolumeController
// {
//     public:
//         // Constructor
//         AudioVolumeController();
//         // Destructor
//         ~ AudioVolumeController();
//         // Functies
//         bool initialize();
//         void setVolume(float volume);
//         float getVolume()
//     private:
//         IAudioEndpointVolume* endpointVolume = nullptr;    
// };

#pragma once

#include <endpointvolume.h>

class AudioVolumeController
{
    public:
        AudioVolumeController();
        ~AudioVolumeController();

        void setVolume(float volume);
        float getVolume();
        void setMute(bool mute);

        void setEndpointVolume(IAudioEndpointVolume*volume);


    private:
        IAudioEndpointVolume* endpointVolume = nullptr;
};
