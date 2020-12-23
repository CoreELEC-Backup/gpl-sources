cmake_minimum_required(VERSION 3.5)
project(peripheral.xarcade)

# --- Add-on Dependencies ------------------------------------------------------

find_package(Kodi REQUIRED)

include_directories(${INCLUDES}
                    ${PROJECT_SOURCE_DIR}/src
                    ${KODI_INCLUDE_DIR}/.. # Hack way with "/..", need bigger Kodi cmake rework to match right include ways (becomes done in future)
)

set(XARCADE_SOURCES
  src/xarcade/XArcadeDevice.cpp
  src/xarcade/XArcadeScanner.cpp
  src/xarcade/XArcadeUtils.cpp
  src/addon.cpp
)

set(XARCADE_HEADERS
  src/utils/CommonDefines.h
  src/utils/CommonMacros.h
  src/xarcade/XArcadeDefines.h
  src/xarcade/XArcadeDevice.h
  src/xarcade/XArcadeScanner.h
  src/xarcade/XArcadeTypes.h
  src/xarcade/XArcadeUtils.h
)

# ------------------------------------------------------------------------------

build_addon(${PROJECT_NAME} XARCADE DEPLIBS)
include(CPack)
