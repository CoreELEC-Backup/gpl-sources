cmake_minimum_required(VERSION 3.1)
project(game.libretro.uae4arm)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR})

find_package(Kodi REQUIRED)
find_package(libretro-uae4arm REQUIRED)

set(UAE4ARM_CUSTOM_BINARY ${UAE4ARM_LIB}
    ${PROJECT_NAME}${CMAKE_SHARED_LIBRARY_SUFFIX})

build_addon(${PROJECT_NAME} UAE4ARM "")
