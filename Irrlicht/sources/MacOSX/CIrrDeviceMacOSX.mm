#include "CIrrDeviceMacOSX.h"

namespace irr
{
	namespace video
	{
		boost::shared_ptr<IVideoDriver> createOpenGLDriver(const SIrrlichtCreationParameters& param, boost::shared_ptr<io::IFileSystem> io, CIrrDeviceMacOSX *device);
	}
}

namespace irr
{
    CIrrDeviceMacOSX::CIrrDeviceMacOSX(const SIrrlichtCreationParameters &params) : CIrrDeviceStub(params)
    {
    }

    CIrrDeviceMacOSX::~CIrrDeviceMacOSX()
    {
    }

    bool CIrrDeviceMacOSX::run() {}

    void CIrrDeviceMacOSX::yield() {}

    void CIrrDeviceMacOSX::sleep(u32 timeMs, bool pauseTimer) {}

    void CIrrDeviceMacOSX::setWindowCaption(const wchar_t *text) {}

    bool CIrrDeviceMacOSX::isWindowActive() const {}

    bool CIrrDeviceMacOSX::isWindowFocused() const {}

    bool CIrrDeviceMacOSX::isWindowMinimized() const {}

    bool CIrrDeviceMacOSX::present(boost::shared_ptr<video::IImage> surface, void *windowId, core::rect<s32> *src) {}

    void CIrrDeviceMacOSX::closeDevice() {}

    void CIrrDeviceMacOSX::setResizable(bool resize) {}

    bool CIrrDeviceMacOSX::isResizable() const {}

    void CIrrDeviceMacOSX::minimizeWindow() {}

    void CIrrDeviceMacOSX::maximizeWindow() {}

    void CIrrDeviceMacOSX::restoreWindow() {}

    bool CIrrDeviceMacOSX::activateJoysticks(core::array<SJoystickInfo> &joystickInfo) {}

    boost::shared_ptr<video::IVideoModeList> CIrrDeviceMacOSX::getVideoModeList() {}

    void CIrrDeviceMacOSX::flush() {}
    void CIrrDeviceMacOSX::setMouseLocation(int x, int y) {}
    void CIrrDeviceMacOSX::setResize(int width, int height) {}
    void CIrrDeviceMacOSX::setCursorVisible(bool visible) {}

    void CIrrDeviceMacOSX::createDriver() {
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
}