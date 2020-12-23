cmake_minimum_required(VERSION 3.5)
project(audiodecoder.modplug)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR})

find_package(Kodi REQUIRED)
find_package(Modplug REQUIRED)

include_directories(${KODI_INCLUDE_DIR}/..
                    ${MODPLUG_INCLUDE_DIRS})

set(MODPLUG_SOURCES src/ModplugCodec.cpp)
set(MODPLUG_HEADERS src/ModplugCodec.h)

set(DEPLIBS ${MODPLUG_LIBRARIES})

if(WIN32)
  add_definitions(-DMODPLUG_STATIC)
endif()

build_addon(audiodecoder.modplug MODPLUG DEPLIBS)

include(CPack)
