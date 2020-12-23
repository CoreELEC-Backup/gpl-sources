cmake_minimum_required(VERSION 3.5)
project(sidplay2)

include(ExternalProject)
externalproject_add(sidplay2
                    SOURCE_DIR ${CMAKE_SOURCE_DIR}
                    UPDATE_COMMAND autoreconf -vif &&
                                   autoreconf -vif libsidplay &&
                                   autoreconf -vif resid &&
                                   autoreconf -vif builders/resid-builder &&
                                   autoreconf -vif builders/hardsid-builder &&
                                   autoreconf -vif libsidutils
                    CONFIGURE_COMMAND CXXFLAGS=-DHAVE_UNIX <SOURCE_DIR>/configure 
                      --prefix=${OUTPUT_DIR}
                      --disable-shared
                      --enable-static
                    INSTALL_COMMAND ""
                    BUILD_IN_SOURCE 1)

install(CODE "execute_process(COMMAND make install WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})")
