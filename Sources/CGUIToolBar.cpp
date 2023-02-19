// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "CGUIToolBar.h"
#ifdef _IRR_COMPILE_WITH_GUI_

#include "IGUISkin.h"
#include "IGUIEnvironment.h"
#include "IVideoDriver.h"
#include "IGUIButton.h"
#include "IGUIFont.h"
#include "CGUIButton.h"

namespace irr
{
namespace gui
{

//! constructor
CGUIToolBar::CGUIToolBar(boost::shared_ptr<IGUIEnvironment> environment, boost::shared_ptr<IGUIElement> parent, s32 id, core::rect<s32> rectangle)
:IGUIToolBar(environment, parent, id, rectangle), ButtonX(5)
{
	#ifdef _DEBUG
	setDebugName("CGUIToolBar");
	#endif
}


//! called if an event happened.
bool CGUIToolBar::OnEvent(const SEvent& event)
{
	if (isEnabled())
	{
		if (event.EventType == EET_MOUSE_INPUT_EVENT &&
			event.MouseInput.Event == EMIE_LMOUSE_PRESSED_DOWN)
		{
			if (AbsoluteClippingRect.isPointInside(core::position2di(event.MouseInput.X, event.MouseInput.Y)))
				return true;
		}
	}

	return IGUIElement::OnEvent(event);
}


//! draws the element and its children
void CGUIToolBar::draw()
{
	if (!IsVisible)
		return;

	boost::shared_ptr<IGUISkin> skin = getSharedEnvironment()->getSkin();
	if (!skin)
		return;

	core::rect<s32> rect = AbsoluteRect;
	core::rect<s32>* clip = &AbsoluteClippingRect;

	// draw frame
	skin->draw3DToolBar(getSharedThis(), rect, clip);

	IGUIElement::draw();
}


//! Updates the absolute position.
void CGUIToolBar::updateAbsolutePosition()
{
	if (!Parent.expired())
	{
		DesiredRect.UpperLeftCorner.X = 0;
		DesiredRect.LowerRightCorner.X = getParent()->getAbsolutePosition().getWidth();
	}

	IGUIElement::updateAbsolutePosition();
}


//! Adds a button to the tool bar
boost::shared_ptr<IGUIButton> CGUIToolBar::addButton(s32 id, const wchar_t* text,const wchar_t* tooltiptext,
	video::ITexture* img, video::ITexture* pressed, bool isPushButton,
	bool useAlphaChannel)
{
	ButtonX += 3;

	core::rect<s32> rectangle(ButtonX,2,ButtonX+1,3);
	if ( img )
	{
		const core::dimension2du &size = img->getOriginalSize();
		rectangle.LowerRightCorner.X = rectangle.UpperLeftCorner.X + size.Width + 8;
		rectangle.LowerRightCorner.Y = rectangle.UpperLeftCorner.Y + size.Height + 6;
	}

	boost::shared_ptr<IGUIEnvironment> lockedEnvironment = getSharedEnvironment();

	if ( text )
	{
		boost::shared_ptr<IGUISkin> skin = lockedEnvironment->getSkin();
		boost::shared_ptr<IGUIFont>  font = skin->getFont(EGDF_BUTTON);
		if ( font )
		{
			core::dimension2d<u32> dim = font->getDimension(text);
			if ( (s32)dim.Width > rectangle.getWidth() )
				rectangle.LowerRightCorner.X = rectangle.UpperLeftCorner.X + dim.Width + 8;
			if ( (s32)dim.Height > rectangle.getHeight() )
				rectangle.LowerRightCorner.Y = rectangle.UpperLeftCorner.Y + dim.Height + 6;
		}
	}

	ButtonX += rectangle.getWidth();

	boost::shared_ptr<IGUIButton> button = boost::make_shared<CGUIButton>(lockedEnvironment, getSharedThis(), id, rectangle);

	if (text)
		button->setText(text);

	if (tooltiptext)
		button->setToolTipText(tooltiptext);

	if (img)
		button->setImage(img);

	if (pressed)
		button->setPressedImage(pressed);

	if (isPushButton)
		button->setIsPushButton(isPushButton);

	if (useAlphaChannel)
		button->setUseAlphaChannel(useAlphaChannel);

	return button;
}

void CGUIToolBar::setWeakThis(boost::shared_ptr<IGUIElement> value)
{
	IGUIElement::setWeakThis(value);

	// calculate position and find other menubars
	s32 y = 0;
	s32 parentwidth = 100;

	if (!Parent.expired())
	{
		boost::shared_ptr<IGUIElement> lockedParent = getParent();

		parentwidth = lockedParent->getAbsolutePosition().getWidth();

		const core::list<boost::shared_ptr<IGUIElement>>& children = lockedParent->getChildren();
		core::list<boost::shared_ptr<IGUIElement>>::ConstIterator it = children.begin();
		for (; it != children.end(); ++it)
		{
			core::rect<s32> r = (*it)->getAbsolutePosition();
			if (r.UpperLeftCorner.X == 0 && r.UpperLeftCorner.Y <= y &&
				r.LowerRightCorner.X == parentwidth)
				y = r.LowerRightCorner.Y;
		}
	}

	core::rect<s32> rr;
	rr.UpperLeftCorner.X = 0;
	rr.UpperLeftCorner.Y = y;
	s32 height = getSharedEnvironment()->getSkin()->getSize(EGDS_MENU_HEIGHT);

	/*boost::shared_ptr<IGUISkin> skin = Environment->getSkin();
	boost::shared_ptr<IGUIFont> font = skin->getFont();
	if (font)
	{
		s32 t = font->getDimension(L"A").Height + 5;
		if (t > height)
			height = t;
	}*/

	rr.LowerRightCorner.X = parentwidth;
	rr.LowerRightCorner.Y = rr.UpperLeftCorner.Y + height;
	setRelativePosition(rr);
}

} // end namespace gui
} // end namespace irr

#endif // _IRR_COMPILE_WITH_GUI_

