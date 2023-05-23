include_directories(
  ${Irrlicht_INCLUDE_DIR} 
  ${TUtils_INCLUDE_DIR} 
  ${TCore_INCLUDE_DIR} 
  ${TAM_INCLUDE_DIR} 
  ${Common_INCLUDE_DIR}
  ${Services_INCLUDE_DIR}
  $ENV{WX_WIDGETS}/include 
  $ENV{WX_WIDGETS}/lib/vc_lib/mswu 
  ${MainWindow_INCLUDE_DIR}
)

add_compile_definitions(${Irrlicht_DEFINITIONS} _UNICODE)

file(GLOB_RECURSE main_window_sources_files "*.cpp")

# Add source to this project's executable.
add_library(MainWindow STATIC ${main_window_sources_files})

if(MSVC)
    include_directories(${CMAKE_BINARY_DIR}/packages/boost/lib/native/include)
else()
    include_directories($ENV{BOOST_ROOT})
endif()

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET MainWindow PROPERTY CXX_STANDARD 17)
endif()

set(MainWindow_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/Watercolor/MainWindow/Sources
                                 ${MainWindow_INCLUDE_DIR}
    CACHE INTERNAL "${PROJECT_NAME}: Include Directories" FORCE)