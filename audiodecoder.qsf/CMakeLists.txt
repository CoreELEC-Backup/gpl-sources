cmake_minimum_required(VERSION 3.5)
project(audiodecoder.qsf)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR})

find_package(Kodi REQUIRED)
find_package(ZLIB REQUIRED)

include_directories(${KODI_INCLUDE_DIR}/..
                    ${ZLIB_INCLUDE_DIR}
                    ${PROJECT_SOURCE_DIR}/lib/psflib
                    ${PROJECT_SOURCE_DIR}/lib/highly_quixotic/Core)

add_subdirectory(lib/psflib)
add_subdirectory(lib/highly_quixotic)

set(QSF_SOURCES src/QSFCodec.cpp)

set(QSF_HEADERS src/CircularBuffer.h
                src/QSFCodec.h
                src/QSoundRom.h)

set(DEPLIBS highly_quixotic psflib ${ZLIB_LIBRARIES})

build_addon(audiodecoder.qsf QSF DEPLIBS)

if(CORE_SYSTEM_NAME MATCHES windows)
  target_compile_definitions(audiodecoder.qsf PRIVATE -Dstrcasecmp=_stricmp -D__inline__=__inline)
endif()

include(CPack)
