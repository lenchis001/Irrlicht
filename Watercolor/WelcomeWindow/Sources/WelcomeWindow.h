#pragma once

#include <wx/wx.h>

#include "irrlicht.h"

#include "boost/smart_ptr.hpp"

#include "TCoreFacade.h"
#include "TamFacade.h"
#include "ServicesFacade.h"

#include "Items/UIThreadFunctionsRunner/UIThreadFunctionsRunner.h"
#include "Items/RecentlyLoadedProjectsList.h"
#include "RecentlyOpenedProjectsService.h"
#include "CommonFacade.h"

namespace Watercolor::WelcomeWindow {
class WelcomeWindow : public wxFrame {
public:
    WelcomeWindow(
        boost::shared_ptr<IFunctionsProcessingManager> functionsProcessingManager, 
        boost::shared_ptr<TAMM::ISoundService> soundService,
        boost::shared_ptr<Common::IProjectDataManager> projectDataManager,
        boost::shared_ptr<Services::Plugins::IPluginService> pluginService,
        boost::shared_ptr<Services::Controls::IControlIdService> controlsIdService);

    bool Destroy() override;
private:
    // Vars ------------------------------------------
    wxSizer* _sizer;
    std::list<boost::shared_ptr<wxWindow>> _components;
    boost::shared_ptr<RecentlyLoadedProjectsList> _recentlyOpenedList;
    wxFrame* _mainWindow;
    boost::shared_ptr<IFunctionsProcessingManager> _functionsProcessingManager;
    boost::shared_ptr<TAMM::ISoundService> _soundService;
    boost::shared_ptr<Common::IProjectDataManager> _projectDataManager;
    boost::shared_ptr<Services::Plugins::IPluginService> _pluginService;
    boost::shared_ptr<Services::Controls::IControlIdService> _controlsIdService;

    Services::RecentlyOpenedProjectsService _recentlyOpenedProjectsService;

    std::map<int, boost::shared_ptr<IPlugin>> _pluginHandlers;

    void _onLoadProject(wxCommandEvent& eventData);
    void _onPluginRequested(wxCommandEvent& e);

    void _loadProject(const std::wstring& path);
    void _runPlugin(boost::shared_ptr<IPlugin> plugin);
    void _runEditor();

    void _updateRecentlyProjectsList();

    wxDECLARE_EVENT_TABLE();
};
}