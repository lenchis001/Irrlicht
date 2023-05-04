include_directories($ENV{BOOST_ROOT} ${TUtils_INCLUDE_DIR})

file(GLOB tcore_sources_files
     "Sources/*.cpp"
     "Sources/**/*.cpp"
)

# Add source to this project's executable.
add_library(TCore STATIC ${tcore_sources_files})

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET TCore PROPERTY CXX_STANDARD 17)
endif()

set(TCore_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/TEngineFramework/TCore/Sources
                                 ${TCore_INCLUDE_DIR}
    CACHE INTERNAL "${PROJECT_NAME}: Include Directories" FORCE)