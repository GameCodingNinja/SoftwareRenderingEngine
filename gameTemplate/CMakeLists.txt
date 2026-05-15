# To build a release version on Linux, from within the gameTemplate folder
# mkdir release
# cd release
# cmake -DCMAKE_BUILD_TYPE=Release ..
# make

# To build a debug version on Linux, from within the gameTemplate folder
# mkdir build
# cd build
# cmake -DCMAKE_BUILD_TYPE=Debug ..
# make

cmake_minimum_required(VERSION 3.10)

project(gametemplate)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -no-pie -std=c++17 -Wall -pthread")

# The version number.
set(gametemplate_VERSION_MAJOR 1)
set(gametemplate_VERSION_MINOR 0)

# Need to do this to find SDL2 libraries
find_package(PkgConfig REQUIRED)
pkg_search_module(SDL2 REQUIRED sdl2)

# List all the include directories
include_directories(
    ${SDL2_INCLUDE_DIRS}
    ../
    ../library )

# List the directories in the order they are to be built
add_subdirectory( ../library ${CMAKE_CURRENT_BINARY_DIR}/library )
add_subdirectory( source )
