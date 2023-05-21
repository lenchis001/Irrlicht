# CMakeList.txt : CMake project for Irrlicht, include source and define
# project specific logic here.
#

set(Irrlicht_DEFINITIONS _IRR_STATIC_LIB_ _IRR_WCHAR_FILESYSTEM NO_IRR_COMPILE_WITH_DIRECTINPUT_JOYSTICK_ UNICODE _CRT_SECURE_NO_WARNINGS
    CACHE INTERNAL "${PROJECT_NAME}: Definitions" FORCE)
add_compile_definitions(${Irrlicht_DEFINITIONS})
include_directories("include" "sources/zlib")

file(GLOB irrlicht_sources_files
     "sources/*.cpp"
     "sources/aesGladman/*.cpp"
     "sources/bzip2/*.c"
     "sources/jpeglib/*.c"
     "sources/libpng/*.c"
     "sources/lzma/*.c"
     "sources/zlib/*.c"
)
add_library(Irrlicht STATIC ${irrlicht_sources_files})

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Irrlicht PROPERTY CXX_STANDARD 17)
endif()

if(MSVC)
    include_directories(${CMAKE_BINARY_DIR}/packages/boost/lib/native/include)
else()
  include_directories($ENV{BOOST_ROOT})
  target_link_libraries(Irrlicht PUBLIC glut GL GLU X11 Xxf86vm)
endif()

set(Irrlicht_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/Irrlicht/include
                                 ${Irrlicht_INCLUDE_DIR}
    CACHE INTERNAL "${PROJECT_NAME}: Include Directories" FORCE)
