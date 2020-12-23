cmake_minimum_required(VERSION 3.5)
project(libupse)

set(SOURCES upse_bios_manager.c
            upse_container_xsf.c
            upse_eventloop.c
            upse_filesystem.c
            upse_loader.c
            upse_loader_psf1.c
            upse_loader_psf2.c
            upse_module.c
            upse_ps1_bios_base.c
            upse_ps1_counters.c
            upse_ps1_dma_manager.c
            upse_ps1_executive.c
            upse_ps1_gpu.c
            upse_ps1_hal.c
            upse_ps1_memory_manager.c
            upse_ps1_spu_base.c
            upse_ps1_spu_dma_handler.c
            upse_ps1_spu_lowpass_filter.c
            upse_ps1_spu_nyquist_filter.c
            upse_ps1_spu_register_io.c
            upse_ps2_iop_base.c
            upse_r3000_abstract.c
            upse_r3000_bytecode_evaluator.c
            upse_string.c
            upse_util.c
            Neill/spu.c
            Neill/spucore.c)

add_definitions(-DEMU_COMPILE -DEMU_LITTLE_ENDIAN)
if(WIN32)
  add_definitions(-D_CRT_NONSTDC_NO_DEPRECATE -Dstrcasecmp=_stricmp)
endif()

set(CMAKE_POSITION_INDEPENDENT_CODE 1)
add_library(upse STATIC ${SOURCES})
