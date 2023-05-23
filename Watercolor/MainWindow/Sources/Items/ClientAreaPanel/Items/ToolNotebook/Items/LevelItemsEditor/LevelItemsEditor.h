#pragma once

#include "wx\notebook.h"
#include <wx/wx.h>

#include "boost/smart_ptr.hpp"

#include "TCoreFacade.h"

#include "Items/AddingObjectsTab/AddingObjectsTab.h"
#include "Items/CurrentLevelNodesTab/CurrentLevelNodesTab.h"

namespace Watercolor {
class LevelItemsEditor : public wxNotebook {
public:
    LevelItemsEditor(wxWindow* parent, boost::shared_ptr<IFunctionsProcessingManager> functionsProcessingManager);
    ~LevelItemsEditor();

private:
    boost::shared_ptr<CurrentLevelNodesTab> _currentLevelNodes;
    boost::shared_ptr<AddingObjectsTab> _addObjects;

    void onNodeLoadedHandler(boost::any);

    Common::GraphicContext* _graphicContext;
};
}