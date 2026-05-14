
# Add the game level executable files
add_executable(
    gametemplate
    game/game.cpp
    game/main.cpp
    smartGUI/smartconfirmbtn.cpp
    smartGUI/smartresolutionbtn.cpp
    smartGUI/smartsettingsmenubtn.cpp
    smartGUI/smartapplysettingsbtn.cpp
    smartGUI/smartfullscreencheckbox.cpp
    smartGUI/smartvsynccheckbox.cpp
    smartGUI/smartdeadzoneslider.cpp
    state/commonstate.cpp
    state/startupstate.cpp
    state/titlescreenstate.cpp
    state/loadstate.cpp
    state/runstate.cpp )

# List all the libraries to link against
target_link_libraries(
    gametemplate
    ${OPENGL_LIBRARIES}
    ${GLEW_LIBRARY}
    ${SDL2_LIBRARIES}
    ${SDL2MIXER_LIBRARIES}
    ${Boost_LIBRARIES}
    ${CMAKE_BINARY_DIR}/angelscript/libangelscript.a
    ${CMAKE_BINARY_DIR}/Box2D/libBox2D.a
    ${CMAKE_BINARY_DIR}/library/liblibrary.a )

install(TARGETS gametemplate DESTINATION ${CMAKE_BINARY_DIR})
