cmake_minimum_required(VERSION 3.5)
project(audiodecoder.ncsf)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR})

find_package(Kodi REQUIRED)
find_package(ZLIB REQUIRED)

include_directories(${KODI_INCLUDE_DIR}/..
                    ${PROJECT_SOURCE_DIR}/lib)

add_subdirectory(lib/psflib)
add_subdirectory(lib/SSEQPlayer)

set(NCSF_SOURCES src/NCSFCodec.cpp)

set(NCSF_HEADERS src/NCSFCodec.h
                 src/CircularBuffer.h)

set(DEPLIBS psflib SSEQPlayer ${ZLIB_LIBRARIES})

build_addon(audiodecoder.ncsf NCSF DEPLIBS)

if(CORE_SYSTEM_NAME MATCHES windows)
  target_compile_definitions(audiodecoder.ncsf PRIVATE -Dstrcasecmp=_stricmp)
endif()

include(CPack)
