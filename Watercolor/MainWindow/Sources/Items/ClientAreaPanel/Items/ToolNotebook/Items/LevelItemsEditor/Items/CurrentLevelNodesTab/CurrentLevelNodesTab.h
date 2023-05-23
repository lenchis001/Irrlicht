#pragma once

#include "wx/propgrid/propgrid.h"
#include <wx/wx.h>

#include "boost/shared_ptr.hpp"

#include "Items/CurrentLevelItemsTree.h"
#include "Items/Graphic/GraphicContext.h"
#include "Items/PropertyGridPanel/SceneNodePropertyGrid.h"

namespace Watercolor {
class CurrentLevelNodesTab : public wxPanel {
    wxStaticBoxSizer* _sizer;
    boost::shared_ptr<CurrentLevelItemsTree> _levelItems;
    boost::shared_ptr<SceneNodePropertyGrid> _selectedObjectPropertyGrid;
    Common::GizmoGraphicContext* _graphicContext;
    boost::shared_ptr<IFunctionsProcessingManager> _functionsProcessingManager;

    void addPropertyGrid(boost::any node);
    void onNodeLoadedHandler(boost::any);

public:
    CurrentLevelNodesTab(wxWindow* parent, boost::shared_ptr<IFunctionsProcessingManager> functionsProcessingManager);
    ~CurrentLevelNodesTab();
};
}