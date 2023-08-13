#include "ProjectDataManager.h"

#include "algorithm"

#include "boost/filesystem.hpp"

#include "Utils/Filesystem.h"

namespace Watercolor::Common
{
    bool ProjectDataManager::_isCorrectProjectDirStructure(
        std::wstring pathToProjectDir) const
    {
        bool answer = true;
        for (auto dir : _internalDiectories)
        {
            if (!boost::filesystem::exists(pathToProjectDir + L"/" + dir.second))
            {
                answer = false;
                break;
            }
        }
        return answer;
    }

    void ProjectDataManager::clear()
    {
        _projectDirectory.clear();
        _currentLevelName.clear();
        _levels.clear();
    }

    bool ProjectDataManager::isProjectOpened()
    {
        return !_projectDirectory.empty();
    }

    bool ProjectDataManager::isLevelOpened() { return !_currentLevelName.empty(); }

    std::wstring ProjectDataManager::getPathToLevel(std::wstring levelName)
    {
        return _levels[levelName];
    }

    std::wstring ProjectDataManager::getPathToSoundsDirectory()
    {
        return _projectDirectory + L"/" + _internalDiectories.at(InternalDerictoriesTypes::SOUNDS);
    }

    std::wstring ProjectDataManager::getPathToViewsDirectory()
    {

        return _projectDirectory + L"/" + _internalDiectories.at(InternalDerictoriesTypes::GUI);
    }

    std::wstring ProjectDataManager::getProjectDirectory()
    {
        return _projectDirectory;
    }

    void ProjectDataManager::setCurrentLevelName(std::wstring levelName)
    {
        _currentLevelName = levelName;
    }

    std::wstring ProjectDataManager::getCurrentLevelName()
    {
        return _currentLevelName;
    }

    bool ProjectDataManager::openProject(std::wstring pathToProjectDir)
    {
        bool answer = true;

        clear();

        if (_isCorrectProjectDirStructure(pathToProjectDir))
        {
            _projectDirectory = pathToProjectDir;

            auto pathesToLevelsFiles = TUtils::Filesystem::collectFilesByTemplate(
                pathToProjectDir + L"/" + _internalDiectories.at(InternalDerictoriesTypes::LEVELS),
                L".+\\.irr$");

            std::transform(
                pathesToLevelsFiles.begin(), pathesToLevelsFiles.end(),
                std::inserter(_levels, _levels.begin()), [](std::wstring pathToLevelFile)
                { return std::pair<std::wstring, std::wstring>(boost::filesystem::wpath(pathToLevelFile).stem().wstring(), pathToLevelFile); });
        }
        else
        {
            answer = false;
        }

        return answer;
    }

    bool ProjectDataManager::createLevel(std::wstring levelName)
    {
        bool answer = true;

        std::wstring templatePath = boost::filesystem::current_path().wstring() + L"/Resources/Levels/EmptyLevelTemplate.irr";
        std::wstring newlevelPath = _projectDirectory + L"/" + _internalDiectories.at(InternalDerictoriesTypes::LEVELS) + L"/" + levelName + L".irr";

        boost::filesystem::copy(templatePath, newlevelPath);

        setCurrentLevelName(levelName);
        _levels.insert(std::pair(levelName, newlevelPath));

        return answer;
    }

    bool ProjectDataManager::createProject(std::wstring pathToParentDirectory,
                                           std::wstring projectName)
    {
        bool answer = true;

        clear();

        // create project directory
        std::wstring pathToProjectDirectory = pathToParentDirectory + L"/" + projectName;
        boost::filesystem::create_directory(pathToProjectDirectory);

        for (auto dir : _internalDiectories)
        {
            if (!boost::filesystem::create_directory(pathToProjectDirectory + L"/" + dir.second))
            {
                answer = false;
                break;
            }
        }

        if (answer)
        {
            _projectDirectory = pathToProjectDirectory;
        }

        return answer;
    }

    std::vector<std::wstring> ProjectDataManager::getLevelsNames() const
    {
        std::vector<std::wstring> result;

        std::transform(_levels.begin(), _levels.end(), std::inserter(result, result.begin()), [](auto pair)
                       { return pair.first; });

        return result;
    }

    ProjectDataManager::ProjectDataManager()
    {
        _internalDiectories.insert({InternalDerictoriesTypes::GUI, L"GUI"});
        _internalDiectories.insert({InternalDerictoriesTypes::SOUNDS, L"Sounds"});
        _internalDiectories.insert({InternalDerictoriesTypes::LEVELS, L"Levels"});
        _internalDiectories.insert({InternalDerictoriesTypes::MODELS, L"Models"});
    }
}