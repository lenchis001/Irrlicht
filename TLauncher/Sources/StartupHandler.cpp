#include "StartupHandler.h"

#include "string"

#include "boost/filesystem.hpp"

#include "Utils/Utils.h"

#define FULLSCREEN_PARAMETER "--fullscreen"

namespace TLauncher {
    bool StartupHandler::parseCommandLine(int argc, char* argv[])
    {
        // full screen
       _isFullscreen = _isArgumentExists(argv, argc, FULLSCREEN_PARAMETER);

        return true;
    }

    bool StartupHandler::isFullscreen()
    {
        return _isFullscreen;
    }

    bool StartupHandler::_isArgumentExists(char** argv, int argc, std::string parameter)
    {
        return std::find_if(
            argv,
            argv + argc,
            [](char* e) { return !strcmp(e, FULLSCREEN_PARAMETER); }
        ) != argv + argc;
    }
}