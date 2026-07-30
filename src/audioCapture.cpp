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
    //this_thread::sleep_for(chrono::milliseconds(10));

    // --------------------------------------------------------------------------------------------------------------------
    //
    // 1. Controleer of er een nieuw audiopakket beschikbaar is
    //
    // --------------------------------------------------------------------------------------------------------------------

    //30-7-2026 maak een variabele packetlength en initialiseer op 0
    UINT32 packetLength = 0;
    // Roep  de memberfunctie GetNextPacketSize op waar de pointer captureClient naar wijst en geef als argument mee de 
    // referentie packetlenght. Sla het resultaat op in variabele hr (datatype HResult)
    HRESULT hr = captureClient->GetNextPacketSize(&packetLength);
    //als hr faalt, 
    if(FAILED(hr))
    {
        // bericht: Packetgrootte ophalen mislukt en return
        cout<<"AudioCapture: Packet grootte ophalen mislukt! "<<endl;
        return;
    }

    // --------------------------------------------------------------------------------------------------------------------
    //
    // 2. Als er geen pakket is, hoeven we niets te doen
    //
    // --------------------------------------------------------------------------------------------------------------------

    if(packetLength==0)
    {
        return;
    }

    // als packetlengte groter is dan 0
    //if(packetLength>0)
    //{
    // Bericht: Nieuw audio pakket, geef lente in frames
    cout<<"AudioCapture: Nieuw Audiopakket! "<<packetLength<< " frames"<<endl;

    // --------------------------------------------------------------------------------------------------------------------
    //
    // 3. Vraag de daadwerkelijke audiobuffer op
    //
    // --------------------------------------------------------------------------------------------------------------------

    // Maak een pointer genaamd data naar een object van Byte dat nog gemaakt moet worden.
    BYTE* data = nullptr;
    // Maak een variabele numFrames van het type UINT32 en zet deze op 0: Hierin komt het aantal frames dat 
    // we uit de buffer ophalen
    // UINT32 numFrames = 0;
    UINT32 numFramesAvailable = 0;

    // Maak een variabele flags van het type DWORD
    DWORD flags = 0;

    // Vraag audiobuffer op en sla op in de variabele hr. Roep via de pointer captureClient de memberfunctie GetBuffer op
    // en geef als parameters mee, de referenties naar data, numFrames, flags en nullptrs.
    hr = captureClient->GetBuffer(&data, &numFramesAvailable, &flags, nullptr,nullptr);

    // Als de getBuffer faalt
    if(FAILED(hr))
    {
        cout<<"AudioCapture: Buffer ophalen mislukt! "<<endl;

        return;
    }
    
    cout<<"AudioCapture: Buffer ontvangen! "<<numFramesAvailable<<" frames"<<endl;

    // --------------------------------------------------------------------------------------------------------------------
    //
    // 4. Controleer of de buffer stiltesamples bevat
    //
    // --------------------------------------------------------------------------------------------------------------------    

    // als audio-buffer door windows als stil wordt gemarkeerd, bericht
    if(flags & AUDCLNT_BUFFERFLAGS_SILENT)
    {
        cout<<"AudioCapture: Buffer bevat stilte!"<<endl;
    }
    else
    {
        // ---------------------------------------------------------------------------------------------------------
        //
        // 5. Interpreteer de ruwe bytes als 32-bit floats
        //
        // ---------------------------------------------------------------------------------------------------------

        //30-7-26: Interpretatie van onze audio 32-bit als floats. 
        //Behandel de data waar de pointer 'data' naar verwijst als float, wen verwijs daarnaar met een pointer sample
        float*samples = reinterpret_cast<float*>(data);

        // er weer uit:
            // //linkerkanaal
        // float leftSample = samples[0];
        // //rechterkanaal
        // float rightSample = samples[1];
        // // Toon de eerste samples: Bericht: AudioCapture: Left Sample
        // cout<<"AudioCapture: Left sample: "<<leftSample<<endl;

        // // Toon de tweede sample: Bericht: AUdioCapture: Right sample
        // cout<<"AudioCapture: Right sample: "<<rightSample<<endl;
    
    
    // ---------------------------------------------------------------------------------------------------------
    //
    // 6. Bereken hoeveel samples we hebben
    //
    // ---------------------------------------------------------------------------------------------------------

        const UINT32 channels = waveFormat->nChannels;

        const UINT32 totalSamples = numFramesAvailable * channels;

    // ---------------------------------------------------------------------------------------------------------
    //
    // 7. Interpreteer de ruwe bytes als 32-bit floats
    //
    // ---------------------------------------------------------------------------------------------------------

        // Loop door samples
        for(UINT32 i=0; i<totalSamples;i++)
        {
            //Lees het oorspronkelijke sample
            float sample = samples[i];

            // Pas de volume Boost toe
            float boostedSample = sample* volumeBoost;

            // Sla het versterkte sample terug op
            samples[i] = boostedSample;
        }
    }

        // ---------------------------------------------------------------------------------------------------------
        //
        // 8. Geef de Buffer vrij
        //
        // ---------------------------------------------------------------------------------------------------------

       
    //Geef aan WASAPI door dat we klaar zijn met deze buffer
    hr = captureClient->ReleaseBuffer(numFramesAvailable);

    // Als ReleaseBuffer faalt
    if(FAILED(hr))
    {
        // Buffer vrijgeven mislukt
        cout<<"AudioCapture: ReleaseBuffer mislukt!"<<endl;
        return;
    }
}

