add_compile_definitions(${Irrlicht_DEFINITIONS})
include_directories(${Irrlicht_INCLUDE_DIR} ${TCore_INCLUDE_DIR} ${TNC_INCLUDE_DIR} ${TAM_INCLUDE_DIR} ${TUtils_INCLUDE_DIR} ${external_includes})

file(GLOB tlauncher_sources_files
     "Sources/*.cpp"
)

add_executable(TLauncher ${tlauncher_sources_files})

if(MSVC)
    target_link_directories(TLauncher PUBLIC 
        ${CMAKE_BINARY_DIR}/packages/boost_filesystem-vc143/lib/native
        ${CMAKE_BINARY_DIR}/packages/boost_thread-vc143/lib/native
        ${CMAKE_BINARY_DIR}/packages/boost_chrono-vc143/lib/native
        ${CMAKE_BINARY_DIR}/packages/boost_locale-vc143/lib/native
    )

    if(${IS_DEBUG_BUILD})
        add_custom_command(TARGET TLauncher POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_BINARY_DIR}/Irrlicht/Irrlicht.dll ${CMAKE_BINARY_DIR}/TLauncher)
    endif()
else()
    file(GLOB boost_lib_files
        $ENV{BOOST_ROOT}/stage/lib/*.a
    )
    target_link_libraries(TLauncher PUBLIC ${boost_lib_files})
endif()

target_link_libraries(TLauncher PUBLIC TAM TCore TNC TUtils Irrlicht)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET TLauncher PROPERTY CXX_STANDARD 17)
endif()
