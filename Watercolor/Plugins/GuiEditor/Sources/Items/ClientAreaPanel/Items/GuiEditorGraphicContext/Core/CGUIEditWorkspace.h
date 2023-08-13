#pragma once

#include "IGUIElement.h"

#include "Utils/EventHandler.h"

namespace GUIEditor {

class GuiEditorGraphicContext;

//! Adding the GUI Editor Workspace to an element allows you
/** to create, edit, load and save any elements supported
		by any loaded factories.
		When you add it without a parent (to the root element)
		it will also allow you to edit, load and save settings in
		the current skin.
	*/

// custom events
enum EGUIEDIT_CUSTOM_EVENTS {
    EGUIEDCE_ATTRIB_EDITOR = MAKE_IRR_ID('g', 'A', 't', 't'),
    EGUIEDCE_OPTION_EDITOR = MAKE_IRR_ID('g', 'O', 'p', 't'),
    EGUIEDCE_ENV_EDITOR = MAKE_IRR_ID('g', 'E', 'n', 'v')
};

class CGUIEditWorkspace : public irr::gui::IGUIElement {
public:
    //! constructor
    CGUIEditWorkspace(GuiEditorGraphicContext* graphicContext, boost::shared_ptr<irr::gui::IGUIEnvironment> environment, irr::s32 id = -1, boost::shared_ptr<irr::gui::IGUIElement> parent = 0);

    //! destructor
    ~CGUIEditWorkspace();

    //! called if an event happened.
    virtual bool OnEvent(const irr::SEvent& event);

    //! Removes a child.
    virtual void removeChild(boost::shared_ptr<irr::gui::IGUIElement> child);

    //! draws the element and its children
    virtual void draw();

    //! Sets the menu command id's
    /** The GUI editor defaults to command ID's from 0xED17 to 0xED17+EGUIEDMC_COUNT
		In the rare case that these are already in use and you wish to use menus
		while the editor is present you can set a new offset here.
		*/
    //! grid drawing...
    virtual void setDrawGrid(bool drawGrid);
    virtual void setGridSize(const irr::core::dimension2di& gridSize);
    virtual void setUseGrid(bool useGrid);
    virtual bool getUseGrid();

    //! returns the first editable element under the mouse
    virtual boost::shared_ptr<IGUIElement> getEditableElementFromPoint(boost::shared_ptr<IGUIElement> start, const irr::core::position2di& point, irr::s32 index = 0);

    //! selecting elements
    virtual void setSelectedElement(boost::shared_ptr<IGUIElement> sel);
    virtual void selectNextSibling();
    virtual void selectPreviousSibling();

    //! returns the selected element
    virtual boost::shared_ptr<IGUIElement> getSelectedElement();

    //! copies the xml of the selected element and all children to the clipboard
    virtual void CopySelectedElementXML();

    //! copies the xml of the selected element and all children to the clipboard
    virtual void PasteXMLToSelectedElement();

    Utils::EventHandler<boost::shared_ptr<irr::gui::IGUIElement>> onGuiElementSelected;

private:
    enum EGUIEDIT_MODE {
        // when we are currently selecting an element
        EGUIEDM_SELECT = 0,
        // selecting a new parent for the selected element
        EGUIEDM_SELECT_NEW_PARENT,

        // moving the selected element
        EGUIEDM_MOVE,
        // resizing the selected element
        EGUIEDM_RESIZE_TL,
        EGUIEDM_RESIZE_T,
        EGUIEDM_RESIZE_TR,
        EGUIEDM_RESIZE_R,
        EGUIEDM_RESIZE_BR,
        EGUIEDM_RESIZE_B,
        EGUIEDM_RESIZE_BL,
        EGUIEDM_RESIZE_L
    };

    enum EGUIEDIT_MENUCOMMANDS {
        //! file commands
        EGUIEDMC_FILE_NEW,
        EGUIEDMC_FILE_LOAD,
        EGUIEDMC_FILE_SAVE,
        //! edit menu
        EGUIEDMC_CUT_ELEMENT,
        EGUIEDMC_COPY_ELEMENT,
        EGUIEDMC_PASTE_ELEMENT,
        EGUIEDMC_DELETE_ELEMENT,
        EGUIEDMC_SET_PARENT,
        EGUIEDMC_BRING_TO_FRONT,
        EGUIEDMC_SAVE_ELEMENT,
        //! grid
        EGUIEDMC_TOGGLE_EDITOR,

        EGUIEDMC_INSERT_XML,

        //! number of menu options
        EGUIEDMC_COUNT
    };

    EGUIEDIT_MODE getModeFromPos(irr::core::position2di p);

    EGUIEDIT_MODE CurrentMode;
    EGUIEDIT_MODE MouseOverMode;
    irr::core::position2di DragStart;
    irr::core::position2di StartMovePos;
    irr::core::rect<irr::s32> SelectedArea;

    irr::core::dimension2di GridSize;
    irr::s32 MenuCommandStart;
    bool DrawGrid, UseGrid;

    boost::shared_ptr<irr::gui::IGUIElement> MouseOverElement,
        SelectedElement;

    GuiEditorGraphicContext* _graphicContext;

    irr::core::rect<irr::s32> TLRect;
    irr::core::rect<irr::s32> TRRect;
    irr::core::rect<irr::s32> TopRect;
    irr::core::rect<irr::s32> BLRect;
    irr::core::rect<irr::s32> LRect;
    irr::core::rect<irr::s32> RRect;
    irr::core::rect<irr::s32> BRRect;
    irr::core::rect<irr::s32> BRect;
};

}
