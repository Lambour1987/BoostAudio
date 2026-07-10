//1-7-2026: Headerfiles; vertel wat er bestaat maar niet wat het doet.

// Headerfile : zorg dat headers altijd maar 1x mee worden genomen
#pragma once

// class AudioEngine met void functie start (public)
class AudioEngine
{
    public:
        // start functie
        void start();
        // stop functie
        void stop();

    private:
        // bool running die begint op false
        bool running = false;
};


