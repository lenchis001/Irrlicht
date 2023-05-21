# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

project ("TEngine")

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_COLOR_MAKEFILE   ON)

if(MSVC)
    find_program(NUGET_EXE NAMES ${PROJECT_SOURCE_DIR}/tools/nuget)
    exec_program(${NUGET_EXE} ARGS install "boost" -Version 1.78.0 -ExcludeVersion -OutputDirectory ${CMAKE_BINARY_DIR}/packages)
    exec_program(${NUGET_EXE} ARGS install "boost_filesystem-vc143" -Version 1.78.0 -ExcludeVersion -OutputDirectory ${CMAKE_BINARY_DIR}/packages)
    exec_program(${NUGET_EXE} ARGS install "boost_thread-vc143" -Version 1.78.0 -ExcludeVersion -OutputDirectory ${CMAKE_BINARY_DIR}/packages)
    exec_program(${NUGET_EXE} ARGS install "boost_chrono-vc143" -Version 1.78.0 -ExcludeVersion -OutputDirectory ${CMAKE_BINARY_DIR}/packages)
    exec_program(${NUGET_EXE} ARGS install "boost_locale-vc143" -Version 1.78.0 -ExcludeVersion -OutputDirectory ${CMAKE_BINARY_DIR}/packages)
endif()

# Include sub-projects.
add_subdirectory ("Irrlicht")
add_subdirectory ("TEngineFramework/TUtils")
add_subdirectory ("TEngineFramework/TCore")
add_subdirectory ("TEngineFramework/TAM")
add_subdirectory ("TEngineFramework/TNC")

add_subdirectory ("TLauncher")

if(MSVC)
    #configure_file(${CMAKE_BINARY_DIR}/packages/OpenAL.Soft/runtimes/win-x86/native/OpenAL32.dll ${CMAKE_BINARY_DIR}/TLauncher COPYONLY)
endif()
