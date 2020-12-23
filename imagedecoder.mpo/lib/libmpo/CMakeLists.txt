cmake_minimum_required(VERSION 3.5)

IF(BIICODE)
  ADD_BIICODE_TARGETS()
  return()
ENDIF()

if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

project(libmpo C)

message(status ${CMAKE_INCLUDE_PATH})

set(LIBRARY_OUTPUT_PATH lib/${CMAKE_BUILD_TYPE})
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -Wall")
if(NOT WIN32)
  set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -Wextra")
endif()



if(PROJECT_BINARY_DIR STREQUAL PROJECT_SOURCE_DIR)
   message(FATAL_ERROR "In-tree build attempt detected, aborting. Set your build dir outside your source dir, delete CMakeCache.txt from source root and try again.")
endif()


include_directories(include)

#define those variables if your libjpeg isn't in your compiler default paths
include_directories(${libjpeg_include_dir})
link_directories(${libjpeg_lib_dir})

IF(WIN32)
   SET(CMAKE_FIND_LIBRARY_SUFFIXES .lib .dll)
ELSE()
   SET(CMAKE_FIND_LIBRARY_SUFFIXES .a)
ENDIF()


file(
        GLOB_RECURSE
        source_files
        src/*
        include/*
)

add_library(mpo_obj OBJECT ${source_files})
#add_library(mpo SHARED $<TARGET_OBJECTS:mpo_obj>)
add_library(mpo_static STATIC $<TARGET_OBJECTS:mpo_obj>)
# target_link_libraries(mpo jpeg)
set_target_properties(mpo_obj PROPERTIES POSITION_INDEPENDENT_CODE ON)
# set_target_properties(mpo PROPERTIES POSITION_INDEPENDENT_CODE ON)
set_target_properties(mpo_static PROPERTIES POSITION_INDEPENDENT_CODE ON)
add_subdirectory(examples)

#INSTALL(TARGETS mpo_static mpo
#  LIBRARY DESTINATION lib
#  ARCHIVE DESTINATION lib
#)
