
# Platform-independent source files
set(LIBRARY_SOURCES
    objectdata/objectdata2d.cpp
    objectdata/objectdatamanager2d.cpp
    objectdata/objectphysicsdata2d.cpp
    objectdata/objectvisualdata2d.cpp
    softwareRender/softwareRender.cpp
    softwareRender/srtexture.cpp
    softwareRender/triangleslope.cpp
    system/worldcamera.cpp
    system/basegame.cpp
    system/device.cpp
    system/eventqueue.cpp
    system/windowfactory.cpp
    utilities/xmlparsehelper.cpp
    utilities/statcounter.cpp
    utilities/genfunc.cpp
    utilities/settings.cpp
    utilities/highresolutiontimer.cpp
    utilities/timer.cpp
    utilities/xmlParser.cpp
    utilities/mathfunc.cpp
    utilities/threadpool.cpp
    managers/texturemanager.cpp
    managers/managerbase.cpp
    managers/vertexbuffermanager.cpp
    2d/sprite2d.cpp
    2d/visualcomponent2d.cpp
    common/object.cpp
    common/worldvalue.cpp
    common/matrix.cpp
    common/bitmask.cpp
    soil/stb_image_aug.c
    soil/image_helper.c
    soil/image_DXT.c )

# Linux-only source files
if(UNIX)
    list(APPEND LIBRARY_SOURCES
        system/x11framebuffer.cpp
        system/x11window.cpp
        system/waylandframebuffer.cpp
        system/waylandwindow.cpp
        system/xdg-shell-protocol.c
        system/xdg-decoration-protocol.c )
endif()

# Windows-only source files
if(WIN32)
    list(APPEND LIBRARY_SOURCES
        system/windowsframebuffer.cpp
        system/windowswindow.cpp )
endif()

# Add the files to the library
add_library(library STATIC ${LIBRARY_SOURCES})

include_directories(
    objectdata
    softwareRender
    system
    utilities
    managers
    2d
    common
    soil )
