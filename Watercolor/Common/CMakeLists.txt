set(Common_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/Watercolor/Common/Sources
                                 ${Common_INCLUDE_DIR}
    CACHE INTERNAL "${PROJECT_NAME}: Include Directories" FORCE)

include_directories(
    ${Irrlicht_INCLUDE_DIR}
    ${TUtils_INCLUDE_DIR}
    ${TCore_INCLUDE_DIR}
    ${Common_INCLUDE_DIR}
    ${external_includes}
)

add_compile_definitions(${Irrlicht_DEFINITIONS} ${external_definitions})

file(GLOB_RECURSE common_sources_files "*.cpp")

# Add source to this project's executable.
add_library(Common STATIC ${common_sources_files})

target_link_libraries(Common PUBLIC TUtils Irrlicht ${external_libs})

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Common PROPERTY CXX_STANDARD 17)
endif()