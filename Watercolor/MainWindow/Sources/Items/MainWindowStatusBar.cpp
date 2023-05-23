#include "MainWindowStatusBar.h"
#include "TCoreFacade.h"

#include "boost/make_shared.hpp"

#include "Utils/Utils.h"

#include "Common/CommonData.h"

#define ON_START_LEVEL_LOADING L"on start level loading status bar handler"
#define ON_FINISH_LEVEL_LOADING L"on finish level loading status bar handler"

namespace Watercolor {
void MainWindowStatusBar::onStartProcess(boost::any) { _loadingStatus->Pulse(); }

void MainWindowStatusBar::onFinishProcess(boost::any)
{
    _loadingStatus->SetValue(0);
}

MainWindowStatusBar::MainWindowStatusBar(wxWindow* parent, boost::shared_ptr<IFunctionsProcessingManager> functionsProcessingManager)
    : wxStatusBar(parent)
    , _functionsProcessingManager(functionsProcessingManager)
{
    this->SetBackgroundColour(wxColour(0x3C, 0x70, 0xF5));

    _sizer = new wxBoxSizer(wxHORIZONTAL);
    this->SetSizer(_sizer);

    _loadingStatus = boost::make_shared<wxGauge>(this, wxID_ANY, 100);
    _sizer->Add(_loadingStatus.get(), 0, wxALL, 5);

    _functionsProcessingManager->addFuctionToQueue(
        ThreadTypes::RENDER_THREAD, [&]() {
            _graphicContext = (Common::GraphicContext*)(wxTheApp->GetTopWindow()->FindWindowById(
                Common::ElementsIDs::MAIN_CANVAS));

            _graphicContext->onInitLevelLoad.addEventHandler(ON_START_LEVEL_LOADING,
                boost::bind(&MainWindowStatusBar::onStartProcess, this, boost::placeholders::_1));
            _graphicContext->onLevelLoaded.addEventHandler(ON_FINISH_LEVEL_LOADING,
                boost::bind(&MainWindowStatusBar::onFinishProcess, this, boost::placeholders::_1));
        });
}

MainWindowStatusBar::~MainWindowStatusBar()
{
}
}