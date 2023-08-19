include_directories(${TUtils_INCLUDE_DIR} ${TCore_INCLUDE_DIR})

file(GLOB tam_sources_files
     "Sources/*.cpp"
     "Sources/**/*.cpp"
)

# Add source to this project's executable.
add_library(TAM STATIC ${tam_sources_files})

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET TAM PROPERTY CXX_STANDARD 17)
endif()

if(MSVC)
    if(NOT EXISTS ${CMAKE_BINARY_DIR}/packages/ogg-msvc-x86)
        find_program(NUGET_EXE NAMES ${PROJECT_SOURCE_DIR}/tools/nuget)
        exec_program(${NUGET_EXE} ARGS install "ogg-msvc-x86" -Version 1.3.2.8787 -ExcludeVersion -OutputDirectory ${CMAKE_BINARY_DIR}/packages)
        exec_program(${NUGET_EXE} ARGS install "vorbis-msvc-x86" -Version 1.3.5.8787 -ExcludeVersion -OutputDirectory ${CMAKE_BINARY_DIR}/packages)
        exec_program(${NUGET_EXE} ARGS install "OpenAL.Soft" -Version 1.19.1 -ExcludeVersion -OutputDirectory ${CMAKE_BINARY_DIR}/packages)
    endif()

    include_directories(
        ${CMAKE_BINARY_DIR}/packages/ogg-msvc-x86/build/native/include
        ${CMAKE_BINARY_DIR}/packages/vorbis-msvc-x86/build/native/include
        ${CMAKE_BINARY_DIR}/packages/OpenAL.Soft/sources
    )

    file(GLOB vorbis_libs
        ${CMAKE_BINARY_DIR}/packages/vorbis-msvc-x86/build/native/lib_release/*.lib
    )
    target_link_libraries(TAM PUBLIC
        ${CMAKE_BINARY_DIR}/packages/OpenAL.Soft/runtimes/win-x86/native/OpenAL32.lib
        ${CMAKE_BINARY_DIR}/packages/ogg-msvc-x86/build/native/lib_release/ogg.lib
        ${vorbis_libs}
    )
else()
    target_link_libraries(TAM PUBLIC openal vorbisfile)
endif()

include_directories(${external_includes})

set(TAM_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/TEngineFramework/TAM/Sources ${TAM_INCLUDE_DIR}
    CACHE INTERNAL "${PROJECT_NAME}: Include Directories" FORCE)
