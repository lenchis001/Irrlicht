#include "ToolNotebook.h"

#include "boost/make_shared.hpp"

#include "Utils/LanguageDependents.h"

#include "Items/AddingObjectsTab.h"
#include "Items/CurrentScreenNodesTab/CurrentScreenNodesTab.h"

namespace GUIEditor {
ToolNotebook::ToolNotebook(wxWindow* parent, boost::shared_ptr<GuiEditorGraphicContext> graphicContext, boost::shared_ptr<IFunctionsProcessingManager> functionsProcessingManager)
    : wxNotebook(parent, wxID_ANY, wxDefaultPosition, wxSize(400, 0))
{
    this->SetBackgroundColour(wxColor(230, 230, 230));

    auto elementsAddingPanel = boost::make_shared<CurrentScreenNodesTab>(this, graphicContext, functionsProcessingManager);
    _elementsAddingPanel.push_back(elementsAddingPanel);
    this->AddPage(elementsAddingPanel.get(), GUI_EDITOR_ELEMENTS_PAGE_TITLE);

    auto environmentSettingsPanel = boost::make_shared<AddingObjectsTab>(this, graphicContext);
    _elementsAddingPanel.push_back(environmentSettingsPanel);
    this->AddPage(environmentSettingsPanel.get(), GUI_EDITOR_ADDING_ELEMENTS_TITLE);
}
}