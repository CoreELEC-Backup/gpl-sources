cmake_minimum_required(VERSION 3.5)
project(audiodecoder.timidity)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR})

find_package(Kodi REQUIRED)

include_directories(${PROJECT_SOURCE_DIR}/lib
                    ${KODI_INCLUDE_DIR}/..) # Hack way with "/..", need bigger Kodi cmake rework to match right include ways (becomes done in future)

add_subdirectory(lib/timidity)

set(TIMIDITY_SOURCES src/MidiScan.cpp
                     src/TimidityCodec.cpp)
set(TIMIDITY_HEADERS src/MidiScan.h
                     src/TimidityCodec.h)

add_definitions(-DLIBRARY_PREFIX="${CMAKE_SHARED_LIBRARY_PREFIX}"
                -DLIBRARY_SUFFIX="${CMAKE_SHARED_LIBRARY_SUFFIX}")

set(TIMIDITY_ADDITIONAL_BINARY $<TARGET_FILE:timidity_0> $<TARGET_FILE:timidity_1>)

if(WIN32)
  find_package(dlfcn-win32 REQUIRED)
  list(APPEND DEPLIBS ${dlfcn-win32_LIBRARIES})
  include_directories(${dlfcn-win32_INCLUDE_DIRS})
  add_definitions(-D_CRT_NONSTDC_NO_DEPRECATE)
endif()

build_addon(audiodecoder.timidity TIMIDITY DEPLIBS)

include(CPack)
