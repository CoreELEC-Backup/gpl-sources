cmake_minimum_required(VERSION 3.5)
project(audiodecoder.organya)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR})

find_package(Kodi REQUIRED)

include_directories(${KODI_INCLUDE_DIR}/.. # Hack way with "/..", need bigger Kodi cmake rework to match right include ways (becomes done in future)
                    ${PROJECT_SOURCE_DIR}/lib/liborganya)

add_subdirectory(lib/liborganya)

set(ORG_SOURCES src/OrganyaCodec.cpp)

set(ORG_HEADERS src/OrganyaCodec.h)

set(DEPLIBS organya)

build_addon(audiodecoder.organya ORG DEPLIBS)

include(CPack)
