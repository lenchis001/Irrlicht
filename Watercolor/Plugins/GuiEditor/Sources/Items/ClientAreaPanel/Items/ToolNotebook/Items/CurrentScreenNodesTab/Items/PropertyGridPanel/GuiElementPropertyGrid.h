#pragma once

#include "wx/propgrid/propgrid.h"
#include "wx/wx.h"

#include "map"
#include "string"

#include "Items/ClientAreaPanel/Items/GuiEditorGraphicContext/GuiEditorGraphicContext.h"

namespace GUIEditor {
typedef boost::function<void(wxPGProperty*)> PropertyChangeHandler;

class GuiElementPropertyGrid : public wxPropertyGrid {
public:
    GuiElementPropertyGrid(wxWindow* parent, std::wstring elementName, boost::shared_ptr<GuiEditorGraphicContext> graphicContext);
    ~GuiElementPropertyGrid();

protected:
    std::wstring _elementName;

    GuiEditorGraphicContext* _graphicContext;

    void addPropertyChangeHandler(std::wstring propertyName,
        PropertyChangeHandler handler);
    void removePropertyChangeHandler(std::wstring propertyName);

private:
    void _addNameProperty();
    void _addPositionProperty();
    void _addTextProperty();

    void _changeNamePropertyHandler(wxPGProperty* eventProperty);
    void _changeTextPropertyHandler(wxPGProperty* eventProperty);
    void _changePositionPropertyHandler(wxPGProperty* eventProperty);
    void _onChangePositionPropertyOutside(boost::shared_ptr<irr::gui::IGUIElement> _);

    irr::core::rect<irr::s32> _parseRect(const std::wstring& textRect);

    void _onPropertyWasSet(wxPropertyGridEvent& eventData);
    std::map<std::wstring, PropertyChangeHandler> _changeHandlers;

    wxDECLARE_EVENT_TABLE();
};
}