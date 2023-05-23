#pragma once

#include "vector"

#include <wx/wx.h>

#include "boost/shared_ptr.hpp"

class AboutDialog : public wxDialog {
public:
    AboutDialog(wxWindow* parent);
    ~AboutDialog();

private:
    wxSizer* _sizer;
    std::vector<boost::shared_ptr<wxWindow>> _controls;
};
