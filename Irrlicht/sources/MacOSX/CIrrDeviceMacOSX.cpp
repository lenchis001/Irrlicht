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
    GLuint vertex_buffer, vertex_shader, fragment_shader, program;
    GLint mvp_location, vpos_location, vcol_location;

    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
      exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    window = glfwCreateWindow(640, 480, "Simple example", NULL, NULL);
    if (!window)
    {
      glfwTerminate();
      exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    createDriver();
    createGUIAndScene();
    glfwSetWindowUserPointer(window, this);

    auto videoDriver = getVideoDriver();

    glfwSetWindowSizeCallback(window, [](GLFWwindow *window, int width, int height)
                              { THAT(window)->getVideoDriver()->OnResize({width, height}); });

    glfwSetMouseButtonCallback(window, [](GLFWwindow *window, int button, int action, int mods) {
      CIrrDeviceMacOSX* that = THAT(window);
      irr::EMOUSE_INPUT_EVENT eventType = irr::EMOUSE_INPUT_EVENT::EMIE_COUNT;

      if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if(action == GLFW_PRESS) {
          eventType = irr::EMOUSE_INPUT_EVENT::EMIE_LMOUSE_PRESSED_DOWN;
        } else if(action == GLFW_RELEASE){
          eventType = irr::EMOUSE_INPUT_EVENT::EMIE_LMOUSE_LEFT_UP;
        }
      }

      if(eventType != irr::EMOUSE_INPUT_EVENT::EMIE_COUNT) {
        that->postMouseEvent(eventType);
      } 
    });

    glfwSetCursorPosCallback(window, [](GLFWwindow *window, double xpos, double ypos)
                             {
      CIrrDeviceMacOSX* that = THAT(window);

      that->_xCursorPosition = xpos;
      that->_yCursorPosition = ypos;

      that->postMouseEvent(irr::EMOUSE_INPUT_EVENT::EMIE_MOUSE_MOVED); });
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

  void CIrrDeviceMacOSX::closeDevice() {
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
    if(event.MouseInput.Event == irr::EMOUSE_INPUT_EVENT::EMIE_LMOUSE_PRESSED_DOWN) {
      mouseEventType = irr::E_MOUSE_BUTTON_STATE_MASK::EMBSM_LEFT;
    } else if(event.MouseInput.Event == irr::EMOUSE_INPUT_EVENT::EMIE_RMOUSE_PRESSED_DOWN) {
      mouseEventType = irr::E_MOUSE_BUTTON_STATE_MASK::EMBSM_RIGHT;
    }
    event.MouseInput.ButtonStates = mouseEventType;

    postEventFromUser(event);
  }
}

#endif