#pragma once

#include <wx/wx.h>
#include "boost/smart_ptr.hpp"

#include "WelcomeWindow.h"

namespace Watercolor {
class AppMain : public wxApp {
    WelcomeWindow::WelcomeWindow* _welcomeWindow;
    boost::shared_ptr<IDiContainer> _diContainer;

    void initDefaultProperties();

    void _registerDependencies(boost::shared_ptr<IDiContainer> container);

public:
    AppMain();

    virtual bool OnInit();
};
}