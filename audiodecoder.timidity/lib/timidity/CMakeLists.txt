project(timidity C)

set(SOURCES timidity/aq.c
            timidity/audio_cnv.c
            timidity/buffer_a.c
            timidity/common.c
            timidity/controls.c
            timidity/sdl_c.c
            timidity/effect.c
            timidity/filter.c
            timidity/freq.c
            timidity/instrum.c
            timidity/miditrace.c
            timidity/mix.c
            timidity/modmid_a.c
            timidity/mt19937ar.c
            timidity/optcode.c
            timidity/output.c
            timidity/playmidi.c
            timidity/quantity.c
            timidity/readmidi.c
            timidity/recache.c
            timidity/resample.c
            timidity/reverb.c
            timidity/sbkconv.c
            timidity/sffile.c
            timidity/sfitem.c
            timidity/smfconv.c
            timidity/smplfile.c
            timidity/sndfont.c
            timidity/tables.c
            timidity/timidity.c
            timidity/version.c
            timidity/wave_a.c
            timidity/wrdt.c
            libarc/url.c
            libarc/url_mem.c
            libarc/url_file.c
            utils/mblock.c
            utils/strtab.c
            utils/fft4g.c
            utils/timer.c
            utils/getopt.c)
 
add_definitions(-DHAVE_ERRNO_H=1 -DTIMID_VERSION="2.15.0")
if(APPLE)
  add_definitions(-DHAVE_STRLCPY=1 -DHAVE_STRLCAT=1)
endif(APPLE)
if(NOT WIN32)
  add_definitions(-DTIME_WITH_SYS_TIME=1 -DHAVE_GETTIMEOFDAY=1)
else()
  include(CheckSymbolExists)
  check_symbol_exists(_X86_ "Windows.h" _X86_)
  check_symbol_exists(_AMD64_ "Windows.h" _X64_)
  check_symbol_exists(_ARM_ "Windows.h" _ARM_)
  check_symbol_exists(_ARM64_ "Windows.h" _ARM64_)

  if(NOT CMAKE_SYSTEM_NAME STREQUAL WindowsStore)
    add_definitions(-D_CRT_NONSTDC_NO_DEPRECATE -D__W32__)
  else()
    add_compile_options(/wd4996)
  endif()
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR} timidity libarc utils)

add_library(timidity_0 SHARED ${SOURCES})
add_library(timidity_1 SHARED ${SOURCES})
if(NOT WIN32)
  target_link_libraries(timidity_0 m)
  target_link_libraries(timidity_1 m)
  target_compile_options(timidity_0 PRIVATE -fno-common)
  target_compile_options(timidity_1 PRIVATE -fno-common)
endif()
set_property(TARGET timidity_0 PROPERTY POSITION_INDEPENDENT_CODE ON)
set_property(TARGET timidity_1 PROPERTY POSITION_INDEPENDENT_CODE ON)
