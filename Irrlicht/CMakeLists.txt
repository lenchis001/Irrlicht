# CMakeList.txt : CMake project for Irrlicht, include source and define
# project specific logic here.
#

set(Irrlicht_DEFINITIONS _IRR_WCHAR_FILESYSTEM NO_IRR_COMPILE_WITH_DIRECTINPUT_JOYSTICK_ UNICODE _CRT_SECURE_NO_WARNINGS _IRR_DONT_DO_MEMORY_DEBUGGING_HERE
    CACHE INTERNAL "${PROJECT_NAME}: Definitions" FORCE)
add_compile_definitions(${Irrlicht_DEFINITIONS} IRRLICHT_EXPORTS)
include_directories("include" "sources/zlib" ${EXTERNAL_INCLUDES})

if(APPLE)
  file(GLOB_RECURSE irrlicht_sources_files
      "sources/*.cpp"
      "sources/*.c"
      "sources/MacOSX/*.m"
      "sources/MacOSX/*.mm"
  )
else()
  file(GLOB_RECURSE irrlicht_sources_files
      "sources/*.cpp"
      "sources/*.c"
  )
endif()

add_library(Irrlicht SHARED ${irrlicht_sources_files})


if(${CMAKE_HOST_SYSTEM_PROCESSOR} STREQUAL "aarch64" OR ${CMAKE_HOST_SYSTEM_PROCESSOR} STREQUAL "arm64")
  add_compile_definitions(PNG_ARM_NEON_OPT=0)
endif()

if(LINUX)
  target_link_libraries(Irrlicht PUBLIC glut GL GLU X11 Xxf86vm)
endif()

if(APPLE)
  include_directories("${HOMEBREW_PATH}/glfw/include")
  target_link_libraries(Irrlicht stdc++ "-framework Foundation -framework OpenGL -framework AppKit ${HOMEBREW_PATH}/glfw/lib/libglfw.dylib")
endif()

set(Irrlicht_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/Irrlicht/include
                         ${Irrlicht_INCLUDE_DIR}
                         CACHE INTERNAL "${PROJECT_NAME}: Include Directories" FORCE)
