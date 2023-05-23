#include "RecentlyLoadedProjectsList.h"

#include "RecentlyLoadedProject.h"

namespace Watercolor {
RecentlyLoadedProjectsList::RecentlyLoadedProjectsList(wxWindow* parent, boost::function<void(const std::wstring&)> onClick)
    : wxScrolled<wxPanel>(parent)
    , _onClickCallback(onClick)
{
    SetBackgroundColour(wxColor(240, 240, 240));

    _sizer = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(_sizer);

    SetScrollRate(10, 10);
}

void RecentlyLoadedProjectsList::initializeList(const std::vector<std::wstring>& pathes)
{
    _controls.clear();

    for (auto& path : pathes) {
        boost::shared_ptr<RecentlyLoadedProject> listItem(new RecentlyLoadedProject(this, path, _onClickCallback));
        _sizer->Add(listItem.get(), 0, wxALL | wxEXPAND, 5);
        _controls.push_back(listItem);
    }
}
}
