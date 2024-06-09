include_directories(${TUtils_INCLUDE_DIR} "Sources/")

file(GLOB_RECURSE tnc_sources_files "*.cpp")

# Add source to this project's executable.
add_library(TNC STATIC ${tnc_sources_files})

 include_directories(${EXTERNAL_INCLUDES} ${CMAKE_BINARY_DIR}/packages/websocketpp)

if(MSVC)
    target_link_directories(TNC PUBLIC 
        ${CMAKE_BINARY_DIR}/packages/boost_random-vc143/lib/native
    )
else()
    file(GLOB boost_lib_files
        $ENV{BOOST_ROOT}/stage/lib/*.a
    )
    target_link_libraries(TNC PUBLIC ${boost_lib_files})
endif()

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET TNC PROPERTY CXX_STANDARD 17)
endif()

if(NOT EXISTS ${CMAKE_BINARY_DIR}/packages/websocketpp)
    exec_program(git ARGS clone "https://github.com/zaphoyd/websocketpp.git" ${CMAKE_BINARY_DIR}/packages/websocketpp)
endif()

set(TNC_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/TEngineFramework/TNC/Sources
                                 ${TNC_INCLUDE_DIR}
    CACHE INTERNAL "${PROJECT_NAME}: Include Directories" FORCE)