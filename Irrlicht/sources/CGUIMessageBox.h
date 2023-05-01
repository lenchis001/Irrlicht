// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_GUI_MESSAGE_BOX_H_INCLUDED__
#define __C_GUI_MESSAGE_BOX_H_INCLUDED__

#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_GUI_

#include "CGUIWindow.h"
#include "IGUIStaticText.h"
#include "IGUIImage.h"
#include "irrArray.h"

namespace irr
{
namespace gui
{
	class CGUIMessageBox : public CGUIWindow
	{
	public:

		//! constructor
		CGUIMessageBox(boost::shared_ptr<IGUIEnvironment> environment, const wchar_t* caption,
			const wchar_t* text, s32 flag,
			boost::shared_ptr<IGUIElement> parent, s32 id, core::rect<s32> rectangle, boost::shared_ptr<video::ITexture> image=0);

		//! destructor
		virtual ~CGUIMessageBox();

		//! called if an event happened.
		virtual bool OnEvent(const SEvent& event);

		//! Writes attributes of the element.
		virtual void serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options) const;

		//! Reads attributes of the element
		virtual void deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options);

		virtual void setWeakThis(boost::shared_ptr<IGUIElement> value) override;

	private:

		void refreshControls();
		void setButton(boost::shared_ptr<IGUIButton>& button, bool isAvailable, const core::rect<s32> & btnRect, const wchar_t * text, boost::shared_ptr<IGUIElement>& focusMe);

		boost::shared_ptr<IGUIButton> OkButton;
		boost::shared_ptr<IGUIButton> CancelButton;
		boost::shared_ptr<IGUIButton> YesButton;
		boost::shared_ptr<IGUIButton> NoButton;
		boost::shared_ptr<IGUIStaticText> StaticText;
		boost::shared_ptr<IGUIImage>  Icon;
		boost::shared_ptr<video::ITexture>  IconTexture;

		s32 Flags;
		core::stringw MessageText;
		bool Pressed;
	};

} // end namespace gui
} // end namespace irr

#endif // _IRR_COMPILE_WITH_GUI_

#endif
