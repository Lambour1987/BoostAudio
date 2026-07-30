
//29-7-2026: NIeuw toegevoegd: definie INITGUID: moet voo rinlcudes van ksmedia.h staan
// BELANGRIJK: define is een preprocessor directive: je definieert een macro: Maak een macro
// met de naam INITGUID: GUID: Globally Unique Identifier. Nog uitzoeken
//BELANGRIJK: volgorde van includes is belangrijk omdat deze zelf ook weer bestanden beatten
#define INITGUID

//17-7-2026 
#include "audiostream.h"

//29-7-2026: Toevoegen
#include <ks.h>
#include <ksmedia.h>

#include <audioclient.h>

#include <iostream>
#include <chrono>
#include <thread>

using namespace std;

//Constructor: geef melding: AudioStream gestart
AudioStream::AudioStream()
{
    //19-7-26: Nieuw toegevoegd:

    HRESULT hr = CoInitialize(NULL);

    if(FAILED(hr))
    {
        cout<<"COM initialisatie mislukt"<<endl;
    }

    cout<<"Audiostream object aangemaakt! "<<endl;

    //19-7-2026: Dubbel, eruit
    // CoInitialize(NULL);
    // cout<<"Audiostream gestart!"<<endl;
}

//Destructor: Bij WASAPI maken we Windows COM-objecten
AudioStream::~AudioStream()
{
    //19-7-2026: nieuw tbv thread
    if(running)
    {
        stop();
    }

    // Als  er een renderClient is dan Releasen
    // van if(renderClient)
    if(renderClient)
    {
        // van renderClient->Release();
        renderClient->Release();

        //naar null verwijzen anders blijft pointer naar oud adres verwijzen (dangling pointer)
        renderClient = nullptr;
    }
    // Als er een audioClient is dan Releasen
    if(audioClient)
    {
        audioClient->Release();
        audioClient = nullptr;
    }
    // Als er een device is dan releasen
    if(device)
    {
        device->Release();
        device = nullptr;
    }

    //19-7-2026: Nu hier waveformat
    if(waveFormat)
    {
        CoTaskMemFree(waveFormat);
        waveFormat=nullptr;
    }

    CoUninitialize();

    // melding: AudioStream afgesloten
    cout<<"AudioStream afgesloten!"<<endl;
}
// functie initialize die teruggeeft een boolean
bool AudioStream::initialize()
{
    // output: AudioStream initialiseren 
    cout<<"AudioStream Initialiseren..."<<endl;

    // maak een variabele hr van het datatype HRESULT
    HRESULT hr;
    // maak een pointer enumerator naar een IMMDeviceEnumerator en wijs nergens naar.
    // BELANGRIJK: Hier maken we dus de pointer niet in de class maar in de functie, omdat deze alleen tijdelijk nodig is.
    // We gebruiken hem alleen om het standaard audioapparaat op te vragen

    IMMDeviceEnumerator*enumerator=nullptr;

    // Maak via COM een MMDeviceEnumerator-object aan
    // __uuidof(MMDeviceEnumerator): welke COM-class willen we maken
    // __uuidof(IMMDeviceEnumerator): als welke interface willen we hem gebruiken
    // (void**)&enumerator: Windows vult onze pointer in
    hr = CoCreateInstance(__uuidof(MMDeviceEnumerator),NULL,CLSCTX_ALL,__uuidof(IMMDeviceEnumerator),(void**)&enumerator);

    // als hr mislukt geef aan Enumerator maken mislukt en retourneer false
    if(FAILED(hr))
    // Roep vervolgens de GetDefaultAudioEndpoint op van de de pointer enumerator met parameters eRender, eConsole en een referentie naar device
    // en sla op in hr
    {
        cout<< "Enumerator maken mislukt."<<endl;
        return false;
    }

    // Vraag het standaard outputappraat (speakers/ headset) op
    hr=enumerator->GetDefaultAudioEndpoint(eRender, eConsole, &device);
    //als hr failed, geef aan dat Audio Device ophalen mislukt is en geef de enumerator vrij en retourneer false
    if(FAILED(hr))
    {
        cout<<"Audio Device ophalen is mislukt."<<endl;
        enumerator->Release();
        return false;
    }

    // Anders gewoon gevonden en geef aan : Audio device gevonden!
    cout<<"Audio Device gevonden!"<<endl;


    //18-7-2026: IAudioClient activeren
    hr = device->Activate(__uuidof(IAudioClient), CLSCTX_ALL, NULL,(void**)&audioClient);
    if(FAILED(hr))
    {
        cout <<"AudioClient ophalen mislukt."<<endl;

        enumerator->Release();

        return false;
    }
    // AudioClient gevonden
    cout<< "AudioClient gevonden!" <<endl;

    //18-7-2026: Maak een pointer waveFormat naar een nog te maken object WAVEFORMATEX
    //19-7-2026: KAn er uit want in headerfile WAVEFORMATEX* waveFormat = nullptr;

    waveFormat = nullptr; 

    // Vraag het mixformaat van Windows op
    hr = audioClient->GetMixFormat(&waveFormat);
    // Als hr faalt, dan Mixformaat ophalen mislukt, release
    if(FAILED(hr))
    {
        cout<<"Mixformaat ophalen mislukt!"<<endl;
        // //19-7-2026: Kan eruit. 
        // if(waveFormat)
        // {
        //     CoTaskMemFree(waveFormat);
        // }
        enumerator->Release();

        return false;
    }
    //Anders wel gelukt
    cout<<"Mixformaat opgehaald!"<<endl;

    //20-7-2026: test:
    cout<<"Channels: "
    <<waveFormat->nChannels
    <<endl;

    cout<<"Bits per sample: "
    <<waveFormat->wBitsPerSample
    <<endl;

    //29-7-2026: Bekijken wat het format is> direct weer weggehaald voor een diepere analyse
    // if(waveFormat->wFormatTag == WAVE_FORMAT_IEEE_FLOAT)
    // {
    //     cout<<"Format: 32-bit float"<<endl;
    // }
    // else if(waveFormat->wFormatTag == WAVE_FORMAT_PCM)
    // {
    //     cout<<"Format: PCM"<<endl;
    // }
    // else if(waveFormat->wFormatTag == WAVE_FORMAT_EXTENSIBLE)
    // {
    //     cout<<"Format: WAVEFORMATEXTENSIBLE"<<endl;
    // }
    // else
    // {
    //     cout<<"Format: onbekend"<<endl;
    // }

    
    if(waveFormat->wFormatTag == WAVE_FORMAT_EXTENSIBLE)
    {
        //29-7-26: We zetten de pointer waveFormat om naar een pointer van een ander type: waveFormat is een poitner
        // van het type WAVEFORMATEX*. Die zetten we om (reinterpret_cast) naar een pointer van het type WAVEFORMATEXTENSIBLE. En deze slaan
        // we op in een pointer extensibleFormat. Dit doen we omdat Waveformatex en waveformatextensible verschillende structren zijn.
        // waveformatextensible bevat meer info.
        WAVEFORMATEXTENSIBLE* extensibleFormat =  reinterpret_cast<WAVEFORMATEXTENSIBLE*>(waveFormat);

        //Pak de member SubFormat uit het WAVEFORMATEXTENSIBLE-object waar extensibleFormat naar wijst.
        if(IsEqualGUID(extensibleFormat->SubFormat, KSDATAFORMAT_SUBTYPE_IEEE_FLOAT))
        {
            cout<<"SubFormat: 32-bit float"<<endl;
        }
        else if(IsEqualGUID(extensibleFormat->SubFormat, KSDATAFORMAT_SUBTYPE_PCM))
        {
            cout<<"SubFormat: PCM"<<endl;
        }
        else
        {
            cout<<"SubFormat: onbekend"<<endl;
        }
    }


    // AudioClient initialiseren
    hr = audioClient->Initialize(
        AUDCLNT_SHAREMODE_SHARED,
        0, //flags
        10000000, //bufferduur: 10.000.000*100nanoseconden = 1 seconde
        0, // Is alleen voor exclusive mode: nu gebruiken we shared mode = 0
        waveFormat, // het formaat dat we hebben opgevraagd
        NULL); //sessie ID indien relevant

        if(FAILED(hr))
        {
            cout<<"AudioClient initaliseren mislukt."<<endl;

            //19-7-26: Kan er CoTaskMemFree(waveFormat);

            enumerator->Release();

            return false;
        }

        cout <<"AudioClient succesvol geinitialiseerd"<<endl;

        // Audio render client ophalen
        // 18-7-26: Van hr = audioClient->GetService(__uuidof(IAudioCaptureClient),(void**)&captureClient); naar
        hr = audioClient->GetService(__uuidof(IAudioRenderClient),(void**)&renderClient);

        if(FAILED(hr))
        {
            cout<<"Render client ophalen mislukt."<<endl;

            //19-7-26: Kan eruit CoTaskMemFree(waveFormat);
                       
            enumerator->Release();

            return false;
        }
        cout<<"Render client gevonden!"<<endl;

        //18-7-26: BUffergrootte ophalen
        hr = audioClient->GetBufferSize(&bufferSize);

        if(FAILED(hr))
        {
            cout<<"Buffer grootte ophalen mislukt!"<<endl;

            //19-7-26: Kan eruit CoTaskMemFree(waveFormat);

            enumerator->Release();

            return false;
        }

    cout<<"Buffer grootte: "<<bufferSize<<" frames"<<endl;

    //waveFormat is niet meer nodig
    //19-7-26: Kan eruit. CoTaskMemFree(waveFormat);

    // Geef de enumerator vrij
    enumerator->Release();

    return true;
}

