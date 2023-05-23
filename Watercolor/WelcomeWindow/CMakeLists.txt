include_directories(
  ${Irrlicht_INCLUDE_DIR} 
  ${TUtils_INCLUDE_DIR} 
  ${TCore_INCLUDE_DIR} 
  ${TAM_INCLUDE_DIR} 
  ${Common_INCLUDE_DIR}
  ${Services_INCLUDE_DIR}
  ${MainWindow_INCLUDE_DIR}
  $ENV{WX_WIDGETS}/include 
  $ENV{WX_WIDGETS}/lib/vc_lib/mswu 
  ${PROJECT_SOURCE_DIR}/Watercolor/WelcomeWindow/Sources
)

add_compile_definitions(${Irrlicht_DEFINITIONS} _UNICODE)

file(GLOB_RECURSE welcome_window_sources_files "*.cpp")

# Add source to this project's executable.
add_executable(WelcomeWindow ${welcome_window_sources_files})

if(MSVC)
    include_directories(${CMAKE_BINARY_DIR}/packages/boost/lib/native/include)
    target_link_directories(WelcomeWindow PUBLIC 
        ${CMAKE_BINARY_DIR}/packages/boost_filesystem-vc143/lib/native
        ${CMAKE_BINARY_DIR}/packages/boost_thread-vc143/lib/native
        ${CMAKE_BINARY_DIR}/packages/boost_chrono-vc143/lib/native
        ${CMAKE_BINARY_DIR}/packages/boost_locale-vc143/lib/native
        ${CMAKE_BINARY_DIR}/packages/boost_iostreams-vc143/lib/native
        $ENV{WX_WIDGETS}/lib/vc_lib
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
    )

    set(IS_DEBUG_BUILD CMAKE_BUILD_TYPE STREQUAL "Debug")

    # Indication to the code that this is a debug build
    if (${IS_DEBUG_BUILD})
    target_link_libraries(WelcomeWindow PUBLIC 
    wxmsw32ud_propgrid.lib
    wxmsw32ud_adv.lib
    wxmsw32ud_core.lib
    wxbase32ud.lib
    wxtiffd.lib
    wxjpegd.lib
    wxpngd.lib
    wxzlibd.lib
    wxregexud.lib
    wxexpatd.lib
    wxmsw32ud_stc.lib
    wxscintillad.lib
    wxmsw32ud_richtext.lib
    wxmsw32ud_html.lib
    wxbase32ud_xml.lib
    wxmsw32ud_media.lib
    )
    endif ()

else()
    include_directories($ENV{BOOST_ROOT})

    file(GLOB boost_lib_files
        $ENV{BOOST_ROOT}/stage/lib/*.a
    )
    target_link_libraries(WelcomeWindow PUBLIC ${boost_lib_files})
endif()

target_link_libraries(WelcomeWindow PUBLIC Common MainWindow Services TAM TCore TNC TUtils Irrlicht)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET WelcomeWindow PROPERTY CXX_STANDARD 17)
endif()
