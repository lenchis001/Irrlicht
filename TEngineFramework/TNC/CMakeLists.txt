include_directories($ENV{BOOST_ROOT} ${TUtils_INCLUDE_DIR} "Sources/")

file(GLOB tnc_sources_files
     "Sources/**/*.cpp"
)

# Add source to this project's executable.
add_library(TNC STATIC ${tnc_sources_files})

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET TNC PROPERTY CXX_STANDARD 17)
endif()
