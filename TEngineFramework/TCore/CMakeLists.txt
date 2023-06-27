include_directories(${TUtils_INCLUDE_DIR} ${PROJECT_SOURCE_DIR}/TGameApi)

file(GLOB tcore_sources_files
     "Sources/*.cpp"
     "Sources/**/*.cpp"
)

# Add source to this project's executable.
add_library(TCore STATIC ${tcore_sources_files})

if(MSVC)
    include_directories(${CMAKE_BINARY_DIR}/packages/boost/lib/native/include)
else()
    include_directories($ENV{BOOST_ROOT})
endif()

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET TCore PROPERTY CXX_STANDARD 17)
endif()

set(TCore_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/TEngineFramework/TCore/Sources
                                 ${TCore_INCLUDE_DIR}
                                 ${PROJECT_SOURCE_DIR}/TGameApi
    CACHE INTERNAL "${PROJECT_NAME}: Include Directories" FORCE)