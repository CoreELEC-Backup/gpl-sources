cmake_minimum_required(VERSION 3.5)
project(vio2sf)

set(SOURCES src/vio2sf/desmume/armcpu.c
            src/vio2sf/desmume/arm_instructions.c
            src/vio2sf/desmume/barray.c
            src/vio2sf/desmume/bios.c
            src/vio2sf/desmume/cp15.c
            src/vio2sf/desmume/FIFO.c
            src/vio2sf/desmume/GPU.c
            src/vio2sf/desmume/isqrt.c
            src/vio2sf/desmume/matrix.c
            src/vio2sf/desmume/mc.c
            src/vio2sf/desmume/MMU.c
            src/vio2sf/desmume/NDSSystem.c
            src/vio2sf/desmume/resampler.c
            src/vio2sf/desmume/state.c
            src/vio2sf/desmume/thumb_instructions.c
            src/vio2sf/desmume/SPU.cpp)

set(CMAKE_POSITION_INDEPENDENT_CODE 1)
add_library(vio2sf STATIC ${SOURCES})

include(CheckCXXSymbolExists)
set(CMAKE_REQUIRED_FLAGS -msse)
check_cxx_symbol_exists(__SSE__ "" HAS_SSE)
if(HAS_SSE)
  target_compile_options(vio2sf PRIVATE -msse)
endif()
