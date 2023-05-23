#pragma once

#include "map"
#include "string"
#include "vector"

#include "InternalDerictoriesTypes/InternalDerictoriesTypes.h"

namespace Watercolor::Common {
    class IProjectDataManager {
    public:
        virtual bool isProjectOpened() = 0;
        virtual bool isLevelOpened() = 0;

        virtual std::wstring getPathToLevel(std::wstring levelName) = 0;
        virtual std::wstring getPathToSoundsDirectory() = 0;
        virtual std::wstring getPathToViewsDirectory() = 0;
        virtual std::wstring getProjectDirectory() = 0;

        virtual void setCurrentLevelName(std::wstring levelName) = 0;
        virtual std::wstring getCurrentLevelName() = 0;

        virtual bool openProject(std::wstring pathToProjectDir) = 0;

        virtual bool createLevel(std::wstring levelName) = 0;
        virtual bool createProject(std::wstring pathToProjectDirectory,
            std::wstring projectName) = 0;

        virtual std::vector<std::wstring> getLevelsNames() const = 0;

        virtual void clear() = 0;
    };
}