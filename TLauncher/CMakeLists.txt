add_compile_definitions(${Irrlicht_DEFINITIONS})
include_directories($ENV{BOOST_ROOT} ${Irrlicht_INCLUDE_DIR} ${TCore_INCLUDE_DIR} ${TNC_INCLUDE_DIR} ${TAM_INCLUDE_DIR} ${TUtils_INCLUDE_DIR})

file(GLOB tlauncher_sources_files
     "Sources/*.cpp"
)

add_executable(TLauncher ${tlauncher_sources_files})

target_link_libraries(TLauncher PUBLIC TAM)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET TLauncher PROPERTY CXX_STANDARD 17)
endif()
