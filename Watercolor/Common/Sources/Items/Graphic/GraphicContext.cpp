#include "GraphicContext.h"
#include "TCoreFacade.h"

#include "GL/gl.h"

#include "boost/make_shared.hpp"

#include "Utils/Utils.h"

#ifdef _IRR_COMPILE_WITH_X11_DEVICE_
#include "gdk/gdk.h"
#include "gtk/gtk.h"
#include <gdk/gdkx.h>
#endif

#define NODE_PROPERTY_CHANGE_PROXY_HANDLER_NAME L"GraphicContext on node property change"

namespace Watercolor::Common {
void GraphicContext::_prepareContext(void* windowId)
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
    _sceneManager = _device->getSceneManager();
    SceneNodeGraphicContextMixin::setSceneManager(_sceneManager);
}

void GraphicContext::_shutdownContext()
{
    shutdown();

    _functionsProcessingManager->processAll(_threadType);

    _device->closeDevice();
}

void GraphicContext::_onKeyDown(wxKeyEvent& eventData)
{
    irr::SEvent irrEventData;

    irrEventData.EventType = irr::EEVENT_TYPE::EET_KEY_INPUT_EVENT;
    irrEventData.KeyInput.Char = eventData.m_uniChar;
    irrEventData.KeyInput.Control = eventData.ControlDown();
    irrEventData.KeyInput.Key = _wxKeyCodeToIrrKeyCore((static_cast<wxKeyCode>(eventData.GetKeyCode())));
    irrEventData.KeyInput.PressedDown = true;
    irrEventData.KeyInput.Shift = eventData.ShiftDown();

    _functionsProcessingManager->addFuctionToQueue(
        _threadType, [=] { _device->postEventFromUser(irrEventData); });
    onIrrEvent.callHandlers(irrEventData);
}

void GraphicContext::_onKeyUp(wxKeyEvent& eventData)
{
    irr::SEvent irrEventData;

    irrEventData.EventType = irr::EEVENT_TYPE::EET_KEY_INPUT_EVENT;
    irrEventData.KeyInput.Char = eventData.m_uniChar;
    irrEventData.KeyInput.Control = eventData.ControlDown();
    irrEventData.KeyInput.Key = _wxKeyCodeToIrrKeyCore((static_cast<wxKeyCode>(eventData.GetKeyCode())));
    irrEventData.KeyInput.PressedDown = false;
    irrEventData.KeyInput.Shift = eventData.ShiftDown();

    _functionsProcessingManager->addFuctionToQueue(
        _threadType, [=] { _device->postEventFromUser(irrEventData); });
    onIrrEvent.callHandlers(irrEventData);
}

void GraphicContext::_onSizeChanged(wxSizeEvent& eventData)
{
    _functionsProcessingManager->addFuctionToQueue(_threadType, [=]() {
        float width = eventData.GetSize().GetWidth();
        width = width ? width : 1;
        float height = eventData.GetSize().GetHeight();
        height = height ? height : 1;

        glViewport(0, 0, width, height);

        auto activeCamera = _sceneManager->getActiveCamera();
        if (activeCamera) {
            irr::core::matrix4 matrix;
            matrix.buildProjectionMatrixPerspectiveLH(width / height, 1.0f,
                activeCamera->getNearValue(),
                activeCamera->getFarValue());
            activeCamera->setProjectionMatrix(matrix);
        }

        _driver->OnResize(irr::core::dimension2d<irr::u32>(width, height));
    });
}

void GraphicContext::pauseRendering() { _renderingMutex->lock(); }

void GraphicContext::resumeRendering() { _renderingMutex->unlock(); }

GraphicContext::GraphicContext(wxWindow* parent, ThreadType threadType,
    boost::shared_ptr<IFunctionsProcessingManager> functionsProcessingManager,
    wxSize contextSize, wxPoint position,
    wxWindowID id)
    : wxPanel(parent, id, position, contextSize)
    , SceneNodeGraphicContextMixin(threadType, functionsProcessingManager)
{
    _renderingMutex = boost::make_shared<boost::mutex>();

#ifdef _IRR_COMPILE_WITH_WINDOWS_DEVICE_
    void* window = reinterpret_cast<void*>(this->GetHWND());
#elif defined(_IRR_COMPILE_WITH_X11_DEVICE_)
    GtkWidget* widget = this->GetHandle();
    gtk_widget_realize( widget );
    GdkWindow* gtkWindow = gtk_widget_get_window(widget);
    auto x11Window = GDK_WINDOW_XID(gtkWindow);

    gtk_widget_set_double_buffered(widget, false);
    
    void* window = reinterpret_cast<void*>(x11Window);
#else
    #error "This type of compilation is not setup yet..."
#endif

    _drawThread = boost::make_shared<boost::thread>([&, window]() {
        _prepareContext(window);

        while (!boost::this_thread::interruption_requested()) {
            // TODO: Mutex is too slow here
            _renderingMutex->lock();

            _functionsProcessingManager->processFunctions(_threadType);
            _reDraw();

            _renderingMutex->unlock();
        }

        _shutdownContext();
    });
}

