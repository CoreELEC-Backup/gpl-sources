cmake_minimum_required(VERSION 3.5)
project(pvr.filmon)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR})

find_package(Kodi REQUIRED)
find_package(JsonCpp REQUIRED)

include_directories(${JSONCPP_INCLUDE_DIRS}
                    ${KODI_INCLUDE_DIR}/..) # Hack way with "/..", need bigger Kodi cmake rework to match right include ways

set(DEPLIBS ${JSONCPP_LIBRARIES})

set(FILMON_SOURCES src/FilmonAPI.cpp
                   src/PVRFilmonData.cpp)

set(FILMON_HEADERS src/FilmonAPI.h
                   src/PVRFilmonData.h)

if(WIN32)
  add_compile_options(/wd4996)
endif()

build_addon(pvr.filmon FILMON DEPLIBS)

include(CPack)
