cmake_minimum_required(VERSION 3.5)
project(pvr.njoy)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR})

find_package(Kodi REQUIRED)
find_package(TinyXML REQUIRED)

include_directories(${TINYXML_INCLUDE_DIR}
                    ${KODI_INCLUDE_DIR}/..) # Hack way with "/..", need bigger Kodi cmake rework to match right include ways

set(NJOY_SOURCES src/N7Xml.cpp)

set(NJOY_HEADERS src/N7Xml.h)

set(DEPLIBS ${TINYXML_LIBRARIES})

build_addon(pvr.njoy NJOY DEPLIBS)

include(CPack)
