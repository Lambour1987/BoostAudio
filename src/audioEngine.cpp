//1-7-2026: 

//iostream en het .h bestand
#include <iostream>

#include <mmdeviceapi.h>

// Propertykeys: definieert sleutels waarmee je info opvraagt zoals naam van de device, fabrikant en type apparaat
// Metadata uit een audio device
//3-7-2026: Eruit #include <functiondiscoverykeys_devpkey.h> nieuw:
//3-7-2026: Toch er weer uit want verkeerde API#include <initguid.h> #include <devpkey.h>
#include <propkey.h>
#include <functiondiscoverykeys_devpkey.h>

//3-7-2026
// kan er weer uit: #include <propkeydef.h>

//eigen headers
#include "audioEngine.h"
#include "audioDeviceManager.h"
#include "audioVolumeController.h"

using namespace std;

//uit de class AudioEngine de functie start gaan we maken
void AudioEngine::start()
{
    // Engine starten
    running = true;
    // Ouput: AudioEngine gestart
    cout<<"AudioEngine gestart!"<<endl;

    //8-7-2026: Opnieuw erbij gezet: Anders foutmelding: enumerator maken mislukt in audioDeviceManager
    //9-7-2026: Er weer uitgehaald: HRESULT hr = CoInitialize(NULL);

    // if(FAILED(hr))
    // {
    //     cout<<"COM initialisatie mislukt" <<endl;
    //     return;
    // }

    // Maak een object aan van de AudioDeviceManager en noem deze manager
    // 10-7-26 kan weg: AudioDeviceManager deviceManager;

    // 7-7-2026: Belangrijk: Momenteel (als deze functie onder start() staat dan is de levensduur van
    // een object manager heel kort. Na het eindegen van de functie wordt manager vernietitgd. Voor testen prima
    // maar in de toekomst moet de AudioEngine de manager bezitten)
    // roep op dit object de memberfunctie listdevices aan();
    //7-7-26: Gewijzigd van manager.listDevices(); naar 
    deviceManager.getDefaultDevice();

    //10-7-26: Kan weg AudioVolumeController volumeController;
    volumeController.setEndpointVolume(
        deviceManager.getEndpointVolume());
    //10-7-2026: gewijzigd van
    // cout << "Huidig volume: "<< manager.getVolume() <<endl;
    cout << "Huidig volume: "<< volumeController.getVolume() <<endl;

    //8-7-2026
    volumeController.setVolume(0.8f);

    //10-7-2026
    volumeController.setMute(true);
    
    //MessageBoxA()
    MessageBoxA(NULL, "BoostAudio draait", "Info", MB_OK);

}

void AudioEngine::stop()
{
    running = false;
    // Output: AudioEngine gestopt
    cout<<"AudioEngine gestopt!"<<endl;

}

