# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

project ("TEngine")

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_COLOR_MAKEFILE   ON)

# Include sub-projects.
add_subdirectory ("Irrlicht")
add_subdirectory ("TEngineFramework/TUtils")
add_subdirectory ("TEngineFramework/TAM")
add_subdirectory ("TEngineFramework/TCore")
add_subdirectory ("TEngineFramework/TNC")

add_subdirectory ("TLauncher")
