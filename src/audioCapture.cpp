//29-7-2026:

#include "audioCapture.h"

#include <iostream>
#include <chrono>
#include <thread>

using namespace std;

AudioCapture::AudioCapture()
{
    // Roep de functie CoInitalize op met argument NULL. Als niet gelukt geef aan de initialisatie voor AudioCapture mislukt is
    // BELANGRIJK: Let op coInitialize staat ook in AudioStream: maar dit gaat om een andere thread!
    HRESULT hr = CoInitialize(NULL);

    if(FAILED(hr))
    {
        cout<<"COM initialisatie voor AudioCapture mislukt!" <<endl;
        return;
    }

    cout<< "AudioCapture object aangemaakt!" <<endl;
}

// Destructor
AudioCapture::~AudioCapture()
{
    //als loopt, stop
    if(running)
    {
        stop();
    }
    if(captureClient)
    {
        captureClient->Release();
        captureClient = nullptr;
    }

    if(audioClient)
    {
        audioClient->Release();
        audioClient = nullptr;
        
    }

    if(device)
    {
        device->Release();
        device = nullptr;
    }

    if(waveFormat)
    {
        CoTaskMemFree(waveFormat);
        waveFormat = nullptr;
    }

    CoUninitialize();

    cout<< "AudioCapture afgesloten!" <<endl;

}

//29-7-2026: 

// functie initialize die teruggeeft een boolean (geen parameters)
bool AudioCapture::initialize()
{
    //Bericht 'AudioCapture initialiseren".
    cout<<"AudioCapture Initialiseren"<<endl;
    // maak variabele hr van het datatype HR result
    HRESULT hr;
    // maak een pointer enumerator naar een nog te maken object IMMDeviceEnumerator
    IMMDeviceEnumerator* enumerator = nullptr;
    // Sla op in variabele hr: Roep de functie CoCreateInstance op met de parameters __uuidof(argument MMDeviceEnumerator, NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator)en
    // pointer naar een pointer void)...Nog eens bekijken wat het doet
    hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&enumerator);
    
    // als hr Failed, bericht: Enumerator maken mislukt en retourneer false
    if(FAILED(hr))
    {
        cout<<"AudioCapture: Enumerator maken mislukt!" <<endl;
        return false;
    }
    //anders: Enumerator gevonden
    cout<<"AudioCapture: Enumerator gevonden!"<<endl;

    // Nu de standaard audiouitgang ophalen
    //sla op in variabele hr: roep membervariabele GetDefaultAudioEndpoint op de variabele enumerator met 
    // parameters eRender, eConsole en &device.
    hr = enumerator->GetDefaultAudioEndpoint(eRender, eConsole, &device);
    

    // Als hr Faalt
    if(FAILED(hr))
    {
        // Bericht: AudioDevice ophalen mislukt
        cout<<"AudioCapture: AudioDevice ophalen mislukt"<<endl;
        // Geef enumerator vrij
        enumerator->Release();
        // retourneer false
        return false;
    }

    // 29-7-2026:: VRAAG WAAROM ONDERSTAANDE NIET BINNEN DE IF FUNCTIE ZETTEN BIJ ELSE?
    //Anders is AudioClient gewoon gevonden
    cout<<"AudioCapture: AudioDevice gevonden"<<endl;

    // IAudioClient activeren
    // sla op in hr: roep de memberfunctie Activate op van het object waar de pointer device naar verwijst met argument __uuidof(IAudioClient), CLSTX_ALL, NULL en 
    // een pointer naar een pointer .......
    hr = device->Activate(__uuidof(IAudioClient), CLSCTX_ALL,NULL,(void**)&audioClient);

    // Als hr Faalt dan
    if(FAILED(hr))
    {
        // AudioClient ophalen mislukt en release
        cout <<"AudioCapture: AudioClient ophalen mislukt!" <<endl;

        enumerator->Release();

        return false;
    }

    cout<<"AudioCapture: AudioClient gevonden" <<endl;

   //sla op in hr = Roep memberfunctie GetMixFormat op met argument: een pointer naar waveformat
    hr = audioClient-> GetMixFormat(&waveFormat);
    // Als hr Faalt
    if(FAILED(hr))
    {
        // Bericht: Mixformaat ophalen mislukt
        cout<<"Mixformaat ophalen mislukt"<<endl;
        // Geef enumerator vrij
        enumerator->Release();
        // retourneer false
        return false;
    }
    // Bericht: mixformaat opgehaald
    cout<<"AudioClient: mixformaat opgehaald" <<endl;


    // 29-7-2026: BELANGRIJK: NIET ZELF AUDIO RENDEREN MAAR KOPIE ONTVANGEN.
    // Initialiseer AudioClient
    // sla op in hr: roep op de memberfunctie Initialize van de pointer (?) audioclient met argumenten AUDCLNT_SHAREMODE_SHARED, AUDCLNT_STREAMFLAGS_LOOPBACK, 10000000, 0, waveformat en NULL
    hr = audioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, AUDCLNT_STREAMFLAGS_LOOPBACK, 10000000,0, waveFormat, NULL);

    // als hr faalt
    if(FAILED(hr))
    {
        //Bericht: AudioClient Initialiseren mislukt
        cout<<"AudioCapture: AudioClient initialiseren mislukt!" <<endl;

        enumerator->Release();

        return false;
    }

    cout<< "AudioCapture: AudioClient succesvol geinitialiseerd!"<<endl;

    // Audioclient ophalen:
    // roep membervariabele GetService van het object waar de pointer audioClient naar verwijst met arguemnten __uuidof(IAudioCaptureClient)
    // en weer een pointer naar een pointer genvoid..... check
    hr = audioClient->GetService(__uuidof(IAudioCaptureClient), (void**)&captureClient);

        
    if(FAILED(hr))
    {
        cout<<"AudioCapture: CaptureClient ophalen mislukt!" <<endl;

        enumerator->Release();

        return false;
    }

    //'anders' (dus kan dit nou ook binnen de if statement): AudioClient succesvol geinitialiseerd
    cout<<"AudioCapture: CaptureClient gevonden!" <<endl;


    //Buffergrootte ophalen
    // slap op in hr: De membervariabele van het object waar audioClient naar wijst en geef als argument een rreferentie mee naar de buffersize (nog checken)
    hr = audioClient->GetBufferSize(&bufferSize);
    // Als hr Faalt
    if(FAILED(hr))
    {
        // Bericht Dat buffergrootte ophalen mislukt is
        cout<< "AudioCapture: Buffer grootte ophalen mislukt!"<<endl;
        // Geef enumerator vrij
        enumerator->Release();
        // Retourneer false
        return false;
    }

    // Anders bericht: Buffergrootte (buffersize) 
    cout <<"Audiocapture buffer grootte: "<<bufferSize<< "frames"<<endl;
    //Geef enumerator vrij
    enumerator->Release();
    // Retourneer true
    return true;
}
    //functie start die niks teruggeeft
