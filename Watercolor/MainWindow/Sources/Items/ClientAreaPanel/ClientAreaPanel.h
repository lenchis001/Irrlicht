#pragma once

#include <wx/wx.h>

#include "boost/smart_ptr.hpp"

#include "Items/Graphic/GizmoGraphicContext.h"
#include "Items/ToolNotebook/ToolNotebook.h"

#include "TCoreFacade.h"

namespace Watercolor {
class ClientAreaPanel : public wxPanel {
public:
    ClientAreaPanel(wxWindow* parent, boost::shared_ptr<IFunctionsProcessingManager> functionsProcessingManager);

private:
    wxSizer* _sizer;
    boost::shared_ptr<Common::GizmoGraphicContext> _mainCanvas;
    boost::shared_ptr<ToolNotebook> _toolBar;
};
}