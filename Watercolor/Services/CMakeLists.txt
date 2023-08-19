include_directories(
  ${TUtils_INCLUDE_DIR} 
  ${TCore_INCLUDE_DIR} 
  ${Common_INCLUDE_DIR}
  ${Services_INCLUDE_DIR}
  ${external_includes}
)

add_compile_definitions(${Irrlicht_DEFINITIONS} ${WX_DEFINITIONS})

file(GLOB_RECURSE services_sources_files "*.cpp")

# Add source to this project's executable.
add_library(Services STATIC ${services_sources_files})

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Services PROPERTY CXX_STANDARD 17)
endif()

set(Services_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/Watercolor/Services/Sources
                                 ${Services_INCLUDE_DIR}
    CACHE INTERNAL "${PROJECT_NAME}: Include Directories" FORCE)