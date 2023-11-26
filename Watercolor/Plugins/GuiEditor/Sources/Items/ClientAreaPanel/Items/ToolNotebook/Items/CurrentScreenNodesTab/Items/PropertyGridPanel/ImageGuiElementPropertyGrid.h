#pragma once

#include "GuiElementPropertyGrid.h"

namespace GUIEditor {
class ImageGuiElementPropertyGrid : public GuiElementPropertyGrid {
public:
    ImageGuiElementPropertyGrid(wxWindow* parent, std::wstring elementName, boost::shared_ptr<GuiEditorGraphicContext> graphicContext);
    ~ImageGuiElementPropertyGrid();

private:
    void _addPathProperty();
    void _addScaleProperty();
    void _changePathPropertyHandler(wxPGProperty* eventProperty);
    void _changeScalePropertyHandler(wxPGProperty* eventProperty);
};
}