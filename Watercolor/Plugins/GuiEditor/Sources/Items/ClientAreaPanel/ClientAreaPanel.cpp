#include "ClientAreaPanel.h"

#include "boost/make_shared.hpp"
#include "boost/make_unique.hpp"

namespace GUIEditor {
ClientAreaPanel::ClientAreaPanel(wxWindow* parent, boost::shared_ptr<IFunctionsProcessingManager> functionsProcessingManager)
    : wxPanel(parent)
{
    _sizer = new wxBoxSizer(wxHORIZONTAL);
    this->SetSizer(_sizer);

    _graphicContext = boost::make_shared<GuiEditorGraphicContext>(this, functionsProcessingManager);
    _sizer->Add(_graphicContext.get(), 1, wxALL | wxEXPAND, 5);

    _toolsPanel = boost::make_unique<ToolNotebook>(this, _graphicContext, functionsProcessingManager);
    _sizer->Add(_toolsPanel.get(), 0, wxTOP | wxBOTTOM | wxEXPAND, 0);
    _sizer->Layout();
}
}