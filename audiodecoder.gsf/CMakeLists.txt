cmake_minimum_required(VERSION 3.5)
project(audiodecoder.gsf)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR})

find_package(Kodi REQUIRED)
find_package(ZLIB REQUIRED)

include(ExternalProject)

include_directories(${KODI_INCLUDE_DIR}/..
                    ${PROJECT_SOURCE_DIR}/lib/psflib
                    ${PROJECT_SOURCE_DIR}/lib/viogsf)

add_subdirectory(lib/psflib)
add_subdirectory(lib/viogsf)

set(GSF_SOURCES src/GSFCodec.cpp)
set(GSF_HEADERS src/GSFCodec.h)

set(DEPLIBS viogsf psflib ${ZLIB_LIBRARIES})

build_addon(audiodecoder.gsf GSF DEPLIBS)

if(CORE_SYSTEM_NAME MATCHES windows)
  target_compile_definitions(audiodecoder.gsf PRIVATE -Dstrcasecmp=_stricmp)
endif()

include(CPack)
