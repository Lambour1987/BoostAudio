#7-7-2026: CMakeLists.txt is een instructieblad voor CMake. Het zegt niet wat je programma doet, maar hoe je project gebouwd moet worden:
#Een recept voor de compiler.
# CMake is eigenlijk een laag bovenopg++ die het organiseren voor je doet


# Dit project heeft minimaal versie 3.20 nodig
cmake_minimum_required(VERSION 3.20)

# Naam van het Project
project(BoostAudio)

# We gebruiken C++17
set(CMAKE_CXX_STANDARD 17)

# 7-7-2026: Dit zorgt ervoor dat we in main.cpp geen #include "../include/audioEngine.h"
# hoeven te schrijven maar gewoon #include "audioEngine.h"
include_directories(include)

#Maak een programma genaamd BoostAudio
add_executable(
    BoostAudio
#Bronbestanden
    src/main.cpp
    src/audioEngine.cpp
    src/audioDeviceManager.cpp
    src/dsp.cpp
    src/audioVolumeController.cpp
)