#pragma once

#include <wx/wx.h>

#include "map"
#include "string"

#include "boost/property_tree/ptree.hpp"
#include "boost/smart_ptr.hpp"

#include "TamFacade.h"
#include "ServicesFacade.h"

#include "Common/LanguageDependents.h"
#include "Items/Graphic/GraphicContext.h"
#include "CommonFacade.h"

namespace Watercolor {
class MainMenu : public wxMenuBar {
    std::map<wxWindowID, std::wstring> _levelItemsIDs;
    std::map<int, boost::shared_ptr<IPlugin>> _pluginHandlers;

    Common::GraphicContext* _graphicContext;

    boost::shared_ptr<IFunctionsProcessingManager> _functionsProcessingManager;
    boost::shared_ptr<TAMM::ISoundService> _soundService;
    boost::shared_ptr<Common::IProjectDataManager> _projectDataManager;

    void _onPluginRequested(wxCommandEvent& e);

    void _createLevel(wxCommandEvent&);
    void _openLevel(wxCommandEvent& eventData);
    void _saveLevel(wxCommandEvent& eventData);
    void _exit(wxCommandEvent& eventData);

    void _showAbout(wxCommandEvent&);

    void _saveLevelChangesExit(bool askConfirmation = false);

public:
    MainMenu(
        boost::shared_ptr<IFunctionsProcessingManager> functionsProcessingManager, 
        boost::shared_ptr<TAMM::ISoundService> soundService,
        boost::shared_ptr<Common::IProjectDataManager> projectDataManager,
        boost::shared_ptr<Services::Plugins::IPluginService> pluginService,
        boost::shared_ptr<Services::Controls::IControlIdService> controlsIdService);
    ~MainMenu();

private:
    wxDECLARE_EVENT_TABLE();
};
}