GraphicContext::~GraphicContext()
{
    this->resumeRendering();

    _drawThread->interrupt();
    _drawThread->join();
}

irr::EKEY_CODE GraphicContext::_wxKeyCodeToIrrKeyCore(wxKeyCode code) const
{
    irr::EKEY_CODE result = irr::EKEY_CODE::KEY_KEY_CODES_COUNT;

    switch (code) {
    case WXK_NONE:
        break;
    case WXK_CONTROL_A:
    case WXK_CONTROL_B:
    case WXK_CONTROL_C:
    case WXK_CONTROL_D:
    case WXK_CONTROL_E:
    case WXK_CONTROL_F:
    case WXK_CONTROL_G:
    case WXK_CONTROL_H:
    case WXK_CONTROL_I:
    case WXK_CONTROL_J:
    case WXK_CONTROL_K:
    case WXK_CONTROL_L:
    case WXK_CONTROL_M:
    case WXK_CONTROL_N:
    case WXK_CONTROL_O:
    case WXK_CONTROL_P:
    case WXK_CONTROL_Q:
    case WXK_CONTROL_R:
    case WXK_CONTROL_S:
    case WXK_CONTROL_T:
    case WXK_CONTROL_U:
    case WXK_CONTROL_V:
    case WXK_CONTROL_W:
    case WXK_CONTROL_X:
    case WXK_CONTROL_Y:
    case WXK_CONTROL_Z:
        result = static_cast<irr::EKEY_CODE>(WXK_CONTROL_A - code + irr::EKEY_CODE::KEY_KEY_A);
        break;
    case WXK_ESCAPE:
        result = irr::EKEY_CODE::KEY_ESCAPE;
        break;
    case WXK_SPACE:
        result = irr::EKEY_CODE::KEY_SPACE;
        break;
    case WXK_DELETE:
        result = irr::EKEY_CODE::KEY_DELETE;
        break;
    case WXK_START:
        break;
    case WXK_LBUTTON:
        result = irr::EKEY_CODE::KEY_LBUTTON;
        break;
    case WXK_RBUTTON:
        result = irr::EKEY_CODE::KEY_RBUTTON;
        break;
    case WXK_CANCEL:
        break;
    case WXK_MBUTTON:
        result = irr::EKEY_CODE::KEY_MBUTTON;
        break;
    case WXK_CLEAR:
        result = irr::EKEY_CODE::KEY_CLEAR;
        break;
    case WXK_SHIFT:
        result = irr::EKEY_CODE::KEY_LSHIFT;
        break;
    case WXK_ALT:
        break;
    case WXK_CONTROL:
        result = irr::EKEY_CODE::KEY_CONTROL;
        break;
    case WXK_MENU:
        result = irr::EKEY_CODE::KEY_MENU;
        break;
    case WXK_PAUSE:
        result = irr::EKEY_CODE::KEY_PAUSE;
        break;
    case WXK_CAPITAL:
        result = irr::EKEY_CODE::KEY_CAPITAL;
        break;
    case WXK_END:
        result = irr::EKEY_CODE::KEY_END;
        break;
    case WXK_HOME:
        result = irr::EKEY_CODE::KEY_HOME;
        break;
    case WXK_LEFT:
        result = irr::EKEY_CODE::KEY_LEFT;
        break;
    case WXK_UP:
        result = irr::EKEY_CODE::KEY_UP;
        break;
    case WXK_RIGHT:
        result = irr::EKEY_CODE::KEY_RIGHT;
        break;
    case WXK_DOWN:
        result = irr::EKEY_CODE::KEY_DOWN;
        break;
    case WXK_SELECT:
        result = irr::EKEY_CODE::KEY_SELECT;
        break;
    case WXK_PRINT:
        result = irr::EKEY_CODE::KEY_PRINT;
        break;
    case WXK_EXECUTE:
        result = irr::EKEY_CODE::KEY_EXECUT;
        break;
    case WXK_SNAPSHOT:
        result = irr::EKEY_CODE::KEY_SNAPSHOT;
        break;
    case WXK_INSERT:
        result = irr::EKEY_CODE::KEY_INSERT;
        break;
    case WXK_HELP:
        result = irr::EKEY_CODE::KEY_HELP;
        break;
    case WXK_NUMPAD0:
    case WXK_NUMPAD1:
    case WXK_NUMPAD2:
    case WXK_NUMPAD3:
    case WXK_NUMPAD4:
    case WXK_NUMPAD5:
    case WXK_NUMPAD6:
    case WXK_NUMPAD7:
    case WXK_NUMPAD8:
    case WXK_NUMPAD9:
        result = static_cast<irr::EKEY_CODE>(WXK_NUMPAD0 - code + irr::EKEY_CODE::KEY_NUMPAD0);
        break;
    case WXK_MULTIPLY:
        result = irr::EKEY_CODE::KEY_MULTIPLY;
        break;
    case WXK_ADD:
        result = irr::EKEY_CODE::KEY_ADD;
        break;
    case WXK_SEPARATOR:
        result = irr::EKEY_CODE::KEY_SEPARATOR;
        break;
    case WXK_SUBTRACT:
        result = irr::EKEY_CODE::KEY_SUBTRACT;
        break;
    case WXK_DECIMAL:
        result = irr::EKEY_CODE::KEY_DECIMAL;
        break;
    case WXK_DIVIDE:
        result = irr::EKEY_CODE::KEY_DIVIDE;
        break;
    case WXK_F1:
    case WXK_F2:
    case WXK_F3:
    case WXK_F4:
    case WXK_F5:
    case WXK_F6:
    case WXK_F7:
    case WXK_F8:
    case WXK_F9:
    case WXK_F10:
    case WXK_F11:
    case WXK_F12:
    case WXK_F13:
    case WXK_F14:
    case WXK_F15:
    case WXK_F16:
    case WXK_F17:
    case WXK_F18:
    case WXK_F19:
    case WXK_F20:
    case WXK_F21:
    case WXK_F22:
    case WXK_F23:
    case WXK_F24:
    case WXK_NUMLOCK:
    case WXK_SCROLL:
    case WXK_PAGEUP:
        result = static_cast<irr::EKEY_CODE>(WXK_F1 - code + irr::EKEY_CODE::KEY_F1);
        break;
    case WXK_PAGEDOWN:
        break;
    case WXK_NUMPAD_SPACE:
        break;
    case WXK_NUMPAD_TAB:
        break;
    case WXK_NUMPAD_ENTER:
        break;
    case WXK_NUMPAD_F1:
        break;
    case WXK_NUMPAD_F2:
        break;
    case WXK_NUMPAD_F3:
        break;
    case WXK_NUMPAD_F4:
        break;
    case WXK_NUMPAD_HOME:
        break;
    case WXK_NUMPAD_LEFT:
        break;
    case WXK_NUMPAD_UP:
        break;
    case WXK_NUMPAD_RIGHT:
        break;
    case WXK_NUMPAD_DOWN:
        break;
    case WXK_NUMPAD_PAGEUP:
        break;
    case WXK_NUMPAD_PAGEDOWN:
        break;
    case WXK_NUMPAD_END:
        break;
    case WXK_NUMPAD_BEGIN:
        break;
    case WXK_NUMPAD_INSERT:
        break;
    case WXK_NUMPAD_DELETE:
        break;
    case WXK_NUMPAD_EQUAL:
        break;
    case WXK_NUMPAD_MULTIPLY:
        break;
    case WXK_NUMPAD_ADD:
        break;
    case WXK_NUMPAD_SEPARATOR:
        break;
    case WXK_NUMPAD_SUBTRACT:
        break;
    case WXK_NUMPAD_DECIMAL:
        break;
    case WXK_NUMPAD_DIVIDE:
        break;
    case WXK_WINDOWS_LEFT:
        break;
    case WXK_WINDOWS_RIGHT:
        break;
    case WXK_WINDOWS_MENU:
        break;
    case WXK_SPECIAL1:
        break;
    case WXK_SPECIAL2:
        break;
    case WXK_SPECIAL3:
        break;
    case WXK_SPECIAL4:
        break;
    case WXK_SPECIAL5:
        break;
    case WXK_SPECIAL6:
        break;
    case WXK_SPECIAL7:
        break;
    case WXK_SPECIAL8:
        break;
    case WXK_SPECIAL9:
        break;
    case WXK_SPECIAL10:
        break;
    case WXK_SPECIAL11:
        break;
    case WXK_SPECIAL12:
        break;
    case WXK_SPECIAL13:
        break;
    case WXK_SPECIAL14:
        break;
    case WXK_SPECIAL15:
        break;
    case WXK_SPECIAL16:
        break;
    case WXK_SPECIAL17:
        break;
    case WXK_SPECIAL18:
        break;
    case WXK_SPECIAL19:
        break;
    case WXK_SPECIAL20:
        break;
    default:
        break;
    }

    return result;
}

#pragma warning(disable : 4407)
wxBEGIN_EVENT_TABLE(GraphicContext, wxPanel)
    EVT_KEY_DOWN(GraphicContext::_onKeyDown)
        EVT_KEY_UP(GraphicContext::_onKeyUp)
            EVT_SIZE(GraphicContext::_onSizeChanged)
                wxEND_EVENT_TABLE()
#pragma warning(default : 4407)

}