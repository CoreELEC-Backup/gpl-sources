cmake_minimum_required(VERSION 3.5)
project(audiodecoder.asap)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR})

find_package(Kodi REQUIRED)

if(WIN32)
  add_definitions("/wd4996")
endif()

include_directories(${KODI_INCLUDE_DIR}/..
                    ${PROJECT_SOURCE_DIR}/lib/asap-code)

set(ASAP_SOURCES src/ASAPCodec.cpp
                 lib/asap-code/asap.c)

set(ASAP_HEADERS src/ASAPCodec.h)

build_addon(audiodecoder.asap ASAP DEPLIBS)

include(CPack)
