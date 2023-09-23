#include "CIrrDeviceMacOSX.h"
#include "iostream"

#ifdef _IRR_COMPILE_WITH_OSX_DEVICE_

#define THAT(win) static_cast<irr::CIrrDeviceMacOSX *>(glfwGetWindowUserPointer(win))

namespace irr
{
  namespace video
  {
    boost::shared_ptr<IVideoDriver> createOpenGLDriver(const SIrrlichtCreationParameters &param, boost::shared_ptr<io::IFileSystem> io, CIrrDeviceMacOSX *device);
  }
}

namespace irr
{
  static void error_callback(int error, const char *description)
  {
    fprintf(stderr, "Error: %s\n", description);
  }

  CIrrDeviceMacOSX::CIrrDeviceMacOSX(const SIrrlichtCreationParameters &params) : CIrrDeviceStub(params)
  {
    _createWindow();

    _subscribeToEvents();
  }

  CIrrDeviceMacOSX::~CIrrDeviceMacOSX()
  {
  }

  bool CIrrDeviceMacOSX::run()
  {
    bool result = !glfwWindowShouldClose(window);
    glfwSwapBuffers(window);
    glfwPollEvents();

    return result;
  }

  void CIrrDeviceMacOSX::yield() {}

  void CIrrDeviceMacOSX::sleep(u32 timeMs, bool pauseTimer) {}

  void CIrrDeviceMacOSX::setWindowCaption(const wchar_t *text)
  {
    char buffer[256];
    memset(buffer, NULL, sizeof(buffer));

    wcstombs(buffer, text, wcslen(text));
    glfwSetWindowTitle(window, buffer);
  }

  bool CIrrDeviceMacOSX::isWindowActive() const
  {
    return true;
  }

  bool CIrrDeviceMacOSX::isWindowFocused() const
  {
    return true;
  }

  bool CIrrDeviceMacOSX::isWindowMinimized() const
  {
    return false;
  }

  bool CIrrDeviceMacOSX::present(boost::shared_ptr<video::IImage> surface, void *windowId, core::rect<s32> *src) {}

  void CIrrDeviceMacOSX::closeDevice()
  {
    glfwDestroyWindow(window);
    glfwTerminate();
  }

  void CIrrDeviceMacOSX::setResizable(bool resize) {}

  bool CIrrDeviceMacOSX::isResizable() const
  {
    return false;
  }

  void CIrrDeviceMacOSX::minimizeWindow() {}

  void CIrrDeviceMacOSX::maximizeWindow() {}

  void CIrrDeviceMacOSX::restoreWindow() {}

  bool CIrrDeviceMacOSX::activateJoysticks(core::array<SJoystickInfo> &joystickInfo) {}

  boost::shared_ptr<video::IVideoModeList> CIrrDeviceMacOSX::getVideoModeList() {}

  void CIrrDeviceMacOSX::flush() {}
  void CIrrDeviceMacOSX::setMouseLocation(int x, int y) {}
  void CIrrDeviceMacOSX::setResize(int width, int height) {}
  void CIrrDeviceMacOSX::setCursorVisible(bool visible) {}

  void CIrrDeviceMacOSX::createDriver()
  {
    switch (CreationParams.DriverType)
    {
    case video::EDT_OPENGL:
#ifdef _IRR_COMPILE_WITH_OPENGL_
      VideoDriver = video::createOpenGLDriver(CreationParams, FileSystem, this);
#endif
      break;
    case video::EDT_NULL:
      VideoDriver = video::createNullDriver(FileSystem, CreationParams.WindowSize);
      break;
    default:
      break;
    }
  }

  void CIrrDeviceMacOSX::postMouseEvent(irr::EMOUSE_INPUT_EVENT eventType)
  {
    irr::SEvent event;
    event.EventType = irr::EEVENT_TYPE::EET_MOUSE_INPUT_EVENT;
    event.MouseInput.Event = eventType;
    event.MouseInput.X = _xCursorPosition;
    event.MouseInput.Y = _yCursorPosition;

    irr::E_MOUSE_BUTTON_STATE_MASK mouseEventType = irr::E_MOUSE_BUTTON_STATE_MASK::EMBSM_FORCE_32_BIT;
    if (event.MouseInput.Event == irr::EMOUSE_INPUT_EVENT::EMIE_LMOUSE_PRESSED_DOWN)
    {
      mouseEventType = irr::E_MOUSE_BUTTON_STATE_MASK::EMBSM_LEFT;
    }
    else if (event.MouseInput.Event == irr::EMOUSE_INPUT_EVENT::EMIE_RMOUSE_PRESSED_DOWN)
    {
      mouseEventType = irr::E_MOUSE_BUTTON_STATE_MASK::EMBSM_RIGHT;
    }
    event.MouseInput.ButtonStates = mouseEventType;

    postEventFromUser(event);
  }

  void CIrrDeviceMacOSX::postKeyEvent(bool isPressed, int glfwKey, bool isShiftPressed)
  {
    irr::SEvent event;
    event.EventType = irr::EEVENT_TYPE::EET_KEY_INPUT_EVENT;
    event.KeyInput.PressedDown = isPressed;
    event.KeyInput.Shift = isShiftPressed;
    event.KeyInput.Control = false;

    event.KeyInput.Char = _keyRemapping.count(glfwKey) ? 0 : glfwKey;
    event.KeyInput.Key = (irr::EKEY_CODE)(_keyRemapping.count(glfwKey) ? _keyRemapping[glfwKey] : 0);

    postEventFromUser(event);
  }

