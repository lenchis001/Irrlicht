#pragma once

#include "map"
#include "string"
#include "vector"

#include "wx/wx.h"

#include "boost/shared_ptr.hpp"

namespace GUIEditor {
    class RichButtonGroupBox : public wxPanel {
        std::vector<boost::shared_ptr<wxWindow>> _controls;
        wxGridSizer* _sizer;

    public:
        RichButtonGroupBox(wxWindow* parent,
            const std::map<wxWindowID, std::wstring>& items,
            const std::wstring& title = L"");
    };
}