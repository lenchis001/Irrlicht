#include "RecentlyLoadedProject.h"

#include "boost/filesystem.hpp"

#include "Common/CommonData.h"
#include "Common/LanguageDependents.h"

namespace Watercolor {
RecentlyLoadedProject::RecentlyLoadedProject(wxWindow* parent, const std::wstring& path, boost::function<void(const std::wstring&)> onClickCallback)
    : wxPanel(parent)
    , _path(path)
    , _onClickCallback(onClickCallback)
{
    SetBackgroundColour(wxColor(250, 250, 250));

    _sizer = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(_sizer);

    auto normalizedPath = boost::filesystem::path(path).normalize();
    auto folderName = normalizedPath.filename().wstring();
    boost::shared_ptr<wxStaticText> levelName(new wxStaticText(this, wxID_ANY, folderName));
    levelName->SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
    _sizer->Add(levelName.get(), 0, wxALL, 10);
    _controls.push_back(levelName);

    boost::shared_ptr<wxStaticText> fullPath(new wxStaticText(this, wxID_ANY, normalizedPath.wstring()));
    fullPath->SetFont(wxFont(8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
    _sizer->Add(fullPath.get(), 0, wxRIGHT | wxLEFT, 10);
    _controls.push_back(fullPath);

    boost::shared_ptr<wxButton> loadButton(new wxButton(this, Common::ElementsIDs::WELCOME_SCREEN_LOAD_RECENTLY_PROJECT, WELCOME_WINDOW_RECENTLY_OPENED_LOAD_BUTTON));
    _sizer->Add(loadButton.get(), 0, wxALL | wxALIGN_RIGHT, 10);
    _controls.push_back(loadButton);
}

void RecentlyLoadedProject::_onClickEventHandler(wxCommandEvent& _)
{
    _onClickCallback(_path);
}

wxBEGIN_EVENT_TABLE(RecentlyLoadedProject, wxPanel)
    EVT_BUTTON(Common::ElementsIDs::WELCOME_SCREEN_LOAD_RECENTLY_PROJECT, RecentlyLoadedProject::_onClickEventHandler)
        wxEND_EVENT_TABLE()
}
