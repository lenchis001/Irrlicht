#pragma once

#include "string"
#include "vector"

namespace Watercolor::Services {
class RecentlyOpenedProjectsService {
public:
    void saveRecentlyOpenedProjects();
    void readRecentlyOpenedProjects();
    void saveAsRecentlyOpened(const std::wstring& projectPath);

    const std::vector<std::wstring>& getRecentlyOpenedProjects();

private:
    std::vector<std::wstring> _recentlyOpened;
};
}
