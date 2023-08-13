#pragma once

#include "wx/wx.h"

#include "boost/shared_ptr.hpp"

#include "Items/ClientAreaPanel/Items/GuiEditorGraphicContext/GuiEditorGraphicContext.h"
#include "Items/PropertyGridPanel/GuiElementPropertyGrid.h"

namespace GUIEditor {
class CurrentScreenNodesTab : public wxScrolled<wxPanel> {
public:
    CurrentScreenNodesTab(wxWindow* parent, boost::shared_ptr<GuiEditorGraphicContext> graphicContext, boost::shared_ptr<IFunctionsProcessingManager> functionsProcessingManager);
    ~CurrentScreenNodesTab();

private:
    void _onUseGridChange(wxCommandEvent& eventData);

    void _onGuiElementSelected(boost::shared_ptr<irr::gui::IGUIElement> element);
    void _onClear(void* _);

    wxSizer* _sizer;

    boost::shared_ptr<wxPropertyGrid> _selectedElementPropertyGrid;
    boost::shared_ptr<GuiEditorGraphicContext> _graphicContext;
    boost::shared_ptr<wxCheckBox> _useGridCheckBox;
    std::vector<boost::shared_ptr<wxWindow>> _controls;
    boost::shared_ptr<IFunctionsProcessingManager> _functionsProcessingManager;

    wxDECLARE_EVENT_TABLE();
};
}
