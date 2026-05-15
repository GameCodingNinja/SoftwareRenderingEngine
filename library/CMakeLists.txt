
# Add the files to the library
add_library(
    library STATIC
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
        common/shaderdata.cpp
        common/matrix.cpp
        common/bitmask.cpp
        soil/stb_image_aug.c
        soil/image_helper.c
        soil/image_DXT.c )

include_directories(
    objectdata
    softwareRender
    system
    utilities
    managers
    2d
    common
    soil )
