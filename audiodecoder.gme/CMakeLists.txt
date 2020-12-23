cmake_minimum_required(VERSION 3.5)
project(audiodecoder.gme)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR})

find_package(Kodi REQUIRED)

include_directories(${KODI_INCLUDE_DIR}/..
                    ${PROJECT_SOURCE_DIR}/lib/Game_Music_Emu/gme
                    ${PROJECT_SOURCE_DIR}/lib/File_Extractor)

add_subdirectory(lib/File_Extractor)
add_subdirectory(lib/Game_Music_Emu)

set(GME_SOURCES src/GMECodec.cpp)
set(GME_HEADERS src/GMECodec.h)

set(DEPLIBS gme fileex)

build_addon(audiodecoder.gme GME DEPLIBS)

include(CPack)
