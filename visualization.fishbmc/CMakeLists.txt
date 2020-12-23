cmake_minimum_required(VERSION 3.5)
project(visualization.fishbmc)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR})

find_package(Kodi REQUIRED)
find_package(glm REQUIRED)

if(NOT WIN32 AND (APP_RENDER_SYSTEM STREQUAL "gl" OR NOT APP_RENDER_SYSTEM))
  find_package(OpenGl REQUIRED)
  set(DEPLIBS ${OPENGL_LIBRARIES})
  set(includes ${OPENGL_INCLUDE_DIR})
  add_definitions(${OPENGL_DEFINITIONS})
else()
  find_package(OpenGLES REQUIRED)
  set(DEPLIBS ${OPENGLES_LIBRARIES})
  set(includes ${OPENGLES_INCLUDE_DIR})
  add_definitions(${OPENGLES_DEFINITIONS})
endif()

add_definitions(-DHAS_SDL_OPENGL)
include_directories(${includes}
                    ${GLM_INCLUDE_DIR}
                    ${KODI_INCLUDE_DIR}/..) # Hack way with "/..", need bigger Kodi cmake rework to match right include ways (becomes done in future)

set(FISH_SOURCES src/analyst.cpp
                 src/audiobuffer.cpp
                 src/blurengine.cpp
                 src/cpudetect.cpp
                 src/fische.cpp
                 src/fishbmc_addon.cpp
                 src/screenbuffer.cpp
                 src/vector.cpp
                 src/vectorfield.cpp
                 src/wavepainter.cpp)

set(FISH_HEADERS src/analyst.h
                 src/audiobuffer.h
                 src/blurengine.h
                 src/fische.h
                 src/fishbmc_addon.h
                 src/fische_internal.h
                 src/screenbuffer.h
                 src/vectorfield.h
                 src/vector.h
                 src/wavepainter.h)

build_addon(visualization.fishbmc FISH DEPLIBS)

include(CPack)
