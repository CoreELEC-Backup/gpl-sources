cmake_minimum_required(VERSION 3.5)
project(audiodecoder.usf)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR})

find_package(Kodi REQUIRED)
find_package(ZLIB REQUIRED)

include_directories(${KODI_INCLUDE_DIR}/..
                    ${ZLIB_INCLUDE_DIRS}
                    ${PROJECT_SOURCE_DIR}/lib/lazyusf
                    ${PROJECT_SOURCE_DIR}/lib/psflib)

add_subdirectory(lib/lazyusf)
add_subdirectory(lib/psflib)

set(USF_SOURCES src/USFCodec.cpp)
set(USF_HEADERS src/USFCodec.h)

set(DEPLIBS lazyusf psflib ${ZLIB_LIBRARIES})

build_addon(audiodecoder.usf USF DEPLIBS)

if(CORE_SYSTEM_NAME MATCHES windows)
  target_compile_definitions(audiodecoder.usf PRIVATE -Dstrcasecmp=_stricmp)
endif()

include(CPack)
