// 7-7-2026: audioDeviceManager.cpp

#include "audioDeviceManager.h"
#include <iostream>

#include <iostream>
//Check of we windows API kunnen gebruiken
#include <windows.h>

//Windows audioheader, hoort bij MultiMedia Device API(MMDevice API)
//Deze definieert COM interfaces zoals IMMDeviceEnumerator, IMMDevice en IMMDeviceCollection
#include <mmdeviceapi.h>
#include <functiondiscoverykeys_devpkey.h>

#include <propsys.h>
//t.b.v. juiste audio device kiezen.

using namespace std;

//9-7-2026: Constructor gemaakt: 
AudioDeviceManager::AudioDeviceManager()
{
    HRESULT hr = CoInitialize(NULL);

    if(FAILED(hr))
    {
        cout<<"COM initialisatie mislukt"<<endl;
    }
}


// Maak een functie listDevices van de class AudioDeviceManager
void AudioDeviceManager::listDevices()
{
    std::cout << "AudioDeviceManager actief!" << std::endl;

    
    // COM Initialiseren
    // We gaan COM gebruiken: Windows systeem voor objecten zoals audio devices, netwerk devices, camera's
    // Roep de functie CoInitialize op met parameter NULL. De functie retourneert een waarde van het type HRESULT en 
    // die slaan we op in de variabele HR
    // HR Result is een status code van windows: OK/FAIL/ specifieke foutcode
    HRESULT hr = CoInitialize(NULL);
   
    // Gebruik macro:Failed. Als hresult een fout geeft stop programma: en geef aan: CoInitialisatie Faalt
    if(FAILED(hr))
    {
        cout<<"CoInitialisatie mislukt"<<endl;
        return;
    }

    // ==================================
    //     OBJECT VAN WINDOWS OPVRAGEN
    // ==================================

    
    // Nu audio device manager object maken via Com. Je krijgt een pointer terug naar een interface waarmee je 
    // de audio hardware kunt opvragen
    // maak een pointer naar een COM object: Pointern naar de interface (contract) IMMDeviceEnumerator genaamd enumerator.
    // Je vraagt: Windows, maak een audio device manager voor mij en geef mij een pointer terug zodat ik hem kan gebruiken
    // COM werkt met interfaces (geen classes die ik maak), GUID, factories (CoCreateInstance) en pointers naar pointers
    IMMDeviceEnumerator*enumerator=nullptr;

    hr = CoCreateInstance(
    // COM object aanmaken met parameters:
    // 1) __uuidof(MMDEViceENumerator): Uniek id van het object
        __uuidof(MMDeviceEnumerator),
    // 2) NULL: geen speciale aggregatie/ geen parent object
        NULL,
    // 3) CLSCTX_ALL: All: doe wat werkt
        CLSCTX_ALL,
    // 4) Ik wil dit object terug als deze interface: We vragen om een MMDeviceEnumerator maar we gebruiken
        __uuidof(IMMDeviceEnumerator),
    // hem als IMMDeviceEnumerator Interface
    // 5) (void**)&enumerator: Type conversion + pointer truc. void**: pointer naar een pointer
        (void**)&enumerator
    );

    // Als hr failed
    if(FAILED(hr))
    {
        // Geef melding DeviceEnumerator faalt
        cout<<"DeviceManager is gefaald"<<endl;
        // Roep CoUninitialize() op en return
        CoUninitialize();
        return;
    }
    
    // ======================================
    //          OPHALEN OUTPUTDEVICES
    // ======================================

    // Vervolgens: We vragen aan Windows audio manager om alle actieve speakers (output devices) op te sommen
    // Maak een pointer naar een lijst van audio devices: maak een nullpointer devices die wijst naar IMMDeviceCollection: 
    // devices: lijst van speakers/ headphones/ audio outputs
    
    IMMDeviceCollection*devices = nullptr;

    // Regel 2: hardware call: enumerator is een pointer naar een COM object dat de interface IMMDeviceEnumerator implementeert
    // Intern maakt windows iets binnen een class maar ik zie alleen de interface. Dus een pointer naar een COM interface object (IMMDeviceEnumerator)
    // en daarop roepen we een methode aan. EnumAudioEndpoints is een functie
    hr = enumerator->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, &devices);

    // Als hr faalt
    if(FAILED(hr))
    {
        // Geef melding Devices ophalen faalt
        cout<<"Devices ophalen faalde"<<endl;
        // geef enumerator vrij
        enumerator->Release();
        // schakel COM weer uit
        CoUninitialize();
        return;
    }

    // Vervolgens. We hebben nu een lijst met devices (actieve speakers): nu gaan we daar doorheen lopen en printen we de namen
    // Maak een variabele count van het type UINT: UINT: unsigned integer: (geheel getal zonder negatieve waarden, gebruikt in windows API). Dus alleen positieve waarden
    UINT count;


    // ======================================
    //          DOOR APPARATEN LOPEN
    // ======================================

    // Roep de functie GetCount aan en geef het adres van count door. Normale functie: int count = devices.getCount();
    // Hier doen we de COM stijl. Devices is een pointer, getcount is een interface mod en werkt via een virtuele tabel
    devices->GetCount(&count);
    // Geef aan dat audio devices zijn gevonden
    cout<<"Audio Devices gevonden: " << count << "\n";
    // Doorloop de lijst met gevonden devices

    //3-7-2026: Device info
    //Maak een array deviceList met ruimte voor 10 pointers naar IMMDevice-objecten

    //Maak een array volumeList met ruimte voor 10 pointers naar IAudioEndpointVolume

    //Maak een array volumeCache met ruimte voor 10 float-waarden

    for(int i=0; i<count;i++)
    {
        // maak een pointer devices naar IMMdevice en zet op 0
        IMMDevice* device = nullptr;
        // roep item methode aan op object waar devices naar wijst. Parameter index i en adres poniter device
        devices->Item(i, &device);


        // 7-7-2026: Dit moet naar een andere klasse op gegeven moment
        // ======================================
        //               VOLUME
        // ======================================


        //3-7-2026: Volume interface ophalen
        //8-7-2026: regel verwijderen: want zit nu in de class IAudioEndpointVolume* endpointVolume = nullptr;

        //7-7-2026: Weggehaald 'HRESULT': HRESULT hr = device->Activate(
        // hr = device->Activate
        // (
        //     __uuidof(IAudioEndpointVolume),
        //     CLSCTX_ALL,
        //     NULL,
        //     (void**)&endpointVolume
        // );

        // //3-7-2026: volume aanpassen
        // if(SUCCEEDED(hr) && endpointVolume)
        // {
        //     // Declareer een float vol (gebruik f)
        //     float vol = 0.0f;
        //     // Roep memberfunctie GetMasterVolumeLevelScalar aan van het COM object waar endpoint Volume naar wijst en geef het adres van de float door
        //     endpointVolume->GetMasterVolumeLevelScalar(&vol);

        //     // Geef het volume
        //     cout <<"Volume: "<<vol<<endl;
        //     // Roep de methode SetMasterVolumeLevelScalar aan op het COM-object waar endpointVolume naar wijst, en zet het volume op 1.0 (100%)
        //     // zonder event callback
        //     //8-7-26: eruit en verplaatst: endpointVolume->SetMasterVolumeLevelScalar(1.0f, NULL);
        //     //volume aanpassen: Roep methode aan op het COM-object waar endpointVolume naar wijst
        //     endpointVolume->Release();
        //     ;
        // }



    // ======================================
    //         NAAM OPHALEN
    // ======================================

        // Maak een pointer props die wijst naar IPropertyStore 
        IPropertyStore* props = nullptr;
        // roep een OpenpropertyStore aan op object waar device naar wijst. Parameter is STGM_READ (alleen lezen), & props
        device->OpenPropertyStore(STGM_READ, &props);
        // maak een variabele varnName met datatype Propvariant (dat is een windows datatype). 
        PROPVARIANT varName;
        // roep de windows functie PropVariantInit aan en geef het adres van varName door 
        PropVariantInit(&varName);
        // roep de GetValue functie aan op het object waar de pointer props naar verwijst, met als parameters de sleutel PKEY_Device_FriendlyName en het adres van varName
        //3-7-2026: eruit gehaald props->GetValue(PKEY_Device_FriendlyName, &varName);
        //3-7-2026: Weer weggehaaldPROPERTYKEY key;
        // key.fmtid = PKEY_Device_FriendlyName.fmtid;
        // key.pid = PKEY_Device_FriendlyName.pid;
        props->GetValue(PKEY_Device_FriendlyName, &varName);

        // props->GetValue(key, &varName);
        // print een streepje, daarna de naam van het audio device (Unicode), en ga naar een nieuwe regel
        // we gebruiken geen cout want char heeft maar 8 bits en windows audio namen UTF-16 dus langer
        wcout << L"- "<<varName.pwszVal <<endl;
        // roep de windows functie PropVariantClear (maak leeg) aan en geef het adres van varName door:
        PropVariantClear(&varName);
        // geef het COM object (property store) terug aan Windows
        props->Release();
        // device release
        device->Release();
    }
    devices->Release();
    enumerator->Release();
    CoUninitialize();

}