void AudioCapture::start()
{
    // Controleer of er een AudioClient beschikbaar is
    if(!audioClient)
    {
        //Bericht: Geen Audioclient beschikbaar
        cout<<"Geen AudioClient beschikbaar"<<endl;
        // return
        return;
    }
    // maak een variabele hr van het datatype HRESULT. Gebruik memberfunction Start van de pointer audioclient
    HRESULT hr = audioClient->Start();
    
    //als hr faalt
    if(FAILED(hr))
    {
        // Capture starten mislukt
        cout<<"Capture Starten mislukt"<<endl;
        return;
    }

    // retourneer    
    // running = waar
    running = true;

    // sla op in het object captureThread van het type Thread. Maak een nieuw thread object en stop deze in captureThread.
    // Roep functie Thread op en geef als argument captureLoop mee. Captureloop is een memberfunctie van AudioCapture
    // Gebruik 'this' zodat hij weet welk object hij moet uitvoeren.
    // BELANGRIJK: een & voor AudioCapture
    captureThread = thread(&AudioCapture::captureLoop, this);
    
    cout<<"AudioCapture thread gestart!"<<endl;

    // Bericht: "AudioCapture gestart"
    cout<<"AudioCapture gestart"<<endl;
}



// functie stop die niets teruggeeft
// 29-7-2026: Gaan we aanpassen nog wanneer we WASAPI gana uitlezen
void AudioCapture::stop()
{
    // Variabele running is false;
    running = false;
    // Als captureThread een thread heeft om op te wachten
    if(captureThread.joinable())
        // Wacht dan tot de thread klaar is met uitvoeren
        captureThread.join();
    // Als audioClient waar is, stop deze
    if(audioClient)
    {
        audioClient->Stop();
    }
    // Bericht dat AudioCapture gestopts is
    cout<<"AudioCapture is gestopt"<<endl;
}
    
// Functie captureLoop() die niets teruggeeft
void AudioCapture::captureLoop()
{
    // Bericht: Audiocapture gestart
    cout<<"Audiocapture gestart"<<endl;
    // Terwijl deze draait, roep de functie process op
    while(running)
    {
        process();
    }
    // bericht dat AudioCapture Thread is gestopt
    cout<<"audioCapture Thread is gestopt";
}
    // functie process die niets teruggeeft
void AudioCapture::process()
{
    //29-7-2026: Wel iets uitgewerkt anders krijgen we een 'busy loop': hoog CPU gebruik

    //this_thread::sleep_for(chrono::milliseconds(10));
    this_thread::sleep_for(chrono::milliseconds(10));
}

