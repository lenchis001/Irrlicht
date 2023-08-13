// Copyright (C) 2002-2012 Nikolaus Gebhardt / Gaz Davidson
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

// Thanks to Midnight for all his testing, bug fixes and patches :)

#include "CGUIEditWorkspace.h"
#include "CMemoryReadWriteFile.h"
#include "IFileSystem.h"
#include "IGUIEnvironment.h"
#include "IOSOperator.h"
#include "IReadFile.h"
#include "IVideoDriver.h"
#include "IXMLWriter.h"
#include "Items/ClientAreaPanel/Items/GuiEditorGraphicContext/GuiEditorGraphicContext.h"

using namespace irr;
using namespace irr::gui;

namespace GUIEditor {

//! constructor
CGUIEditWorkspace::CGUIEditWorkspace(GuiEditorGraphicContext* graphicContext, boost::shared_ptr<IGUIEnvironment> environment, s32 id, boost::shared_ptr<IGUIElement> parent)
    : IGUIElement(EGUIET_ELEMENT, environment, parent ? parent : environment->getRootGUIElement(), id, environment->getRootGUIElement()->getAbsolutePosition())
    , CurrentMode(EGUIEDM_SELECT)
    , MouseOverMode(EGUIEDM_SELECT)
    , GridSize(10, 10)
    , MenuCommandStart(0x3D17)
    , DrawGrid(false)
    , UseGrid(true)
    , MouseOverElement(0)
    , SelectedElement(0)
    , _graphicContext(graphicContext)
{
#ifdef _DEBUG
    setDebugName("CGUIEditWorkspace");
#endif

    // this element is never saved.
    setSubElement(true);

    // it resizes to fit a resizing window
    setAlignment(EGUIA_UPPERLEFT, EGUIA_LOWERRIGHT, EGUIA_UPPERLEFT, EGUIA_LOWERRIGHT);
}

//! destructor
CGUIEditWorkspace::~CGUIEditWorkspace()
{
}

CGUIEditWorkspace::EGUIEDIT_MODE CGUIEditWorkspace::getModeFromPos(core::position2di p)
{
    if (SelectedElement) {
        core::rect<s32> r = SelectedElement->getAbsolutePosition();

        if (TLRect.isPointInside(p))
            return EGUIEDM_RESIZE_TL;

        else if (TRRect.isPointInside(p))
            return EGUIEDM_RESIZE_TR;

        else if (BLRect.isPointInside(p))
            return EGUIEDM_RESIZE_BL;

        else if (BRRect.isPointInside(p))
            return EGUIEDM_RESIZE_BR;

        else if (TopRect.isPointInside(p))
            return EGUIEDM_RESIZE_T;

        else if (BRect.isPointInside(p))
            return EGUIEDM_RESIZE_B;

        else if (LRect.isPointInside(p))
            return EGUIEDM_RESIZE_L;

        else if (RRect.isPointInside(p))
            return EGUIEDM_RESIZE_R;

        else if (getEditableElementFromPoint(SelectedElement, p) == SelectedElement)
            return EGUIEDM_MOVE;

        else
            return EGUIEDM_SELECT;
    }

    return EGUIEDM_SELECT;
}

boost::shared_ptr<IGUIElement> CGUIEditWorkspace::getEditableElementFromPoint(boost::shared_ptr<IGUIElement> start, const core::position2di& point, s32 index)
{
    boost::shared_ptr<IGUIElement> target = 0;

    // we have to search from back to front.

    core::list<boost::shared_ptr<IGUIElement>>::ConstIterator it = start->getChildren().getLast();
    s32 count = 0;
    while (it != start->getChildren().end()) {
        target = getEditableElementFromPoint((*it), point);
        if (target) {
            if (!target->isSubElement() && !isMyChild(target) && target.get() != this && target->getType() != irr::gui::EGUI_ELEMENT_TYPE::EGUIET_COUNT) {
                if (index == count)
                    return target;
                else
                    count++;
            } else
                target = 0;
        }
        --it;
    }

    if (start->getAbsolutePosition().isPointInside(point))
        target = start;

    return target;
}

void CGUIEditWorkspace::setSelectedElement(boost::shared_ptr<IGUIElement> sel)
{
    boost::shared_ptr<IGUIEnvironment> lockedEnvironment = getSharedEnvironment();
    boost::shared_ptr<IGUIElement> focus = lockedEnvironment->getFocus();
    // we only give focus back to children
    if (!isMyChild(focus))
        focus = 0;

    if (SelectedElement != getParent()) {
        if (SelectedElement != sel) {
            SelectedElement = sel;
        }
    } else
        SelectedElement = 0;

    if (focus)
        lockedEnvironment->setFocus(focus);
    else
        lockedEnvironment->setFocus(getSharedThis());

    onGuiElementSelected.callHandlers(SelectedElement);
}

boost::shared_ptr<IGUIElement> CGUIEditWorkspace::getSelectedElement()
{
    return SelectedElement;
}
void CGUIEditWorkspace::selectNextSibling()
{
    boost::shared_ptr<IGUIElement> p = 0;

    if (SelectedElement && SelectedElement->getParent())
        p = SelectedElement->getParent();
    else
        p = getParent();

    core::list<boost::shared_ptr<IGUIElement>>::ConstIterator it = p->getChildren().begin();
    // find selected element
    if (SelectedElement)
        while (*it != SelectedElement)
            ++it;
    if (it != p->getChildren().end())
        ++it;
    // find next non sub-element
    while (it != p->getChildren().end() && (*it)->isSubElement())
        ++it;

    if (it != p->getChildren().end())
        setSelectedElement(*it);
}
void CGUIEditWorkspace::selectPreviousSibling()
{
    boost::shared_ptr<IGUIElement> p = 0;

    if (SelectedElement && SelectedElement->getParent())
        p = SelectedElement->getParent();
    else
        p = getParent();

    core::list<boost::shared_ptr<IGUIElement>>::ConstIterator it = p->getChildren().getLast();
    // find selected element
    if (SelectedElement)
        while (*it != SelectedElement)
            --it;
    if (it != p->getChildren().end())
        --it;
    // find next non sub-element
    while (it != p->getChildren().end() && (*it)->isSubElement())
        --it;

    if (it != p->getChildren().end())
        setSelectedElement(*it);
}

//! called if an event happened.
bool CGUIEditWorkspace::OnEvent(const SEvent& e)
{
    IGUIFileOpenDialog* dialog = 0;
    switch (e.EventType) {
    case EET_KEY_INPUT_EVENT:
        if (!e.KeyInput.PressedDown) {
            switch (e.KeyInput.Key) {
            case KEY_DELETE:
                if (SelectedElement) {
                    boost::shared_ptr<IGUIElement> el = SelectedElement;
                    setSelectedElement(0);
                    MouseOverElement = 0;
                    el->remove();
                }
                break;
            case KEY_KEY_X:
                if (e.KeyInput.Control && SelectedElement) {
                    // cut
                    CopySelectedElementXML();
                    // delete element
                    boost::shared_ptr<IGUIElement> el = SelectedElement;
                    setSelectedElement(0);
                    MouseOverElement = 0;
                    el->remove();
                }
                break;
            case KEY_KEY_C:
                // copy
                if (e.KeyInput.Control && SelectedElement) {
                    CopySelectedElementXML();
                }
                break;
            case KEY_KEY_V:
                // paste
                if (e.KeyInput.Control) {
                    PasteXMLToSelectedElement();
                }
                break;
            default:
                break;
            }

            return true;
        }
        break;

    case EET_MOUSE_INPUT_EVENT: {
        boost::shared_ptr<IGUIElement> lockedParent = getParent();

        switch (e.MouseInput.Event) {
        case EMIE_MOUSE_WHEEL: {
            f32 wheel = e.MouseInput.Wheel;

            if (wheel > 0)
                selectPreviousSibling();
            else
                selectNextSibling();
        } break;
        case EMIE_LMOUSE_PRESSED_DOWN: {
            core::position2di p = core::position2di(e.MouseInput.X, e.MouseInput.Y);

            boost::shared_ptr<IGUIElement> newSelection = getElementFromPoint(p);

            if (newSelection.get() != this && isMyChild(newSelection)) // redirect event
            {
                getSharedEnvironment()->setFocus(newSelection);
                return true;
            }

            // hide the gui editor
            if (CurrentMode == EGUIEDM_SELECT) {
                if (SelectedElement) {
                    // start moving or dragging
                    CurrentMode = getModeFromPos(p);

                    if (CurrentMode == EGUIEDM_MOVE)
                        StartMovePos = SelectedElement->getAbsolutePosition().UpperLeftCorner;

                    DragStart = p;
                    SelectedArea = SelectedElement->getAbsolutePosition();
                }

                if (CurrentMode < EGUIEDM_MOVE) {
                    // selecting an element...
                    MouseOverElement = getEditableElementFromPoint(lockedParent, p);

                    if (MouseOverElement == lockedParent)
                        MouseOverElement = 0;

                    setSelectedElement(MouseOverElement);
                }
            }

            break;
        }
        case EMIE_LMOUSE_LEFT_UP:

            // make window visible again
            if (CurrentMode == EGUIEDM_SELECT_NEW_PARENT) {
                if (SelectedElement) {
                    MouseOverElement = getEditableElementFromPoint(lockedParent,
                        core::position2di(e.MouseInput.X, e.MouseInput.Y));
                    if (MouseOverElement) {
                        MouseOverElement->addChild(SelectedElement);
                        setSelectedElement(0);
                        setSelectedElement(SelectedElement);
                    }
                }
                CurrentMode = EGUIEDM_SELECT;
            }
            else if (CurrentMode >= EGUIEDM_MOVE) {
                boost::shared_ptr<IGUIElement> sel = SelectedElement;
                // unselect
                setSelectedElement(0);

                // move
                core::position2d<s32> p(0, 0);
                if (sel->getParent())
                    p = sel->getParent()->getAbsolutePosition().UpperLeftCorner;

                // select
                setSelectedElement(sel);

                _graphicContext->setSelectedElementPosition(SelectedArea - p);

                // reset selection mode...
                CurrentMode = EGUIEDM_SELECT;
            }
            break;
        case EMIE_MOUSE_MOVED:
            // always on top
            lockedParent->bringToFront(getSharedThis());

            // if selecting
            if (CurrentMode == EGUIEDM_SELECT || CurrentMode == EGUIEDM_SELECT_NEW_PARENT) {

                core::position2di p = core::position2di(e.MouseInput.X, e.MouseInput.Y);

                // highlight the element that the mouse is over
                MouseOverElement = getEditableElementFromPoint(lockedParent, p);
                if (MouseOverElement == lockedParent) {
                    MouseOverElement = 0;
                }

                if (CurrentMode == EGUIEDM_SELECT) {
                    MouseOverMode = getModeFromPos(p);
                    if (MouseOverMode > EGUIEDM_MOVE) {
                        MouseOverElement = SelectedElement;
                    }
                }
            }
            else if (CurrentMode == EGUIEDM_MOVE) {
                // get difference
                core::position2di p = core::position2di(e.MouseInput.X, e.MouseInput.Y);
                p -= DragStart;

                // apply to top corner
                p = StartMovePos + p;
                if (UseGrid) {
                    p.X = (p.X / GridSize.Width) * GridSize.Width;
                    p.Y = (p.Y / GridSize.Height) * GridSize.Height;
                }

                SelectedArea += p - SelectedArea.UpperLeftCorner;
            }
            else if (CurrentMode > EGUIEDM_MOVE) {
                // get difference from start position
                core::position2di p = core::position2di(e.MouseInput.X, e.MouseInput.Y);
                if (UseGrid) {
                    p.X = (p.X / GridSize.Width) * GridSize.Width;
                    p.Y = (p.Y / GridSize.Height) * GridSize.Height;
                }

                switch (CurrentMode) {
                case EGUIEDM_RESIZE_T:
                    SelectedArea.UpperLeftCorner.Y = p.Y;
                    break;
                case EGUIEDM_RESIZE_B:
                    SelectedArea.LowerRightCorner.Y = p.Y;
                    break;
                case EGUIEDM_RESIZE_L:
                    SelectedArea.UpperLeftCorner.X = p.X;
                    break;
                case EGUIEDM_RESIZE_R:
                    SelectedArea.LowerRightCorner.X = p.X;
                    break;
                case EGUIEDM_RESIZE_TL:
                    SelectedArea.UpperLeftCorner = p;
                    break;
                case EGUIEDM_RESIZE_TR:
                    SelectedArea.UpperLeftCorner.Y = p.Y;
                    SelectedArea.LowerRightCorner.X = p.X;
                    break;
                case EGUIEDM_RESIZE_BL:
                    SelectedArea.UpperLeftCorner.X = p.X;
                    SelectedArea.LowerRightCorner.Y = p.Y;
                    break;
                case EGUIEDM_RESIZE_BR:
                    SelectedArea.LowerRightCorner = p;
                    break;
                default:
                    break;
                }
            }

            break;
        default:
            break;
        }
        break;
    }
    default:
        break;
    }

    // even if we didn't absorb the event,
    // we never pass events back to the GUI we're editing!
    return false;
}

//! draws the element and its children
void CGUIEditWorkspace::draw()
{
    boost::shared_ptr<video::IVideoDriver> driver = getSharedEnvironment()->getVideoDriver();

    if (DrawGrid) {
        // draw the grid

        core::rect<s32> r = getAbsolutePosition();

        s32 cy = r.UpperLeftCorner.Y;
        while (cy < r.LowerRightCorner.Y) {
            s32 cx = r.UpperLeftCorner.X;
            while (cx < r.LowerRightCorner.X) {
                driver->draw2DRectangle(video::SColor(40, 0, 0, 90), core::rect<s32>(cx + 1, cy + 1, GridSize.Width + cx, GridSize.Height + cy));
                cx += GridSize.Width;
            }
            cy += GridSize.Height;
        }
    }
    if (MouseOverElement && MouseOverElement != SelectedElement && MouseOverElement != getParent()) {
        core::rect<s32> r = MouseOverElement->getAbsolutePosition();
        driver->draw2DRectangle(video::SColor(100, 0, 0, 255), r);
    }
    if (SelectedElement && CurrentMode == EGUIEDM_SELECT) {
        driver->draw2DRectangle(video::SColor(100, 0, 255, 0), SelectedElement->getAbsolutePosition());
    }
    if (CurrentMode >= EGUIEDM_MOVE) {
        driver->draw2DRectangle(video::SColor(100, 255, 0, 0), SelectedArea);
    }

    if ((SelectedElement && CurrentMode >= EGUIEDM_MOVE) || (SelectedElement && MouseOverElement == SelectedElement && MouseOverMode >= EGUIEDM_MOVE)) {
        // draw handles for moving
        EGUIEDIT_MODE m = CurrentMode;
        core::rect<s32> r = SelectedArea;
        if (m < EGUIEDM_MOVE) {
            m = MouseOverMode;
            r = SelectedElement->getAbsolutePosition();
        }

        core::position2di d = core::position2di(4, 4);

        TLRect = core::rect<s32>(r.UpperLeftCorner, r.UpperLeftCorner + d);
        TRRect = core::rect<s32>(r.LowerRightCorner.X - 4, r.UpperLeftCorner.Y, r.LowerRightCorner.X, r.UpperLeftCorner.Y + 4);
        TopRect = core::rect<s32>(r.getCenter().X - 2, r.UpperLeftCorner.Y, r.getCenter().X + 2, r.UpperLeftCorner.Y + 4);
        BLRect = core::rect<s32>(r.UpperLeftCorner.X, r.LowerRightCorner.Y - 4, r.UpperLeftCorner.X + 4, r.LowerRightCorner.Y);
        LRect = core::rect<s32>(r.UpperLeftCorner.X, r.getCenter().Y - 2, r.UpperLeftCorner.X + 4, r.getCenter().Y + 2);
        RRect = core::rect<s32>(r.LowerRightCorner.X - 4, r.getCenter().Y - 2, r.LowerRightCorner.X, r.getCenter().Y + 2);
        BRRect = core::rect<s32>(r.LowerRightCorner - d, r.LowerRightCorner);
        BRect = core::rect<s32>(r.getCenter().X - 2, r.LowerRightCorner.Y - 4, r.getCenter().X + 2, r.LowerRightCorner.Y);

        // top left
        if (m == EGUIEDM_RESIZE_T || m == EGUIEDM_RESIZE_L || m == EGUIEDM_RESIZE_TL || m == EGUIEDM_MOVE)
            driver->draw2DRectangle(video::SColor(100, 255, 255, 255), TLRect);

        if (m == EGUIEDM_RESIZE_T || m == EGUIEDM_RESIZE_R || m == EGUIEDM_RESIZE_TR || m == EGUIEDM_MOVE)
            driver->draw2DRectangle(video::SColor(100, 255, 255, 255), TRRect);

        if (m == EGUIEDM_RESIZE_T || m == EGUIEDM_MOVE)
            driver->draw2DRectangle(video::SColor(100, 255, 255, 255), TopRect);

        if (m == EGUIEDM_RESIZE_L || m == EGUIEDM_RESIZE_BL || m == EGUIEDM_RESIZE_B || m == EGUIEDM_MOVE)
            driver->draw2DRectangle(video::SColor(100, 255, 255, 255), BLRect);

        if (m == EGUIEDM_RESIZE_L || m == EGUIEDM_MOVE)
            driver->draw2DRectangle(video::SColor(100, 255, 255, 255), LRect);

        if (m == EGUIEDM_RESIZE_R || m == EGUIEDM_MOVE)
            driver->draw2DRectangle(video::SColor(100, 255, 255, 255), RRect);

        if (m == EGUIEDM_RESIZE_R || m == EGUIEDM_RESIZE_BR || m == EGUIEDM_RESIZE_B || m == EGUIEDM_MOVE)
            driver->draw2DRectangle(video::SColor(100, 255, 255, 255), BRRect);

        if (m == EGUIEDM_RESIZE_B || m == EGUIEDM_MOVE)
            driver->draw2DRectangle(video::SColor(100, 255, 255, 255), BRect);
    }

    IGUIElement::draw();
}

void CGUIEditWorkspace::setDrawGrid(bool drawGrid)
{
    DrawGrid = drawGrid;
}

void CGUIEditWorkspace::setGridSize(const core::dimension2di& gridSize)
{
    GridSize = gridSize;
    if (GridSize.Width < 2)
        GridSize.Width = 2;
    if (GridSize.Height < 2)
        GridSize.Height = 2;
}

void CGUIEditWorkspace::setUseGrid(bool useGrid)
{
    UseGrid = useGrid;
}

bool CGUIEditWorkspace::getUseGrid()
{
    return UseGrid;
}

//! Removes a child.
void CGUIEditWorkspace::removeChild(boost::shared_ptr<IGUIElement> child)
{
    IGUIElement::removeChild(child);

    if (Children.empty())
        remove();
}

void CGUIEditWorkspace::CopySelectedElementXML()
{
    core::stringc XMLText;
    core::stringw wXMLText;
    // create memory write file
    boost::shared_ptr<io::CMemoryReadWriteFile> memWrite = boost::make_shared<io::CMemoryReadWriteFile>("#Clipboard#");

    boost::shared_ptr<IGUIEnvironment> lockedEnvironment = getSharedEnvironment();

    // save gui to mem file
    io::IXMLWriter* xml = lockedEnvironment->getFileSystem()->createXMLWriter(memWrite);
    lockedEnvironment->writeGUIElement(xml, SelectedElement);

    // copy to clipboard- wide chars not supported yet :(
    wXMLText = (wchar_t*)&memWrite->getData()[0];
    u32 i = memWrite->getData().size() / sizeof(wchar_t);
    if (wXMLText.size() > i)
        wXMLText[i] = L'\0';
    XMLText = wXMLText.c_str();
    xml->drop();
    lockedEnvironment->getOSOperator()->copyToClipboard(XMLText.c_str());
}

void CGUIEditWorkspace::PasteXMLToSelectedElement()
{
    boost::shared_ptr<IGUIEnvironment> lockedEnvironment = getSharedEnvironment();

    // get clipboard data
    const char* p = lockedEnvironment->getOSOperator()->getTextFromClipboard();

    // convert to stringw
    // TODO: we should have such a function in core::string
    size_t lenOld = strlen(p);
    wchar_t* ws = new wchar_t[lenOld + 1];
    size_t len = mbstowcs(ws, p, lenOld);
    ws[len] = 0;
    irr::core::stringw wXMLText(ws);
    delete[] ws;

    boost::shared_ptr<io::CMemoryReadWriteFile> memWrite = boost::make_shared<io::CMemoryReadWriteFile>("#Clipboard#");

    io::IXMLWriter* xmlw = lockedEnvironment->getFileSystem()->createXMLWriter(memWrite);
    xmlw->writeXMLHeader(); // it needs one of those
    xmlw->drop();

    // write clipboard data
    memWrite->write((void*)&wXMLText[0], wXMLText.size() * sizeof(wchar_t));

    // rewind file
    memWrite->seek(0, false);

    // read xml
    lockedEnvironment->loadGUI(memWrite, SelectedElement);

    // reset focus
    lockedEnvironment->setFocus(getSharedThis());
}
}