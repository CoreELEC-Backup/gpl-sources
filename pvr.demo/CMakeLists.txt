cmake_minimum_required(VERSION 3.5)
project(pvr.demo)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR})

find_package(Kodi REQUIRED)
find_package(TinyXML REQUIRED)

include_directories(${KODI_INCLUDE_DIR}/.. # Hack way with "/..", need bigger Kodi cmake rework to match right include ways
                    ${TINYXML_INCLUDE_DIRS})

set(DEPLIBS ${TINYXML_LIBRARIES})

set(PVRDEMO_SOURCES src/PVRDemo.cpp)

set(PVRDEMO_HEADERS src/PVRDemo.h)

build_addon(pvr.demo PVRDEMO DEPLIBS)

include(CPack)