void AudioDeviceManager::getDefaultDevice()
{
    std::cout<< "Standaard audio device ophalen..." << std::endl;

    //8-7-26: maak een pointer genaamd enumerator naar een nog te maken object IMMdeviceEnumerator*
    IMMDeviceEnumerator *enumerator = nullptr;

    // Maak een variabele hr van het type HRESULT en sla daarin de returnwaarde van COCreateInstance() op.
    // Parameters zijn __uidof(MMDeviceEnumerator),NULL, CLSCTX_ALL, __uidof(IMMDeviceEnumerator),(void**)&enumerator
    // BELANGRIJK: Argumenten bij een functieaanroep kunnen op hun beurt ook weer functie aanroepen zijn.
    // Bijvoorbeeld print(convert(uppercase(removeSpaces(readFile(openFile(path))))); Dit is een geneste loop en lijkt
    // op miniatuurversie van search engine.
    HRESULT hr = CoCreateInstance(__uuidof(MMDeviceEnumerator),NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&enumerator);

    // als hr faalt
    if(FAILED(hr))
    {
        // Print: Enumerator maken mislukt en retourneer
        cout<<"Enumerator maken mislukt. HRESULT: "<<hex<<hr<<endl;
        return;
    }

    // maak een pointer defaultDevice naar een COM interfacetype IMMDevice en stel deze op null.
    IMMDevice *defaultDevice = nullptr;

    // roep de memberfunctie GetDefaultAudioEndpoint op van de functie enumerator en geef als parameters door
    // eRender, eConsole, &defaultDevice. Sla op in hr
    hr = enumerator->GetDefaultAudioEndpoint(eRender, eConsole, &defaultDevice);

    // als hr Failed()
    if(FAILED(hr))
    {
        //bericht: "Standaard apparaat ophalen mislukt"
        cout << "Standaard apparaat ophalen mislukt"<<endl;
        // BELANGRIJK: door retourneren stopt de functie en 
        // Geef enumerator vrij en retourneer
        enumerator->Release();
        return;
    }

    // bericht: Standaard apparaat gevonden!
    cout<<"Standaard apparaat gevonden!"<<endl;
    
    //maak een pointer naar een nog te maken object van IPropertyStore en noem deze props
    IPropertyStore *props = nullptr;
    // roep de memberfunctie OpenPropertyStore op van object defaultDevice met parameters STGM_READ en een pointer naar een pointer props
    // sla het resultaat op in hr
    hr = defaultDevice->OpenPropertyStore(STGM_READ, &props);

    // Als hr faalt
    if(FAILED(hr))
    {
        // Print een bericht dat het ophalen van de PropertyStore mislukt is
        cout<<"Ophalen van PropertySrote is mislukt"<<endl;
        // Geef de defaultDevice vrij
        defaultDevice->Release();
        // Geef de enumerator vrij
        enumerator->Release();
        // retourneer het resultaat (dus einde functie)
        return;
    }

    // PROPVARIANT: Met hoofdletters omdat het een windows type is.
    // maak van het datatype PROPVARIANT varname
    PROPVARIANT varName;

    // Roep de functie PropVariantInit op en geef door een pointer als parameter
    PropVariantInit(&varName);

    // Vraag van de pointer props de memberfunctie GetValue op en geef als parameters PKEY_Device_FriendlyName, en als adres vanName
    props->GetValue(PKEY_Device_FriendlyName, &varName);

    // print (windows apparaatnaam Unicode tekst, dus geen gewone char): 
    wcout<<L"Standaard apparaat: "<<varName.pwszVal<<endl;

    // Maak een pointer endpointVolume naar een nog te maken object IAudioEndpointVolume
    //8-7-26: Eruit gehaald: IAudioEndpointVolume *endpointVolume = nullptr;
    // Roep op het object defaultDevice de Activate Functie op en geef als parameters door __uuidof(IAudioEndpointVolume, CLSCTX_ALL, NULL en void(**)&endpointerVolume)
    // void(**)&endpointVolume: endpointVolume is een pointer Nog uitzoeken
    //10-7-2026: Dit kan eruit: En er weer in 
    hr = defaultDevice->Activate(__uuidof(IAudioEndpointVolume),CLSCTX_ALL,NULL,(void**)&endpointVolume);
    // als hr Failed
    if(FAILED(hr))
    {
        // print dat Volume interface ophalen mislukt is
        cout<<"Volume interface ophalen is mislukt"<<endl;
        // Geef props, defaultDevice en enumerator vrij
        PropVariantClear(&varName);
        props->Release();
        defaultDevice->Release();
        enumerator->Release();
        return;
    }
    // Anders Volume interface gevonden!
    cout<< "Volume Interface gevonden!"<<endl;

    //8-7-2026: nieuw stuk
    //10-7-2026: Dat kan er uit en verplaatst naar andere functie
    // declareer een float volume en initialiseer op 0
    // float volume = 0.0f;
    //roep de memberfunctie GetMasterVolumeLevelScalar op het object endpointerVolume op en geef de pointer naar volume door
    //endpointVolume->GetMasterVolumeLevelScalar(&volume);
    // print het huidige volume
    //cout<<"Het huidige volume is "<< volume << endl;

    // //Nu gaan we het mastervolume naar 50% zetten
    // endpointVolume->SetMasterVolumeLevelScalar(0.5f, NULL);

    // Opruimen: Release: 
    // PropVariantClear, endpointVolume, props, defaultDevice, enumerator
    // PropVariant is een struct
    PropVariantClear(&varName);

    // endpointVolume Releasen
    //9-7-2026 Eruit want later releasen endpointVolume->Release();
    // Gebruik Release voor COM objecten
    props->Release();
    // geef defaultDevice vrij
    defaultDevice->Release();
    // geef enumerator vrij
    enumerator->Release();
}

//9-7-2026: Destructor
// AudioDeviceManager::~AudioDeviceManager()
// {
//     if(endpointVolume)
//     {
//         endpointVolume->Release();
//         endpointVolume = nullptr;
//     }

//     CoUninitialize();
// }

//10-7-2026: Kan eruit was om te debuggen
IAudioEndpointVolume* AudioDeviceManager::getEndpointVolume()
{
    cout << "Endpoint teruggeven: ";

    if(endpointVolume)
    {
        cout << "bestaat!" << endl;
    }
    else
    {
        cout << "nullptr!" << endl;
    }

    return endpointVolume;
}

//10-7-2026 Wordt:
AudioDeviceManager::~AudioDeviceManager()
{
    cout<<"AudioDeviceManager afgesloten"<<endl;
}
