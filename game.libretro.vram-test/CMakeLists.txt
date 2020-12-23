cmake_minimum_required(VERSION 3.1)
project(game.libretro.vram-test)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR})

find_package(Kodi REQUIRED)
find_package(libretro-vram-test REQUIRED)

set(VRAM-TEST_CUSTOM_BINARY ${VRAM-TEST_LIB}
    ${PROJECT_NAME}${CMAKE_SHARED_LIBRARY_SUFFIX})

build_addon(${PROJECT_NAME} VRAM-TEST "")

include(CPack)
