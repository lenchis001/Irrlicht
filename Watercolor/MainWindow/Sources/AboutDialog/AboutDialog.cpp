#include "AboutDialog.h"

#include "wx\font.h"

#include "boost/filesystem.hpp"
#include "boost/make_shared.hpp"

#include "Common/LanguageDependents.h"

AboutDialog::AboutDialog(wxWindow* parent)
    : wxDialog(parent, wxID_ANY, ABOUT_WINDOW_TITLE, wxDefaultPosition)
{
    this->SetBackgroundColour(wxColour(0xFF, 0xFF, 0xFF));

    _sizer = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(_sizer);

    auto appName = boost::make_shared<wxStaticText>(this, wxID_ANY, "Watercolor");
    appName->SetFont(wxFontInfo(18)
                         .Family(wxFONTFAMILY_SWISS)
                         .Encoding(wxFONTENCODING_DEFAULT));
    _controls.push_back(appName);
    _sizer->Add(appName.get(), 0, wxALL, 15);

    auto appInfo = boost::make_shared<wxStaticText>(this, wxID_ANY, "Editor of TEngine projects.\nAlpha version.\nLeontii Khachuev. 2019");
    appInfo->SetMinClientSize({ 350, 60 });
    appInfo->SetFont(wxFontInfo(12)
                         .Family(wxFONTFAMILY_SWISS)
                         .Encoding(wxFONTENCODING_DEFAULT));
    _controls.push_back(appInfo);
    _sizer->Add(appInfo.get(), 1, wxALL, 15);

    this->Fit();
}

AboutDialog::~AboutDialog() {}
