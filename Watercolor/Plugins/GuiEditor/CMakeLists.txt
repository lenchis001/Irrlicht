include_directories(
  ${Irrlicht_INCLUDE_DIR} 
  "Sources"
  ${WATERCOLOR_PLUGIN_API}
  ${EXTERNAL_INCLUDES}
)

add_compile_definitions(${Irrlicht_DEFINITIONS} ${EXTERNAL_DEFINITIONS} _UNICODE)

file(GLOB_RECURSE gui_editor_sources_files "*.cpp")

# Add source to this project's executable.
add_library(GuiEditor SHARED ${gui_editor_sources_files})

if(MSVC)
    target_link_directories(GuiEditor PUBLIC 
        ${CMAKE_BINARY_DIR}/packages/boost_filesystem-vc143/lib/native
        ${CMAKE_BINARY_DIR}/packages/boost_thread-vc143/lib/native
        ${CMAKE_BINARY_DIR}/packages/boost_chrono-vc143/lib/native
        ${CMAKE_BINARY_DIR}/packages/boost_locale-vc143/lib/native
    )
endif()

target_link_libraries(GuiEditor PUBLIC Common Irrlicht ${EXTERNAL_LIBS})

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET GuiEditor PROPERTY CXX_STANDARD 17)
endif()

if(MSVC)
    add_custom_command(TARGET GuiEditor POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_BINARY_DIR}/Watercolor/Plugins/GuiEditor/GuiEditor.dll ${CMAKE_BINARY_DIR}/Watercolor/WelcomeWindow/Plugins/V1/GuiEditor.dll)
endif()