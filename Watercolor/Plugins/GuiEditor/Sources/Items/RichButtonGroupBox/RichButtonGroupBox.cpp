#include "RichButtonGroupBox.h"

#include "boost/make_shared.hpp"

namespace GUIEditor {
    RichButtonGroupBox::RichButtonGroupBox(
        wxWindow* parent,
        const std::map<wxWindowID, std::wstring>& items,
        const std::wstring& title)
        : wxPanel(parent)
    {
        this->SetBackgroundColour(wxColour(0xF5, 0xF5, 0xF5));
        _sizer = new wxGridSizer(2);
        this->SetSizer(_sizer);

        if (!title.empty()) {
            auto titleLabel = boost::make_shared<wxStaticText>(this, wxID_ANY, title);
            _sizer->Add(titleLabel.get(), 0, wxALL | wxEXPAND, 15);
            _controls.push_back(titleLabel);
            _sizer->AddStretchSpacer();
        }

        for (auto item : items) {
            auto button = boost::make_shared<wxButton>(this, item.first, item.second);

            _controls.push_back(button);
            _sizer->Add(button.get(), 0, wxALL | wxEXPAND, 10);
        }
    }
}