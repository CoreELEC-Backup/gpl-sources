cmake_minimum_required(VERSION 3.5)
project(fileex)

set (SOURCES fex/fex.cpp
             fex/blargg_common.cpp
             fex/File_Extractor.cpp
             fex/blargg_errors.cpp
             fex/Data_Reader.cpp
             fex/Binary_Extractor.cpp)

include_directories(${PROJECT_SOURCE_DIR})

add_library(fileex STATIC ${SOURCES})
set_property(TARGET fileex PROPERTY POSITION_INDEPENDENT_CODE ON)
