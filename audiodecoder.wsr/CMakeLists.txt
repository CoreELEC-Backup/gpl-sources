cmake_minimum_required(VERSION 3.5)
project(audiodecoder.wsr)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR})

find_package(Kodi REQUIRED)

include(ExternalProject)

include_directories(${PROJECT_SOURCE_DIR}/lib
                    ${KODI_INCLUDE_DIR}/..)

add_subdirectory(lib/in_wsr)

set(WSR_SOURCES src/WSRCodec.cpp)
set(WSR_HEADERS src/WSRCodec.h)

add_definitions(-DLIBRARY_PREFIX="${CMAKE_SHARED_LIBRARY_PREFIX}"
                -DLIBRARY_SUFFIX="${CMAKE_SHARED_LIBRARY_SUFFIX}")

set(WSR_ADDITIONAL_BINARY $<TARGET_FILE:in_wsr_track> $<TARGET_FILE:in_wsr_0> $<TARGET_FILE:in_wsr_1>)

if(WIN32)
  find_package(dlfcn-win32 REQUIRED)
  list(APPEND DEPLIBS ${dlfcn-win32_LIBRARIES})
  include_directories(${dlfcn-win32_INCLUDE_DIRS})
  add_definitions(-D_CRT_NONSTDC_NO_DEPRECATE)
endif()

build_addon(audiodecoder.wsr WSR DEPLIBS)

include(CPack)
