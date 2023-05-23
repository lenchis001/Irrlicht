#pragma once

#include <wx/wx.h>

#include "boost/shared_ptr.hpp"

#include "Items/ClientAreaPanel/ClientAreaPanel.h"
#include "Items/ClientAreaPanel/Items/ToolNotebook/ToolNotebook.h"
#include "Items/MainMenu.h"
#include "Items/MainWindowStatusBar.h"
#include "Items/ToolMenu.h"
#include "Items/UIThreadFunctionsRunner/UIThreadFunctionsRunner.h"
#include "ProjectDataManager\IProjectDataManager.h"

namespace Watercolor {
class MainWindow : public wxFrame {
public:
    MainWindow(
        wxWindow* parent, 
        boost::shared_ptr<IFunctionsProcessingManager> functionsProcessingManager, 
        boost::shared_ptr<TAMM::ISoundService> soundService,
        boost::shared_ptr<Common::IProjectDataManager> projectDataManager,
        boost::shared_ptr<Services::Plugins::IPluginService> pluginService,
        boost::shared_ptr<Services::Controls::IControlIdService> controlsIdService);
    ~MainWindow();

private:
    boost::shared_ptr<Common::UIThreadFunctionsRunner> _commandsRunner;
    wxBoxSizer* _sizer;
    MainMenu* _mainMenu;
    boost::shared_ptr<ToolMenu> _toolMenu;
    boost::shared_ptr<ClientAreaPanel> _clientAreaPanel;
    boost::shared_ptr<MainWindowStatusBar> _mainStatusBar;
    boost::shared_ptr<wxStaticText> _welcomeLabel;
    boost::shared_ptr<IFunctionsProcessingManager> _functionsProcessingManager;

    bool _areControlsLoaded;
    Common::GraphicContext* _graphicContext;

    void _onLevelLoaded();
};
}