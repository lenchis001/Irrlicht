#include "PropertyGridFactory.h"

#include "GuiElementPropertyGrid.h"
#include "ImageGuiElementPropertyGrid.h"
#include "ListBoxGuiElementPropertyGrid.h"

#include "Utils/Convert.h"

namespace GUIEditor {
boost::shared_ptr<GuiElementPropertyGrid> PropertyGridFactory::getGrid(
    wxWindow* parent, boost::shared_ptr<irr::gui::IGUIElement> element, boost::shared_ptr<GuiEditorGraphicContext> graphicContext)
{
    boost::shared_ptr<GuiElementPropertyGrid> grid(nullptr);

    if (element) {
        std::wstring nodeName = Utils::toWstring(element->getName());

        if (element) {
            auto nodeType = element->getType();

            switch (nodeType) {
            case irr::gui::EGUIET_IMAGE:
                grid = boost::make_shared<ImageGuiElementPropertyGrid>(parent, nodeName, graphicContext);
                break;
            case irr::gui::EGUIET_LIST_BOX:
                grid = boost::make_shared<ListBoxGuiElementPropertyGrid>(parent, nodeName, graphicContext);
                break;
            case irr::gui::EGUIET_BUTTON:
            case irr::gui::EGUIET_CHECK_BOX:
            case irr::gui::EGUIET_COMBO_BOX:
            case irr::gui::EGUIET_CONTEXT_MENU:
            case irr::gui::EGUIET_MENU:
            case irr::gui::EGUIET_EDIT_BOX:
            case irr::gui::EGUIET_FILE_OPEN_DIALOG:
            case irr::gui::EGUIET_COLOR_SELECT_DIALOG:
            case irr::gui::EGUIET_IN_OUT_FADER:
            case irr::gui::EGUIET_MESH_VIEWER:
            case irr::gui::EGUIET_MESSAGE_BOX:
            case irr::gui::EGUIET_MODAL_SCREEN:
            case irr::gui::EGUIET_SCROLL_BAR:
            case irr::gui::EGUIET_SPIN_BOX:
            case irr::gui::EGUIET_STATIC_TEXT:
            case irr::gui::EGUIET_TAB:
            case irr::gui::EGUIET_TAB_CONTROL:
            case irr::gui::EGUIET_TABLE:
            case irr::gui::EGUIET_TOOL_BAR:
            case irr::gui::EGUIET_TREE_VIEW:
            case irr::gui::EGUIET_WINDOW:
            case irr::gui::EGUIET_ELEMENT:
            case irr::gui::EGUIET_ROOT:
            case irr::gui::EGUIET_COUNT:
            case irr::gui::EGUIET_FORCE_32_BIT:
                grid = boost::make_shared<GuiElementPropertyGrid>(parent, nodeName, graphicContext);
            default:
                break;
            }
        }
    }

    return grid;
}
}