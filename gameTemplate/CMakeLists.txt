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

cmake_minimum_required(VERSION 3.25)

project(gametemplate)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# The version number.
set(gametemplate_VERSION_MAJOR 1)
set(gametemplate_VERSION_MINOR 0)

if(UNIX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -no-pie -Wall -pthread")

    # Find packages
    find_package(PkgConfig REQUIRED)
    find_package(X11 REQUIRED)
    pkg_search_module(WAYLAND_CLIENT REQUIRED wayland-client)
    pkg_search_module(ALSA REQUIRED alsa)
endif()

# List all the include directories
include_directories(
    ${X11_INCLUDE_DIR}
    ${WAYLAND_CLIENT_INCLUDE_DIRS}
    ${ALSA_INCLUDE_DIRS}
    ../
    ../library )

# List the directories in the order they are to be built
add_subdirectory( ../library ${CMAKE_CURRENT_BINARY_DIR}/library )
add_subdirectory( source )
