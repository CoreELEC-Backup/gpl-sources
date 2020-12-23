# - Try to find MPGHIP
# Once done this will define
#
# MPGHIP_FOUND - system has libmpghip
# MPGHIP_LIBRARIES - The limpghip libraries

find_library(MPGHIP_LIBRARIES mpghip)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(MPGHIP DEFAULT_MSG MPGHIP_LIBRARIES)

mark_as_advanced(MPGHIP_LIBRARIES)
