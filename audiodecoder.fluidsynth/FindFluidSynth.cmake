# - Try to find fluidsynth
# Once done this will define
#
# FLUIDSYNTH_FOUND - system has libfluidsynth
# FLUIDSYNTH_INCLUDE_DIRS - the libfluidsynth include directory
# FLUIDSYNTH_LIBRARIES - The libfluidsynth libraries

find_package(PkgConfig)
if(PKG_CONFIG_FOUND)
  pkg_check_modules(PC_FLUIDSYNTH fluidsynth QUIET)
endif()

find_path(FLUIDSYNTH_INCLUDE_DIRS fluidsynth.h
                                  PATHS ${PC_FLUIDSYNTH_INCLUDEDIR})
find_library(FLUIDSYNTH_LIBRARIES fluidsynth
                                  PATHS ${PC_FLUIDSYNTH_LIBDIR})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(FluidSynth REQUIRED_VARS FLUIDSYNTH_INCLUDE_DIRS FLUIDSYNTH_LIBRARIES)

mark_as_advanced(FLUIDSYNTH_INCLUDE_DIRS FLUIDSYNTH_LIBRARIES)
