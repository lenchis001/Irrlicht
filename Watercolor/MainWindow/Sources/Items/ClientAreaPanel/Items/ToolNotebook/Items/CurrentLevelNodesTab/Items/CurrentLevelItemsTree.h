#pragma once

#include "string"

#include "wx/treectrl.h"
#include "wx/wx.h"

#include "Items/Graphic/GizmoGraphicContext.h"

namespace Watercolor {
class CurrentLevelItemsTree : public wxTreeCtrl {
public:
    CurrentLevelItemsTree(wxWindow* parent, boost::shared_ptr<IFunctionsProcessingManager> functionsProcessingManager);
    ~CurrentLevelItemsTree();

private:
    wxDECLARE_EVENT_TABLE();

    Common::GizmoGraphicContext* _graphicContext;

    std::wstring _sourceItemName;

    boost::shared_ptr<IFunctionsProcessingManager> _functionsProcessingManager;

    void onListObjectWasSelected(wxTreeEvent& eventData);
    void updateCurrentLevelItemsTree(void*);
    void onNodeWasAdded(boost::shared_ptr<irr::scene::ISceneNode> newNode);

    void OnBeginDrag(wxTreeEvent& eventData);
    void OnEndDrag(wxTreeEvent& eventData);
    void onKeyUp(wxKeyEvent& eventData);
};
}