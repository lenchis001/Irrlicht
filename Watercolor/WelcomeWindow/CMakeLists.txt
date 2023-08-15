include_directories(
  ${Irrlicht_INCLUDE_DIR} 
  ${TUtils_INCLUDE_DIR} 
  ${TCore_INCLUDE_DIR} 
  ${TAM_INCLUDE_DIR} 
  ${Common_INCLUDE_DIR}
  ${Services_INCLUDE_DIR}
  ${MainWindow_INCLUDE_DIR}
  ${PROJECT_SOURCE_DIR}/Watercolor/WelcomeWindow/Sources
  ${WX_INCLUDE_DIR}
)

add_compile_definitions(${Irrlicht_DEFINITIONS} ${WX_DEFINITIONS} _UNICODE)

file(GLOB_RECURSE welcome_window_sources_files "*.cpp")

# Add source to this project's executable.
add_executable(WelcomeWindow WIN32 ${welcome_window_sources_files})

if(MSVC)
    include_directories(${CMAKE_BINARY_DIR}/packages/boost/lib/native/include $ENV{WX_WIDGETS}/lib/vc_lib/mswu)
    target_link_directories(WelcomeWindow PUBLIC 
        ${CMAKE_BINARY_DIR}/packages/boost_filesystem-vc143/lib/native
        ${CMAKE_BINARY_DIR}/packages/boost_thread-vc143/lib/native
        ${CMAKE_BINARY_DIR}/packages/boost_chrono-vc143/lib/native
        ${CMAKE_BINARY_DIR}/packages/boost_locale-vc143/lib/native
        ${CMAKE_BINARY_DIR}/packages/boost_iostreams-vc143/lib/native
    )

    target_link_libraries(WelcomeWindow PUBLIC 
      kernel32.lib
      user32.lib
      gdi32.lib
      comdlg32.lib
      winspool.lib
      winmm.lib
      shell32.lib
      shlwapi.lib
      comctl32.lib
      ole32.lib
      oleaut32.lib
      uuid.lib
      rpcrt4.lib
      advapi32.lib
      version.lib
      wsock32.lib
      wininet.lib
      opengl32.lib
    )

    # Indication to the code that this is a debug build
    if (${IS_DEBUG_BUILD})
    file(GLOB_RECURSE welcome_window_libs "$ENV{WX_WIDGETS}/lib/vc_lib/*[!i]d.lib" "$ENV{WX_WIDGETS}/lib/vc_lib/*ud*.lib")

    add_custom_command(TARGET WelcomeWindow POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_BINARY_DIR}/Irrlicht/Irrlicht.dll ${CMAKE_BINARY_DIR}/Watercolor/WelcomeWindow)

    add_custom_command(TARGET WelcomeWindow POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E rm -rf ${CMAKE_BINARY_DIR}/Watercolor/WelcomeWindow/Resources)

    add_custom_command(TARGET WelcomeWindow POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/Watercolor/MainWindow/Resources ${CMAKE_BINARY_DIR}/Watercolor/WelcomeWindow/Resources)

    else()
    file(GLOB_RECURSE welcome_window_libs "$ENV{WX_WIDGETS}/lib/vc_lib/*u_*.lib" "$ENV{WX_WIDGETS}/lib/vc_lib/*u.lib" "$ENV{WX_WIDGETS}/lib/vc_lib/*[gb].lib")
    endif ()

    target_link_libraries(WelcomeWindow PUBLIC ${welcome_window_libs})

else()
    include_directories($ENV{BOOST_ROOT})

    target_link_libraries(WelcomeWindow PUBLIC 
      ${external_lib_files}
      ${WX_LIBS}
    )
endif()

target_link_libraries(WelcomeWindow PUBLIC Common MainWindow Services TAM TCore TNC TUtils Irrlicht)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET WelcomeWindow PROPERTY CXX_STANDARD 17)
endif()
