#include "CurrentLevelItemsTree.h"

#include "TCoreFacade.h"
#include "TUtils.h"

#include "boost/function.hpp"

#include "Common/CommonData.h"
#include "Items/Graphic/GraphicContext.h"
#include "ThreadTypes/ThreadTypes.h"

#define ON_LEVEL_LOADED_HANDLER L"on level loaded tree handler"
#define ON_NODE_LOADED_HANDLER L"on node added tree handler"
#define ON_NODE_REMOED_HANDLER L"on node removed tree handler"
#define ON_NODE_PROPERTY_CHANGED_HANDLER \
    L"on node preoperty chnaged tree handler"

namespace Watercolor {
void CurrentLevelItemsTree::updateCurrentLevelItemsTree(void*)
{
    if (_graphicContext) {
        boost::function<void(wxTreeItemId subID,
            boost::property_tree::wptree subTree)>
            createMap =
                [&](wxTreeItemId subID, boost::property_tree::wptree subTree) {
                    for (auto item : subTree) {
                        if (!item.first.empty()) {
                            auto newItem = this->AppendItem(subID, item.first);
                            createMap(newItem, item.second);
                        }
                    }
                };

        this->DeleteAllItems();
        this->AddRoot(L"Level");

        auto objectsTree = _graphicContext->getObjectsTree();
        auto rootItem = this->GetRootItem();

        createMap(rootItem, objectsTree);
        this->ExpandAll();
    }
}

void CurrentLevelItemsTree::onNodeWasAdded(boost::shared_ptr<irr::scene::ISceneNode> newNode)
{
    auto newNodeName = TUtils::toWstring(newNode->getName());

    updateCurrentLevelItemsTree(nullptr);

    this->_graphicContext->selectNode(newNodeName);
}

void CurrentLevelItemsTree::OnBeginDrag(wxTreeEvent& eventData)
{
    auto sourceItemID = eventData.GetItem();
    _sourceItemName = this->GetItemText(sourceItemID);
    if (_sourceItemName != L"Level") {
        eventData.Allow();
    }
}

void CurrentLevelItemsTree::OnEndDrag(wxTreeEvent& eventData)
{
    auto destinationItemID = eventData.GetItem();

    if (destinationItemID) {
        std::wstring destinationItemName = this->GetItemText(destinationItemID).ToStdWstring();

        if (destinationItemName != L"Level" && _sourceItemName != destinationItemName) {
            _graphicContext->changeParent(_sourceItemName, destinationItemName);
        }
    }
}

void CurrentLevelItemsTree::onKeyUp(wxKeyEvent& eventData)
{
    switch (eventData.GetKeyCode()) {
    case wxKeyCode::WXK_DELETE: {
        auto selectedItemID = this->GetSelection();
        if (selectedItemID.IsOk()) {
            std::wstring selectedItemName = this->GetItemText(selectedItemID).ToStdWstring();

            _graphicContext->removeNode(selectedItemName);
        }
    }
    }
}

void CurrentLevelItemsTree::onListObjectWasSelected(wxTreeEvent& eventData)
{
    auto selectedItemID = this->GetSelection();

    if (selectedItemID.IsOk()) {
        std::wstring selectedItemName = this->GetItemText(selectedItemID).ToStdWstring();

        this->_graphicContext->selectNode(selectedItemName);
    }
}

CurrentLevelItemsTree::CurrentLevelItemsTree(wxWindow* parent, boost::shared_ptr<IFunctionsProcessingManager> functionsProcessingManager)
    : wxTreeCtrl(parent, Common::ElementsIDs::CURRENT_LEVEL_OBJECTS_LIST,
        wxDefaultPosition, wxSize(350, 200))
    , _functionsProcessingManager(functionsProcessingManager)
{
    _functionsProcessingManager->addFuctionToQueue(
        ThreadTypes::RENDER_THREAD, [=]() {
            this->_graphicContext = (Common::GizmoGraphicContext*)(wxTheApp->GetTopWindow()->FindWindowById(
                Common::ElementsIDs::MAIN_CANVAS));

            _graphicContext->onLevelLoaded.addEventHandler(ON_LEVEL_LOADED_HANDLER,
                boost::bind(&CurrentLevelItemsTree::updateCurrentLevelItemsTree, this, boost::placeholders::_1));
            _graphicContext->onNodeAdded.addEventHandler(ON_NODE_LOADED_HANDLER,
                boost::bind(&CurrentLevelItemsTree::onNodeWasAdded, this, boost::placeholders::_1));
            _graphicContext->onAfterNodeRemove.addEventHandler(ON_NODE_REMOED_HANDLER,
                boost::bind(&CurrentLevelItemsTree::updateCurrentLevelItemsTree, this, boost::placeholders::_1));
            _graphicContext->onNodePropertyChanged.addEventHandler(ON_NODE_PROPERTY_CHANGED_HANDLER,
                boost::bind(&CurrentLevelItemsTree::updateCurrentLevelItemsTree, this, boost::placeholders::_1));
        });
}

CurrentLevelItemsTree::~CurrentLevelItemsTree()
{
    _graphicContext->onLevelLoaded.removeEventHandler(ON_LEVEL_LOADED_HANDLER);
    _graphicContext->onNodeAdded.removeEventHandler(ON_NODE_LOADED_HANDLER);
    _graphicContext->onAfterNodeRemove.removeEventHandler(ON_NODE_REMOED_HANDLER);
    _graphicContext->onNodePropertyChanged.removeEventHandler(ON_NODE_PROPERTY_CHANGED_HANDLER);
}

wxBEGIN_EVENT_TABLE(CurrentLevelItemsTree, wxTreeCtrl)
    EVT_TREE_SEL_CHANGED(Common::ElementsIDs::CURRENT_LEVEL_OBJECTS_LIST,
        CurrentLevelItemsTree::onListObjectWasSelected)
        EVT_TREE_BEGIN_DRAG(Common::ElementsIDs::CURRENT_LEVEL_OBJECTS_LIST,
            CurrentLevelItemsTree::OnBeginDrag)
            EVT_TREE_END_DRAG(Common::ElementsIDs::CURRENT_LEVEL_OBJECTS_LIST,
                CurrentLevelItemsTree::OnEndDrag)
                EVT_KEY_UP(CurrentLevelItemsTree::onKeyUp) wxEND_EVENT_TABLE()
}