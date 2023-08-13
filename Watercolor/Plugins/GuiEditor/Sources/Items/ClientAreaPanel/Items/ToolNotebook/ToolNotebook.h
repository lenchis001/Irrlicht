#pragma once

#include "memory"
#include "vector"

#include "wx/notebook.h"
#include "wx/wx.h"

#include "boost/smart_ptr.hpp"

#include "Items/ClientAreaPanel/Items/GuiEditorGraphicContext/GuiEditorGraphicContext.h"

namespace GUIEditor {
class ToolNotebook : public wxNotebook {
public:
    ToolNotebook(wxWindow* parent, boost::shared_ptr<GuiEditorGraphicContext> graphicContext, boost::shared_ptr<IFunctionsProcessingManager> functionsProcessingManager);

private:
    std::vector<boost::shared_ptr<wxScrolled<wxPanel>>> _elementsAddingPanel;
};
}