#pragma once

#include "list"
#include "string"

#include "boost/function.hpp"
#include "boost/shared_ptr.hpp"

#include "wx/wx.h"

namespace Watercolor {
class RecentlyLoadedProjectsList : public wxScrolled<wxPanel> {
public:
    RecentlyLoadedProjectsList(wxWindow* parent, boost::function<void(const std::wstring&)> onClick);

    void initializeList(const std::vector<std::wstring>& pathes);

private:
    wxSizer* _sizer;
    std::list<boost::shared_ptr<wxWindow>> _controls;

    boost::function<void(const std::wstring&)> _onClickCallback;
};
}
