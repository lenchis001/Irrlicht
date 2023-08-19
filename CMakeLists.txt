# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

project ("TEngine")

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_COLOR_MAKEFILE   ON)

if(UNIX)
set(GCC_COVERAGE_COMPILE_FLAGS "-pthread")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}" )
SET(CMAKE_CXX_FLAGS -pthread)
else()
SET(CMAKE_CXX_FLAGS           "${CMAKE_CXX_FLAGS} /D \"SAFESEH:NO\"")
SET(CMAKE_EXE_LINKER_FLAGS    "${CMAKE_EXE_LINKER_FLAGS} /SAFESEH:NO")
SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /SAFESEH:NO")
SET(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} /SAFESEH:NO")
endif()

set(IS_DEBUG_BUILD CMAKE_BUILD_TYPE STREQUAL "Debug")

if(MSVC)
    #Get external dependencies
    if(NOT EXISTS ${CMAKE_BINARY_DIR}/packages/boost)
        find_program(NUGET_EXE NAMES ${PROJECT_SOURCE_DIR}/tools/nuget)
        exec_program(${NUGET_EXE} ARGS install "boost" -Version 1.82.0 -ExcludeVersion -OutputDirectory ${CMAKE_BINARY_DIR}/packages)
        exec_program(${NUGET_EXE} ARGS install "boost_filesystem-vc143" -Version 1.82.0 -ExcludeVersion -OutputDirectory ${CMAKE_BINARY_DIR}/packages)
        exec_program(${NUGET_EXE} ARGS install "boost_thread-vc143" -Version 1.82.0 -ExcludeVersion -OutputDirectory ${CMAKE_BINARY_DIR}/packages)
        exec_program(${NUGET_EXE} ARGS install "boost_chrono-vc143" -Version 1.82.0 -ExcludeVersion -OutputDirectory ${CMAKE_BINARY_DIR}/packages)
        exec_program(${NUGET_EXE} ARGS install "boost_locale-vc143" -Version 1.82.0 -ExcludeVersion -OutputDirectory ${CMAKE_BINARY_DIR}/packages)
    endif()

    #Setup external libraries defenitions
    set(WX_DEFINITIONS __WXMSW__ CACHE INTERNAL "${PROJECT_NAME}: Definitions" FORCE)

    #Setup external libraries includes
    if(${IS_DEBUG_BUILD})
        set(WX_BUILD_TYPE_INCLUDE $ENV{WX_WIDGETS}/lib/vc_lib/mswud)
    else()
        set(WX_BUILD_TYPE_INCLUDE $ENV{WX_WIDGETS}/lib/vc_lib/mswu)
    endif()

    #Setup external libraries references
    set(external_includes $ENV{WX_WIDGETS}/include
                          ${WX_BUILD_TYPE_INCLUDE}
                          ${CMAKE_BINARY_DIR}/packages/boost/lib/native/include
    CACHE INTERNAL "${PROJECT_NAME}: Include Directories" FORCE)


    if (${IS_DEBUG_BUILD})
    file(GLOB_RECURSE WX_LIBS "$ENV{WX_WIDGETS}/lib/vc_lib/*[!i]d.lib" "$ENV{WX_WIDGETS}/lib/vc_lib/*ud*.lib")
    else()
    file(GLOB_RECURSE WX_LIBS "$ENV{WX_WIDGETS}/lib/vc_lib/*u_*.lib" "$ENV{WX_WIDGETS}/lib/vc_lib/*u.lib" "$ENV{WX_WIDGETS}/lib/vc_lib/*[gb].lib")
    endif ()

    set(external_libs 
      kernel32.lib
      user32.lib
      gdi32.lib
      comdlg32.lib
      winspool.lib
      winmm.lib
      shell32.lib
      shlwapi.lib
      comctl32.lib
      ole32.lib
      oleaut32.lib
      uuid.lib
      rpcrt4.lib
      advapi32.lib
      version.lib
      wsock32.lib
      wininet.lib
      opengl32.lib
      ${WX_LIBS}
    )
else()
    set(WX_INCLUDE_DIR  $ENV{WX_WIDGETS}/include
                        $ENV{WX_WIDGETS}/gtk-build/lib/wx/include/gtk3-unicode-3.2
                        /usr/include/gtk-3.0
                        /usr/include/glib-2.0
                        /usr/lib/x86_64-linux-gnu/glib-2.0/include
                        /usr/include/pango-1.0
                        /usr/include/harfbuzz
                        /usr/include/cairo
                        /usr/include/gdk-pixbuf-2.0
                        /usr/include/atk-1.0
    CACHE INTERNAL "${PROJECT_NAME}: Include Directories" FORCE)
    set(WX_DEFINITIONS WXUSINGDLL __WXGTK__ _FILE_OFFSET_BITS=64 PIC wxUSE_PROPGRID wxUSE_ODCOMBOBOX CACHE INTERNAL "${PROJECT_NAME}: Definitions" FORCE)
    
    file(GLOB external_lib_files
        $ENV{BOOST_ROOT}/stage/lib/*.a
        $ENV{WX_WIDGETS}/gtk-build/lib/*.so
    )
    set(external_libs 
        gtk-3
        gdk-3
        gobject-2.0
        pangocairo-1.0
        cairo
        gdk_pixbuf-2.0
        gio-2.0
        pangoft2-1.0
        pango-1.0
        glib-2.0
        fontconfig
        gtk-3
        ${external_lib_files}
    )
endif()

set(WATERCOLOR_PLUGIN_API ${PROJECT_SOURCE_DIR}/Watercolor/Services/Sources/Plugins/Api)

# Include sub-projects.
add_subdirectory ("Irrlicht")
add_subdirectory ("TEngineFramework/TUtils")
add_subdirectory ("TEngineFramework/TCore")
add_subdirectory ("TEngineFramework/TAM")
add_subdirectory ("TEngineFramework/TNC")

add_subdirectory ("TLauncher")

add_subdirectory ("Watercolor/Common")
add_subdirectory ("Watercolor/Services")
add_subdirectory ("Watercolor/MainWindow")

add_subdirectory ("Watercolor/Plugins/GuiEditor")

add_subdirectory ("Watercolor/WelcomeWindow")

if(MSVC AND ${IS_DEBUG_BUILD})
    configure_file(${CMAKE_BINARY_DIR}/packages/OpenAL.Soft/runtimes/win-x86/native/OpenAL32.dll ${CMAKE_BINARY_DIR}/TLauncher COPYONLY)
    configure_file(${CMAKE_BINARY_DIR}/packages/OpenAL.Soft/runtimes/win-x86/native/soft_oal.dll ${CMAKE_BINARY_DIR}/TLauncher COPYONLY)
    
    configure_file(${CMAKE_BINARY_DIR}/packages/OpenAL.Soft/runtimes/win-x86/native/OpenAL32.dll ${CMAKE_BINARY_DIR}/Watercolor/WelcomeWindow COPYONLY)
    configure_file(${CMAKE_BINARY_DIR}/packages/OpenAL.Soft/runtimes/win-x86/native/soft_oal.dll ${CMAKE_BINARY_DIR}/Watercolor/WelcomeWindow COPYONLY)
endif()