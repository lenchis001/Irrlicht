# CMakeList.txt : CMake project for Irrlicht, include source and define
# project specific logic here.
#

add_compile_definitions(_IRR_STATIC_LIB_ _IRR_WCHAR_FILESYSTEM NO_IRR_COMPILE_WITH_DIRECTINPUT_JOYSTICK_ UNICODE _CRT_SECURE_NO_WARNINGS)
include_directories("include" $ENV{BOOST_ROOT} "sources/zlib")

file(GLOB irrlicht_sources_files
     "sources/*.cpp"
     "sources/aesGladman/*.cpp"
     "sources/bzip2/*.c"
     "sources/jpeglib/*.c"
     "sources/libpng/*.c"
     "sources/lzma/*.c"
     "sources/zlib/*.c"
)

# Add source to this project's executable.
add_library(Irrlicht STATIC ${irrlicht_sources_files})

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Irrlicht PROPERTY CXX_STANDARD 17)
endif()

set(Irrlicht_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/Irrlicht/include
                                 ${Irrlicht_INCLUDE_DIR}
    CACHE INTERNAL "${PROJECT_NAME}: Include Directories" FORCE)

set(Irrlicht_DEFINITIONS ${Irrlicht_DEFINITIONS}
    CACHE INTERNAL "${PROJECT_NAME}: Definitions" FORCE)
