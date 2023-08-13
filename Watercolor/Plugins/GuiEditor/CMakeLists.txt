include_directories(
  ${Irrlicht_INCLUDE_DIR} 
  ${WX_INCLUDE_DIR}
  # CMAKE_CURRENT_SOURCE_DIR
  "/home/leon/Projects/Irrlicht/Watercolor/Plugins/GuiEditor/Sources"
  "/home/leon/Projects/Irrlicht/Watercolor/Services/Sources/Plugins/Api"
)

add_compile_definitions(${Irrlicht_DEFINITIONS} ${WX_DEFINITIONS} _UNICODE)

file(GLOB_RECURSE gui_editor_sources_files "*.cpp")

# Add source to this project's executable.
add_library(GuiEditor SHARED ${gui_editor_sources_files})

if(MSVC)
    include_directories(${CMAKE_BINARY_DIR}/packages/boost/lib/native/include $ENV{WX_WIDGETS}/lib/vc_lib/mswu)
else()
    include_directories($ENV{BOOST_ROOT})

    target_link_libraries(GuiEditor PUBLIC ${external_lib_files} ${WX_LIBS})
endif()

target_link_libraries(GuiEditor PUBLIC Common Irrlicht)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET GuiEditor PROPERTY CXX_STANDARD 17)
endif()