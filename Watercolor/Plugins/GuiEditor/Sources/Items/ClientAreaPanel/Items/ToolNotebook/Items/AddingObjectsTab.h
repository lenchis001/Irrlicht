#pragma once

#include "vector"

#include "wx/wx.h"

#include "boost/shared_ptr.hpp"

#include "Items/ClientAreaPanel/Items/GuiEditorGraphicContext/GuiEditorGraphicContext.h"

namespace GUIEditor {
class AddingObjectsTab : public wxScrolled<wxPanel> {
public:
    AddingObjectsTab(wxWindow* parent, boost::shared_ptr<GuiEditorGraphicContext> graphicContext);

private:
    void _onAddButton(wxCommandEvent& event);
    void _onAddImage(wxCommandEvent& event);
    void _onAddWindow(wxCommandEvent& event);
    void _onAddLabel(wxCommandEvent& event);
    void _onAddTextbox(wxCommandEvent& event);
    void _onAddCheckBox(wxCommandEvent& event);
    void _onAddListBox(wxCommandEvent& event);

    wxSizer* _sizer;
    std::vector<boost::shared_ptr<wxWindow>> _controls;

    boost::shared_ptr<GuiEditorGraphicContext> _graphicContext;

    wxDECLARE_EVENT_TABLE();
};
}
