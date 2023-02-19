// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "CGUIMenu.h"
#ifdef _IRR_COMPILE_WITH_GUI_

#include "IGUISkin.h"
#include "IGUIEnvironment.h"
#include "IVideoDriver.h"
#include "IGUIFont.h"
#include "IGUIWindow.h"

#include "os.h"

namespace irr
{
namespace gui
{

//! constructor
CGUIMenu::CGUIMenu(boost::shared_ptr<IGUIEnvironment> environment, boost::shared_ptr<IGUIElement> parent,
		s32 id, core::rect<s32> rectangle)
	: CGUIContextMenu(environment, parent, id, rectangle, false, true)
{
	#ifdef _DEBUG
	setDebugName("CGUIMenu");
	#endif

	Type = EGUIET_MENU;
}


//! draws the element and its children
void CGUIMenu::draw()
{
	if (!IsVisible)
		return;

	boost::shared_ptr<IGUISkin> skin = getSharedEnvironment()->getSkin();
	boost::shared_ptr<IGUIFont> font = skin->getFont(EGDF_MENU);

	if (font != LastFont)
	{
		LastFont = font;

		recalculateSize();
	}

	core::rect<s32> rect = AbsoluteRect;
	boost::shared_ptr<IGUIElement> lockedThis = getSharedThis();

	// draw frame

	skin->draw3DToolBar(lockedThis, rect, &AbsoluteClippingRect);

	// loop through all menu items

	rect = AbsoluteRect;

	for (s32 i=0; i<(s32)Items.size(); ++i)
	{
		if (!Items[i].IsSeparator)
		{
			rect = getRect(Items[i], AbsoluteRect);

			// draw highlighted
			if (i == HighLighted && Items[i].Enabled)
			{
				skin->draw3DSunkenPane(lockedThis, skin->getColor(EGDC_3D_DARK_SHADOW),
					true, true, rect, &AbsoluteClippingRect);
			}
			// draw text

			EGUI_DEFAULT_COLOR c = EGDC_BUTTON_TEXT;

			if (i == HighLighted)
				c = EGDC_HIGH_LIGHT_TEXT;

			if (!Items[i].Enabled)
				c = EGDC_GRAY_TEXT;

			if (font)
				font->draw(Items[i].Text.c_str(), rect,
					skin->getColor(c), true, true, &AbsoluteClippingRect);
		}
	}

	IGUIElement::draw();
}


//! called if an event happened.
bool CGUIMenu::OnEvent(const SEvent& event)
{
	if (isEnabled())
	{
		boost::shared_ptr<IGUIElement> lockedThis = getSharedThis();
		boost::shared_ptr<IGUIElement> lockedParent = getParent();

		switch(event.EventType)
		{
		case EET_GUI_EVENT:
			switch(event.GUIEvent.EventType)
			{
			case gui::EGET_ELEMENT_FOCUS_LOST:
				if (event.GUIEvent.Caller.get() == this && !isMyChild(event.GUIEvent.Element))
				{
					closeAllSubMenus();
					HighLighted = -1;
				}
				break;
			case gui::EGET_ELEMENT_FOCUSED:
				if (event.GUIEvent.Caller.get() == this && lockedParent)
				{
					lockedParent->bringToFront(getSharedThis());
				}
				break;
			default:
				break;
			}
			break;
		case EET_MOUSE_INPUT_EVENT: {
			boost::shared_ptr<IGUIEnvironment> lockedEnvironment = getSharedEnvironment();

			switch (event.MouseInput.Event)
			{
			case EMIE_LMOUSE_PRESSED_DOWN:
			{
				if (!lockedEnvironment->hasFocus(lockedThis))
				{
					lockedEnvironment->setFocus(lockedThis);
				}

				if (lockedParent)
					lockedParent->bringToFront(lockedThis);

				core::position2d<s32> p(event.MouseInput.X, event.MouseInput.Y);
				bool shouldCloseSubMenu = hasOpenSubMenu();
				if (!AbsoluteClippingRect.isPointInside(p))
				{
					shouldCloseSubMenu = false;
				}
				highlight(core::position2d<s32>(event.MouseInput.X, event.MouseInput.Y), true);
				if (shouldCloseSubMenu)
				{
					lockedEnvironment->removeFocus(lockedThis);
				}

				return true;
			}
			case EMIE_LMOUSE_LEFT_UP:
			{
				core::position2d<s32> p(event.MouseInput.X, event.MouseInput.Y);
				if (!AbsoluteClippingRect.isPointInside(p))
				{
					s32 t = sendClick(p);
					if ((t == 0 || t == 1) && lockedEnvironment->hasFocus(lockedThis))
						lockedEnvironment->removeFocus(lockedThis);
				}

				return true;
			}
			case EMIE_MOUSE_MOVED:
				if (lockedEnvironment->hasFocus(lockedThis) && HighLighted >= 0)
				{
					s32 oldHighLighted = HighLighted;
					highlight(core::position2d<s32>(event.MouseInput.X, event.MouseInput.Y), true);
					if (HighLighted < 0)
						HighLighted = oldHighLighted;   // keep last hightlight active when moving outside the area
				}
				return true;
			default:
				break;
			}
			break;
		}
		default:
			break;
		}
	}

	return IGUIElement::OnEvent(event);
}

void CGUIMenu::recalculateSize()
{
	core::rect<s32> clientRect; // client rect of parent
	boost::shared_ptr<IGUIElement> lockedParent = getParent();

	if (lockedParent && lockedParent->hasType(EGUIET_WINDOW) )
	{
		clientRect = boost::static_pointer_cast<IGUIWindow>(getParent())->getClientRect();
	}
	else if (lockedParent)
	{
		clientRect = core::rect<s32>(0,0, lockedParent->getAbsolutePosition().getWidth(),
			lockedParent->getAbsolutePosition().getHeight());
	}
	else
	{
		clientRect = RelativeRect;
	}


	boost::shared_ptr<IGUISkin> skin = getSharedEnvironment()->getSkin();
	boost::shared_ptr<IGUIFont> font = skin->getFont(EGDF_MENU);

	if (!font)
	{
		if (lockedParent && skin)
			RelativeRect = core::rect<s32>(clientRect.UpperLeftCorner.X, clientRect.UpperLeftCorner.Y,
					clientRect.LowerRightCorner.X, clientRect.UpperLeftCorner.Y+skin->getSize(EGDS_MENU_HEIGHT));
		return;
	}

	core::rect<s32> rect;
	rect.UpperLeftCorner = clientRect.UpperLeftCorner;
	s32 height = font->getDimension(L"A").Height + 5;
	//if (skin && height < skin->getSize ( EGDS_MENU_HEIGHT ))
	//	height = skin->getSize(EGDS_MENU_HEIGHT);
	s32 width = rect.UpperLeftCorner.X;
	s32 i;

	for (i=0; i<(s32)Items.size(); ++i)
	{
		if (Items[i].IsSeparator)
		{
			Items[i].Dim.Width = 0;
			Items[i].Dim.Height = height;
		}
		else
		{
			Items[i].Dim = font->getDimension(Items[i].Text.c_str());
			Items[i].Dim.Width += 20;
		}

		Items[i].PosY = width;
		width += Items[i].Dim.Width;
	}

	width = clientRect.getWidth();

	rect.LowerRightCorner.X = rect.UpperLeftCorner.X + width;
	rect.LowerRightCorner.Y = rect.UpperLeftCorner.Y + height;

	setRelativePosition(rect);

	// recalculate submenus
	for (i=0; i<(s32)Items.size(); ++i)
		if (Items[i].SubMenu)
		{
			// move submenu
			s32 w = Items[i].SubMenu->getAbsolutePosition().getWidth();
			s32 h = Items[i].SubMenu->getAbsolutePosition().getHeight();

			Items[i].SubMenu->setRelativePosition(
				core::rect<s32>(Items[i].PosY, height ,
					Items[i].PosY+w-5, height+h));
		}
}


//! returns the item highlight-area
core::rect<s32> CGUIMenu::getHRect(const SItem& i, const core::rect<s32>& absolute) const
{
	core::rect<s32> r = absolute;
	r.UpperLeftCorner.X += i.PosY;
	r.LowerRightCorner.X = r.UpperLeftCorner.X + i.Dim.Width;
	return r;
}


//! Gets drawing rect of Item
core::rect<s32> CGUIMenu::getRect(const SItem& i, const core::rect<s32>& absolute) const
{
	return getHRect(i, absolute);
}


void CGUIMenu::updateAbsolutePosition()
{
	boost::shared_ptr<IGUIElement> lockedParent = getParent();

	if (lockedParent)
		DesiredRect.LowerRightCorner.X = lockedParent->getAbsolutePosition().getWidth();

	IGUIElement::updateAbsolutePosition();
}

void CGUIMenu::setWeakThis(boost::shared_ptr<IGUIElement> value)
{
	IGUIElement::setWeakThis(value);

	setNotClipped(false);

	recalculateSize();
}


} // end namespace
} // end namespace

#endif // _IRR_COMPILE_WITH_GUI_

