cmake_minimum_required(VERSION 3.5)
project(audiodecoder.vgmstream)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR})

find_package(Kodi REQUIRED)

include_directories(${KODI_INCLUDE_DIR}/..
                    ${PROJECT_SOURCE_DIR}/lib/vgmstream
                    ${PROJECT_SOURCE_DIR}/lib/vgmstream/ext_includes)

# Override vgmstream options
set(USE_FDKAAC OFF CACHE BOOL "Use FDK-AAC/QAAC for support of MP4 AAC" FORCE)
set(USE_FFMPEG OFF CACHE BOOL "Use FFmpeg for support of many codecs" FORCE)
set(USE_MPEG OFF CACHE BOOL "Use libmpg123 for support of MPEG (MP1/MP2/MP3)" FORCE)
set(USE_VORBIS OFF CACHE BOOL "Use libvorbis for support of Vorbis" FORCE)
set(USE_MAIATRAC3PLUS OFF CACHE BOOL "Use MAIATRAC3+ for support of ATRAC3+" FORCE)
set(USE_G7221 OFF CACHE BOOL "Use libg7221_decode for support of ITU-T G.722.1 annex C" FORCE)
set(USE_G719 OFF CACHE BOOL "Use libg719_decode for support ITU-T G.719" FORCE)
set(USE_ATRAC9 OFF CACHE BOOL "Use LibAtrac9 for support of ATRAC9" FORCE)
set(USE_CELT OFF CACHE BOOL "Use libcelt for support of FSB CELT versions 0.6.1 and 0.11.0" FORCE)
set(BUILD_CLI OFF CACHE BOOL "Build vgmstream CLI" FORCE)
set(BUILD_FB2K OFF CACHE BOOL "Build foobar2000 component" FORCE)
set(BUILD_WINAMP OFF CACHE BOOL "Build Winamp plugin" FORCE)
set(BUILD_XMPLAY OFF CACHE BOOL "Build XMPlay plugin" FORCE)
set(BUILD_AUDACIOUS OFF CACHE BOOL "Build Audacious plugin" FORCE)
add_subdirectory(lib/vgmstream)

set(VGM_SOURCES src/VGMCodec.cpp)
set(VGM_HEADERS src/VGMCodec.h)

set(DEPLIBS libvgmstream)

build_addon(audiodecoder.vgmstream VGM DEPLIBS)

include(CPack)
