# - Try to find sidplay2 
# Once done this will define
#
# SIDPLAY2_FOUND - system has libsidplay2
# SIDPLAY2_INCLUDE_DIRS - the libsidplay2 include directory
# SIDPLAY2_LIBRARIES - The libsidplay2 libraries

find_package(PkgConfig)
if(PKG_CONFIG_FOUND)
  pkg_check_modules(PC_SIDPLAY2 libsidplay2 QUIET)
endif()

find_path(SIDPLAY2_INCLUDE_DIRS sidplay/sidplay2.h
                                PATHS ${PC_SIDPLAY2_INCLUDEDIR})
find_library(SIDPLAY2_LIBRARIES sidplay2
                                PATHS ${PC_SIDPLAY2_LIBDIR})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Sidplay2 REQUIRED_VARS SIDPLAY2_INCLUDE_DIRS SIDPLAY2_LIBRARIES)

mark_as_advanced(SIDPLAY2_INCLUDE_DIRS SIDPLAY2_LIBRARIES)
