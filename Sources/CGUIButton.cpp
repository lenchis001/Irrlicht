// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "CGUIButton.h"
#ifdef _IRR_COMPILE_WITH_GUI_

#include "IGUISkin.h"
#include "IGUIEnvironment.h"
#include "IVideoDriver.h"
#include "IGUIFont.h"
#include "os.h"

namespace irr
{
namespace gui
{

//! constructor
CGUIButton::CGUIButton(boost::shared_ptr<IGUIEnvironment> environment, boost::shared_ptr<IGUIElement> parent,
			s32 id, core::rect<s32> rectangle, bool noclip)
: IGUIButton(environment, parent, id, rectangle),
	SpriteBank(0), OverrideFont(0), Image(0), PressedImage(0),
	ClickTime(0), HoverTime(0), FocusTime(0), 
	IsPushButton(false), Pressed(false),
	UseAlphaChannel(false), DrawBorder(true), ScaleImage(false),
	_noClip(noclip)
{
	#ifdef _DEBUG
	setDebugName("CGUIButton");
	#endif
}


//! destructor
CGUIButton::~CGUIButton()
{
	if (Image)
		Image->drop();

	if (PressedImage)
		PressedImage->drop();

	if (SpriteBank)
		SpriteBank->drop();
}


//! Sets if the images should be scaled to fit the button
void CGUIButton::setScaleImage(bool scaleImage)
{
	ScaleImage = scaleImage;
}


//! Returns whether the button scale the used images
bool CGUIButton::isScalingImage() const
{
	_IRR_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX;
	return ScaleImage;
}


//! Sets if the button should use the skin to draw its border
void CGUIButton::setDrawBorder(bool border)
{
	DrawBorder = border;
}


void CGUIButton::setSpriteBank(IGUISpriteBank* sprites)
{
	if (sprites)
		sprites->grab();

	if (SpriteBank)
		SpriteBank->drop();

	SpriteBank = sprites;
}


void CGUIButton::setSprite(EGUI_BUTTON_STATE state, s32 index, video::SColor color, bool loop)
{
	if (SpriteBank)
	{
		ButtonSprites[(u32)state].Index	= index;
		ButtonSprites[(u32)state].Color	= color;
		ButtonSprites[(u32)state].Loop	= loop;
	}
	else
	{
		ButtonSprites[(u32)state].Index = -1;
	}
}


//! called if an event happened.
bool CGUIButton::OnEvent(const SEvent& event)
{
	if (!isEnabled())
		return IGUIElement::OnEvent(event);

	switch(event.EventType)
	{
	case EET_KEY_INPUT_EVENT:
		if (event.KeyInput.PressedDown &&
			(event.KeyInput.Key == KEY_RETURN || event.KeyInput.Key == KEY_SPACE))
		{
			if (!IsPushButton)
				setPressed(true);
			else
				setPressed(!Pressed);

			return true;
		}
		if (Pressed && !IsPushButton && event.KeyInput.PressedDown && event.KeyInput.Key == KEY_ESCAPE)
		{
			setPressed(false);
			return true;
		}
		else
		if (!event.KeyInput.PressedDown && Pressed &&
			(event.KeyInput.Key == KEY_RETURN || event.KeyInput.Key == KEY_SPACE))
		{

			if (!IsPushButton)
				setPressed(false);

			if (!Parent.expired())
			{
				SEvent newEvent;
				newEvent.EventType = EET_GUI_EVENT;
				newEvent.GUIEvent.Caller = getSharedThis();
				newEvent.GUIEvent.Element = 0;
				newEvent.GUIEvent.EventType = EGET_BUTTON_CLICKED;
				getParent()->OnEvent(newEvent);
			}
			return true;
		}
		break;
	case EET_GUI_EVENT:
		if (event.GUIEvent.Caller.get() == this)
		{
			if (event.GUIEvent.EventType == EGET_ELEMENT_FOCUS_LOST)
			{
				if (!IsPushButton)
					setPressed(false);
				FocusTime = os::Timer::getTime();
			}
			else if (event.GUIEvent.EventType == EGET_ELEMENT_FOCUSED)
			{
				FocusTime = os::Timer::getTime();				
			}
			else if (event.GUIEvent.EventType == EGET_ELEMENT_HOVERED || event.GUIEvent.EventType == EGET_ELEMENT_LEFT)
			{
				HoverTime = os::Timer::getTime();
			}
		}
		break;
	case EET_MOUSE_INPUT_EVENT:
		if (event.MouseInput.Event == EMIE_LMOUSE_PRESSED_DOWN)
		{
			boost::shared_ptr<IGUIEnvironment> lockedEnvironment = getSharedEnvironment();

			if (lockedEnvironment->hasFocus(getSharedThis()) &&
				!AbsoluteClippingRect.isPointInside(core::position2d<s32>(event.MouseInput.X, event.MouseInput.Y)))
			{
					lockedEnvironment->removeFocus(getSharedThis());
					return false;
			}

			if (!IsPushButton)
				setPressed(true);

			lockedEnvironment->setFocus(getSharedThis());
			return true;
		}
		else
		if (event.MouseInput.Event == EMIE_LMOUSE_LEFT_UP)
		{
			bool wasPressed = Pressed;

			if ( !AbsoluteClippingRect.isPointInside( core::position2d<s32>(event.MouseInput.X, event.MouseInput.Y ) ) )
			{
				if (!IsPushButton)
					setPressed(false);
				return true;
			}

			if (!IsPushButton)
				setPressed(false);
			else
			{
				setPressed(!Pressed);
			}

			if ((!IsPushButton && wasPressed && !Parent.expired()) ||
				(IsPushButton && wasPressed != Pressed))
			{
				SEvent newEvent;
				newEvent.EventType = EET_GUI_EVENT;
				newEvent.GUIEvent.Caller = getSharedThis();
				newEvent.GUIEvent.Element = 0;
				newEvent.GUIEvent.EventType = EGET_BUTTON_CLICKED;
				getParent()->OnEvent(newEvent);
			}

			return true;
		}
		break;
	default:
		break;
	}

	return Parent.expired() ? false : getParent()->OnEvent(event);
}


//! draws the element and its children
void CGUIButton::draw()
{
	if (!IsVisible)
		return;

	boost::shared_ptr<IGUIEnvironment> lockedEnvironment = getSharedEnvironment();
	boost::shared_ptr<IGUISkin> skin = lockedEnvironment->getSkin();
	boost::shared_ptr<video::IVideoDriver> driver = lockedEnvironment->getVideoDriver();

	// todo:	move sprite up and text down if the pressed state has a sprite
	const core::position2di spritePos = AbsoluteRect.getCenter();

	if (!Pressed)
	{
		if (DrawBorder)
			skin->draw3DButtonPaneStandard(getSharedThis(), AbsoluteRect, &AbsoluteClippingRect);

		if (Image)
		{
			core::position2d<s32> pos = spritePos;
			pos.X -= ImageRect.getWidth() / 2;
			pos.Y -= ImageRect.getHeight() / 2;

			driver->draw2DImage(Image,
					ScaleImage? AbsoluteRect :
						core::recti(pos, ImageRect.getSize()),
					ImageRect, &AbsoluteClippingRect,
					0, UseAlphaChannel);
		}
	}
	else
	{
		if (DrawBorder)
			skin->draw3DButtonPanePressed(getSharedThis(), AbsoluteRect, &AbsoluteClippingRect);

		if (PressedImage)
		{
			core::position2d<s32> pos = spritePos;
			pos.X -= PressedImageRect.getWidth() / 2;
			pos.Y -= PressedImageRect.getHeight() / 2;

			if (Image == PressedImage && PressedImageRect == ImageRect)
			{
				pos.X += skin->getSize(EGDS_BUTTON_PRESSED_IMAGE_OFFSET_X);
				pos.Y += skin->getSize(EGDS_BUTTON_PRESSED_IMAGE_OFFSET_Y);
			}
			driver->draw2DImage(PressedImage,
					ScaleImage? AbsoluteRect :
						core::recti(pos, PressedImageRect.getSize()),
					PressedImageRect, &AbsoluteClippingRect,
					0, UseAlphaChannel);
		}
	}

	if (SpriteBank)
	{
		// pressed / unpressed animation
		u32 state = Pressed ? (u32)EGBS_BUTTON_DOWN : (u32)EGBS_BUTTON_UP;
		if (ButtonSprites[state].Index != -1)
		{
			SpriteBank->draw2DSprite(ButtonSprites[state].Index, spritePos,
			 	&AbsoluteClippingRect, ButtonSprites[state].Color, ClickTime, os::Timer::getTime(),
				ButtonSprites[state].Loop, true);
		}

		// focused / unfocused animation
		state = lockedEnvironment->hasFocus(getSharedThis()) ? (u32)EGBS_BUTTON_FOCUSED : (u32)EGBS_BUTTON_NOT_FOCUSED;
		if (ButtonSprites[state].Index != -1)
		{
			SpriteBank->draw2DSprite(ButtonSprites[state].Index, spritePos,
			 	&AbsoluteClippingRect, ButtonSprites[state].Color, FocusTime, os::Timer::getTime(),
				ButtonSprites[state].Loop, true);
		}

		// mouse over / off animation
		if (isEnabled())
		{
			state = lockedEnvironment->getHovered().get() == this ? (u32)EGBS_BUTTON_MOUSE_OVER : (u32)EGBS_BUTTON_MOUSE_OFF;
			if (ButtonSprites[state].Index != -1)
			{
				SpriteBank->draw2DSprite(ButtonSprites[state].Index, spritePos,
				 	&AbsoluteClippingRect, ButtonSprites[state].Color, HoverTime, os::Timer::getTime(),
					ButtonSprites[state].Loop, true);
			}
		}
	}

	if (Text.size())
	{
		boost::shared_ptr<IGUIFont> font = getActiveFont();

		core::rect<s32> rect = AbsoluteRect;
		if (Pressed)
		{
			rect.UpperLeftCorner.X += skin->getSize(EGDS_BUTTON_PRESSED_TEXT_OFFSET_X);
			rect.UpperLeftCorner.Y += skin->getSize(EGDS_BUTTON_PRESSED_TEXT_OFFSET_Y);
		}

		if (font)
			font->draw(Text.c_str(), rect,
				skin->getColor(isEnabled() ? EGDC_BUTTON_TEXT : EGDC_GRAY_TEXT),
				true, true, &AbsoluteClippingRect);
	}

	IGUIElement::draw();
}


//! sets another skin independent font. if this is set to zero, the button uses the font of the skin.
void CGUIButton::setOverrideFont(boost::shared_ptr<IGUIFont> font)
{
	if (OverrideFont == font)
		return;

	OverrideFont = font;
}

//! Gets the override font (if any)
boost::shared_ptr<IGUIFont>  CGUIButton::getOverrideFont() const
{
	return OverrideFont;
}

//! Get the font which is used right now for drawing
boost::shared_ptr<IGUIFont> CGUIButton::getActiveFont() 
{
	if ( OverrideFont )
		return OverrideFont;
	boost::shared_ptr<IGUISkin> skin = getSharedEnvironment()->getSkin();
	if (skin)
		return skin->getFont(EGDF_BUTTON);
	return 0;
}

//! Sets an image which should be displayed on the button when it is in normal state.
void CGUIButton::setImage(video::ITexture* image)
{
	if (image)
		image->grab();
	if (Image)
		Image->drop();

	Image = image;
	if (image)
		ImageRect = core::rect<s32>(core::position2d<s32>(0,0), image->getOriginalSize());

	if (!PressedImage)
		setPressedImage(Image);
}


//! Sets the image which should be displayed on the button when it is in its normal state.
void CGUIButton::setImage(video::ITexture* image, const core::rect<s32>& pos)
{
	setImage(image);
	ImageRect = pos;
}


//! Sets an image which should be displayed on the button when it is in pressed state.
void CGUIButton::setPressedImage(video::ITexture* image)
{
	if (image)
		image->grab();

	if (PressedImage)
		PressedImage->drop();

	PressedImage = image;
	if (image)
		PressedImageRect = core::rect<s32>(core::position2d<s32>(0,0), image->getOriginalSize());
}


//! Sets the image which should be displayed on the button when it is in its pressed state.
void CGUIButton::setPressedImage(video::ITexture* image, const core::rect<s32>& pos)
{
	setPressedImage(image);
	PressedImageRect = pos;
}


//! Sets if the button should behave like a push button. Which means it
//! can be in two states: Normal or Pressed. With a click on the button,
//! the user can change the state of the button.
void CGUIButton::setIsPushButton(bool isPushButton)
{
	IsPushButton = isPushButton;
}


//! Returns if the button is currently pressed
bool CGUIButton::isPressed() const
{
	_IRR_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX;
	return Pressed;
}


//! Sets the pressed state of the button if this is a pushbutton
void CGUIButton::setPressed(bool pressed)
{
	if (Pressed != pressed)
	{
		ClickTime = os::Timer::getTime();
		Pressed = pressed;
	}
}


//! Returns whether the button is a push button
bool CGUIButton::isPushButton() const
{
	_IRR_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX;
	return IsPushButton;
}


//! Sets if the alpha channel should be used for drawing images on the button (default is false)
void CGUIButton::setUseAlphaChannel(bool useAlphaChannel)
{
	UseAlphaChannel = useAlphaChannel;
}


//! Returns if the alpha channel should be used for drawing images on the button
bool CGUIButton::isAlphaChannelUsed() const
{
	_IRR_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX;
	return UseAlphaChannel;
}


bool CGUIButton::isDrawingBorder() const
{
	_IRR_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX;
	return DrawBorder;
}


//! Writes attributes of the element.
void CGUIButton::serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options=0) const
{
	IGUIButton::serializeAttributes(out,options);

	out->addBool	("PushButton",		IsPushButton );
	if (IsPushButton)
		out->addBool("Pressed",		Pressed);

	out->addTexture ("Image",		Image);
	out->addRect	("ImageRect",		ImageRect);
	out->addTexture	("PressedImage",	PressedImage);
	out->addRect	("PressedImageRect",	PressedImageRect);

	out->addBool	("UseAlphaChannel",	isAlphaChannelUsed());
	out->addBool	("Border",		isDrawingBorder());
	out->addBool	("ScaleImage",		isScalingImage());

	//   out->addString  ("OverrideFont",	OverrideFont);
}


//! Reads attributes of the element
void CGUIButton::deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options=0)
{
	IGUIButton::deserializeAttributes(in,options);

	IsPushButton	= in->getAttributeAsBool("PushButton");
	Pressed		= IsPushButton ? in->getAttributeAsBool("Pressed") : false;

	core::rect<s32> rec = in->getAttributeAsRect("ImageRect");
	if (rec.isValid())
		setImage( in->getAttributeAsTexture("Image"), rec);
	else
		setImage( in->getAttributeAsTexture("Image") );

	rec = in->getAttributeAsRect("PressedImageRect");
	if (rec.isValid())
		setPressedImage( in->getAttributeAsTexture("PressedImage"), rec);
	else
		setPressedImage( in->getAttributeAsTexture("PressedImage") );

	setDrawBorder(in->getAttributeAsBool("Border"));
	setUseAlphaChannel(in->getAttributeAsBool("UseAlphaChannel"));
	setScaleImage(in->getAttributeAsBool("ScaleImage"));

	//   setOverrideFont(in->getAttributeAsString("OverrideFont"));

	updateAbsolutePosition();
}

void CGUIButton::setWeakThis(boost::shared_ptr<IGUIElement> value)
{
	IGUIElement::setWeakThis(value);

	setNotClipped(_noClip);

	// Initialize the sprites.
	for (u32 i = 0; i < EGBS_COUNT; ++i)
		ButtonSprites[i].Index = -1;

	// This element can be tabbed.
	setTabStop(true);
	setTabOrder(-1);
}


} // end namespace gui
} // end namespace irr

#endif // _IRR_COMPILE_WITH_GUI_

