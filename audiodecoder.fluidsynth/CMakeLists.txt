cmake_minimum_required(VERSION 3.5)
project(audiodecoder.fluidsynth)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR})

find_package(Kodi REQUIRED)
find_package(FluidSynth REQUIRED)

include(ExternalProject)

include_directories(${KODI_INCLUDE_DIR}/..
                    ${FLUIDSYNTH_INCLUDE_DIRS})

set(FLUID_SOURCES src/FluidDecoder.cpp)
set(FLUID_HEADERS src/FluidDecoder.h)

set(DEPLIBS ${FLUIDSYNTH_LIBRARIES})

# the cmake var SOUND_FONT can be used to set a platform specific default sound font
# e.g. -DSOUND_FONT=/path/to/some_sound_font.sf2
option(SOUND_FONT "Default sound font" "")


build_addon(audiodecoder.fluidsynth FLUID DEPLIBS)

include(CPack)
