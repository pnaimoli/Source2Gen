cmake_minimum_required(VERSION 2.8)
project(Source2Gen)

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
        message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

set(CMAKE_BUILD_TYPE Debug)
set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/bin)

file(GLOB_RECURSE Source2Gen_SOURCES "Source2Gen/*.cpp" "Source2Gen/shared/*.cpp")
file(GLOB_RECURSE Source2Gen_HEADERS "Source2Gen/*.hpp" "Source2Gen/shared/*.hpp")

LINK_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/bin)

include_directories("Source2Gen" "Source2Gen/shared")

add_definitions(-DSOURCE2_OUTPUT="${LIBRARY_OUTPUT_PATH}")

add_library(Source2Gen SHARED ${Source2Gen_HEADERS} ${Source2Gen_SOURCES})
