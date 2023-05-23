#include "ToolNotebook.h"

#include "boost/make_unique.hpp"

#include "Common/LanguageDependents.h"

namespace Watercolor {
ToolNotebook::ToolNotebook(wxWindow* parent, boost::shared_ptr<IFunctionsProcessingManager> functionsProcessingManager)
    : wxNotebook(parent, wxID_ANY, wxDefaultPosition)
{
    SetMinSize({ 400, 400 });

    _levelItemsEditor = boost::make_unique<LevelItemsEditor>(this, functionsProcessingManager);
    this->AddPage(_levelItemsEditor.get(), TOOLS_BAR_LEVELS_ITEMS_EDITOR);
}
}