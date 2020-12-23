cmake_minimum_required(VERSION 3.5)
project(audioencoder.wav)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR})

find_package(Kodi REQUIRED)

include_directories(${KODI_INCLUDE_DIR}/..) # Hack way with "/..", need bigger Kodi cmake rework to match right include ways (becomes done in future)

set(WAV_SOURCES src/EncoderWav.cpp)
set(DEPLIBS)

build_addon(audioencoder.wav WAV DEPLIBS)

include(CPack)
