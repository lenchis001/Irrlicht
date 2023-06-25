#include "TCoreFacade.h"

#include "MainMenu.h"

#include "Utils/Utils.h"

#include "AboutDialog/AboutDialog.h"
#include "CommonFacade.h"
#include "ThreadTypes/ThreadTypes.h"
#include "Utils/WatercolorUtils.h"

namespace Watercolor {
void MainMenu::_openLevel(wxCommandEvent& eventData)
{
    if (_projectDataManager->isProjectOpened()) {
        auto levelsNames = _projectDataManager->getLevelsNames();
        auto wxLevelsNames = Utils::toWxArrayString(levelsNames);
        auto levelChoiseDialog = boost::make_shared<wxSingleChoiceDialog>(
            this, SELECT_LEVEL_DIALOG_WELCOME_LABEL, SELECT_LEVEL_DIALOG_TITLE, wxLevelsNames);
        if (levelChoiseDialog->ShowModal() == wxID_OK) {
            int selectedLevelIndex = levelChoiseDialog->GetSelection();
            if (selectedLevelIndex != -1) {
                std::wstring selectedLevelName = wxLevelsNames[selectedLevelIndex].ToStdWstring();
                _projectDataManager->setCurrentLevelName(selectedLevelName);
                std::wstring selectedPath = _projectDataManager->getPathToLevel(selectedLevelName);

                _graphicContext->loadLevel(selectedPath);
            }
        }
    } else {
        wxMessageBox(L"You should open/create a project before.");
    }
}

void MainMenu::_saveLevel(wxCommandEvent& eventData)
{
    if (!_projectDataManager->isLevelOpened()) {
        wxMessageBox(L"You should open/create a level before.");
    } else {
        _saveLevelChangesExit(false);
    }
}

void MainMenu::_exit(wxCommandEvent& eventData)
{
    _saveLevelChangesExit(true);
    wxTheApp->Exit();
}

void MainMenu::_createLevel(wxCommandEvent&)
{
    if (!_projectDataManager->isProjectOpened()) {
        wxMessageBox(L"You should open/create a project before.");
    } else {
        auto levelNameEntryDialog = boost::make_shared<wxTextEntryDialog>(this, "Type level name: ");
        if (levelNameEntryDialog->ShowModal() == wxID_OK) {
            std::wstring levelName = levelNameEntryDialog->GetValue().ToStdWstring();
            if (!levelName.empty()) {
                if (_projectDataManager->createLevel(levelName)) {
                    std::wstring selectedPath = _projectDataManager->getPathToLevel(levelName);

                    _graphicContext->loadLevel(selectedPath);
                } else {
                    wxMessageBox(L"Can't create a level.");
                }
            }
        }
    }
}

void MainMenu::_showAbout(wxCommandEvent&)
{
    AboutDialog(wxTheApp->GetTopWindow()).ShowModal();
}

void MainMenu::_saveLevelChangesExit(bool askConfirmation)
{
    if (_projectDataManager->isLevelOpened()) {
        wxStandardID answer;
        if (askConfirmation) {
            wxMessageDialog dialog(this, SAVE_BEFORE_EXIT_DIALOG_WELCOME_LABEL, SAVE_BEFORE_EXIT_DIALOG_TITLE, wxYES_NO);
            answer = static_cast<wxStandardID>(dialog.ShowModal());
        } else {
            answer = wxID_YES;
        }

        if (answer == wxID_YES) {
            _graphicContext->saveLevel();
        }
    }
}

void MainMenu::_onPluginRequested(wxCommandEvent& e)
{
    int pluginId = e.GetId();
    #ifdef _WINDOWS_
    HWND handle = this->MSWGetParent();
    #else
    int handle = 0;
    #endif
    _pluginHandlers[pluginId]->onTopMenuItemClicked(_projectDataManager->getProjectDirectory(), handle);
}

MainMenu::MainMenu(
    boost::shared_ptr<IFunctionsProcessingManager> functionsProcessingManager, 
    boost::shared_ptr<TAMM::ISoundService> soundService,
    boost::shared_ptr<Common::IProjectDataManager> projectDataManager,
    boost::shared_ptr<Services::Plugins::IPluginService> pluginService,
    boost::shared_ptr<Services::Controls::IControlIdService> controlsIdService) :
    _functionsProcessingManager(functionsProcessingManager)
    , _soundService(soundService)
    , _projectDataManager(projectDataManager)
{
    _functionsProcessingManager->addFuctionToQueue(
        ThreadTypes::RENDER_THREAD, [=]() {
            this->_graphicContext = (Common::GraphicContext*)(wxTheApp->GetTopWindow()->FindWindowById(
                Common::ElementsIDs::MAIN_CANVAS));
        });

    // File ------------------------------------------------
    wxMenu* fileMenu = new wxMenu;
    fileMenu->Append(Common::ElementsIDs::CREATE_NEW_LEVEL_MAIN_MENU_ITEM,
        FILE_MENU_CREATE_LEVEL);
    fileMenu->Append(Common::ElementsIDs::OPEN_LEVEL_MAIN_MENU_ITEM,
        FILE_MENU_OPEN_LEVEL);
    fileMenu->Append(Common::ElementsIDs::SAVE_LEVEL_MAIN_MENU_ITEM,
        FILE_MENU_SAVE_LEVEL);
    fileMenu->AppendSeparator();
    fileMenu->Append(Common::ElementsIDs::EXIT_MAIN_MENU_ITEM, FILE_MENU_EXIT);
    // -----------
    this->Append(fileMenu, FILE_MENU);
    // -----------
    // Tools -----
    wxMenu* toolsMenu = new wxMenu();

    std::map<wxWindowID, std::wstring> buttons{};
    auto& plugins = pluginService->getByType(PluginType::TOP_MENU);
    for (auto& plugin : plugins) {
        int buttonId = controlsIdService->getFree();

        toolsMenu->Append(buttonId, plugin->getName());

        Bind(wxEVT_MENU, &MainMenu::_onPluginRequested, this, buttonId);
        _pluginHandlers.insert(std::pair(buttonId, plugin));
    }

    // -----------
    this->Append(toolsMenu, TOOLS_MENU);
    // -----------
    // Help ------
    wxMenu* helpMenu = new wxMenu;
    helpMenu->Append(Common::ElementsIDs::OPEN_ABOUT_MAIN_MENU_ITEM, HELP_MENU_ABOUT);
    this->Append(helpMenu, HELP_MENU);
    // -----------------------------------------------------
}

MainMenu::~MainMenu() {}

wxBEGIN_EVENT_TABLE(MainMenu, wxMenuBar)
    EVT_MENU(Common::ElementsIDs::CREATE_NEW_LEVEL_MAIN_MENU_ITEM,
        MainMenu::_createLevel)
                EVT_MENU(Common::ElementsIDs::OPEN_ABOUT_MAIN_MENU_ITEM,
                    MainMenu::_showAbout)
                            EVT_MENU(Common::ElementsIDs::OPEN_LEVEL_MAIN_MENU_ITEM,
                                MainMenu::_openLevel)
                                EVT_MENU(Common::ElementsIDs::SAVE_LEVEL_MAIN_MENU_ITEM,
                                    MainMenu::_saveLevel)
                                    EVT_MENU(Common::ElementsIDs::EXIT_MAIN_MENU_ITEM,
                                        MainMenu::_exit)
                                        wxEND_EVENT_TABLE()
}