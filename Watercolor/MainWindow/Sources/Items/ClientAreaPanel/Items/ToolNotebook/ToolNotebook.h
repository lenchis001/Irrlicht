#pragma once

#include "memory"

#include "wx/notebook.h"
#include <wx/wx.h>

#include "Items/LevelItemsEditor/LevelItemsEditor.h"

namespace Watercolor {
class ToolNotebook : public wxNotebook {
    std::unique_ptr<CurrentLevelNodesTab> _levelItemsEditor;

public:
    ToolNotebook(wxWindow* parent, boost::shared_ptr<IFunctionsProcessingManager> functionsProcessingManager);
};
}