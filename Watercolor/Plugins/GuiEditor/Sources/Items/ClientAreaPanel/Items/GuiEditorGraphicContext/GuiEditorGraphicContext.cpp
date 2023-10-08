#include "GuiEditorGraphicContext.h"

#include "boost/filesystem.hpp"

#include "Core/CGUIEditWorkspace.h"

#include "Utils/Convert.h"
#include "Utils/ElementIDs.h"

#ifdef _IRR_COMPILE_WITH_X11_DEVICE_
#include "gdk/gdk.h"
#include "gtk/gtk.h"
#include <gdk/gdkx.h>
#endif

#define ACTIVE_SUB_VIEW_NAME L"active subview name"

#define GUI_EDITOR_IRR_EVENT_HANDLER L"gui editor irr handler"
#define ON_GUI_ELEMENT_SELECTED_HANDLER L"gui editor element selected handler"

namespace GUIEditor {
GuiEditorGraphicContext::GuiEditorGraphicContext(
    wxWindow* parent, 
    boost::shared_ptr<IFunctionsProcessingManager> functionsProcessingManager) :
    wxPanel(parent, Utils::ElementsIDs::GUI_EDITOR_CANVAS),
    _functionsProcessingManager(functionsProcessingManager)
{
    _renderingMutex = boost::make_shared<boost::mutex>();

    #ifdef _WINDOWS_
        void *window = reinterpret_cast<void *>(this->GetHWND());
#elif defined(GDK_WINDOWING_X11)
        GtkWidget *widget = this->GetHandle();
        gtk_widget_realize(widget);
        GdkWindow *gtkWindow = gtk_widget_get_window(widget);
        auto x11Window = GDK_WINDOW_XID(gtkWindow);

        gtk_widget_set_double_buffered(widget, false);

        void *window = reinterpret_cast<void *>(x11Window);
#else
#error "This type of compilation is not setup yet..."
#endif

    _drawThread = boost::make_shared<boost::thread>([&, window]() {
        _prepareContext(window);

        while (!boost::this_thread::interruption_requested()) {
            // TODO: Mutex is too slow here
            _renderingMutex->lock();

            _functionsProcessingManager->processFunctions(ThreadTypes::RENDER_THREAD);
            _reDraw();

            _renderingMutex->unlock();
        }

        _shutdownContext();
    });


    _functionsProcessingManager->addFuctionToQueue(ThreadTypes::RENDER_THREAD, [&]() {
        _guiEnvironment = _device->getGUIEnvironment();

        _editWorkspace = boost::make_shared<CGUIEditWorkspace>(this, _guiEnvironment);
        _editWorkspace->setWeakThis(_editWorkspace);

        _editWorkspace->setDrawGrid(true);
        _editWorkspace->onGuiElementSelected.addEventHandler(ON_GUI_ELEMENT_SELECTED_HANDLER, onGuiElementSelected);

        makeNew();
    });

    onIrrEvent.addEventHandler(GUI_EDITOR_IRR_EVENT_HANDLER, [&](auto irrEventData) {
        _functionsProcessingManager->addFuctionToQueue(ThreadTypes::GUI_THREAD, [=]() {
            _editWorkspace->OnEvent(irrEventData);
        });
    });
}

GuiEditorGraphicContext::~GuiEditorGraphicContext()
{
    _drawThread->interrupt();
    _drawThread->join();

    onIrrEvent.removeEventHandler(GUI_EDITOR_IRR_EVENT_HANDLER);
}

void GuiEditorGraphicContext::_clearWorkspace()
{
    _functionsProcessingManager->addFuctionToQueue(ThreadTypes::RENDER_THREAD, [=]() {
        auto children = _guiEnvironment->getRootGUIElement()->getChildren();

        for (auto child : children) {
            if (child != _editWorkspace) {
                child->remove();
            }
        }

        onClear.callHandlers();
    });
}

void GuiEditorGraphicContext::_shutdownContext()
{
    _functionsProcessingManager->processAll(ThreadTypes::RENDER_THREAD);
}

void GuiEditorGraphicContext::_prepareContext(void *windowId)
{
    irr::SIrrlichtCreationParameters param;
    param.DriverType = irr::video::E_DRIVER_TYPE::EDT_OPENGL;
    param.WindowSize = irr::core::dimension2d<int>(this->GetMaxWidth(), this->GetMaxHeight());
    param.Bits = 32;
    param.Stencilbuffer = true;
    param.Vsync = true;
    param.WindowId = windowId;

    _device = irr::createDeviceEx(param);
    _driver = _device->getVideoDriver();
}

void GuiEditorGraphicContext::_onMouseEvent(wxMouseEvent& eventData)
{
    if (!_editWorkspace) {
        return;
    }

    irr::SEvent irrEventData;

    irr::EMOUSE_INPUT_EVENT eventType = irr::EMOUSE_INPUT_EVENT::EMIE_COUNT;

    if (eventData.LeftUp()) {
        eventType = irr::EMOUSE_INPUT_EVENT::EMIE_LMOUSE_LEFT_UP;
    } else if (eventData.LeftDown()) {
        eventType = irr::EMOUSE_INPUT_EVENT::EMIE_LMOUSE_PRESSED_DOWN;
    } else if (eventData.LeftDClick()) {
        eventType = irr::EMOUSE_INPUT_EVENT::EMIE_LMOUSE_DOUBLE_CLICK;
    } else if (eventData.RightUp()) {
        eventType = irr::EMOUSE_INPUT_EVENT::EMIE_RMOUSE_LEFT_UP;
    } else if (eventData.RightDown()) {
        eventType = irr::EMOUSE_INPUT_EVENT::EMIE_RMOUSE_PRESSED_DOWN;
    } else if (eventData.RightDClick()) {
        eventType = irr::EMOUSE_INPUT_EVENT::EMIE_RMOUSE_DOUBLE_CLICK;
    } else {
        eventType = irr::EMOUSE_INPUT_EVENT::EMIE_MOUSE_MOVED;

        if (eventData.LeftIsDown()) {
            irrEventData.MouseInput.ButtonStates = irr::E_MOUSE_BUTTON_STATE_MASK::EMBSM_LEFT;
        } else if (eventData.RightIsDown()) {
            irrEventData.MouseInput.ButtonStates = irr::E_MOUSE_BUTTON_STATE_MASK::EMBSM_RIGHT;
        }
    }

    irrEventData.EventType = irr::EEVENT_TYPE::EET_MOUSE_INPUT_EVENT;
    irrEventData.MouseInput.Event = eventType;
    irrEventData.MouseInput.X = eventData.GetX();
    irrEventData.MouseInput.Y = eventData.GetY();

    _functionsProcessingManager->addFuctionToQueue(ThreadTypes::RENDER_THREAD, [=]() {
        _editWorkspace->OnEvent(irrEventData);
    });

    if (irrEventData.MouseInput.Event != irr::EMOUSE_INPUT_EVENT::EMIE_MOUSE_MOVED) {
        this->SetFocus();
    }
}

void GuiEditorGraphicContext::_onKeyEvent(wxKeyEvent& eventData)
{
    if (!_editWorkspace) {
        return;
    }

    switch (eventData.GetKeyCode()) {
    case wxKeyCode::WXK_DELETE:
        if (_editWorkspace->getSelectedElement()) {
            _editWorkspace->getSelectedElement()->remove();
            _editWorkspace->setSelectedElement(nullptr);
        }
        break;
    default:
        break;
    }
}

void GuiEditorGraphicContext::_onSizeChanged(wxSizeEvent& eventData)
{
    _functionsProcessingManager->addFuctionToQueue(ThreadTypes::RENDER_THREAD, [=]() {
        float width = eventData.GetSize().GetWidth();
        width = width ? width : 1;
        float height = eventData.GetSize().GetHeight();
        height = height ? height : 1;

        _driver->OnResize(irr::core::dimension2d<irr::u32>(width, height));
    });
}

void GuiEditorGraphicContext::addButton(AddGuiElementEventHandler callback)
{
    _functionsProcessingManager->addFuctionToQueue(ThreadTypes::RENDER_THREAD, [=]() {
        auto selectedElement = _editWorkspace->getSelectedElement();

        auto button = _guiEnvironment->addButton(irr::core::recti(10, 10, 100, 30), selectedElement);
        callback(button);
    });
}

void GuiEditorGraphicContext::addLabel(AddGuiElementEventHandler callback)
{
    _functionsProcessingManager->addFuctionToQueue(ThreadTypes::RENDER_THREAD, [=]() {
        auto selectedElement = _editWorkspace->getSelectedElement();
        auto label = _guiEnvironment->addStaticText(L"Static text", irr::core::recti(10, 10, 100, 30), false, true, selectedElement);

        callback(label);
    });
}

void GuiEditorGraphicContext::addTextbox(AddGuiElementEventHandler callback)
{
    _functionsProcessingManager->addFuctionToQueue(ThreadTypes::RENDER_THREAD, [=]() {
        auto selectedElement = _editWorkspace->getSelectedElement();
        auto textBox = _guiEnvironment->addEditBox(L"Text box", irr::core::recti(10, 10, 100, 30), true, selectedElement);

        callback(textBox);
    });
}

void GuiEditorGraphicContext::addWindow(AddGuiElementEventHandler callback)
{
    _functionsProcessingManager->addFuctionToQueue(ThreadTypes::RENDER_THREAD, [=]() {
        auto selectedElement = _editWorkspace->getSelectedElement();
        auto window = _guiEnvironment->addWindow(irr::core::recti(10, 10, 100, 30), false, nullptr, selectedElement);

        callback(window);
    });
}

void GuiEditorGraphicContext::addCheckBox(AddGuiElementEventHandler callback)
{
    _functionsProcessingManager->addFuctionToQueue(ThreadTypes::RENDER_THREAD, [=]() {
        auto selectedElement = _editWorkspace->getSelectedElement();
        auto checkBox = _guiEnvironment->addCheckBox(false, irr::core::recti(10, 10, 100, 30), selectedElement);

        callback(checkBox);
    });
}

void GuiEditorGraphicContext::addImage(AddGuiElementEventHandler callback)
{
    _functionsProcessingManager->addFuctionToQueue(ThreadTypes::RENDER_THREAD, [=]() {
        auto selectedElement = _editWorkspace->getSelectedElement();
        auto image = _guiEnvironment->addImage(irr::core::recti(10, 10, 100, 30), selectedElement);

        callback(image);
    });
}

void GuiEditorGraphicContext::addListBox(AddGuiElementEventHandler callback)
{
    _functionsProcessingManager->addFuctionToQueue(ThreadTypes::RENDER_THREAD, [=]() {
        auto selectedElement = _editWorkspace->getSelectedElement();
        auto listBox = _guiEnvironment->addListBox(irr::core::recti(10, 10, 100, 30), selectedElement);

        callback(listBox);
    });
}

std::wstring GuiEditorGraphicContext::getSelectedElementName() const
{
    return Utils::toWstring(_editWorkspace->getSelectedElement()->getName());
}

void GuiEditorGraphicContext::setSelectedElementName(const std::wstring& newName, const BoolEventCallback& callback)
{
    _functionsProcessingManager->addFuctionToQueue(ThreadTypes::RENDER_THREAD, [=]() {
        auto selectedElement = _editWorkspace->getSelectedElement();
        auto result = _guiEnvironment->addListBox(irr::core::recti(10, 10, 100, 30), selectedElement);

        callback(result.get());
    });
}

irr::core::recti GuiEditorGraphicContext::getSelectedElementPosition() const
{
    return _editWorkspace->getSelectedElement()->getRelativePosition();
}

void GuiEditorGraphicContext::setSelectedElementPosition(const irr::core::position2di& newPosition, const BoolEventCallback& callback)
{
    _functionsProcessingManager->addFuctionToQueue(ThreadTypes::RENDER_THREAD, [=]() {
        auto selectedElement = _editWorkspace->getSelectedElement();

        if (selectedElement) {
            selectedElement->setRelativePosition(newPosition);

            onPositionChanged.callHandlers(_editWorkspace->getSelectedElement());
        }

        callback(selectedElement.get());
    });
}

void GuiEditorGraphicContext::setSelectedElementPosition(const irr::core::recti& newPosition, const BoolEventCallback& callback)
{
    _functionsProcessingManager->addFuctionToQueue(ThreadTypes::RENDER_THREAD, [=]() {
        auto selectedElement = _editWorkspace->getSelectedElement();

        if (selectedElement) {
            selectedElement->setRelativePosition(newPosition);

            onPositionChanged.callHandlers(_editWorkspace->getSelectedElement());
        }

        callback(selectedElement.get());
    });
}

std::wstring GuiEditorGraphicContext::getSelectedElementText() const
{
    return _editWorkspace->getSelectedElement()->getText();
}

void GuiEditorGraphicContext::setSelectedElementText(const std::wstring& newText, const BoolEventCallback& callback)
{
    _functionsProcessingManager->addFuctionToQueue(ThreadTypes::RENDER_THREAD, [=]() {
        auto selectedElement = _editWorkspace->getSelectedElement();
    
        if (selectedElement) {
            selectedElement->setText(newText.c_str());
        }
    
        callback(selectedElement.get());
    });
}

std::wstring GuiEditorGraphicContext::getSelectImageElementPath() const
{
    // todo: should be async
    auto selectedElement = _editWorkspace->getSelectedElement();

    auto imageElement = boost::dynamic_pointer_cast<irr::gui::IGUIImage>(selectedElement);

    if(imageElement && imageElement->getImage()){
        return std::wstring((wchar_t*)(imageElement->getImage()->getName().getPath().c_str()));
     } else {
        return std::wstring();
     }
}

void GuiEditorGraphicContext::setSelectedImageElementPath(const std::wstring& newPath, const VoidEventCallback& callback)
{
    _functionsProcessingManager->addFuctionToQueue(ThreadTypes::RENDER_THREAD, [=]() {
        auto selectedElement = _editWorkspace->getSelectedElement();

        auto imageElement = boost::dynamic_pointer_cast<irr::gui::IGUIImage>(selectedElement);

        if (imageElement) {
            auto newTexture = _guiEnvironment->getVideoDriver()->getTexture(newPath.c_str());
            imageElement->setImage(newTexture);
        }

        callback();
    });
}

std::vector<std::wstring> GuiEditorGraphicContext::getSelectedListBoxOptions() const
{
    auto selectedElement = _editWorkspace->getSelectedElement();

    auto listBox = boost::dynamic_pointer_cast<irr::gui::IGUIListBox>(selectedElement);

    if (!listBox) {
        throw std::runtime_error("Element is not a ListBox");
    }

    std::vector<std::wstring> result(listBox->getItemCount());

    for (unsigned i = 0; i < listBox->getItemCount(); i++) {
        result[i] = listBox->getListItem(i);
    }

    return result;
}

void GuiEditorGraphicContext::setSelectedListBoxOptions(const std::vector<std::wstring>& options)
{
    auto selectedElement = _editWorkspace->getSelectedElement();
    
    auto listBox = boost::dynamic_pointer_cast<irr::gui::IGUIListBox>(selectedElement);

    if (!listBox) {
        throw std::runtime_error("Element is not a ListBox");
    }

    listBox->clear();
    for (const auto& option : options) {
        listBox->addItem(option.c_str());
    }
}

void GuiEditorGraphicContext::makeNew(VoidEventCallback callback)
{
    loadSubView(L"", callback);
}

void GuiEditorGraphicContext::loadSubView(const std::wstring& path, VoidEventCallback callback)
{
    _clearWorkspace();

    // get rid off this
    _functionsProcessingManager->addFuctionToQueue(ThreadTypes::RENDER_THREAD, [=]() {
        this->_currentViewPath = path;

        if (path.empty()) {
            //_guiEnvironment->clear();
        }else {
            _guiEnvironment->loadGUI(path.c_str());
        }

        callback();
    });
}

void GuiEditorGraphicContext::saveSubView(BoolEventCallback callback)
{
    _functionsProcessingManager->addFuctionToQueue(ThreadTypes::RENDER_THREAD, [=]() {
        auto result = _guiEnvironment->saveGUI(_currentViewPath.c_str());

        callback(result);
    });
}

void GuiEditorGraphicContext::saveSubViewAs(const std::wstring& path, BoolEventCallback callback)
{
    _currentViewPath = path;

    saveSubView(callback);
}

bool GuiEditorGraphicContext::removeSubView()
{
    bool result = false;

    if (isViewPathSet()) {
        boost::filesystem::remove(_currentViewPath);

        makeNew();
        result = true;
    }

    return result;
}

bool GuiEditorGraphicContext::isViewPathSet()
{
    return !_currentViewPath.empty();
}

boost::shared_ptr<CGUIEditWorkspace> GuiEditorGraphicContext::getGuiEditWorkspace()
{
    return _editWorkspace;
}

wxBEGIN_EVENT_TABLE(GuiEditorGraphicContext, wxPanel)
    EVT_MOUSE_EVENTS(GuiEditorGraphicContext::_onMouseEvent)
        EVT_KEY_UP(GuiEditorGraphicContext::_onKeyEvent)
            EVT_SIZE(GuiEditorGraphicContext::_onSizeChanged)
            wxEND_EVENT_TABLE()
}
