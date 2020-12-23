# - Try to find resid
# Once done this will define
#
# RESID_FOUND - system has libresid-builder
# RESID_INCLUDE_DIRS - the libresid-builder include directory
# RESID_LIBRARIES - The libresid-builder libraries

find_path(RESID_INCLUDE_DIRS sidplay/builders/resid.h)
find_library(RESID_LIBRARIES resid-builder PATH_SUFFIXES sidplay/builders)
if(WIN32)
  find_library(RESID_LIBRARY resid)
  if(RESID_LIBRARY)
    list(APPEND RESID_LIBRARIES ${RESID_LIBRARY})
  endif()
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Resid DEFAULT_MSG RESID_INCLUDE_DIRS RESID_LIBRARIES)

mark_as_advanced(RESID_INCLUDE_DIRS RESID_LIBRARIES)
