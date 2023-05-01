include_directories($ENV{BOOST_ROOT})

file(GLOB tam_sources_files
     "Sources/**/*.cpp"
)

# Add source to this project's executable.
add_library(TAM STATIC ${tam_sources_files})

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET TAM PROPERTY CXX_STANDARD 17)
endif()

# TODO: Add tests and install targets if needed.
