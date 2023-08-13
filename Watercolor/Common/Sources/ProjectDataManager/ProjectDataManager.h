#pragma once

#include "map"
#include "string"
#include "vector"

#include "boost/smart_ptr.hpp"

#include "IProjectDataManager.h"

namespace Watercolor::Common {
class ProjectDataManager : public IProjectDataManager {
public:
    bool isProjectOpened() override;
    bool isLevelOpened() override;

    std::wstring getPathToLevel(std::wstring levelName) override;
    std::wstring getPathToSoundsDirectory() override;
    std::wstring getPathToViewsDirectory() override;
    std::wstring getProjectDirectory() override;

    void setCurrentLevelName(std::wstring levelName) override;
    std::wstring getCurrentLevelName() override;

    bool openProject(std::wstring pathToProjectDir) override;

    bool createLevel(std::wstring levelName) override;
    bool createProject(std::wstring pathToProjectDirectory,
        std::wstring projectName) override;

    std::vector<std::wstring> getLevelsNames() const override;
    ProjectDataManager();

    void clear() override;
private:
    float _version;
    std::wstring _projectDirectory;
    std::wstring _currentLevelName;
    std::map<std::wstring, std::wstring> _levels;
    std::map<InternalDerictoriesTypes, std::wstring> _internalDiectories;

    bool _isCorrectProjectDirStructure(std::wstring pathToProjectDir) const;
};
}