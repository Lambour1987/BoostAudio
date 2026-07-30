//1-7-2026: Headerfiles; vertel wat er bestaat maar niet wat het doet.

// Headerfile : zorg dat headers altijd maar 1x mee worden genomen
#pragma once

//18-7-2026: Erbij
#include <mmdeviceapi.h>
#include <endpointvolume.h>

#include "audioDeviceManager.h"
#include "audioVolumeController.h"
#include "audiostream.h"
#include "audioCapture.h"



// class AudioEngine met void functie start (public)
class AudioEngine
{  

    private:
        // bool running die begint op false
        //10-7-26: bool running = false;
        AudioDeviceManager deviceManager;
        AudioVolumeController volumeController;
        //19-7-2026: Toevoegen een object audioStream van de class AudioStream
        AudioStream audioStream;
        AudioCapture audioCapture;
        
        bool running = false;

        //3-7-2026: Device info: Ipv vectoren array gebruiken: We gaan er vanuit dat er max 10 devices zijn
        //Hoewel de vector dynamisch is is deze langzamer in runtime dan een vaste array. Keuze valt op array

        //17-7-2026: Het maximum aantal devices dat we toestaan is 10. Waarde mag na het maken niet meer veradneren
        // Omdat we in een class staan. Is er maar 1 kopie van deze waarde voor de class
        static const int MAX_DEVICES = 10;

        //Maak een array deviceList met ruimte voor 10 pointers naar IMMDevice-objecten
        IMMDevice* deviceList[MAX_DEVICES];
        //Maak een array volumeList met ruimte voor 10 pointers naar IAudioEndpointVolume
        IAudioEndpointVolume* volumeList[MAX_DEVICES];
        //Maak een array volumeCache met ruimte voor 10 float-waarden
        float volumeCache[MAX_DEVICES];
    
    public:
        // 17-7-26: constructor en destructor
        AudioEngine();
        ~AudioEngine();

        // start functie
        void start();
        // stop functie
        void stop();
};


 
