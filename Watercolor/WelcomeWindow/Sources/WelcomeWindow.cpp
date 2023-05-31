#include "WelcomeWindow.h"

#include "Common/CommonData.h"
#include "Common/LanguageDependents.h"
#include "ThreadTypes/ThreadTypes.h"

#include "Items/RichButtonGroupBox/RichButtonGroupBox.h"
#include "MainWindow.h"

namespace Watercolor::WelcomeWindow {
WelcomeWindow::WelcomeWindow(
    boost::shared_ptr<IFunctionsProcessingManager> functionsProcessingManager, 
    boost::shared_ptr<TAMM::ISoundService> soundService,
    boost::shared_ptr<Common::IProjectDataManager> projectDataManager,
    boost::shared_ptr<Services::Plugins::IPluginService> pluginService,
    boost::shared_ptr<Services::Controls::IControlIdService> controlsIdService)
    : wxFrame(NULL, Common::ElementsIDs::WELCOME_WINDOW, wxString(MAIN_WINDOW_TITLE), wxDefaultPosition),
    _functionsProcessingManager(functionsProcessingManager)
    , _soundService(soundService)
    , _projectDataManager(projectDataManager)
    , _pluginService(pluginService)
    , _controlsIdService(controlsIdService)
{
    // Set window properies
    this->SetBackgroundColour(wxColour(0xFF, 0xFF, 0xFF));
    this->SetMinSize(wxSize(350, 550));
    // ----------------------------------------------------

    // Main sizer
    _sizer = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(_sizer);

    boost::shared_ptr<wxStaticText> title(new wxStaticText(this, wxID_ANY, WELCOME_WINDOW_TITLE));
    title->SetFont(wxFont(16, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
    _sizer->Add(title.get(), 0, wxALL | wxALIGN_CENTER, 10);
    _components.push_back(title);

    boost::shared_ptr<wxStaticText> recentlyOpened(new wxStaticText(this, wxID_ANY, WELCOME_WINDOW_RECENTLY_OPENED_LABEL));
    recentlyOpened->SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
    _sizer->Add(recentlyOpened.get(), 0, wxALL, 10);
    _components.push_back(recentlyOpened);

    _recentlyOpenedList = boost::shared_ptr<RecentlyLoadedProjectsList>(new RecentlyLoadedProjectsList(this, boost::bind(&WelcomeWindow::_loadProject, this, boost::placeholders::_1)));
    _updateRecentlyProjectsList();
    _sizer->Add(_recentlyOpenedList.get(), 1, wxRIGHT | wxLEFT | wxEXPAND, 10);
    _components.push_back(_recentlyOpenedList);

    std::map<wxWindowID, std::wstring> buttons {
        { Common::ElementsIDs::WELCOME_SCREEN_LOAD_PROJECT, WELCOME_WINDOW_LOAD_PROJECT_BUTTON }
    };
    auto plugins = pluginService->getByType(PluginType::WELCOME_WINDOW);
    for (auto plugin : plugins) {
        int buttonId = _controlsIdService->getFree();

        buttons.insert(std::pair(buttonId, plugin->getName()));

        Bind(wxEVT_BUTTON, &WelcomeWindow::_onPluginRequested, this, buttonId);
        _pluginHandlers.insert(std::pair(buttonId, plugin));
    }

    boost::shared_ptr<Common::RichButtonGroupBox> otherActions=boost::make_shared<Common::RichButtonGroupBox>(
        this,
        buttons,
        WELCOME_WINDOW_OTHER_ACTIONS_LABEL
    );
    _sizer->Add(otherActions.get(), 0, wxALL | wxEXPAND, 10);
    _components.push_back(otherActions);

    CenterOnScreen();
    SetFocus();
}

bool WelcomeWindow::Destroy()
{
    for (auto handler : _pluginHandlers) {
       Unbind(wxEVT_BUTTON, &WelcomeWindow::_onPluginRequested, this, handler.first);
    }

    return wxFrame::Destroy();
}

void WelcomeWindow::_onLoadProject(wxCommandEvent& eventData)
{
    wxDirDialog dirDialog(this);
    dirDialog.ShowModal();

    std::wstring selectedPath = dirDialog.GetPath().ToStdWstring();
    if (!selectedPath.empty()) {
        _loadProject(selectedPath);
    }
}

void WelcomeWindow::_onPluginRequested(wxCommandEvent& e)
{
    _projectDataManager->clear();

    int pluginId = e.GetId();
    auto newProjectPath = _pluginHandlers[pluginId]->onWelcomeWindowButtonClicked(
        #if (_WINDOWS_)
        this->GetHWND()
        #else
        0    
        #endif
    );

    if (!newProjectPath.empty()) {
        _projectDataManager->openProject(newProjectPath);

        if (_projectDataManager->isProjectOpened()) {
            _recentlyOpenedProjectsService.saveAsRecentlyOpened(_projectDataManager->getProjectDirectory());
            _runEditor();
        }
    }
}

void WelcomeWindow::_loadProject(const std::wstring& path)
{
    if (!_projectDataManager->openProject(path)) {
        wxMessageBox(L"Can't open project directory.");
    } else {
        _recentlyOpenedProjectsService.saveAsRecentlyOpened(path);
        _runEditor();
    }
}

void WelcomeWindow::_runPlugin(boost::shared_ptr<IPlugin> plugin)
{
    _projectDataManager->clear();

    auto newProjectPath = plugin->onWelcomeWindowButtonClicked(
#if (_WINDOWS_)
        this->GetHWND()
#else
        0    
#endif
    );

    if (!newProjectPath.empty()) {
        _projectDataManager->openProject(newProjectPath);

        if (_projectDataManager->isProjectOpened()) {
            _recentlyOpenedProjectsService.saveAsRecentlyOpened(_projectDataManager->getProjectDirectory());
            _runEditor();
        }
    }
}

void WelcomeWindow::_runEditor()
{
    this->HideWithEffect(wxShowEffect::wxSHOW_EFFECT_SLIDE_TO_TOP);

    _mainWindow = new MainWindow(this, _functionsProcessingManager, _soundService, _projectDataManager, _pluginService, _controlsIdService);
    _mainWindow->Show();
    _mainWindow->Bind(wxEVT_CLOSE_WINDOW, [&](auto _) {
        _mainWindow->Destroy();

        _updateRecentlyProjectsList();

        this->ShowWithEffect(wxShowEffect::wxSHOW_EFFECT_SLIDE_TO_BOTTOM);
    });
}

void WelcomeWindow::_updateRecentlyProjectsList()
{
    _recentlyOpenedProjectsService.readRecentlyOpenedProjects();
    auto recentlyOpenedProject = _recentlyOpenedProjectsService.getRecentlyOpenedProjects();
    _recentlyOpenedList->initializeList(recentlyOpenedProject);
}

wxBEGIN_EVENT_TABLE(WelcomeWindow, wxFrame)
        EVT_BUTTON(Common::ElementsIDs::WELCOME_SCREEN_LOAD_PROJECT, WelcomeWindow::_onLoadProject)
wxEND_EVENT_TABLE()
}