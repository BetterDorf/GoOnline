# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

# Name of the solution
# TODO: Rename solution
project ("GoOnline")

if ( NOT CMAKE_BUILD_TYPE )
    message( "Build Type not set, defaulting to Debug..." )
    set( CMAKE_BUILD_TYPE Debug )
endif()


include_directories(common/include/)
file(GLOB_RECURSE PROJECT_COMMON common/src/*.cpp common/include/*.h[pp])

# Include sub-projects.
# TODO: Rename project
set(EXECUTABLE_CUSTOM_NAME "GoOnlineServer")
add_subdirectory ("GoOnlineServer")

set(EXECUTABLE_CUSTOM_NAME "GoOnlineClient")
add_subdirectory ("GoOnlineClient")
