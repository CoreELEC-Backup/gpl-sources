cmake_minimum_required(VERSION 3.5)
project(audiodecoder.stsound)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR})

find_package(Kodi REQUIRED)

include_directories(${KODI_INCLUDE_DIR}/.. # Hack way with "/..", need bigger Kodi cmake rework to match right include ways (becomes done in future)
                    ${PROJECT_SOURCE_DIR}/lib/stsound/StSoundLibrary)

if(WIN32)
  add_definitions(-D_CRT_SECURE_NO_WARNINGS)
else()
  add_definitions(-D_LINUX)
endif()

set(STSOUND_SOURCES src/YMCodec.cpp
                    lib/stsound/StSoundLibrary/digidrum.cpp
                    lib/stsound/StSoundLibrary/LZH/LzhLib.cpp
                    lib/stsound/StSoundLibrary/Ym2149Ex.cpp
                    lib/stsound/StSoundLibrary/Ymload.cpp
                    lib/stsound/StSoundLibrary/YmMusic.cpp
                    lib/stsound/StSoundLibrary/YmUserInterface.cpp)

set(STSOUND_HEADERS src/YMCodec.h
                    lib/stsound/StSoundLibrary/digidrum.h
                    lib/stsound/StSoundLibrary/StSoundLibrary.h
                    lib/stsound/StSoundLibrary/Ym2149Ex.h
                    lib/stsound/StSoundLibrary/YmLoad.h
                    lib/stsound/StSoundLibrary/YmMusic.h
                    lib/stsound/StSoundLibrary/YmTypes.h)

build_addon(audiodecoder.stsound STSOUND DEPLIBS)

include(CPack)
