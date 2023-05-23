#include "LevelItemsEditor.h"
#include "TCoreFacade.h"

#include "boost/make_shared.hpp"

#include "Utils/Utils.h"

#include "Common/CommonData.h"
#include "Common/LanguageDependents.h"

#define ON_NODE_WAS_LOADED_HANDLER_NAME \
    L"level items editor tab node was loadded handler"

namespace Watercolor {
void LevelItemsEditor::onNodeLoadedHandler(boost::any)
{
    int index = this->FindPage(_currentLevelNodes.get());
    this->SetSelection(index);
}

LevelItemsEditor::LevelItemsEditor(wxWindow* parent, boost::shared_ptr<IFunctionsProcessingManager> functionsProcessingManager)
    : wxNotebook(parent, wxID_ANY, wxDefaultPosition, wxSize(500, 9999),
        wxBK_RIGHT)
{
    _currentLevelNodes = boost::make_shared<CurrentLevelNodesTab>(this, functionsProcessingManager);
    this->AddPage(_currentLevelNodes.get(),
        TOOLS_BAR_LEVELS_ITEMS_EDITOR_ON_LEVEL_ITEMS);

    _addObjects = boost::make_shared<AddingObjectsTab>(this, functionsProcessingManager);
    this->AddPage(_addObjects.get(), TOOLS_BAR_LEVELS_ITEMS_EDITOR_ADD_OBJECT);

    _graphicContext = (Common::GraphicContext*)(wxTheApp->GetTopWindow()->FindWindowById(
        Common::ElementsIDs::MAIN_CANVAS));

    _graphicContext->onNodeAdded.addEventHandler(ON_NODE_WAS_LOADED_HANDLER_NAME,
        boost::bind(&LevelItemsEditor::onNodeLoadedHandler, this, boost::placeholders::_1));
}

LevelItemsEditor::~LevelItemsEditor()
{
    _graphicContext->onNodeAdded.removeEventHandler(ON_NODE_WAS_LOADED_HANDLER_NAME);
}
}