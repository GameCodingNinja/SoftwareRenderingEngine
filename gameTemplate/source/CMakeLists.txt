
# Add the game level executable files
add_executable(
    gametemplate
    game/game.cpp
    game/main.cpp
    state/commonstate.cpp
    state/startupstate.cpp
    state/titlescreenstate.cpp )

# List all the libraries to link against
target_link_libraries(
    gametemplate
    library
    ${SDL2_LIBRARIES} )

install(TARGETS gametemplate DESTINATION ${CMAKE_BINARY_DIR})