  void CIrrDeviceMacOSX::_setupKeyMapping()
  {
    _keyRemapping[GLFW_KEY_BACKSPACE] = irr::EKEY_CODE::KEY_BACK;
    _keyRemapping[GLFW_KEY_LEFT] = irr::EKEY_CODE::KEY_LEFT;
    _keyRemapping[GLFW_KEY_LEFT_SHIFT] = irr::EKEY_CODE::KEY_SHIFT;
    _keyRemapping[GLFW_KEY_RIGHT_SHIFT] = irr::EKEY_CODE::KEY_SHIFT;
    _keyRemapping[GLFW_KEY_CAPS_LOCK] = irr::EKEY_CODE::KEY_CAPITAL;
    _keyRemapping[GLFW_KEY_TAB] = irr::EKEY_CODE::KEY_TAB;

    _shiftKeyMapping[(int)'1'] = (int)'!';
    _shiftKeyMapping[(int)'2'] = (int)'@';
    _shiftKeyMapping[(int)'3'] = (int)'#';
    _shiftKeyMapping[(int)'4'] = (int)'$';
    _shiftKeyMapping[(int)'5'] = (int)'%';
    _shiftKeyMapping[(int)'6'] = (int)'^';
    _shiftKeyMapping[(int)'7'] = (int)'&';
    _shiftKeyMapping[(int)'8'] = (int)'*';
    _shiftKeyMapping[(int)'9'] = (int)'(';
    _shiftKeyMapping[(int)'0'] = (int)')';
    _shiftKeyMapping[(int)'-'] = (int)'_';
    _shiftKeyMapping[(int)'='] = (int)'+';
    _shiftKeyMapping[(int)'`'] = (int)'~';
    _shiftKeyMapping[(int)';'] = (int)':';
    _shiftKeyMapping[(int)'\''] = (int)'"';
    _shiftKeyMapping[(int)','] = (int)'<';
    _shiftKeyMapping[(int)'.'] = (int)'>';
    _shiftKeyMapping[(int)'/'] = (int)'?';
    _shiftKeyMapping[(int)'\\'] = (int)'|';
  }

  void CIrrDeviceMacOSX::_createWindow()
  {
    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
      exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    window = glfwCreateWindow(CreationParams.WindowSize.Width, CreationParams.WindowSize.Height, "", NULL, NULL);
    if (!window)
    {
      glfwTerminate();
      exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    // create cursor control
    CursorControl = boost::make_shared<CCursorControl>(CreationParams.WindowSize, this);
    createDriver();
    createGUIAndScene();
    glfwSetWindowUserPointer(window, this);
  }

  void CIrrDeviceMacOSX::_subscribeToEvents()
  {
    glfwSetWindowSizeCallback(window, [](GLFWwindow *window, int width, int height)
                              { THAT(window)->getVideoDriver()->OnResize({width, height}); });

    glfwSetMouseButtonCallback(window, [](GLFWwindow *window, int button, int action, int mods)
                               {
      CIrrDeviceMacOSX* that = THAT(window);
      irr::EMOUSE_INPUT_EVENT eventType = irr::EMOUSE_INPUT_EVENT::EMIE_COUNT;

      if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if(action == GLFW_PRESS) {
          eventType = irr::EMOUSE_INPUT_EVENT::EMIE_LMOUSE_PRESSED_DOWN;
        } else if(action == GLFW_RELEASE){
          eventType = irr::EMOUSE_INPUT_EVENT::EMIE_LMOUSE_LEFT_UP;
        }
      }

      if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        if(action == GLFW_PRESS) {
          eventType = irr::EMOUSE_INPUT_EVENT::EMIE_RMOUSE_PRESSED_DOWN;
        } else if(action == GLFW_RELEASE){
          eventType = irr::EMOUSE_INPUT_EVENT::EMIE_RMOUSE_LEFT_UP;
        }
      }

      if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
        if(action == GLFW_PRESS) {
          eventType = irr::EMOUSE_INPUT_EVENT::EMIE_MMOUSE_PRESSED_DOWN;
        } else if(action == GLFW_RELEASE){
          eventType = irr::EMOUSE_INPUT_EVENT::EMIE_MMOUSE_LEFT_UP;
        }
      }

      if(eventType != irr::EMOUSE_INPUT_EVENT::EMIE_COUNT) {
        that->postMouseEvent(eventType);
      } });

    glfwSetCursorPosCallback(window, [](GLFWwindow *window, double xpos, double ypos)
                             {
      CIrrDeviceMacOSX* that = THAT(window);

      that->_xCursorPosition = xpos;
      that->_yCursorPosition = ypos;

      that->postMouseEvent(irr::EMOUSE_INPUT_EVENT::EMIE_MOUSE_MOVED); });

    glfwSetKeyCallback(window, [](GLFWwindow *window, int key, int scancode, int action, int mode)
                       {
        CIrrDeviceMacOSX* that = THAT(window);

      bool isShiftPressed = mode == GLFW_MOD_SHIFT;

      int resultKey = key;
      if(!isShiftPressed && key >=65 && key<=90) {
        resultKey +=32;
      } else if(isShiftPressed && that->_shiftKeyMapping.count(key)) {
        resultKey = that->_shiftKeyMapping[key];
      }

      that->postKeyEvent(action == GLFW_PRESS || action == GLFW_REPEAT, resultKey, isShiftPressed); });

    getVideoDriver()->OnResize(CreationParams.WindowSize);

    _setupKeyMapping();
  }
}

#endif