cmake_minimum_required(VERSION 3.5)
project(imagedecoder.mpo)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR})

find_package(Kodi REQUIRED)
find_package(JPEG REQUIRED)

include_directories(${KODI_INCLUDE_DIR}/..
                    ${JPEG_INCLUDE_DIR})

add_subdirectory(lib/libmpo EXCLUDE_FROM_ALL)

set(MPO_SOURCES src/MPOPicture.cpp)

set(DEPLIBS mpo_static ${JPEG_LIBRARIES})

build_addon(imagedecoder.mpo MPO DEPLIBS)

include(CPack)
