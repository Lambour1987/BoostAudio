//1-7-2026

#include <iostream>
#include "audioEngine.h"

int main()
{
    std::cout << "BoostAudio gestart!" <<std::endl;

    // maak van de class een object engine aan
    AudioEngine engine;

    // gebruik hierop de memberfunctie
    engine.start();

    return 0;

}