cmake_minimum_required(VERSION 3.5)
project(audiodecoder.snesapu)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR})

find_package(Kodi REQUIRED)

include_directories(${KODI_INCLUDE_DIR}/..
                    ${PROJECT_SOURCE_DIR}/lib/snes_spc)

add_subdirectory(lib/snes_spc)

set(SNES_SOURCES src/SPCCodec.cpp)
set(SNES_HEADERS src/SPCCodec.h)

set(DEPLIBS snes_spc)

build_addon(audiodecoder.snesapu SNES DEPLIBS)

include(CPack)
