#include "ToolNotebook.h"

#include "boost/make_unique.hpp"

#include "Common/LanguageDependents.h"

namespace Watercolor {
ToolNotebook::ToolNotebook(wxWindow* parent, boost::shared_ptr<IFunctionsProcessingManager> functionsProcessingManager)
    : wxNotebook(parent, wxID_ANY, wxDefaultPosition)
{
    SetMinSize({ 400, 400 });

    _currentLevelNodesTab = boost::make_unique<CurrentLevelNodesTab>(this, functionsProcessingManager);
    this->AddPage(_currentLevelNodesTab.get(), TOOLS_BAR_LEVELS_ITEMS_EDITOR_ON_LEVEL_ITEMS);

    _addingObjectsTab = boost::make_unique<AddingObjectsTab>(this, functionsProcessingManager);
    this->AddPage(_addingObjectsTab.get(), TOOLS_BAR_LEVELS_ITEMS_EDITOR_ADD_OBJECT);
}
}