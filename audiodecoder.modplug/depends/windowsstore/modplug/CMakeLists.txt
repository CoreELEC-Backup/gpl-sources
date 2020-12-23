cmake_minimum_required(VERSION 3.5)
project(modplug)

add_definitions(-D_CRT_NONSTDC_NO_DEPRECATE -D_CRT_SECURE_NO_WARNINGS -DMSC_VER=_MSC_VER -DMODPLUG_STATIC)

set(MODPLUG_SOURCES src/fastmix.cpp
                    src/load_669.cpp
                    src/load_abc.cpp
                    src/load_amf.cpp
                    src/load_ams.cpp
                    src/load_dbm.cpp
                    src/load_dmf.cpp
                    src/load_dsm.cpp
                    src/load_far.cpp
                    src/load_it.cpp
                    src/load_j2b.cpp
                    src/load_mdl.cpp
                    src/load_med.cpp
                    src/load_mid.cpp
                    src/load_mod.cpp
                    src/load_mt2.cpp
                    src/load_mtm.cpp
                    src/load_okt.cpp
                    src/load_pat.cpp
                    src/load_psm.cpp
                    src/load_ptm.cpp
                    src/load_s3m.cpp
                    src/load_stm.cpp
                    src/load_ult.cpp
                    src/load_umx.cpp
                    src/load_wav.cpp
                    src/load_xm.cpp
                    src/mmcmp.cpp
                    src/modplug.cpp
                    src/snd_dsp.cpp
                    src/sndfile.cpp
                    src/snd_flt.cpp
                    src/snd_fx.cpp
                    src/sndmix.cpp)

include_directories(${PROJECT_SOURCE_DIR}/src/libmodplug)

add_library(modplug ${MODPLUG_SOURCES})
set(HEADERS ${PROJECT_SOURCE_DIR}/src/modplug.h
            ${PROJECT_SOURCE_DIR}/src/libmodplug/it_defs.h
            ${PROJECT_SOURCE_DIR}/src/libmodplug/sndfile.h
            ${PROJECT_SOURCE_DIR}/src/libmodplug/stdafx.h)

install(FILES ${HEADERS} DESTINATION ${CMAKE_INSTALL_PREFIX}/include/libmodplug)
install(TARGETS modplug DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
