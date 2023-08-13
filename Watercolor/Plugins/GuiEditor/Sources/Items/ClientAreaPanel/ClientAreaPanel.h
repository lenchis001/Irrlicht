#pragma once

#include "wx/wx.h"

#include "boost/shared_ptr.hpp"

#include "Services/FunctionsProcessing/IFunctionsProcessingManager.h"

#include "Items/ToolNotebook/ToolNotebook.h"

namespace GUIEditor {
class ClientAreaPanel : public wxPanel {
public:
    ClientAreaPanel(wxWindow* parent, boost::shared_ptr<IFunctionsProcessingManager> functionsProcessingManager);

private:
    wxSizer* _sizer;

    boost::shared_ptr<GuiEditorGraphicContext> _graphicContext;
    std::unique_ptr<ToolNotebook> _toolsPanel;
};
}