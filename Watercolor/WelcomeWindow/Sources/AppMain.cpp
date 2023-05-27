#include "AppMain.h"
#include "WelcomeWindow.h"

#include "TAMFacade.h"
#include "CommonDependencyRegistrator.h"

#include "Common/CommonData.h"
#include "ServicesFacade.h"

namespace Watercolor {
void AppMain::initDefaultProperties()
{
}

AppMain::AppMain()
{
    _diContainer = boost::make_shared<IDiContainer>();
}

bool AppMain::OnInit()
{
    _registerDependencies(_diContainer);

    initDefaultProperties();

    wxImage::AddHandler(new wxJPEGHandler);
    wxImage::AddHandler(new wxPNGHandler);
    wxImage::AddHandler(new wxICOHandler);

    auto functionsProcessingManager = _diContainer->resolve<IFunctionsProcessingManager>();
    auto soundService = _diContainer->resolve<TAMM::ISoundService>();
    auto projectDataManager = _diContainer->resolve<Common::IProjectDataManager>();
    auto pluginsService = _diContainer->resolve<Watercolor::Services::Plugins::IPluginService>();
    auto controlIdService = _diContainer->resolve<Watercolor::Services::Controls::IControlIdService>();

    pluginsService->loadPlugins();

    _welcomeWindow = new WelcomeWindow::WelcomeWindow(functionsProcessingManager, soundService, projectDataManager, pluginsService, controlIdService);
    _welcomeWindow->Show();

    return true;
}

void AppMain::_registerDependencies(boost::shared_ptr<IDiContainer> container) {
    registerTCoreDependencies(container);
    registerTamDependencies(container);

    registerCommonDependencies(container);
    registerServicesDependencies(container);
}
}