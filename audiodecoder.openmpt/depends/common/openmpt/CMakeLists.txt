cmake_minimum_required(VERSION 3.5)
project(openmpt)

include(ExternalProject)
externalproject_add(openmpt
                    SOURCE_DIR ${CMAKE_SOURCE_DIR}
                    UPDATE_COMMAND autoreconf -vif
                    CONFIGURE_COMMAND <SOURCE_DIR>/configure
                      --prefix=${OUTPUT_DIR}
                      --without-mpg123
                      --without-portaudio
                      --without-pulseaudio
                      --without-portaudiocpp
                      --without-sndfile
                      --without-flac
                      --without-ogg
                      --without-vorbis
                      --without-vorbisfile
                      --disable-shared
                      --with-pic
                      --disable-openmpt123
                      --disable-examples
                    INSTALL_COMMAND ""
                    BUILD_IN_SOURCE 1)

install(CODE "execute_process(COMMAND make install WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})")
