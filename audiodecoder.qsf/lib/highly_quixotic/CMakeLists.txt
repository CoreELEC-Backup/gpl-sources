cmake_minimum_required(VERSION 3.5)
project(highly_quixotic)

set(SOURCES Core/kabuki.c
            Core/qmix.c
            Core/qsound.c
            Core/qsound_ctr.c
            Core/z80.c)

add_definitions(-DEMU_COMPILE -DEMU_LITTLE_ENDIAN)

add_library(highly_quixotic STATIC ${SOURCES})
set_property(TARGET highly_quixotic PROPERTY POSITION_INDEPENDENT_CODE ON)
