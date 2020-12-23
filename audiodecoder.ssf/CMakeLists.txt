cmake_minimum_required(VERSION 3.5)
project(audiodecoder.ssf)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR})

find_package(Kodi REQUIRED)
find_package(ZLIB REQUIRED)

include_directories(${KODI_INCLUDE_DIR}/..
                    ${PROJECT_SOURCE_DIR}/lib/psflib
                    ${PROJECT_SOURCE_DIR}/lib/Highly_Theoretical/Core)

add_subdirectory(lib/psflib)
add_subdirectory(lib/Highly_Theoretical)

set(SSF_SOURCES src/SSFCodec.cpp)
set(SSF_HEADERS src/CircularBuffer.h
                src/SSFCodec.h)

set(DEPLIBS highly_theoretical psflib ${ZLIB_LIBRARIES})

build_addon(audiodecoder.ssf SSF DEPLIBS)

if(CORE_SYSTEM_NAME MATCHES windows)
  target_compile_definitions(audiodecoder.ssf PRIVATE -Dstrcasecmp=_stricmp)
endif()

include(CPack)
