cmake_minimum_required(VERSION 3.5)
project(screensaver.asteroids)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR})

find_package(Kodi REQUIRED)
find_package(glm REQUIRED)

if(NOT WIN32)
  if(APP_RENDER_SYSTEM STREQUAL "gl" OR NOT APP_RENDER_SYSTEM)
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
endif()

include_directories(${GLM_INCLUDE_DIR}
                    ${KODI_INCLUDE_DIR}/..) # Hack way with "/..", need bigger Kodi cmake rework to match right include ways

set(ASTEROIDS_SOURCES src/asteroid.cpp
                      src/Asteroids.cpp
                      src/bullet.cpp
                      src/main.cpp
                      src/ship.cpp)

set(ASTEROIDS_HEADERS src/asteroid.h
                      src/Asteroids.h
                      src/bullet.h
                      src/main.h
                      src/ship.h
                      src/timer.h
                      src/types.h)

build_addon(screensaver.asteroids ASTEROIDS DEPLIBS)

include(CPack)
