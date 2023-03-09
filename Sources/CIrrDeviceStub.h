// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_IRR_DEVICE_STUB_H_INCLUDED__
#define __C_IRR_DEVICE_STUB_H_INCLUDED__

#include "IrrlichtDevice.h"
#include "IImagePresenter.h"
#include "SIrrCreationParameters.h"
#include "CVideoModeList.h"

namespace irr
{
	// lots of prototypes:
	class ILogger;
	class CLogger;
	class IRandomizer;

	namespace gui
	{
		class IGUIEnvironment;
		boost::shared_ptr<IGUIEnvironment> createGUIEnvironment(boost::shared_ptr<io::IFileSystem> fs,
			boost::shared_ptr<video::IVideoDriver> Driver, boost::shared_ptr<IOSOperator> op);
	}

	namespace scene
	{
		boost::shared_ptr<scene::ISceneManager> createSceneManager(boost::shared_ptr<video::IVideoDriver> driver,
			boost::shared_ptr<io::IFileSystem> fs, boost::shared_ptr<gui::ICursorControl> cc, boost::shared_ptr<gui::IGUIEnvironment> gui);
	}

	namespace io
	{
		boost::shared_ptr<IFileSystem> createFileSystem();
	}

	namespace video
	{
		boost::shared_ptr<IVideoDriver> createSoftwareDriver(const core::dimension2d<u32>& windowSize,
				bool fullscreen, boost::shared_ptr<io::IFileSystem> io,
				video::IImagePresenter* presenter);
		boost::shared_ptr<IVideoDriver> createBurningVideoDriver(const irr::SIrrlichtCreationParameters& params,
				boost::shared_ptr<io::IFileSystem> io, video::IImagePresenter* presenter);
		boost::shared_ptr<IVideoDriver> createNullDriver(boost::shared_ptr<io::IFileSystem> io, const core::dimension2d<u32>& screenSize);
	}



	//! Stub for an Irrlicht Device implementation
	class CIrrDeviceStub : public IrrlichtDevice
	{
	public:

		//! constructor
		CIrrDeviceStub(const SIrrlichtCreationParameters& param);

		//! destructor
		virtual ~CIrrDeviceStub();

		//! returns the video driver
		virtual boost::shared_ptr<video::IVideoDriver> getVideoDriver();

		//! return file system
		virtual boost::shared_ptr<io::IFileSystem> getFileSystem();

		//! returns the gui environment
		virtual boost::shared_ptr<gui::IGUIEnvironment> getGUIEnvironment();

		//! returns the scene manager
		virtual boost::shared_ptr<scene::ISceneManager> getSceneManager();

		//! \return Returns a pointer to the mouse cursor control interface.
		virtual boost::shared_ptr<gui::ICursorControl> getCursorControl();

		//! Returns a pointer to a list with all video modes supported by the gfx adapter.
		virtual boost::shared_ptr<video::IVideoModeList> getVideoModeList();

		//! Returns a pointer to the ITimer object. With it the current Time can be received.
		virtual boost::shared_ptr<ITimer> getTimer();

		//! Returns the version of the engine.
		virtual const char* getVersion() const;

		//! send the event to the right receiver
		virtual bool postEventFromUser(const SEvent& event);

		//! Sets a new event receiver to receive events
		virtual void setEventReceiver(IEventReceiver* receiver);

		//! Returns pointer to the current event receiver. Returns 0 if there is none.
		virtual IEventReceiver* getEventReceiver();

		//! Sets the input receiving scene manager.
		/** If set to null, the main scene manager (returned by GetSceneManager()) will receive the input */
		virtual void setInputReceivingSceneManager(boost::shared_ptr<scene::ISceneManager> sceneManager);

		//! Returns a pointer to the logger.
		virtual ILogger* getLogger();

		//! Provides access to the engine's currently set randomizer.
		virtual IRandomizer* getRandomizer() const;

		//! Sets a new randomizer.
		virtual void setRandomizer(IRandomizer* r);

		//! Creates a new default randomizer.
		virtual IRandomizer* createDefaultRandomizer() const;

		//! Returns the operation system opertator object.
		virtual boost::shared_ptr<IOSOperator> getOSOperator();

		//! Checks if the window is running in fullscreen mode.
		virtual bool isFullscreen() const;

		//! get color format of the current window
		virtual video::ECOLOR_FORMAT getColorFormat() const;

		//! Activate any joysticks, and generate events for them.
		virtual bool activateJoysticks(core::array<SJoystickInfo> & joystickInfo);

		//! Set the current Gamma Value for the Display
		virtual bool setGammaRamp( f32 red, f32 green, f32 blue, f32 brightness, f32 contrast );

		//! Get the current Gamma Value for the Display
		virtual bool getGammaRamp( f32 &red, f32 &green, f32 &blue, f32 &brightness, f32 &contrast );

		//! Set the maximal elapsed time between 2 clicks to generate doubleclicks for the mouse. It also affects tripleclick behavior.
		//! When set to 0 no double- and tripleclicks will be generated.
		virtual void setDoubleClickTime( u32 timeMs );

		//! Get the maximal elapsed time between 2 clicks to generate double- and tripleclicks for the mouse.
		virtual u32 getDoubleClickTime() const;

		//! Remove all messages pending in the system message loop
		virtual void clearSystemMessages();


	protected:

		void createGUIAndScene();

		//! checks version of SDK and prints warning if there might be a problem
		bool checkVersion(const char* version);

		//! Compares to the last call of this function to return double and triple clicks.
		//! \return Returns only 1,2 or 3. A 4th click will start with 1 again.
		virtual u32 checkSuccessiveClicks(s32 mouseX, s32 mouseY, EMOUSE_INPUT_EVENT inputEvent );

		void calculateGammaRamp ( u16 *ramp, f32 gamma, f32 relativebrightness, f32 relativecontrast );
		void calculateGammaFromRamp ( f32 &gamma, const u16 *ramp );

		boost::shared_ptr<video::IVideoDriver> VideoDriver;
		boost::shared_ptr<gui::IGUIEnvironment> GUIEnvironment;
		boost::shared_ptr<scene::ISceneManager> SceneManager;
		boost::shared_ptr<ITimer> Timer;
		boost::shared_ptr<gui::ICursorControl> CursorControl;
		IEventReceiver* UserReceiver;
		CLogger* Logger;
		boost::shared_ptr<IOSOperator> Operator;
		IRandomizer* Randomizer;
		boost::shared_ptr<io::IFileSystem> FileSystem;
		boost::shared_ptr<scene::ISceneManager> InputReceivingSceneManager;

		struct SMouseMultiClicks
		{
			SMouseMultiClicks()
				: DoubleClickTime(500), CountSuccessiveClicks(0), LastClickTime(0), LastMouseInputEvent(EMIE_COUNT)
			{}

			u32 DoubleClickTime;
			u32 CountSuccessiveClicks;
			u32 LastClickTime;
			core::position2di LastClick;
			EMOUSE_INPUT_EVENT LastMouseInputEvent;
		};
		SMouseMultiClicks MouseMultiClicks;
		boost::shared_ptr<video::CVideoModeList> VideoModeList;
		SIrrlichtCreationParameters CreationParams;
		bool Close;
	};

} // end namespace irr

#endif

