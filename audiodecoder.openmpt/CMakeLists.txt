cmake_minimum_required(VERSION 3.5)
project(audiodecoder.openmpt)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR})

find_package(Kodi REQUIRED)
find_package(Openmpt REQUIRED)
find_package(ZLIB REQUIRED)

include_directories(${ZLIB_INCLUDE_DIR}
                    ${KODI_INCLUDE_DIR}/..
                    ${OPENMPT_INCLUDE_DIRS})

set(OPENMPT_SOURCES src/OpenMptCodec.cpp)
set(OPENMPT_HEADERS src/OpenMptCodec.h)

set(DEPLIBS ${OPENMPT_LIBRARIES} ${ZLIB_LIBRARIES})

build_addon(audiodecoder.openmpt OPENMPT DEPLIBS)

include(CPack)
