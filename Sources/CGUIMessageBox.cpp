// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "CGUIMessageBox.h"
#ifdef _IRR_COMPILE_WITH_GUI_

#include "IGUISkin.h"
#include "IGUIEnvironment.h"
#include "IGUIButton.h"
#include "IGUIFont.h"
#include "ITexture.h"

namespace irr
{
namespace gui
{

//! constructor
CGUIMessageBox::CGUIMessageBox(boost::shared_ptr<IGUIEnvironment> environment, const wchar_t* caption,
	const wchar_t* text, s32 flags,
	boost::shared_ptr<IGUIElement> parent, s32 id, core::rect<s32> rectangle, video::ITexture* image)
: CGUIWindow(environment, parent, id, rectangle),
	OkButton(0), CancelButton(0), YesButton(0), NoButton(0), StaticText(0),
	Icon(0), IconTexture(image),
	Flags(flags), MessageText(text), Pressed(false)
{
	#ifdef _DEBUG
	setDebugName("CGUIMessageBox");
	#endif

	if (caption)
		setText(caption);
}


//! destructor
CGUIMessageBox::~CGUIMessageBox()
{
	if ( IconTexture )
		IconTexture->drop();
}

void CGUIMessageBox::setButton(boost::shared_ptr<IGUIButton>& button, bool isAvailable, const core::rect<s32> & btnRect, const wchar_t * text, boost::shared_ptr<IGUIElement>& focusMe)
{
	// add/remove ok button
	if (isAvailable)
	{
		if (!button)
		{
			button = getSharedEnvironment()->addButton(btnRect, getSharedThis());
			button->setSubElement(true);
		}
		else
			button->setRelativePosition(btnRect);

		button->setText(text);

		focusMe = button;
	}
	else if (button)
	{
		button->remove();
		button =0;
	}
}

void CGUIMessageBox::refreshControls()
{
	// Layout can be seen as 4 boxes (a layoutmanager would be nice)
	// One box at top over the whole width for title
	// Two boxes with same height at the middle beside each other for icon and for text
	// One box at the bottom for the buttons

	boost::shared_ptr<IGUIEnvironment> lockedEnvironment = getSharedEnvironment();
	const boost::shared_ptr<IGUISkin> skin = lockedEnvironment->getSkin();
	boost::shared_ptr<IGUIElement> lockedThis = getSharedThis();

	const s32 buttonHeight   = skin->getSize(EGDS_BUTTON_HEIGHT);
	const s32 buttonWidth    = skin->getSize(EGDS_BUTTON_WIDTH);
	const s32 titleHeight    = skin->getSize(EGDS_WINDOW_BUTTON_WIDTH)+2;	// titlebar has no own constant
	const s32 buttonDistance = skin->getSize(EGDS_WINDOW_BUTTON_WIDTH);
	const s32 borderWidth 	 = skin->getSize(EGDS_MESSAGE_BOX_GAP_SPACE);

	// add the static text for the message
	core::rect<s32> staticRect;
	staticRect.UpperLeftCorner.X = borderWidth;
	staticRect.UpperLeftCorner.Y = titleHeight + borderWidth;
	staticRect.LowerRightCorner.X = staticRect.UpperLeftCorner.X + skin->getSize(EGDS_MESSAGE_BOX_MAX_TEXT_WIDTH);
	staticRect.LowerRightCorner.Y = staticRect.UpperLeftCorner.Y + skin->getSize(EGDS_MESSAGE_BOX_MAX_TEXT_HEIGHT);
	if (!StaticText)
	{
		StaticText = lockedEnvironment->addStaticText(MessageText.c_str(), staticRect, false, false, lockedThis);

		StaticText->setWordWrap(true);
		StaticText->setSubElement(true);
	}
	else
	{
		StaticText->setRelativePosition(staticRect);
		StaticText->setText(MessageText.c_str());
	}

	s32 textHeight  = StaticText->getTextHeight();
	s32 textWidth = StaticText->getTextWidth() + 6;	// +6 because the static itself needs that
	const s32 iconHeight = IconTexture ? IconTexture->getOriginalSize().Height : 0;

	if ( textWidth < skin->getSize(EGDS_MESSAGE_BOX_MIN_TEXT_WIDTH) )
		textWidth = skin->getSize(EGDS_MESSAGE_BOX_MIN_TEXT_WIDTH) + 6;
	// no neeed to check for max because it couldn't get larger due to statictextbox.
	if ( textHeight < skin->getSize(EGDS_MESSAGE_BOX_MIN_TEXT_HEIGHT) )
		textHeight = skin->getSize(EGDS_MESSAGE_BOX_MIN_TEXT_HEIGHT);
	if ( textHeight > skin->getSize(EGDS_MESSAGE_BOX_MAX_TEXT_HEIGHT) )
		textHeight = skin->getSize(EGDS_MESSAGE_BOX_MAX_TEXT_HEIGHT);

	// content is text + icons + borders (but not titlebar)
	s32 contentHeight = textHeight > iconHeight ? textHeight : iconHeight;
	contentHeight += borderWidth;
	s32 contentWidth = 0;

	// add icon
	if ( IconTexture )
	{
		core::position2d<s32> iconPos;
		iconPos.Y = titleHeight + borderWidth;
		if ( iconHeight < textHeight )
			iconPos.Y += (textHeight-iconHeight) / 2;
		iconPos.X = borderWidth;

		if (!Icon)
		{
			Icon = lockedEnvironment->addImage(IconTexture, iconPos, true, lockedThis);
			Icon->setSubElement(true);
		}
		else
		{
			core::rect<s32> iconRect( iconPos.X, iconPos.Y, iconPos.X + IconTexture->getOriginalSize().Width, iconPos.Y + IconTexture->getOriginalSize().Height );
			Icon->setRelativePosition(iconRect);
		}

		contentWidth += borderWidth + IconTexture->getOriginalSize().Width;
	}
	else if ( Icon )
	{
		Icon->remove();
		Icon = 0;
	}

	// position text
	core::rect<s32> textRect;
	textRect.UpperLeftCorner.X = contentWidth + borderWidth;
	textRect.UpperLeftCorner.Y = titleHeight + borderWidth;
	if ( textHeight < iconHeight )
		textRect.UpperLeftCorner.Y += (iconHeight-textHeight) / 2;
	textRect.LowerRightCorner.X = textRect.UpperLeftCorner.X + textWidth;
	textRect.LowerRightCorner.Y = textRect.UpperLeftCorner.Y + textHeight;
	contentWidth += 2*borderWidth + textWidth;
	StaticText->setRelativePosition( textRect );

	// find out button size needs
	s32 countButtons = 0;
	if (Flags & EMBF_OK)
		++countButtons;
	if (Flags & EMBF_CANCEL)
		++countButtons;
	if (Flags & EMBF_YES)
		++countButtons;
	if (Flags & EMBF_NO)
		++countButtons;

	s32 buttonBoxWidth = countButtons * buttonWidth + 2 * borderWidth;
	if ( countButtons > 1 )
		buttonBoxWidth += (countButtons-1) * buttonDistance;
	s32 buttonBoxHeight = buttonHeight + 2 * borderWidth;

	// calc new message box sizes
	core::rect<s32> tmp = getRelativePosition();
	s32 msgBoxHeight = titleHeight + contentHeight + buttonBoxHeight;
	s32 msgBoxWidth = contentWidth > buttonBoxWidth ? contentWidth : buttonBoxWidth;

	boost::shared_ptr<IGUIElement> lockedParent = getParent();
	// adjust message box position
	tmp.UpperLeftCorner.Y = (lockedParent->getAbsolutePosition().getHeight() - msgBoxHeight) / 2;
	tmp.LowerRightCorner.Y = tmp.UpperLeftCorner.Y + msgBoxHeight;
	tmp.UpperLeftCorner.X = (lockedParent->getAbsolutePosition().getWidth() - msgBoxWidth) / 2;
	tmp.LowerRightCorner.X = tmp.UpperLeftCorner.X + msgBoxWidth;
	setRelativePosition(tmp);

	// add buttons

	core::rect<s32> btnRect;
	btnRect.UpperLeftCorner.Y = titleHeight + contentHeight + borderWidth;
	btnRect.LowerRightCorner.Y = btnRect.UpperLeftCorner.Y + buttonHeight;
	btnRect.UpperLeftCorner.X = borderWidth;
	if ( contentWidth > buttonBoxWidth )
		btnRect.UpperLeftCorner.X += (contentWidth - buttonBoxWidth) / 2;	// center buttons
	btnRect.LowerRightCorner.X = btnRect.UpperLeftCorner.X + buttonWidth;

	boost::shared_ptr<IGUIElement> focusMe = 0;
	setButton(OkButton, (Flags & EMBF_OK) != 0, btnRect, skin->getDefaultText(EGDT_MSG_BOX_OK), focusMe);
	if ( Flags & EMBF_OK )
		btnRect += core::position2d<s32>(buttonWidth + buttonDistance, 0);
	setButton(CancelButton, (Flags & EMBF_CANCEL) != 0, btnRect, skin->getDefaultText(EGDT_MSG_BOX_CANCEL), focusMe);
	if ( Flags & EMBF_CANCEL )
		btnRect += core::position2d<s32>(buttonWidth + buttonDistance, 0);
	setButton(YesButton, (Flags & EMBF_YES) != 0, btnRect, skin->getDefaultText(EGDT_MSG_BOX_YES), focusMe);
	if ( Flags & EMBF_YES )
		btnRect += core::position2d<s32>(buttonWidth + buttonDistance, 0);
	setButton(NoButton, (Flags & EMBF_NO) != 0, btnRect, skin->getDefaultText(EGDT_MSG_BOX_NO), focusMe);

	if (lockedEnvironment->hasFocus(lockedThis) && focusMe)
		lockedEnvironment->setFocus(focusMe);
}


//! called if an event happened.
bool CGUIMessageBox::OnEvent(const SEvent& event)
{
	if (isEnabled())
	{
		boost::shared_ptr<IGUIElement> lockedParent = getParent();
		boost::shared_ptr<IGUIEnvironment> lockedEnvironment = getSharedEnvironment();

		SEvent outevent;
		outevent.EventType = EET_GUI_EVENT;
		outevent.GUIEvent.Caller = getSharedThis();
		outevent.GUIEvent.Element = 0;

		switch(event.EventType)
		{
		case EET_KEY_INPUT_EVENT:

			if (event.KeyInput.PressedDown)
			{
				switch (event.KeyInput.Key)
				{
				case KEY_RETURN:
					if (OkButton)
					{
						OkButton->setPressed(true);
						Pressed = true;
					}
					break;
				case KEY_KEY_Y:
					if (YesButton)
					{
						YesButton->setPressed(true);
						Pressed = true;
					}
					break;
				case KEY_KEY_N:
					if (NoButton)
					{
						NoButton->setPressed(true);
						Pressed = true;
					}
					break;
				case KEY_ESCAPE:
					if (Pressed)
					{
						// cancel press
						if (OkButton) OkButton->setPressed(false);
						if (YesButton) YesButton->setPressed(false);
						if (NoButton) NoButton->setPressed(false);
						Pressed = false;
					}
					else
					if (CancelButton)
					{
						CancelButton->setPressed(true);
						Pressed = true;
					}
					else
					if (CloseButton && CloseButton->isVisible())
					{
						CloseButton->setPressed(true);
						Pressed = true;
					}
					break;
				default: // no other key is handled here
					break;
				}
			}
			else
			if (Pressed)
			{
				if (OkButton && event.KeyInput.Key == KEY_RETURN)
				{
					setVisible(false);	// this is a workaround to make sure it's no longer the hovered element, crashes on pressing 1-2 times ESC
					lockedEnvironment->setFocus(0);
					outevent.GUIEvent.EventType = EGET_MESSAGEBOX_OK;
					lockedParent->OnEvent(outevent);
					remove();
					return true;
				}
				else
				if ((CancelButton || CloseButton) && event.KeyInput.Key == KEY_ESCAPE)
				{
					setVisible(false);	// this is a workaround to make sure it's no longer the hovered element, crashes on pressing 1-2 times ESC
					lockedEnvironment->setFocus(0);
					outevent.GUIEvent.EventType = EGET_MESSAGEBOX_CANCEL;
					lockedParent->OnEvent(outevent);
					remove();
					return true;
				}
				else
				if (YesButton && event.KeyInput.Key == KEY_KEY_Y)
				{
					setVisible(false);	// this is a workaround to make sure it's no longer the hovered element, crashes on pressing 1-2 times ESC
					lockedEnvironment->setFocus(0);
					outevent.GUIEvent.EventType = EGET_MESSAGEBOX_YES;
					lockedParent->OnEvent(outevent);
					remove();
					return true;
				}
				else
				if (NoButton && event.KeyInput.Key == KEY_KEY_N)
				{
					setVisible(false);	// this is a workaround to make sure it's no longer the hovered element, crashes on pressing 1-2 times ESC
					lockedEnvironment->setFocus(0);
					outevent.GUIEvent.EventType = EGET_MESSAGEBOX_NO;
					lockedParent->OnEvent(outevent);
					remove();
					return true;
				}
			}
			break;
		case EET_GUI_EVENT:
			if (event.GUIEvent.EventType == EGET_BUTTON_CLICKED)
			{
				if (event.GUIEvent.Caller == OkButton)
				{
					setVisible(false);	// this is a workaround to make sure it's no longer the hovered element, crashes on pressing 1-2 times ESC
					lockedEnvironment->setFocus(0);
					outevent.GUIEvent.EventType = EGET_MESSAGEBOX_OK;
					lockedParent->OnEvent(outevent);
					remove();
					return true;
				}
				else
				if (event.GUIEvent.Caller == CancelButton ||
					event.GUIEvent.Caller == CloseButton)
				{
					setVisible(false);	// this is a workaround to make sure it's no longer the hovered element, crashes on pressing 1-2 times ESC
					lockedEnvironment->setFocus(0);
					outevent.GUIEvent.EventType = EGET_MESSAGEBOX_CANCEL;
					lockedParent->OnEvent(outevent);
					remove();
					return true;
				}
				else
				if (event.GUIEvent.Caller == YesButton)
				{
					setVisible(false);	// this is a workaround to make sure it's no longer the hovered element, crashes on pressing 1-2 times ESC
					lockedEnvironment->setFocus(0);
					outevent.GUIEvent.EventType = EGET_MESSAGEBOX_YES;
					lockedParent->OnEvent(outevent);
					remove();
					return true;
				}
				else
				if (event.GUIEvent.Caller == NoButton)
				{
					setVisible(false);	// this is a workaround to make sure it's no longer the hovered element, crashes on pressing 1-2 times ESC
					lockedEnvironment->setFocus(0);
					outevent.GUIEvent.EventType = EGET_MESSAGEBOX_NO;
					lockedParent->OnEvent(outevent);
					remove();
					return true;
				}
			}
			break;
		default:
			break;
		}
	}

	return CGUIWindow::OnEvent(event);
}


//! Writes attributes of the element.
void CGUIMessageBox::serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options=0) const
{
	CGUIWindow::serializeAttributes(out,options);

	out->addBool	("OkayButton",		(Flags & EMBF_OK)	!= 0 );
	out->addBool	("CancelButton",	(Flags & EMBF_CANCEL)	!= 0 );
	out->addBool	("YesButton",		(Flags & EMBF_YES)	!= 0 );
	out->addBool	("NoButton",		(Flags & EMBF_NO)	!= 0 );
	out->addTexture	("Texture",			IconTexture);

	out->addString	("MessageText",		MessageText.c_str());
}


//! Reads attributes of the element
void CGUIMessageBox::deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options=0)
{
	Flags = 0;

	Flags  = in->getAttributeAsBool("OkayButton")  ? EMBF_OK     : 0;
	Flags |= in->getAttributeAsBool("CancelButton")? EMBF_CANCEL : 0;
	Flags |= in->getAttributeAsBool("YesButton")   ? EMBF_YES    : 0;
	Flags |= in->getAttributeAsBool("NoButton")    ? EMBF_NO     : 0;

	if ( IconTexture )
	{
		IconTexture->drop();
		IconTexture = NULL;
	}
	IconTexture = in->getAttributeAsTexture("Texture");
	if ( IconTexture )
		IconTexture->grab();

	MessageText = in->getAttributeAsStringW("MessageText").c_str();

	CGUIWindow::deserializeAttributes(in,options);

	refreshControls();
}

void CGUIMessageBox::setWeakThis(boost::shared_ptr<IGUIElement> value)
{
	IGUIElement::setWeakThis(value);

	// set element type
	Type = EGUIET_MESSAGE_BOX;

	boost::shared_ptr<IGUIEnvironment> lockedEnvironment = getSharedEnvironment();

	// remove focus
	lockedEnvironment->setFocus(0);

	// remove buttons

	getMaximizeButton()->remove();
	getMinimizeButton()->remove();

	lockedEnvironment->setFocus(getSharedThis());

	if (IconTexture)
		IconTexture->grab();

	refreshControls();
}


} // end namespace gui
} // end namespace irr

#endif // _IRR_COMPILE_WITH_GUI_

