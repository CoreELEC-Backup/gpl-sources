cmake_minimum_required(VERSION 3.5)
project(audiodecoder.nosefart)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR})

find_package(Kodi REQUIRED)

include_directories(${PROJECT_SOURCE_DIR}/lib
                    ${PROJECT_SOURCE_DIR}/lib/nosefart/src/
                    ${PROJECT_SOURCE_DIR}/lib/nosefart/src/cpu/nes6502
                    ${PROJECT_SOURCE_DIR}/lib/nosefart/src/machine
                    ${PROJECT_SOURCE_DIR}/lib/nosefart/src/sndhrdw
                    ${KODI_INCLUDE_DIR}/..)

add_subdirectory(lib/nosefart)

set(NOSEFART_SOURCES src/NSFCodec.cpp)
set(NOSEFART_HEADERS src/NSFCodec.h)

add_definitions(-DNSF_PLAYER -DNES6502_MEM_ACCESS_CTRL)
add_definitions(-DLIBRARY_PREFIX="${CMAKE_SHARED_LIBRARY_PREFIX}"
                -DLIBRARY_SUFFIX="${CMAKE_SHARED_LIBRARY_SUFFIX}")

set(NOSEFART_ADDITIONAL_BINARY $<TARGET_FILE:nosefart_tag> $<TARGET_FILE:nosefart_0> $<TARGET_FILE:nosefart_1>)

if(WIN32)
  find_package(dlfcn-win32 REQUIRED)
  list(APPEND DEPLIBS ${dlfcn-win32_LIBRARIES})
  include_directories(${dlfcn-win32_INCLUDE_DIRS})
  add_definitions(-D_CRT_NONSTDC_NO_DEPRECATE)
endif()

build_addon(audiodecoder.nosefart NOSEFART DEPLIBS)

include(CPack)
