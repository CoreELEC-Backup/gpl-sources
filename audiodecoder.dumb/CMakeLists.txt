cmake_minimum_required(VERSION 3.5)
project(audiodecoder.dumb)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR})

find_package(Kodi REQUIRED)

if(NOT WIN32)
  # For Dumb to define 64-bit-type for offsets
  add_definitions(-DDUMB_OFF_T_CUSTOM=int64_t)
endif()

set(BUILD_EXAMPLES OFF CACHE BOOL "Build example binaries")
set(BUILD_ALLEGRO4 OFF CACHE BOOL "Build Allegro4 support")
add_subdirectory(lib/dumb)
set(DUMB_LIBRARIES dumb)
set(DUMB_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/lib/dumb/include)

include_directories(${KODI_INCLUDE_DIR}/..
                    ${DUMB_INCLUDE_DIRS})

set(DUMB_SOURCES src/DumbCodec.cpp)
set(DUMB_HEADERS src/DumbCodec.h)

set(DEPLIBS ${DUMB_LIBRARIES})

build_addon(audiodecoder.dumb DUMB DEPLIBS)

include(CPack)
