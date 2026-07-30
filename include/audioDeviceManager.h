//7-7-2026: Kan COM gebruiken, audio devices vinden, namen tonen

#pragma once

//8-7-26: nieuw erbij
#include <endpointvolume.h>

class AudioDeviceManager
{
    public:
    //9-7-2026: Constructor
    //10-7-2026: Constructor en destructor kan je in principe willekeurig neerzetten net als de functies
        AudioDeviceManager();
    //9-7-2026: Destructor
        ~AudioDeviceManager();

        void listDevices();
        void getDefaultDevice();
        IAudioEndpointVolume* getEndpointVolume();
        //17-7-2026: Kan eruit
        // void setVolume(float volume);
        // float getVolume();
    private:
        IAudioEndpointVolume* endpointVolume = nullptr;
};