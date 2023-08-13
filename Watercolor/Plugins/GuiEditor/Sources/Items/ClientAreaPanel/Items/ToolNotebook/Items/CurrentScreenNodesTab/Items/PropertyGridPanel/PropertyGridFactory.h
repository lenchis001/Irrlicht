#pragma once

#include "wx/propgrid/propgrid.h"
#include "wx/wx.h"

#include "irrlicht.h"

#include "boost/smart_ptr.hpp"

#include "GuiElementPropertyGrid.h"

namespace GUIEditor {
class PropertyGridFactory {
public:
    static boost::shared_ptr<GuiElementPropertyGrid> getGrid(
        wxWindow* parent, boost::shared_ptr<irr::gui::IGUIElement> element, boost::shared_ptr<GuiEditorGraphicContext> graphicContext);
};
}
