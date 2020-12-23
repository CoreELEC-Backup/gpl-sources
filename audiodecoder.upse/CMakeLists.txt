cmake_minimum_required(VERSION 3.5)
project(audiodecoder.upse)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR})

find_package(Kodi REQUIRED)
find_package(ZLIB REQUIRED)

include_directories(${KODI_INCLUDE_DIR}/..
                    ${PROJECT_SOURCE_DIR}/lib/libupse)

add_subdirectory(lib/libupse)

set(UPSE_SOURCES src/UPSECodec.cpp)
set(UPSE_HEADERS src/UPSECodec.h)

if(UNIX AND NOT APPLE)
  set(DEPLIBS -Wl,--whole-archive upse -Wl,--no-whole-archive ${ZLIB_LIBRARIES})
else()
  set(DEPLIBS upse ${ZLIB_LIBRARIES})
endif()

build_addon(audiodecoder.upse UPSE DEPLIBS)

include(CPack)
