file(GLOB tutils_sources_files
     "Sources/**/*.cpp"
)

# Add source to this project's executable.
add_library(TUtils STATIC ${tutils_sources_files})

include_directories(${EXTERNAL_INCLUDES})

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET TUtils PROPERTY CXX_STANDARD 17)
endif()

set(TUtils_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/TEngineFramework/TUtils/Sources
                                 ${TUtils_INCLUDE_DIR}
    CACHE INTERNAL "${PROJECT_NAME}: Include Directories" FORCE)