#include "CurrentLevelNodesTab.h"

#include "boost/make_shared.hpp"

#include "Utils/Utils.h"

#include "Common/CommonData.h"
#include "Common/LanguageDependents.h"
#include "Items/Graphic/GraphicContext.h"
#include "Items/PropertyGridPanel/PropertyGridFactory.h"
#include "Items/PropertyGridPanel/SceneNodePropertyGrid.h"
#include "ThreadTypes/ThreadTypes.h"

#define ON_3D_OBJECT_WAS_SELECTED_HANDLER \
    L"on 3d object was selected property grid handler"
#define ON_NODE_REMOVE_HANDLER L"on map update property grid handler"

using namespace std;

namespace Watercolor {
void CurrentLevelNodesTab::addPropertyGrid(boost::any node)
{
    auto sceneNode = boost::any_cast<boost::shared_ptr<irr::scene::ISceneNode>>(node);

    _functionsProcessingManager->addFuctionToQueue(ThreadTypes::UI_THREAD, [=]() {
        this->_selectedObjectPropertyGrid = nullptr;
        this->_selectedObjectPropertyGrid = PropertyGridFactory::getGrid(
            this, sceneNode);
        if (_selectedObjectPropertyGrid.get()) {
            this->_sizer->Add(_selectedObjectPropertyGrid.get());
        }
        this->_sizer->Layout();
    });
}

void CurrentLevelNodesTab::onNodeLoadedHandler(boost::any)
{
    // if selected node was remove - we should remove associated property grid
    if (_selectedObjectPropertyGrid) {
        wstring selectedObjectName = _selectedObjectPropertyGrid->getObjectName();
        if (!_graphicContext->isNodeExists(selectedObjectName)) {
            _functionsProcessingManager->addFuctionToQueue(
                ThreadTypes::UI_THREAD, [=]() {
                    this->_selectedObjectPropertyGrid = nullptr;
                    this->_sizer->RecalcSizes();
                });
        }
    }
}

CurrentLevelNodesTab::CurrentLevelNodesTab(wxWindow* parent, boost::shared_ptr<IFunctionsProcessingManager> functionsProcessingManager)
    : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxSize(9999, 9999))
    , _functionsProcessingManager(functionsProcessingManager)
{
    this->SetBackgroundColour(wxColour(0xFF, 0xFF, 0xFF));
    _sizer = new wxStaticBoxSizer(wxVERTICAL, this);
    this->SetSizer(_sizer);

    _functionsProcessingManager->addFuctionToQueue(
        ThreadTypes::RENDER_THREAD, [=]() {
            this->_graphicContext = (Common::GizmoGraphicContext*)(wxTheApp->GetTopWindow()->FindWindowById(
                Common::ElementsIDs::MAIN_CANVAS));

            // Event handlers
            _graphicContext->onNodeSelected.addEventHandler(ON_3D_OBJECT_WAS_SELECTED_HANDLER,
                boost::bind(&CurrentLevelNodesTab::addPropertyGrid, this, boost::placeholders::_1));
            _graphicContext->onAfterNodeRemove.addEventHandler(ON_NODE_REMOVE_HANDLER,
                boost::bind(&CurrentLevelNodesTab::onNodeLoadedHandler, this, boost::placeholders::_1));
        });

    // Components
    // Models list
    _sizer->AddSpacer(10);
    _sizer->Add(new wxStaticText(
        this, wxID_ANY, TOOLS_BAR_LEVELS_ITEMS_EDITOR_CURRENT_LEVEL_ITEMS_LABEL));

    _sizer->AddSpacer(10);
    _levelItems = boost::make_shared<CurrentLevelItemsTree>(this, _functionsProcessingManager);
    _sizer->Add(_levelItems.get(), wxALL);
    // Properties of models managment
    _sizer->AddSpacer(10);
    _sizer->Add(new wxStaticText(
        this, wxID_ANY, TOOLS_BAR_LEVELS_ITEMS_EDITOR_SELECTED_ITEM_PROPERTIES));
    _sizer->AddSpacer(10);
}

CurrentLevelNodesTab::~CurrentLevelNodesTab()
{
    _graphicContext->onNodeSelected.removeEventHandler(ON_3D_OBJECT_WAS_SELECTED_HANDLER);
    _graphicContext->onAfterNodeRemove.removeEventHandler(ON_NODE_REMOVE_HANDLER);
}
}