// functie start() die niets teruggeeft
void AudioStream::start()
{
    //19-7: kan weg want dubbel  running = true;

    //19-7-2026: Erbij gezet
    if(!audioClient)
    {
        cout<<"Geen AudioClient beschikbaar"<<endl;
        return;
    }

    HRESULT hr = audioClient->Start();

    if(FAILED(hr))
    {
        cout<<"AudioStream starten mislukt"<<endl;
        return;
    }

    running = true;

    //19-7-26: Maak een nieuwe thread die de memberfunctie AudioStream::audioloop() uitvoert op dit specifieke object
    audioThread = std::thread(&AudioStream::audioLoop, this);

    // output: AudioStream gestart
    cout<<"AudioStream gestart"<<endl;
}
// functie stop() die niets teruggeeft
void AudioStream::stop()
{
    running = false;
    //19-7-26:Main thread moet wachten tot de audioThread klaar is
    //join: wachten tot thread klaar is
    if(audioThread.joinable())
    {
        audioThread.join();
    }

    if(audioClient)
    {
        audioClient->Stop();
    }

    // output: 'Audiostream gestopt!'
    cout<<"AudioStream gestopt!"<<endl;
}

//19-7-26: Tijdelijke audioloop

void AudioStream::audioLoop()
{
    cout<<"Audio Thread gestart!"<<endl;

    while(running)
    {
        process();
    }

    cout<<"Audio thread gestopt!"<<endl;
}

void AudioStream::process()
{
    // 19-7-26: Tijdelijke test
    //20-7-2026: Eruit
    // this_thread::sleep_for(std::chrono::milliseconds(10));
    
    UINT32 padding = 0;

    // Hoeveel frames zitten er nog in de Windows buffer
    HRESULT hr = audioClient->GetCurrentPadding(&padding);

    if(FAILED(hr))
    {
        cout<< "Padding ophalen mislukt"<<endl;
        return;
    }

    // Aantal frames beschikbaar is bufferomvange minus padding: Hoeveel ruimte hebbe wij om nieuwe
    // audio te schrijven?
    UINT32 framesAvailable = bufferSize - padding;

    //Als er geen frames meer aanwezig zijn retourneer
    if(framesAvailable == 0)
    {
        return;
    }

    //Maak een pointer data die nog niet naar een object BYTE wijst
    BYTE* data = nullptr;

    // variabele hr: 
    hr = renderClient->GetBuffer(framesAvailable, &data);

    if(FAILED(hr))
    {
        cout<<"Buffer ophalen mislukt"<<endl;
        return;
    }

    // HIer komt de audiobewerking


    renderClient->ReleaseBuffer(framesAvailable, 0);

};
