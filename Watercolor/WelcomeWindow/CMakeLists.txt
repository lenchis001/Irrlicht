include_directories(
  ${Irrlicht_INCLUDE_DIR} 
  ${TUtils_INCLUDE_DIR} 
  ${TCore_INCLUDE_DIR} 
  ${TAM_INCLUDE_DIR} 
  ${Common_INCLUDE_DIR}
  ${Services_INCLUDE_DIR}
  ${MainWindow_INCLUDE_DIR}
  "Sources"
  ${external_includes}
)

add_compile_definitions(${Irrlicht_DEFINITIONS} ${WX_DEFINITIONS} _UNICODE)

file(GLOB_RECURSE welcome_window_sources_files "*.cpp")

# Add source to this project's executable.
add_executable(WelcomeWindow WIN32 ${welcome_window_sources_files})

if(MSVC)
    target_link_directories(WelcomeWindow PUBLIC 
        ${CMAKE_BINARY_DIR}/packages/boost_filesystem-vc143/lib/native
        ${CMAKE_BINARY_DIR}/packages/boost_thread-vc143/lib/native
        ${CMAKE_BINARY_DIR}/packages/boost_chrono-vc143/lib/native
        ${CMAKE_BINARY_DIR}/packages/boost_locale-vc143/lib/native
        ${CMAKE_BINARY_DIR}/packages/boost_iostreams-vc143/lib/native
    )

    add_custom_command(TARGET WelcomeWindow POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_BINARY_DIR}/Irrlicht/Irrlicht.dll ${CMAKE_BINARY_DIR}/Watercolor/WelcomeWindow)

    add_custom_command(TARGET WelcomeWindow POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E rm -rf ${CMAKE_BINARY_DIR}/Watercolor/WelcomeWindow/Resources)

    add_custom_command(TARGET WelcomeWindow POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/Watercolor/MainWindow/Resources ${CMAKE_BINARY_DIR}/Watercolor/WelcomeWindow/Resources)
endif()

target_link_libraries(WelcomeWindow PUBLIC Common MainWindow Services TAM TCore TNC TUtils Irrlicht ${external_libs})

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET WelcomeWindow PROPERTY CXX_STANDARD 17)
endif()
