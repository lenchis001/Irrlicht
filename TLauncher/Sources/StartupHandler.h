#pragma once

#include "boost/smart_ptr.hpp"

namespace TLauncher {
    class StartupHandler {
    public:
        bool parseCommandLine(int argc, char* argv[]);
        bool isFullscreen();
    private:
        bool _isArgumentExists(char** argv, int argc, std::string parameter);

        bool _isFullscreen;
    };
}