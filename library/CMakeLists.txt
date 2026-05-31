
# Platform-independent source files
set(LIBRARY_SOURCES
    objectdata/objectdata2d.cpp
    objectdata/objectdata3d.cpp
    objectdata/objectdatamanager.cpp
    objectdata/objectphysicsdata2d.cpp
    objectdata/objectvisualdata2d.cpp
    objectdata/objectvisualdata3d.cpp
    softwareRender/softwareRender.cpp
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
    utilities/objmeshloader.cpp
    managers/texturemanager.cpp
    managers/managerbase.cpp
    managers/vertexbuffermanager.cpp
    managers/cameramanager.cpp
    managers/shadermanager.cpp
    managers/lightmanager.cpp
    sprite/sprite.cpp
    2d/visualcomponent2d.cpp
    3d/visualcomponent3d.cpp
    common/object.cpp
    common/matrix.cpp
    common/camera.cpp
    common/bitmask.cpp
    soil/stb_image_aug.c
    soil/image_helper.c
    soil/image_DXT.c
    sound/mixengine.cpp
    sound/wavcodec.cpp
    sound/audiodevicefactory.cpp )

# Linux-only source files
if(UNIX)
    list(APPEND LIBRARY_SOURCES
        system/x11framebuffer.cpp
        system/x11window.cpp
        system/waylandframebuffer.cpp
        system/waylandwindow.cpp
        system/xdg-shell-protocol.c
        system/xdg-decoration-protocol.c
        sound/alsaaudiodevice.cpp )
endif()

# Windows-only source files
if(WIN32)
    list(APPEND LIBRARY_SOURCES
        system/windowsframebuffer.cpp
        system/windowswindow.cpp
        sound/wasapiaudiodevice.cpp )
endif()

# Add the files to the library
add_library(library STATIC ${LIBRARY_SOURCES})

# Suppress -Wclass-memaccess for third-party xmlParser.cpp (GCC/Clang only)
if(NOT MSVC)
    set_source_files_properties(utilities/xmlParser.cpp PROPERTIES COMPILE_FLAGS "-Wno-class-memaccess")
endif()

include_directories(
    objectdata
    softwareRender
    system
    utilities
    managers
    sprite
    2d
    3d
    common
    soil
    sound )
