//9-7-2026: 
//kan blijkbaar eruit
// #include "audioVolumeController.h"

// #include <iostream>
// #include <windows.h>
// #include <mmdeviceapi.h>

// using namespace std;

// //Constructor
// AudioVolumeController::AudioVolumeController()
// {
//     HRESULT hr = CoInitialize(NULL);

//     if(FAILED(hr))
//     {
//         cout<<"COM initialisatie mislukt"<<endl;
//     }
// }

#include "audioVolumeController.h"
#include <iostream>

using namespace std;

AudioVolumeController::AudioVolumeController()
{
    cout<<"AudioVolumeController gestart!"<<endl;
}

void AudioVolumeController::setVolume(float volume)
{
    cout<<"Volume instellen op: "<< volume <<endl;
   
    if(endpointVolume)
    {
        //9-7-2026: debug kan eruit
        // cout<<"EndpointVolume bestaat!"<<endl;

        //9-7-2026: Deze 3 regels zijn debugregels
        UINT channels = 0;
        endpointVolume->GetChannelCount(&channels);
        cout<<"Aantal kanalen: "<<channels<<endl;


        // cout<<"Ik ga nu windows aanroepen..."<<endl;
        // Roep op het object endpointVolume de memberfunctie SetMasterVolumeLevelScalar op met parameters volume en null
        HRESULT hr = endpointVolume->SetMasterVolumeLevelScalar(volume, NULL);
        // Debug test met mute
        // HRESULT hr=endpointVolume->SetMute(TRUE,NULL);
        //9-7-2026: debug kan eruit
        // cout<<"Windows heeft geantwoord"<<endl;
        // cout<<"HRESULT: 0x"<<hex<<hr<<endl;
        // als gelukt
        if(SUCCEEDED(hr))
        {
            // Bericht: volume succesvol aangepast
            cout << "Volume succesvol aangepast!" <<endl;
        }
        // anders
        else
        {
            // Bericht: Volume aanpassen mislukt
            cout << "Volume aanpassen mislukt" <<endl;
        }

    }
    else
    {
        cout<<"Geen volume interface beschikbaar" <<endl;
    }
}

float AudioVolumeController::getVolume()
{
    float volume = 0.0f;

    //10-7-2026: Deze functie hier geplaats
    if(!endpointVolume)
    {
        return -1.0f;
    }

    endpointVolume->GetMasterVolumeLevelScalar(&volume);

    return volume;
}

void AudioVolumeController::setMute(bool mute)
{
    if(!endpointVolume)
    {
        cout<<"Geen volume interface beschikbaar"<<endl;
        return;
    }

    HRESULT hr = endpointVolume->SetMute(mute, nullptr);

    if(SUCCEEDED(hr))
    {
        cout<<"Mute succesvol aangepast!"<<endl;
    }
    else
    {
        cout<<"Mute aanpassen mislukt!"<<endl;
    }
}

//10-7-26: Kan eruit was voor testen
void AudioVolumeController::setEndpointVolume(IAudioEndpointVolume* volume)
{
    cout << "VolumeController krijgt endpoint: ";

    if(volume)
    {
        cout << "bestaat!" << endl;
    }
    else
    {
        cout << "nullptr!" << endl;
    }

    endpointVolume = volume;
}

AudioVolumeController::~AudioVolumeController()
{
    
    if(endpointVolume)
    {
        //17-7-2026: eruit: endpointVolume->Release();
        endpointVolume=nullptr;
    }

    cout<<"AudioVolumeController afgesloten!"<<endl;
}
