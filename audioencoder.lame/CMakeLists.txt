cmake_minimum_required(VERSION 3.5)
project(audioencoder.lame)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR})

find_package(Kodi REQUIRED)
find_package(Lame REQUIRED)
if(WIN32)
  find_package(MPGHIP REQUIRED)
endif()

include_directories(${KODI_INCLUDE_DIR}/.. # Hack way with "/..", need bigger Kodi cmake rework to match right include ways (becomes done in future)
                    ${LAME_INCLUDE_DIRS})

set(LAME_SOURCES src/EncoderLame.cpp)

set(DEPLIBS ${LAME_LIBRARIES})
if(WIN32)
  list(APPEND DEPLIBS ${MPGHIP_LIBRARIES})
endif()

build_addon(audioencoder.lame LAME DEPLIBS)

include(CPack)
