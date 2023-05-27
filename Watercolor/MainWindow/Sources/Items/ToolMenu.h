#pragma once

#include <wx/wx.h>

#include "boost/any.hpp"
#include "boost/smart_ptr.hpp"

#include "TCoreFacade.h"

#include "Items/Graphic/GizmoGraphicContext.h"
#include "ProjectDataManager/IProjectDataManager.h"

namespace Watercolor {
class ToolMenu : public wxToolBar {
public:
    ToolMenu(
        wxWindow* parent, 
        boost::shared_ptr<IFunctionsProcessingManager> functionsProcessingManager, 
        boost::shared_ptr<Common::IProjectDataManager> projectDataManager);
    ~ToolMenu();

private:
    Common::GizmoGraphicContext* _graphicContext;

    boost::shared_ptr<Common::IProjectDataManager> _projectDataManager;

    void _openVisualCode(wxCommandEvent& eventData);
    void _playLauncher(wxCommandEvent& eventData);

    void _selectGizmo(wxCommandEvent& eventData);
    void _moveGizmo(wxCommandEvent& eventData);
    void _rotateGizmo(wxCommandEvent& eventData);
    void _scaleGizmo(wxCommandEvent& eventData);

    void onGizmoManipulationTypeChanged(boost::any value);

    wxDECLARE_EVENT_TABLE();
};
}