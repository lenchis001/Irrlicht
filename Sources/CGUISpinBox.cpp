// Copyright (C) 2006-2012 Michael Zeilfelder
// This file uses the licence of the Irrlicht Engine.

#include "CGUISpinBox.h"
#ifdef _IRR_COMPILE_WITH_GUI_

#include "CGUIEditBox.h"
#include "CGUIButton.h"
#include "IGUIEnvironment.h"
#include "IEventReceiver.h"
#include "fast_atof.h"
#include <wchar.h>


namespace irr
{
namespace gui
{

//! constructor
CGUISpinBox::CGUISpinBox(const wchar_t* text, bool border,boost::shared_ptr<IGUIEnvironment> environment,
			boost::shared_ptr<IGUIElement> parent, s32 id, const core::rect<s32>& rectangle)
: IGUISpinBox(environment, parent, id, rectangle),
	EditBox(0), ButtonSpinUp(0), ButtonSpinDown(0), StepSize(1.f),
	RangeMin(-FLT_MAX), RangeMax(FLT_MAX), FormatString(L"%f"),
	DecimalPlaces(-1), _text(text), _border(border), _rectangle(rectangle)
{
	#ifdef _DEBUG
	setDebugName("CGUISpinBox");
	#endif
}


//! destructor
CGUISpinBox::~CGUISpinBox()
{
}

void CGUISpinBox::refreshSprites()
{
	boost::shared_ptr<IGUISpriteBank> sb = 0;
	boost::shared_ptr<IGUIEnvironment> lockedEnvironment = getSharedEnvironment();

	if (lockedEnvironment && lockedEnvironment->getSkin())
	{
		sb = lockedEnvironment->getSkin()->getSpriteBank();
	}

	if (sb)
	{
		boost::shared_ptr<IGUISkin> skin = lockedEnvironment->getSkin();
		CurrentIconColor = skin->getColor(isEnabled() ? EGDC_WINDOW_SYMBOL : EGDC_GRAY_WINDOW_SYMBOL);
		ButtonSpinDown->setSpriteBank(sb);
		ButtonSpinDown->setSprite(EGBS_BUTTON_UP, skin->getIcon(EGDI_SMALL_CURSOR_DOWN), CurrentIconColor);
		ButtonSpinDown->setSprite(EGBS_BUTTON_DOWN, skin->getIcon(EGDI_SMALL_CURSOR_DOWN), CurrentIconColor);
		ButtonSpinUp->setSpriteBank(sb);
		ButtonSpinUp->setSprite(EGBS_BUTTON_UP, skin->getIcon(EGDI_SMALL_CURSOR_UP), CurrentIconColor);
		ButtonSpinUp->setSprite(EGBS_BUTTON_DOWN, skin->getIcon(EGDI_SMALL_CURSOR_UP), CurrentIconColor);
	}
	else
	{
		ButtonSpinDown->setText(L"-");
		ButtonSpinUp->setText(L"+");
	}
}

boost::shared_ptr<IGUIEditBox> CGUISpinBox::getEditBox() const
{
	return EditBox;
}


void CGUISpinBox::setValue(f32 val)
{
	wchar_t str[100];

	swprintf(str, 99, FormatString.c_str(), val);
	EditBox->setText(str);
	verifyValueRange();
}


f32 CGUISpinBox::getValue() const
{
	const wchar_t* val = EditBox->getText();
	if ( !val )
		return 0.f;
	core::stringc tmp(val);
	return core::fast_atof(tmp.c_str());
}


void CGUISpinBox::setRange(f32 min, f32 max)
{
	if (max<min)
		core::swap(min, max);
	RangeMin = min;
	RangeMax = max;

	// we have to round the range - otherwise we can get into an infinte setValue/verifyValueRange cycle.
	wchar_t str[100];
	swprintf(str, 99, FormatString.c_str(), RangeMin);
	RangeMin = core::fast_atof(core::stringc(str).c_str());
	swprintf(str, 99, FormatString.c_str(), RangeMax);
	RangeMax = core::fast_atof(core::stringc(str).c_str());

	verifyValueRange();
}


f32 CGUISpinBox::getMin() const
{
	return RangeMin;
}


f32 CGUISpinBox::getMax() const
{
	return RangeMax;
}


f32 CGUISpinBox::getStepSize() const
{
	return StepSize;
}


void CGUISpinBox::setStepSize(f32 step)
{
	StepSize = step;
}


//! Sets the number of decimal places to display.
void CGUISpinBox::setDecimalPlaces(s32 places)
{
	DecimalPlaces = places;
	if (places == -1)
		FormatString = "%f";
	else
	{
		FormatString = "%.";
		FormatString += places;
		FormatString += "f";
	}
	setRange( RangeMin, RangeMax );
	setValue(getValue());
}


bool CGUISpinBox::OnEvent(const SEvent& event)
{
	if (IsEnabled)
	{
		bool changeEvent = false;
		switch(event.EventType)
		{
		case EET_MOUSE_INPUT_EVENT:
			switch(event.MouseInput.Event)
			{
			case EMIE_MOUSE_WHEEL:
				{
					f32 val = getValue() + (StepSize * (event.MouseInput.Wheel < 0 ? -1.f : 1.f));
					setValue(val);
					changeEvent = true;
				}
				break;
			default:
				break;
			}
			break;

		case EET_GUI_EVENT:
			if (event.GUIEvent.EventType == EGET_BUTTON_CLICKED)
			{
				if (event.GUIEvent.Caller == ButtonSpinUp)
				{
					f32 val = getValue();
					val += StepSize;
					setValue(val);
					changeEvent = true;
				}
				else if ( event.GUIEvent.Caller == ButtonSpinDown)
				{
					f32 val = getValue();
					val -= StepSize;
					setValue(val);
					changeEvent = true;
				}
			}
			if (event.GUIEvent.EventType == EGET_EDITBOX_CHANGED || event.GUIEvent.EventType == EGET_EDITBOX_ENTER)
			{
				if (event.GUIEvent.Caller == EditBox)
				{
					verifyValueRange();
					changeEvent = true;
				}
			}
			break;
		default:
		break;
		}

		if ( changeEvent )
		{
			SEvent e;
			e.EventType = EET_GUI_EVENT;
			e.GUIEvent.Caller = getSharedThis();
			e.GUIEvent.Element = 0;

			e.GUIEvent.EventType = EGET_SPINBOX_CHANGED;
			if ( !Parent.expired() )
				getParent()->OnEvent(e);
			return true;
		}
	}

	return IGUIElement::OnEvent(event);
}


void CGUISpinBox::draw()
{
	if ( !isVisible() )
		return;

	boost::shared_ptr<IGUISkin> skin = getSharedEnvironment()->getSkin();
	if (!skin)
		return;

	video::SColor iconColor = skin->getColor(isEnabled() ? EGDC_WINDOW_SYMBOL : EGDC_GRAY_WINDOW_SYMBOL);
	if ( iconColor != CurrentIconColor )
	{
		refreshSprites();
	}

	IGUISpinBox::draw();
}

void CGUISpinBox::verifyValueRange()
{
	f32 val = getValue();
	if ( val+core::ROUNDING_ERROR_f32 < RangeMin )
		val = RangeMin;
	else if ( val-core::ROUNDING_ERROR_f32 > RangeMax )
		val = RangeMax;
	else
		return;

	setValue(val);
}


//! Sets the new caption of the element
void CGUISpinBox::setText(const wchar_t* text)
{
	EditBox->setText(text);
	setValue(getValue());
	verifyValueRange();
}


//! Returns caption of this element.
const wchar_t* CGUISpinBox::getText() const
{
	return EditBox->getText();
}


//! Writes attributes of the element.
void CGUISpinBox::serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options) const
{
	IGUIElement::serializeAttributes(out, options);
	out->addFloat("Min", getMin());
	out->addFloat("Max", getMax());
	out->addFloat("Step", getStepSize());
	out->addInt("DecimalPlaces", DecimalPlaces);
}


//! Reads attributes of the element
void CGUISpinBox::deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options)
{
	IGUIElement::deserializeAttributes(in, options);
	setRange(in->getAttributeAsFloat("Min"), in->getAttributeAsFloat("Max"));
	setStepSize(in->getAttributeAsFloat("Step"));
	setDecimalPlaces(in->getAttributeAsInt("DecimalPlaces"));
}

