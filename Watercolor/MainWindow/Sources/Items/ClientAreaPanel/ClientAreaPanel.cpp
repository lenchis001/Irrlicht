#include "ClientAreaPanel.h"
#include "TCoreFacade.h"

#include "Common/CommonData.h"
#include "ThreadTypes/ThreadTypes.h"

namespace Watercolor {
ClientAreaPanel::ClientAreaPanel(wxWindow* parent, boost::shared_ptr<IFunctionsProcessingManager> functionsProcessingManager)
    : wxPanel(parent)
{
    // Client area sizer
    _sizer = new wxStaticBoxSizer(wxHORIZONTAL, this);
    this->SetSizer(_sizer);
    // ----------------------
    // OpenGL parent panel
    _mainCanvas = boost::make_shared<Common::GizmoGraphicContext>(
        this, ThreadTypes::RENDER_THREAD, functionsProcessingManager, wxDefaultSize, wxDefaultPosition, Common::ElementsIDs::MAIN_CANVAS);
    _sizer->Add(_mainCanvas.get(), 1, wxALL | wxEXPAND, 0);
    _sizer->AddSpacer(10);
    // Tool bar
    _toolBar = boost::make_shared<ToolNotebook>(this, functionsProcessingManager);
    _sizer->Add(_toolBar.get(), 0, wxALL | wxEXPAND, 0);
}
}