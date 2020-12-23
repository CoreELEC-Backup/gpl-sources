cmake_minimum_required(VERSION 3.5)
project(lazyusf)

set(SOURCES audio.c
            cpu.c
            dma.c
            exception.c
            interpreter_cpu.c
            interpreter_ops.c
            main.c
            memory.c
            pif.c
            registers.c
            tlb.c
            usf.c
            rsp/rsp.c
            rsp_hle/alist_audio.c
            rsp_hle/alist.c
            rsp_hle/alist_naudio.c
            rsp_hle/alist_nead.c
            rsp_hle/audio.c
            rsp_hle/cicx105.c
            rsp_hle/hle.c
            rsp_hle/jpeg.c
            rsp_hle/memory.c
            rsp_hle/mp3.c
            rsp_hle/musyx.c
            rsp_hle/plugin.c)

set(CMAKE_POSITION_INDEPENDENT_CODE 1)
add_library(lazyusf STATIC ${SOURCES})

include(CheckCXXSymbolExists)
set(CMAKE_REQUIRED_FLAGS -msse2)
check_cxx_symbol_exists(__SSE2__ "" HAS_SSE2)
if(HAS_SSE2)
  target_compile_definitions(lazyusf PUBLIC -DARCH_MIN_SSE2)
  target_compile_options(lazyusf PRIVATE -msse2)
elseif(NOT WIN32)
  target_compile_definitions(lazyusf PUBLIC -DARCH_MIN_ARM_NEON)
endif()
