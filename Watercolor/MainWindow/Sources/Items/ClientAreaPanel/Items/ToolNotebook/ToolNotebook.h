#pragma once

#include "memory"

#include "wx/notebook.h"
#include <wx/wx.h>

#include "Items/AddingObjectsTab/AddingObjectsTab.h"
#include "Items/CurrentLevelNodesTab/CurrentLevelNodesTab.h"

namespace Watercolor {
class ToolNotebook : public wxNotebook {
    std::unique_ptr<CurrentLevelNodesTab> _currentLevelNodesTab;
    std::unique_ptr<AddingObjectsTab> _addingObjectsTab;

public:
    ToolNotebook(wxWindow* parent, boost::shared_ptr<IFunctionsProcessingManager> functionsProcessingManager);
};
}