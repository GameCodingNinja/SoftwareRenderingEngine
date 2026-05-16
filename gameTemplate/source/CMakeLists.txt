
# Add the game level executable files
if(WIN32)
    add_executable(
        gametemplate WIN32
        game/game.cpp
        game/main.cpp
        state/commonstate.cpp
        state/startupstate.cpp
        state/titlescreenstate.cpp )
else()
    add_executable(
        gametemplate
        game/game.cpp
        game/main.cpp
        state/commonstate.cpp
        state/startupstate.cpp
        state/titlescreenstate.cpp )
endif()

# List all the libraries to link against
if(WIN32)
    target_link_libraries(
        gametemplate
        library
        user32
        gdi32 )
else()
    target_link_libraries(
        gametemplate
        library
        ${X11_LIBRARIES}
        ${WAYLAND_CLIENT_LIBRARIES} )
endif()

install(TARGETS gametemplate DESTINATION ${CMAKE_BINARY_DIR})
