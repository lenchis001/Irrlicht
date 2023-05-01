include_directories($ENV{BOOST_ROOT} ${Irrlicht_INCLUDE_DIR})

file(GLOB tlauncher_sources_files
     "Sources/*.cpp"
)

# Add source to this project's executable.
add_executable(TLauncher ${tlauncher_sources_files})

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET TLauncher PROPERTY CXX_STANDARD 17)
endif()
