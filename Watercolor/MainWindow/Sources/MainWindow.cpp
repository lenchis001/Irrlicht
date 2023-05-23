#include "MainWindow.h"
#include "TCoreFacade.h"

#include "boost/bind.hpp"
#include "boost/make_shared.hpp"

#include "Common/CommonData.h"
#include "Common/LanguageDependents.h"
#include "ThreadTypes/ThreadTypes.h"

#define ON_LEVEL_LOADED_MAIN_WINDOW_HANDLER L"ON_LEVEL_LOADED_MAIN_WINDOW_HANDLER"

namespace Watercolor {
MainWindow::MainWindow(
    wxWindow* parent, 
    boost::shared_ptr<IFunctionsProcessingManager> functionsProcessingManager, 
    boost::shared_ptr<TAMM::ISoundService> soundService,
    boost::shared_ptr<Common::IProjectDataManager> projectDataManager,
    boost::shared_ptr<Services::Plugins::IPluginService> pluginService,
    boost::shared_ptr<Services::Controls::IControlIdService> controlsIdService)
    : wxFrame(parent, Common::ElementsIDs::MAIN_WINDOW, wxString(MAIN_WINDOW_TITLE), wxDefaultPosition)
    , _functionsProcessingManager(functionsProcessingManager)
{
    // Set window properies
    this->SetBackgroundColour(wxColour(0xFF, 0xFF, 0xFF));
    this->SetMinSize(wxSize(1280, 700));
    // ----------------------------------------------------
    _mainMenu = new MainMenu(functionsProcessingManager, soundService, projectDataManager, pluginService, controlsIdService);
    this->SetMenuBar(_mainMenu);
    // ----------------------------------------------------
    // Main sizer
    _sizer = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(_sizer);

    // Tool menu
    _toolMenu = boost::make_shared<ToolMenu>(this, _functionsProcessingManager, projectDataManager);
    _toolMenu->Hide();
    _sizer->Add(_toolMenu.get(), 0, wxALL | wxEXPAND);
    _sizer->AddSpacer(10);
    // -------------------------------------
    // Client Area panel
    _clientAreaPanel = boost::make_shared<ClientAreaPanel>(this, _functionsProcessingManager);
    _sizer->Add(_clientAreaPanel.get(), 1, wxALL | wxEXPAND, 0);
    _clientAreaPanel->Hide();
    _sizer->AddSpacer(10);
    // -------------------------------------
    // Welcome label
    _welcomeLabel = boost::shared_ptr<wxStaticText>(new wxStaticText(this, wxID_ANY, L"Welcome to Watercolor! Open or create a level to start work."));
    _welcomeLabel->SetFont(wxFontInfo(12)
                               .Family(wxFONTFAMILY_SWISS)
                               .Encoding(wxFONTENCODING_DEFAULT));
    _sizer->Add(_welcomeLabel.get(), 0, wxALIGN_CENTER | wxALL);
    // ------------------------------------
    // Status bar
    _mainStatusBar = boost::make_shared<MainWindowStatusBar>(this, _functionsProcessingManager);
    this->SetStatusBar(_mainStatusBar.get());
    // ------------------------------------
    _commandsRunner = boost::make_shared<Common::UIThreadFunctionsRunner>(_functionsProcessingManager);
    // todo: change this approach
    _commandsRunner->Start(300);

    _functionsProcessingManager->addFuctionToQueue(
        ThreadTypes::RENDER_THREAD, [&]() {
            this->_graphicContext = (Common::GraphicContext*)(wxTheApp->GetTopWindow()->FindWindowById(
                Common::ElementsIDs::MAIN_CANVAS));

            this->_graphicContext->onLevelLoaded.addEventHandler(ON_LEVEL_LOADED_MAIN_WINDOW_HANDLER, boost::bind(&MainWindow::_onLevelLoaded, this));
        });

    Fit();
}
MainWindow::~MainWindow()
{
    this->_graphicContext->onLevelLoaded.removeEventHandler(ON_LEVEL_LOADED_MAIN_WINDOW_HANDLER);
}
void MainWindow::_onLevelLoaded()
{
    _functionsProcessingManager->addFuctionToQueue(ThreadTypes::UI_THREAD, [&]() {
        if (!_areControlsLoaded) {
            _areControlsLoaded = true;

            _welcomeLabel.reset();

            _toolMenu->Show();
            _clientAreaPanel->Show();
            _sizer->Layout();
        }
    });
}
}