void CGUISpinBox::setWeakThis(boost::shared_ptr<IGUIElement> value)
{
	IGUIElement::setWeakThis(value);

	CurrentIconColor = video::SColor(255, 255, 255, 255);
	s32 ButtonWidth = 16;

	boost::shared_ptr<IGUIEnvironment> lockedEnvironment = getSharedEnvironment();
	boost::shared_ptr<IGUIElement> lockedThis = getSharedThis();

	ButtonSpinDown = lockedEnvironment->addButton(
		core::rect<s32>(_rectangle.getWidth() - ButtonWidth, _rectangle.getHeight() / 2 + 1,
			_rectangle.getWidth(), _rectangle.getHeight()), lockedThis);
	ButtonSpinDown->setSubElement(true);
	ButtonSpinDown->setTabStop(false);
	ButtonSpinDown->setAlignment(EGUIA_LOWERRIGHT, EGUIA_LOWERRIGHT, EGUIA_CENTER, EGUIA_LOWERRIGHT);

	ButtonSpinUp = lockedEnvironment->addButton(
		core::rect<s32>(_rectangle.getWidth() - ButtonWidth, 0,
			_rectangle.getWidth(), _rectangle.getHeight() / 2), lockedThis);
	ButtonSpinUp->setSubElement(true);
	ButtonSpinUp->setTabStop(false);
	ButtonSpinUp->setAlignment(EGUIA_LOWERRIGHT, EGUIA_LOWERRIGHT, EGUIA_UPPERLEFT, EGUIA_CENTER);

	const core::rect<s32> rectEdit(0, 0, _rectangle.getWidth() - ButtonWidth - 1, _rectangle.getHeight());
	EditBox = lockedEnvironment->addEditBox(_text, rectEdit, _border, lockedThis, -1);
	EditBox->setSubElement(true);
	EditBox->setAlignment(EGUIA_UPPERLEFT, EGUIA_LOWERRIGHT, EGUIA_UPPERLEFT, EGUIA_LOWERRIGHT);

	refreshSprites();
}


} // end namespace gui
} // end namespace irr

#endif // _IRR_COMPILE_WITH_GUI_

