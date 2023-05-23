#pragma once

#include "list"
#include "string"

#include "boost/function.hpp"
#include "boost/shared_ptr.hpp"

#include "wx/wx.h"

namespace Watercolor {
class RecentlyLoadedProject : public wxPanel {
public:
    RecentlyLoadedProject(wxWindow* parent, const std::wstring& path, boost::function<void(const std::wstring&)> onClickCallback);

private:
    std::wstring _path;
    wxSizer* _sizer;
    std::list<boost::shared_ptr<wxWindow>> _controls;

    boost::function<void(const std::wstring&)> _onClickCallback;

    void _onClickEventHandler(wxCommandEvent& _);

    wxDECLARE_EVENT_TABLE();
};
}
