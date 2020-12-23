# - Try to find ffmpeg
# Once done this will define
#
#  FFMPEG_FOUND - system has ffmpeg
#  FFMPEG_INCLUDE_DIRS - the ffmpeg include directory
#  FFMPEG_LIBRARIES - Link these to use ffmpeg
#  FFMPEG_DEFINITIONS - Compiler switches required for using ffmpeg
#
#  Copyright (c) 2008 Andreas Schneider <mail@cynapses.org>
#  Modified for other libraries by Lasse Kärkkäinen <tronic>
#
#  Redistribution and use is allowed according to the terms of the New
#  BSD license.
#


# include this to handle the QUIETLY and REQUIRED arguments
include(FindPackageHandleStandardArgs)
include(GetPrerequisites)

if (FFMPEG_LIBRARIES AND FFMPEG_INCLUDE_DIRS)
  # in cache already
  set(FFMPEG_FOUND TRUE)
else (FFMPEG_LIBRARIES AND FFMPEG_INCLUDE_DIRS)
  # use pkg-config to get the directories and then use these values
  # in the FIND_PATH() and FIND_LIBRARY() calls
  find_package(PkgConfig)
  if (PKG_CONFIG_FOUND)
    pkg_check_modules(_FFMPEG_AVCODEC libavcodec QUIET)
    pkg_check_modules(_FFMPEG_AVFORMAT libavformat QUIET)
    pkg_check_modules(_FFMPEG_SWSCALE libswscale QUIET)
    pkg_check_modules(_FFMPEG_AVUTIL libavutil QUIET)
  endif (PKG_CONFIG_FOUND)

  find_path(FFMPEG_AVCODEC_INCLUDE_DIR
    NAMES avcodec.h
    PATHS ${_FFMPEG_AVCODEC_INCLUDE_DIRS} /usr/include /usr/include/ffmpeg-compat /usr/local/include /opt/local/include /sw/include
    PATH_SUFFIXES ffmpeg libavcodec
  )

  find_path(FFMPEG_AVUTIL_INCLUDE_DIR
    NAMES avutil.h
    PATHS ${_FFMPEG_AVUTIL_INCLUDE_DIRS} /usr/include /usr/include/ffmpeg-compat /usr/local/include /opt/local/include /sw/include
    PATH_SUFFIXES ffmpeg libavutil
  )

  find_path(FFMPEG_AVFORMAT_INCLUDE_DIR
    NAMES avformat.h
    PATHS ${_FFMPEG_AVFORMAT_INCLUDE_DIRS} /usr/include /usr/include/ffmpeg-compat /usr/local/include /opt/local/include /sw/include
    PATH_SUFFIXES ffmpeg libavformat
  )

  find_path(FFMPEG_SWSCALE_INCLUDE_DIR
    NAMES swscale.h
    PATHS ${_FFMPEG_SWSCALE_INCLUDE_DIRS} /usr/include /usr/include/ffmpeg-compat /usr/local/include /opt/local/include /sw/include
    PATH_SUFFIXES ffmpeg libswscale
  )

  find_library(FFMPEG_AVCODEC_LIBRARY
    NAMES avcodec
    PATHS ${_FFMPEG_AVCODEC_LIBRARY_DIRS} /usr/lib /usr/lib/ffmpeg-compat /usr/local/lib /opt/local/lib /sw/lib
  )

  find_library(FFMPEG_AVUTIL_LIBRARY
    NAMES avutil
    PATHS ${_FFMPEG_AVUTIL_LIBRARY_DIRS} /usr/lib /usr/lib/ffmpeg-compat /usr/local/lib /opt/local/lib /sw/lib
  )

  find_library(FFMPEG_AVFORMAT_LIBRARY
    NAMES avformat
    PATHS ${_FFMPEG_AVFORMAT_LIBRARY_DIRS} /usr/lib /usr/lib/ffmpeg-compat /usr/local/lib /opt/local/lib /sw/lib
  )

  find_library(FFMPEG_SWSCALE_LIBRARY
    NAMES swscale
    PATHS ${_FFMPEG_SWSCALE_LIBRARY_DIRS} /usr/lib /usr/lib/ffmpeg-compat /usr/local/lib /opt/local/lib /sw/lib
  )

  if (FFMPEG_AVCODEC_LIBRARY AND FFMPEG_AVUTIL_LIBRARY AND FFMPEG_AVFORMAT_LIBRARY AND FFMPEG_SWSCALE_LIBRARY)
    set(FFMPEG_FOUND TRUE)
  endif ()

  if (FFMPEG_FOUND)
    message (STATUS "Found FFmpeg")
    get_filename_component(FFMPEG_INCLUDE_DIR ${FFMPEG_AVCODEC_INCLUDE_DIR} PATH)
    set(FFMPEG_INCLUDE_DIRS
      ${FFMPEG_INCLUDE_DIR}
      ${FFMPEG_AVCODEC_INCLUDE_DIR}
      ${FFMPEG_AVFORMAT_INCLUDE_DIR}
      ${FFMPEG_SWSCALE_INCLUDE_DIR}
    )

    set(FFMPEG_LIBRARIES
      ${FFMPEG_AVCODEC_LIBRARY}
      ${FFMPEG_AVUTIL_LIBRARY}
      ${FFMPEG_AVFORMAT_LIBRARY}
      ${FFMPEG_SWSCALE_LIBRARY}
    )
  else()
    message (STATUS "Could NOT find FFmpeg")
  endif (FFMPEG_FOUND)

  # show the FFMPEG_INCLUDE_DIRS and FFMPEG_LIBRARIES variables only in the advanced view
  mark_as_advanced(FFMPEG_INCLUDE_DIRS FFMPEG_LIBRARIES)
  mark_as_advanced(FFMPEG_SWSCALE_INCLUDE_DIR  FFMPEG_SWSCALE_LIBRARY FFMPEG_AVCODEC_INCLUDE_DIR)
  mark_as_advanced(FFMPEG_AVCODEC_LIBRARY FFMPEG_AVFORMAT_INCLUDE_DIR FFMPEG_AVFORMAT_LIBRARY)
endif (FFMPEG_LIBRARIES AND FFMPEG_INCLUDE_DIRS)

