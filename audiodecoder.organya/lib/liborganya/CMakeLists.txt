cmake_minimum_required(VERSION 3.5)
project(orgyana)

set(SOURCES blip_buf.c
            decoder.cpp
            file.cpp)

add_library(organya STATIC ${SOURCES})
set_property(TARGET organya PROPERTY POSITION_INDEPENDENT_CODE ON)

if(CORE_SYSTEM_NAME MATCHES windows)
  target_compile_definitions(organya PRIVATE -D__inline__=__inline)
endif()
