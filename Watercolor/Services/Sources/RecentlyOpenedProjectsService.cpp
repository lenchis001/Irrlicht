#include "RecentlyOpenedProjectsService.h"

#include "boost/filesystem.hpp"

#include "Utils/Filesystem.h"
#include "Utils/Utils.h"

#define RECENTLY_OPENED_LIST_FILE L"recently opened.json"

namespace Watercolor::Services {
void RecentlyOpenedProjectsService::saveRecentlyOpenedProjects()
{
    TUtils::Filesystem::writeVector(RECENTLY_OPENED_LIST_FILE, _recentlyOpened);
}

void RecentlyOpenedProjectsService::readRecentlyOpenedProjects()
{
    _recentlyOpened.clear();

    if (boost::filesystem::exists(RECENTLY_OPENED_LIST_FILE)) {
        _recentlyOpened = TUtils::Filesystem::readVector(RECENTLY_OPENED_LIST_FILE);
    }
}

void RecentlyOpenedProjectsService::saveAsRecentlyOpened(const std::wstring& projectPath)
{
    _recentlyOpened.insert(_recentlyOpened.begin(), projectPath);

    _recentlyOpened = TUtils::unique(_recentlyOpened);

    saveRecentlyOpenedProjects();
}
const std::vector<std::wstring>& RecentlyOpenedProjectsService::getRecentlyOpenedProjects()
{
    return _recentlyOpened;
}
}