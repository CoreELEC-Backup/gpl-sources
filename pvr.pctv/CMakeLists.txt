cmake_minimum_required(VERSION 3.5)
project(pvr.pctv)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR})

find_package(Kodi REQUIRED)
find_package(JsonCpp REQUIRED)

include_directories(${JSONCPP_INCLUDE_DIRS}
                    ${KODI_INCLUDE_DIR}/..) # Hack way with "/..", need bigger Kodi cmake rework to match right include ways

set(DEPLIBS ${JSONCPP_LIBRARIES})

set(PCTV_SOURCES src/addon.cpp
                 src/md5.cpp
                 src/PctvData.cpp
                 src/rest.cpp)

set(PCTV_HEADERS src/addon.h
                 src/md5.h
                 src/PctvData.h
                 src/rest.h)

# To prevent build exit on deprecated warnings (from JsonCpp)
if(WIN32)
  add_compile_options(/wd4996)
endif()

build_addon(pvr.pctv PCTV DEPLIBS)

include(CPack)
