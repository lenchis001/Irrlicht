#pragma once

#include "GuiElementPropertyGrid.h"

namespace GUIEditor {
class ListBoxGuiElementPropertyGrid : public GuiElementPropertyGrid {
public:
    ListBoxGuiElementPropertyGrid(wxWindow* parent, std::wstring elementName, boost::shared_ptr<GuiEditorGraphicContext> graphicContext);

private:
    void _addOptionsProperty();
    void _changeOptionsPropertyHandler(wxPGProperty* eventProperty);
};
}