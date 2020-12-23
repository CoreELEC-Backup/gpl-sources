cmake_minimum_required(VERSION 3.5)
project(audiodecoder.sidplay)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR})

find_package(Kodi REQUIRED)
find_package(Sidplay2 REQUIRED)
find_package(Resid REQUIRED)

if(WIN32)
  add_definitions(-DHAVE_MSWINDOWS)
else()
  add_definitions(-DHAVE_UNIX)
endif()

set(SID_SOURCES src/SIDCodec.cpp)
set(SID_HEADERS src/SIDCodec.h)

list(APPEND DEPLIBS ${SIDPLAY2_LIBRARIES}
                    ${RESID_LIBRARIES})

include_directories(${KODI_INCLUDE_DIR}/..
                    ${RESID_INCLUDE_DIRS}
                    ${SIDPLAY2_INCLUDE_DIRS})

build_addon(audiodecoder.sidplay SID DEPLIBS)

include(CPack)
