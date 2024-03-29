
// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "CGUIEnvironment.h"

#ifdef _IRR_COMPILE_WITH_GUI_

#include "IVideoDriver.h"

#include "CGUISkin.h"
#include "CGUIButton.h"
#include "CGUIWindow.h"
#include "CGUIScrollBar.h"
#include "CGUIFont.h"
#include "CGUISpriteBank.h"
#include "CGUIImage.h"
#include "CGUIMeshViewer.h"
#include "CGUICheckBox.h"
#include "CGUIListBox.h"
#include "CGUITreeView.h"
#include "CGUIImageList.h"
#include "CGUIFileOpenDialog.h"
#include "CGUIColorSelectDialog.h"
#include "CGUIStaticText.h"
#include "CGUIEditBox.h"
#include "CGUISpinBox.h"
#include "CGUIInOutFader.h"
#include "CGUIMessageBox.h"
#include "CGUIModalScreen.h"
#include "CGUITabControl.h"
#include "CGUIContextMenu.h"
#include "CGUIComboBox.h"
#include "CGUIMenu.h"
#include "CGUIToolBar.h"
#include "CGUITable.h"

#include "CDefaultGUIElementFactory.h"
#include "IWriteFile.h"
#include "IXMLWriter.h"

#include "BuiltInFont.h"
#include "os.h"

namespace irr
{
namespace gui
{

const wchar_t* IRR_XML_FORMAT_GUI_ENV			= L"irr_gui";
const wchar_t* IRR_XML_FORMAT_GUI_ELEMENT		= L"element";
const wchar_t* IRR_XML_FORMAT_GUI_ELEMENT_ATTR_TYPE	= L"type";

const io::path CGUIEnvironment::DefaultFontName = "#DefaultFont";

//! constructor
CGUIEnvironment::CGUIEnvironment(boost::shared_ptr<io::IFileSystem> fs, boost::shared_ptr<video::IVideoDriver> driver, boost::shared_ptr<IOSOperator> op)
: IGUIEnvironment(driver), IGUIElement(EGUIET_ROOT, 0, 0, 0, core::rect<s32>(core::position2d<s32>(0,0), driver ? core::dimension2d<s32>(driver->getScreenSize()) : core::dimension2d<s32>(0,0))),
	Hovered(0), HoveredNoSubelement(0), Focus(0), LastHoveredMousePos(0,0), CurrentSkin(0),
	FileSystem(fs), UserReceiver(0), Operator(op)
{


	#ifdef _DEBUG
	IGUIEnvironment::setDebugName("CGUIEnvironment");
	#endif
}


//! destructor
CGUIEnvironment::~CGUIEnvironment()
{
	if ( HoveredNoSubelement && HoveredNoSubelement.get() != this )
	{
		HoveredNoSubelement = 0;
	}

	if (Hovered && Hovered.get() != this)
	{
		Hovered = 0;
	}

	Focus.reset();

	if (ToolTip.Element)
	{
		ToolTip.Element = 0;
	}

	// drop skin
	if (CurrentSkin)
	{
		CurrentSkin = 0;
	}

	if (Operator)
	{
		Operator = 0;
	}
}


void CGUIEnvironment::loadBuiltInFont()
{
	boost::shared_ptr<io::IReadFile> file = io::createMemoryReadFile(BuiltInFontData, BuiltInFontDataSize, DefaultFontName, false);

	boost::shared_ptr<CGUIFont> font = boost::make_shared<CGUIFont>(getSharedEnvironment(), DefaultFontName);

	if (!font->load(file))
	{
		os::Printer::log("Error: Could not load built-in Font. Did you compile without the BMP loader?", ELL_ERROR);
		return;
	}

	SFont f;
	f.NamedPath.setPath(DefaultFontName);
	f.Font = font;
	Fonts.push_back(f);
}


//! draws all gui elements
void CGUIEnvironment::drawAll()
{
	boost::shared_ptr<video::IVideoDriver> lockedDriver = getVideoDriver();

	if (lockedDriver)
	{
		core::dimension2d<s32> dim(lockedDriver->getScreenSize());
		if (AbsoluteRect.LowerRightCorner.X != dim.Width ||
			AbsoluteRect.LowerRightCorner.Y != dim.Height)
		{
			// resize gui environment
			DesiredRect.LowerRightCorner = dim;
			AbsoluteClippingRect = DesiredRect;
			AbsoluteRect = DesiredRect;
			updateAbsolutePosition();
		}
	}

	// make sure tooltip is always on top
	if (ToolTip.Element)
		bringToFront(ToolTip.Element);

	draw();
	OnPostRender ( os::Timer::getTime () );
}


//! sets the focus to an element
bool CGUIEnvironment::setFocus(boost::shared_ptr<IGUIElement> element)
{
	if (Focus == element)
	{
		_IRR_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX;
		return false;
	}

	// GUI Environment should not get the focus
	if (element.get() == this)
		element = 0;

	// focus may change or be removed in this call
	boost::shared_ptr<IGUIElement> currentFocus = 0;
	if (Focus)
	{
		currentFocus = Focus;
		SEvent e;
		e.EventType = EET_GUI_EVENT;
		e.GUIEvent.Caller = Focus;
		e.GUIEvent.Element = element;
		e.GUIEvent.EventType = EGET_ELEMENT_FOCUS_LOST;
		if (Focus->OnEvent(e))
		{
			_IRR_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX;
			return false;
		}
		currentFocus = 0;
	}

	if (element)
	{
		currentFocus = Focus;

		// send focused event
		SEvent e;
		e.EventType = EET_GUI_EVENT;
		e.GUIEvent.Caller = element;
		e.GUIEvent.Element = Focus;
		e.GUIEvent.EventType = EGET_ELEMENT_FOCUSED;
		if (element->OnEvent(e))
		{
			_IRR_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX;
			return false;
		}
	}

	// element is the new focus so it doesn't have to be dropped
	Focus = element;

	return true;
}


//! returns the element with the focus
boost::shared_ptr<IGUIElement> CGUIEnvironment::getFocus() const
{
	return Focus;
}

//! returns the element last known to be under the mouse cursor
boost::shared_ptr<IGUIElement> CGUIEnvironment::getHovered() const
{
	return Hovered;
}


//! removes the focus from an element
bool CGUIEnvironment::removeFocus(boost::shared_ptr<IGUIElement> element)
{
	if (Focus && Focus==element)
	{
		SEvent e;
		e.EventType = EET_GUI_EVENT;
		e.GUIEvent.Caller = Focus;
		e.GUIEvent.Element = 0;
		e.GUIEvent.EventType = EGET_ELEMENT_FOCUS_LOST;
		if (Focus->OnEvent(e))
		{
			_IRR_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX;
			return false;
		}
	}
	if (Focus)
	{
		Focus = 0;
	}

	return true;
}


//! Returns if the element has focus
bool CGUIEnvironment::hasFocus(boost::shared_ptr<IGUIElement> element) const
{
	_IRR_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX;
	return (element == Focus);
}



//! returns the current file system
boost::shared_ptr<io::IFileSystem> CGUIEnvironment::getFileSystem() const
{
	return FileSystem;
}


//! returns a pointer to the OS operator
boost::shared_ptr<IOSOperator> CGUIEnvironment::getOSOperator() const
{
	return Operator;
}


//! clear all GUI elements
void CGUIEnvironment::clear()
{
	// Remove the focus
	if (Focus)
	{
		Focus = 0;
	}

	if (Hovered && Hovered.get() != this)
	{
		Hovered = 0;
	}
	if ( HoveredNoSubelement && HoveredNoSubelement.get() != this)
	{
		HoveredNoSubelement = 0;
	}

	// get the root's children in case the root changes in future
	const core::list<boost::shared_ptr<IGUIElement>>& children = getRootGUIElement()->getChildren();

	while (!children.empty())
		(*children.getLast())->remove();
}


//! called by ui if an event happened.
bool CGUIEnvironment::OnEvent(const SEvent& event)
{
	bool ret = false;
	if (UserReceiver
		&& (event.EventType != EET_MOUSE_INPUT_EVENT)
		&& (event.EventType != EET_KEY_INPUT_EVENT)
		&& (event.EventType != EET_GUI_EVENT || event.GUIEvent.Caller.get() != this))
	{
		ret = UserReceiver->OnEvent(event);
	}

	_IRR_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX;
	return ret;
}

//
void CGUIEnvironment::OnPostRender( u32 time )
{
	// launch tooltip
	if ( ToolTip.Element == 0 &&
		HoveredNoSubelement && HoveredNoSubelement.get() != this &&
		(time - ToolTip.EnterTime >= ToolTip.LaunchTime
		|| (time - ToolTip.LastTime >= ToolTip.RelaunchTime && time - ToolTip.LastTime < ToolTip.LaunchTime)) &&
		HoveredNoSubelement->getToolTipText().size() &&
		getSkin() &&
		getSkin()->getFont(EGDF_TOOLTIP)
		)
	{
		core::rect<s32> pos;

		pos.UpperLeftCorner = LastHoveredMousePos;
		core::dimension2du dim = getSkin()->getFont(EGDF_TOOLTIP)->getDimension(HoveredNoSubelement->getToolTipText().c_str());
		dim.Width += getSkin()->getSize(EGDS_TEXT_DISTANCE_X)*2;
		dim.Height += getSkin()->getSize(EGDS_TEXT_DISTANCE_Y)*2;

		pos.UpperLeftCorner.Y -= dim.Height+1;
		pos.LowerRightCorner.Y = pos.UpperLeftCorner.Y + dim.Height-1;
		pos.LowerRightCorner.X = pos.UpperLeftCorner.X + dim.Width;

		pos.constrainTo(getAbsolutePosition());

		ToolTip.Element = addStaticText(HoveredNoSubelement->getToolTipText().c_str(), pos, true, true, getSharedThis(), -1, true);
		ToolTip.Element->setOverrideColor(getSkin()->getColor(EGDC_TOOLTIP));
		ToolTip.Element->setBackgroundColor(getSkin()->getColor(EGDC_TOOLTIP_BACKGROUND));
		ToolTip.Element->setOverrideFont(getSkin()->getFont(EGDF_TOOLTIP));
		ToolTip.Element->setSubElement(true);

		s32 textHeight = ToolTip.Element->getTextHeight();
		pos = ToolTip.Element->getRelativePosition();
		pos.LowerRightCorner.Y = pos.UpperLeftCorner.Y + textHeight;
		ToolTip.Element->setRelativePosition(pos);
	}

	if (ToolTip.Element && ToolTip.Element->isVisible() )	// (isVisible() check only because we might use visibility for ToolTip one day)
	{
		ToolTip.LastTime = time;

		// got invisible or removed in the meantime?
		if ( !HoveredNoSubelement ||
			!HoveredNoSubelement->isVisible() ||
			!HoveredNoSubelement->getParent()
			)	// got invisible or removed in the meantime?
		{
			ToolTip.Element->remove();
			ToolTip.Element = 0;
		}
	}

	IGUIElement::OnPostRender ( time );
}


//
void CGUIEnvironment::updateHoveredElement(core::position2d<s32> mousePos)
{
	boost::shared_ptr<IGUIElement> lastHovered = Hovered;
	boost::shared_ptr<IGUIElement> lastHoveredNoSubelement = HoveredNoSubelement;
	LastHoveredMousePos = mousePos;

	Hovered = getElementFromPoint(mousePos);

	if ( ToolTip.Element && Hovered == ToolTip.Element )
	{
		// When the mouse is over the ToolTip we remove that so it will be re-created at a new position.
		// Note that ToolTip.EnterTime does not get changed here, so it will be re-created at once.
		ToolTip.Element->remove();
		ToolTip.Element = 0;

		// Get the real Hovered
		Hovered = getElementFromPoint(mousePos);
	}

	// for tooltips we want the element itself and not some of it's subelements
	HoveredNoSubelement = Hovered;
	while ( HoveredNoSubelement && HoveredNoSubelement->isSubElement() )
	{
		HoveredNoSubelement = HoveredNoSubelement->getParent();
	}

	if (Hovered != lastHovered)
	{
		SEvent event;
		event.EventType = EET_GUI_EVENT;

		if (lastHovered)
		{
			event.GUIEvent.Caller = lastHovered;
			event.GUIEvent.Element = 0;
			event.GUIEvent.EventType = EGET_ELEMENT_LEFT;
			lastHovered->OnEvent(event);
		}

		if ( Hovered )
		{
			event.GUIEvent.Caller  = Hovered;
			event.GUIEvent.Element = Hovered;
			event.GUIEvent.EventType = EGET_ELEMENT_HOVERED;
			Hovered->OnEvent(event);
		}
	}

	if ( lastHoveredNoSubelement != HoveredNoSubelement )
	{
		if (ToolTip.Element)
		{
			ToolTip.Element->remove();
			ToolTip.Element = 0;
		}

		if ( HoveredNoSubelement )
		{
			u32 now = os::Timer::getTime();
			ToolTip.EnterTime = now;
		}
	}
}


//! This sets a new event receiver for gui events. Usually you do not have to
//! use this method, it is used by the internal engine.
void CGUIEnvironment::setUserEventReceiver(IEventReceiver* evr)
{
	UserReceiver = evr;
}


//! posts an input event to the environment
bool CGUIEnvironment::postEventFromUser(const SEvent& event)
{
	switch(event.EventType)
	{
	case EET_GUI_EVENT:
		// hey, why is the user sending gui events..?
		break;
	case EET_MOUSE_INPUT_EVENT:

		updateHoveredElement(core::position2d<s32>(event.MouseInput.X, event.MouseInput.Y));

		if (event.MouseInput.Event == EMIE_LMOUSE_PRESSED_DOWN)
			if ( (Hovered && Hovered != Focus) || !Focus )
		{
			setFocus(Hovered);
		}

		// sending input to focus
		if (Focus && Focus->OnEvent(event))
			return true;

		// focus could have died in last call
		if (!Focus && Hovered)
		{
			_IRR_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX;
			return Hovered->OnEvent(event);
		}

		break;
	case EET_KEY_INPUT_EVENT:
		{
			if (Focus && Focus->OnEvent(event))
				return true;

			// For keys we handle the event before changing focus to give elements the chance for catching the TAB
			// Send focus changing event
			if (event.EventType == EET_KEY_INPUT_EVENT &&
				event.KeyInput.PressedDown &&
				event.KeyInput.Key == KEY_TAB)
			{
				boost::shared_ptr<IGUIElement> next = getNextElement(event.KeyInput.Shift, event.KeyInput.Control);
				if (next && next != Focus)
				{
					if (setFocus(next))
						return true;
				}
			}

		}
		break;
	default:
		break;
	} // end switch

	_IRR_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX;
	return false;
}


//! returns the current gui skin
boost::shared_ptr<IGUISkin> CGUIEnvironment::getSkin() const
{
	return CurrentSkin;
}


//! Sets a new GUI Skin
void CGUIEnvironment::setSkin(boost::shared_ptr<IGUISkin> skin)
{
	if (CurrentSkin==skin)
		return;

	CurrentSkin = skin;
}


//! Creates a new GUI Skin based on a template.
/** \return Returns a pointer to the created skin.
If you no longer need the skin, you should call IGUISkin::drop().
See IReferenceCounted::drop() for more information. */
boost::shared_ptr<IGUISkin> CGUIEnvironment::createSkin(EGUI_SKIN_TYPE type)
{
	boost::shared_ptr<IGUISkin> skin = boost::make_shared<CGUISkin>(type, getVideoDriver());

	boost::shared_ptr<IGUIFont> builtinfont = getBuiltInFont();
	boost::shared_ptr<IGUIFontBitmap> bitfont = 0;
	if (builtinfont && builtinfont->getType() == EGFT_BITMAP)
		bitfont = boost::static_pointer_cast<IGUIFontBitmap>(builtinfont);

	boost::shared_ptr<IGUISpriteBank> bank = 0;
	skin->setFont(builtinfont);

	if (bitfont)
		bank = bitfont->getSpriteBank();

	skin->setSpriteBank(bank);

	return skin;
}


//! Returns the default element factory which can create all built in elements
boost::shared_ptr<IGUIElementFactory> CGUIEnvironment::getDefaultGUIElementFactory() const
{
	return getGUIElementFactory(0);
}


//! Adds an element factory to the gui environment.
/** Use this to extend the gui environment with new element types which it should be
able to create automaticly, for example when loading data from xml files. */
void CGUIEnvironment::registerGUIElementFactory(boost::shared_ptr<IGUIElementFactory> factoryToAdd)
{
	if (factoryToAdd)
	{
		GUIElementFactoryList.push_back(factoryToAdd);
	}
}


//! Returns amount of registered scene node factories.
u32 CGUIEnvironment::getRegisteredGUIElementFactoryCount() const
{
	return GUIElementFactoryList.size();
}


//! Returns a scene node factory by index
boost::shared_ptr<IGUIElementFactory> CGUIEnvironment::getGUIElementFactory(u32 index) const
{
	if (index < GUIElementFactoryList.size())
		return GUIElementFactoryList[index];
	else
		return 0;
}


//! adds a GUI Element using its name
boost::shared_ptr<IGUIElement> CGUIEnvironment::addGUIElement(const c8* elementName, boost::shared_ptr<IGUIElement> parent)
{
	boost::shared_ptr<IGUIElement> node=0;

	if (!parent)
		parent = getSharedThis();

	for (s32 i=GUIElementFactoryList.size()-1; i>=0 && !node; --i)
		node = GUIElementFactoryList[i]->addGUIElement(elementName, parent);


	return node;
}


//! Saves the current gui into a file.
//! \param filename: Name of the file .
bool CGUIEnvironment::saveGUI(const io::path& filename, boost::shared_ptr<IGUIElement> start)
{
	boost::shared_ptr<io::IWriteFile> file = FileSystem->createAndWriteFile(filename);
	if (!file)
	{
		_IRR_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX;
		return false;
	}

	bool ret = saveGUI(file, start);
	_IRR_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX;
	return ret;
}


//! Saves the current gui into a file.
bool CGUIEnvironment::saveGUI(boost::shared_ptr<io::IWriteFile> file, boost::shared_ptr<IGUIElement> start)
{
	if (!file)
	{
		_IRR_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX;
		return false;
	}

	io::IXMLWriter* writer = FileSystem->createXMLWriter(file);
	if (!writer)
	{
		_IRR_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX;
		return false;
	}

	writer->writeXMLHeader();
	writeGUIElement(writer, start ? start : getSharedThis());
	writer->drop();

	return true;
}


//! Loads the gui. Note that the current gui is not cleared before.
//! \param filename: Name of the file.
bool CGUIEnvironment::loadGUI(const io::path& filename, boost::shared_ptr<IGUIElement> parent)
{
	boost::shared_ptr<io::IReadFile> read = FileSystem->createAndOpenFile(filename);
	if (!read)
	{
		os::Printer::log("Unable to open gui file", filename, ELL_ERROR);
		_IRR_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX;
		return false;
	}

	bool ret = loadGUI(read, parent);

	_IRR_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX;
	return ret;
}


//! Loads the gui. Note that the current gui is not cleared before.
bool CGUIEnvironment::loadGUI(boost::shared_ptr<io::IReadFile> file, boost::shared_ptr<IGUIElement> parent)
{
	if (!file)
	{
		os::Printer::log("Unable to open GUI file", ELL_ERROR);
		_IRR_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX;
		return false;
	}

	io::IXMLReader* reader = FileSystem->createXMLReader(file);
	if (!reader)
	{
		os::Printer::log("GUI is not a valid XML file", file->getFileName(), ELL_ERROR);
		_IRR_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX;
		return false;
	}

	// read file
	while(reader->read())
	{
		readGUIElement(reader, parent);
	}

	// finish up

	reader->drop();
	return true;
}


//! reads an element
void CGUIEnvironment::readGUIElement(io::IXMLReader* reader, boost::shared_ptr<IGUIElement> node)
{
	if (!reader)
		return;

	io::EXML_NODE nodeType = reader->getNodeType();

	if (nodeType == io::EXN_NONE || nodeType == io::EXN_UNKNOWN || nodeType == io::EXN_ELEMENT_END)
		return;

	boost::shared_ptr<IGUIElement> deferedNode = 0;
	if (!wcscmp(IRR_XML_FORMAT_GUI_ENV, reader->getNodeName()))
	{
		// GuiEnvironment always must be this as it would serialize into a wrong element otherwise.
		// So we use the given node next time
		if ( node && node.get() != this )
			deferedNode = node;
		node = getSharedThis(); // root
	}
	else if	(!wcscmp(IRR_XML_FORMAT_GUI_ELEMENT, reader->getNodeName()))
	{
		// find node type and create it
		const core::stringc attrName = reader->getAttributeValue(IRR_XML_FORMAT_GUI_ELEMENT_ATTR_TYPE);

		node = addGUIElement(attrName.c_str(), node);

		if (!node)
			os::Printer::log("Could not create GUI element of unknown type", attrName.c_str());
	}

	// read attributes

	while(reader->read())
	{
		bool endreached = false;

		switch (reader->getNodeType())
		{
		case io::EXN_ELEMENT_END:
			if (!wcscmp(IRR_XML_FORMAT_GUI_ELEMENT,  reader->getNodeName()) ||
				!wcscmp(IRR_XML_FORMAT_GUI_ENV, reader->getNodeName()))
			{
				endreached = true;
			}
			break;
		case io::EXN_ELEMENT:
			if (!wcscmp(L"attributes", reader->getNodeName()))
			{
				// read attributes
				io::IAttributes* attr = FileSystem->createEmptyAttributes(getVideoDriver());
				attr->read(reader, true);

				if (node)
					node->deserializeAttributes(attr);

				attr->drop();
			}
			else
			if (!wcscmp(IRR_XML_FORMAT_GUI_ELEMENT, reader->getNodeName()) ||
				!wcscmp(IRR_XML_FORMAT_GUI_ENV, reader->getNodeName()))
			{
				if ( deferedNode )
					readGUIElement(reader, deferedNode);
				else
					readGUIElement(reader, node);
			}
			else
			{
				os::Printer::log("Found unknown element in irrlicht GUI file",
						core::stringc(reader->getNodeName()).c_str());
			}

			break;
		default:
			break;
		}

		if (endreached)
			break;
	}
}

void CGUIEnvironment::setWeakThis(boost::shared_ptr<IGUIElement> value)
{
	IGUIElement::setWeakThis(value);

	// environment is root tab group
	Environment = boost::dynamic_pointer_cast<IGUIEnvironment>(getSharedThis());
	setTabGroup(true);

	// gui factory
	boost::shared_ptr<IGUIElementFactory> factory = boost::make_shared<CDefaultGUIElementFactory>(getSharedEnvironment());
	registerGUIElementFactory(factory);

	loadBuiltInFont();

	boost::shared_ptr<IGUISkin> skin = createSkin(gui::EGST_WINDOWS_METALLIC);
	setSkin(skin);

	//set tooltip default
	ToolTip.LastTime = 0;
	ToolTip.EnterTime = 0;
	ToolTip.LaunchTime = 1000;
	ToolTip.RelaunchTime = 500;
	ToolTip.Element = 0;
}


//! writes an element
void CGUIEnvironment::writeGUIElement(io::IXMLWriter* writer, boost::shared_ptr<IGUIElement> node)
{
	if (!writer || !node )
		return;

	const wchar_t* name = 0;
	io::IAttributes* attr = nullptr;
	if (!node->isNonSaveable()) {

		// write properties

		attr = FileSystem->createEmptyAttributes();
		node->serializeAttributes(attr);

		// all gui elements must have at least one attribute
		// if they have nothing then we ignore them.
		if (attr->getAttributeCount() != 0)
		{
			if (node.get() == this)
			{
				name = IRR_XML_FORMAT_GUI_ENV;
				writer->writeElement(name, false);
			}
			else
			{
				name = IRR_XML_FORMAT_GUI_ELEMENT;
				writer->writeElement(name, false, IRR_XML_FORMAT_GUI_ELEMENT_ATTR_TYPE,
					core::stringw(node->getTypeName()).c_str());
			}

			writer->writeLineBreak();
			writer->writeLineBreak();

			attr->write(writer);
			writer->writeLineBreak();
		}
	}

	// write children

	core::list<boost::shared_ptr<IGUIElement>>::ConstIterator it = node->getChildren().begin();
	for (; it != node->getChildren().end(); ++it)
	{
		if (!(*it)->isSubElement())
			writeGUIElement(writer, (*it));
	}

	if (!node->isNonSaveable()) {
		// write closing brace if required
		if (attr->getAttributeCount() != 0)
		{
			writer->writeClosingTag(name);
			writer->writeLineBreak();
			writer->writeLineBreak();
		}

		attr->drop();
	}
}


//! Writes attributes of the environment
void CGUIEnvironment::serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options) const
{
	boost::shared_ptr<IGUISkin> skin = getSkin();

	if (skin)
	{
		out->addEnum("Skin", getSkin()->getType(), GUISkinTypeNames);
		skin->serializeAttributes(out, options);
	}
}


//! Reads attributes of the environment
void CGUIEnvironment::deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options)
{
	if (in->existsAttribute("Skin"))
	{
		boost::shared_ptr<IGUISkin>skin = getSkin();

		EGUI_SKIN_TYPE t = (EGUI_SKIN_TYPE) in->getAttributeAsEnumeration("Skin",GUISkinTypeNames);
		if ( !skin || t != skin->getType())
		{
			skin = createSkin(t);
			setSkin(skin);
		}

		skin = getSkin();

		if (skin)
		{
			skin->deserializeAttributes(in, options);
		}

	}

	boost::shared_ptr<video::IVideoDriver> lockedDriver = getVideoDriver();

	RelativeRect = AbsoluteRect =
			core::rect<s32>(core::position2d<s32>(0,0),
				lockedDriver ? core::dimension2di(lockedDriver->getScreenSize()) : core::dimension2d<s32>(0,0));
}


//! adds a button. The returned pointer must not be dropped.
boost::shared_ptr<IGUIButton> CGUIEnvironment::addButton(const core::rect<s32>& rectangle, boost::shared_ptr<IGUIElement> parent, s32 id, const wchar_t* text, const wchar_t *tooltiptext)
{
	boost::shared_ptr<IGUIButton> button = boost::make_shared<CGUIButton>(getSharedEnvironment(), parent ? parent : getSharedThis(), id, rectangle);
	button->setWeakThis(button);

	if (text)
		button->setText(text);

	if ( tooltiptext )
		button->setToolTipText ( tooltiptext );

	return button;
}


//! adds a window. The returned pointer must not be dropped.
boost::shared_ptr<IGUIWindow> CGUIEnvironment::addWindow(const core::rect<s32>& rectangle, bool modal,
		const wchar_t* text, boost::shared_ptr<IGUIElement> parent, s32 id)
{
	parent = parent ? parent : getSharedThis();

	boost::shared_ptr<IGUIWindow> win = boost::make_shared<CGUIWindow>(getSharedEnvironment(), parent, id, rectangle);
	win->setWeakThis(win);

	if (text)
		win->setText(text);
	
	if (modal)
	{
		// Careful, don't just set the modal as parent above. That will mess up the focus (and is hard to change because we have to be very
		// careful not to get virtual function call, like OnEvent, in the window.
		boost::shared_ptr<CGUIModalScreen>  modalScreen = boost::make_shared<CGUIModalScreen>(getSharedEnvironment(), parent, -1);
		modalScreen->setWeakThis(modalScreen);

		modalScreen->addChild(win);
	}

	return win;
}


//! adds a modal screen. The returned pointer must not be dropped.
boost::shared_ptr<IGUIElement> CGUIEnvironment::addModalScreen(boost::shared_ptr<IGUIElement> parent)
{
	parent = parent ? parent : getSharedThis();

	boost::shared_ptr<IGUIElement> win = boost::make_shared<CGUIModalScreen>(getSharedEnvironment(), parent, -1);
	win->setWeakThis(win);

	return win;
}


//! Adds a message box.
boost::shared_ptr<IGUIWindow> CGUIEnvironment::addMessageBox(const wchar_t* caption, const wchar_t* text,
	bool modal, s32 flag, boost::shared_ptr<IGUIElement> parent, s32 id, boost::shared_ptr<video::ITexture> image)
{
	if (!CurrentSkin)
		return 0;

	parent = parent ? parent : getSharedThis();

	core::rect<s32> rect;
	core::dimension2d<u32> screenDim, msgBoxDim;

	screenDim.Width = parent->getAbsolutePosition().getWidth();
	screenDim.Height = parent->getAbsolutePosition().getHeight();
	msgBoxDim.Width = 2;
	msgBoxDim.Height = 2;

	rect.UpperLeftCorner.X = (screenDim.Width - msgBoxDim.Width) / 2;
	rect.UpperLeftCorner.Y = (screenDim.Height - msgBoxDim.Height) / 2;
	rect.LowerRightCorner.X = rect.UpperLeftCorner.X + msgBoxDim.Width;
	rect.LowerRightCorner.Y = rect.UpperLeftCorner.Y + msgBoxDim.Height;

	boost::shared_ptr<IGUIWindow> win = boost::make_shared<CGUIMessageBox>(getSharedEnvironment(), caption, text, flag,
		parent, id, rect, image);
	win->setWeakThis(win);

	if (modal)
	{
		// Careful, don't just set the modal as parent above. That will mess up the focus (and is hard to change because we have to be very
		// careful not to get virtual function call, like OnEvent, in the CGUIMessageBox.
		boost::shared_ptr<CGUIModalScreen>  modalScreen = boost::make_shared<CGUIModalScreen>(getSharedEnvironment(), parent, -1);
		modalScreen->setWeakThis(modalScreen);

		modalScreen->addChild( win );
	}


	return win;
}


//! adds a scrollbar. The returned pointer must not be dropped.
boost::shared_ptr<IGUIScrollBar> CGUIEnvironment::addScrollBar(bool horizontal, const core::rect<s32>& rectangle, boost::shared_ptr<IGUIElement> parent, s32 id)
{
	boost::shared_ptr<IGUIScrollBar> bar = boost::make_shared<CGUIScrollBar>(horizontal, getSharedEnvironment(), parent ? parent : getSharedThis(), id, rectangle);
	bar->setWeakThis(bar);

	return bar;
}

//! Adds a table to the environment
boost::shared_ptr<IGUITable> CGUIEnvironment::addTable(const core::rect<s32>& rectangle, boost::shared_ptr<IGUIElement> parent, s32 id, bool drawBackground)
{
	boost::shared_ptr<CGUITable> b = boost::make_shared<CGUITable>(getSharedEnvironment(), parent ? parent : getSharedThis(), id, rectangle, true, drawBackground, false);
	b->setWeakThis(b);

	return b;
}

boost::shared_ptr<IGUIElement> CGUIEnvironment::addEmpty(boost::shared_ptr<IGUIElement> parent, s32 id)
{
	auto empty = boost::make_shared<IGUIElement>(
		irr::gui::EGUI_ELEMENT_TYPE::EGUIT_EMPTY, 
		getSharedEnvironment(),
		parent ? parent: getSharedThis(),
		id,
		irr::core::recti({ 0,0 }, { 65536, 65536 }));
	empty->setWeakThis(empty);

	return empty;
}


//! Adds an image element.
boost::shared_ptr<IGUIImage> CGUIEnvironment::addImage(boost::shared_ptr<video::ITexture> image, core::position2d<s32> pos,
	bool useAlphaChannel, boost::shared_ptr<IGUIElement> parent, s32 id, const wchar_t* text)
{
	core::dimension2d<s32> sz(0,0);
	if (image)
		sz = core::dimension2d<s32>(image->getOriginalSize());

	boost::shared_ptr<IGUIImage> img = boost::make_shared<CGUIImage>(getSharedEnvironment(), parent ? parent : getSharedThis(),
		id, core::rect<s32>(pos, sz));
	img->setWeakThis(img);

	if (text)
		img->setText(text);

	if (useAlphaChannel)
		img->setUseAlphaChannel(true);

	if (image)
		img->setImage(image);

	img->setWeakThis(img);
	return img;
}


//! adds an image. The returned pointer must not be dropped.
boost::shared_ptr<IGUIImage> CGUIEnvironment::addImage(const core::rect<s32>& rectangle, boost::shared_ptr<IGUIElement> parent, s32 id, const wchar_t* text, bool useAlphaChannel)
{
	boost::shared_ptr<IGUIImage> img = boost::make_shared<CGUIImage>(getSharedEnvironment(), parent ? parent : getSharedThis(),
		id, rectangle);
	img->setWeakThis(img);

	if (text)
		img->setText(text);

	if ( useAlphaChannel )
		img->setUseAlphaChannel(true);

	return img;
}


//! adds an mesh viewer. The returned pointer must not be dropped.
boost::shared_ptr<IGUIMeshViewer> CGUIEnvironment::addMeshViewer(const core::rect<s32>& rectangle, boost::shared_ptr<IGUIElement> parent, s32 id, const wchar_t* text)
{
	boost::shared_ptr<IGUIMeshViewer> v = boost::make_shared<CGUIMeshViewer>(getSharedEnvironment(), parent ? parent : getSharedThis(),
		id, rectangle);
	v->setWeakThis(v);

	if (text)
		v->setText(text);

	return v;
}


//! adds a checkbox
boost::shared_ptr<IGUICheckBox> CGUIEnvironment::addCheckBox(bool checked, const core::rect<s32>& rectangle, boost::shared_ptr<IGUIElement> parent, s32 id, const wchar_t* text)
{
	boost::shared_ptr<IGUICheckBox> b = boost::make_shared<CGUICheckBox>(checked, getSharedEnvironment(),
		parent ? parent : getSharedThis(), id, rectangle);
	b->setWeakThis(b);

	if (text)
		b->setText(text);

	return b;
}


//! adds a list box
boost::shared_ptr<IGUIListBox> CGUIEnvironment::addListBox(const core::rect<s32>& rectangle,
					boost::shared_ptr<IGUIElement> parent, s32 id, bool drawBackground)
{
	boost::shared_ptr<IGUIListBox> b = boost::make_shared<CGUIListBox>(getSharedEnvironment(), parent ? parent : getSharedThis(), id, rectangle,
		true, drawBackground, false);
	b->setWeakThis(b);

	if (CurrentSkin && CurrentSkin->getSpriteBank())
	{
		b->setSpriteBank(CurrentSkin->getSpriteBank());
	}
	else if (getBuiltInFont() && getBuiltInFont()->getType() == EGFT_BITMAP)
	{
		b->setSpriteBank( boost::static_pointer_cast<IGUIFontBitmap>(getBuiltInFont())->getSpriteBank());
	}

	return b;
}

//! adds a tree view
boost::shared_ptr<IGUITreeView> CGUIEnvironment::addTreeView(const core::rect<s32>& rectangle,
					 boost::shared_ptr<IGUIElement> parent, s32 id,
					 bool drawBackground,
					 bool scrollBarVertical, bool scrollBarHorizontal)
{
	boost::shared_ptr<IGUITreeView> b = boost::make_shared<CGUITreeView>(getSharedEnvironment(), parent ? parent : getSharedThis(), id, rectangle,
		true, drawBackground, scrollBarVertical, scrollBarHorizontal);
	b->setWeakThis(b);

	b->setIconFont ( getBuiltInFont () );

	return b;
}

//! adds a file open dialog. The returned pointer must not be dropped.
boost::shared_ptr<IGUIFileOpenDialog> CGUIEnvironment::addFileOpenDialog(const wchar_t* title,
				bool modal, boost::shared_ptr<IGUIElement> parent, s32 id,
				bool restoreCWD, io::path::char_type* startDir)
{
	parent = parent ? parent : getSharedThis();

	boost::shared_ptr<IGUIFileOpenDialog> d = boost::make_shared<CGUIFileOpenDialog>(title, getSharedEnvironment(), parent, id,
			restoreCWD, startDir);
	d->setWeakThis(d);

	if (modal)
	{
		// Careful, don't just set the modal as parent above. That will mess up the focus (and is hard to change because we have to be very
		// careful not to get virtual function call, like OnEvent, in the window.
		boost::shared_ptr<CGUIModalScreen>  modalScreen = boost::make_shared<CGUIModalScreen>(getSharedEnvironment(), parent, -1);
		modalScreen->setWeakThis(modalScreen);
		modalScreen->addChild(d);
	}

	return d;
}


//! adds a color select dialog. The returned pointer must not be dropped.
boost::shared_ptr<IGUIColorSelectDialog> CGUIEnvironment::addColorSelectDialog(const wchar_t* title,
				bool modal, boost::shared_ptr<IGUIElement> parent, s32 id)
{
	parent = parent ? parent : getSharedThis();

	boost::shared_ptr<IGUIColorSelectDialog> d = boost::make_shared<CGUIColorSelectDialog>( title,
		getSharedEnvironment(), parent, id);
	d->setWeakThis(d);

	if (modal)
	{
		// Careful, don't just set the modal as parent above. That will mess up the focus (and is hard to change because we have to be very
		// careful not to get virtual function call, like OnEvent, in the window.
		boost::shared_ptr<CGUIModalScreen>  modalScreen = boost::make_shared<CGUIModalScreen>(getSharedEnvironment(), parent, -1);
		modalScreen->setWeakThis(modalScreen);
		modalScreen->addChild(d);
	}

	return d;
}


//! adds a static text. The returned pointer must not be dropped.
boost::shared_ptr<IGUIStaticText> CGUIEnvironment::addStaticText(const wchar_t* text,
				const core::rect<s32>& rectangle,
				bool border, bool wordWrap,
				boost::shared_ptr<IGUIElement> parent, s32 id, bool background)
{
	boost::shared_ptr<IGUIStaticText> d = boost::make_shared<CGUIStaticText>(text, border, getSharedEnvironment(),
			parent ? parent : getSharedThis(), id, rectangle, background);
	d->setWeakThis(d);

	d->setWordWrap(wordWrap);

	return d;
}


//! Adds an edit box. The returned pointer must not be dropped.
boost::shared_ptr<IGUIEditBox> CGUIEnvironment::addEditBox(const wchar_t* text,
			const core::rect<s32>& rectangle, bool border,
			boost::shared_ptr<IGUIElement> parent, s32 id)
{
	boost::shared_ptr<IGUIEditBox> d = boost::make_shared<CGUIEditBox>(text, border, getSharedEnvironment(),
			parent ? parent : getSharedThis(), id, rectangle);
	d->setWeakThis(d);

	return d;
}


//! Adds a spin box to the environment
boost::shared_ptr<IGUISpinBox> CGUIEnvironment::addSpinBox(const wchar_t* text,
					 const core::rect<s32> &rectangle,
					 bool border,boost::shared_ptr<IGUIElement> parent, s32 id)
{
	boost::shared_ptr<IGUISpinBox> d = boost::make_shared<CGUISpinBox>(text, border, getSharedEnvironment(),
		parent ? parent : getSharedThis(), id, rectangle);
	d->setWeakThis(d);

	return d;
}


//! Adds a tab control to the environment.
boost::shared_ptr<IGUITabControl> CGUIEnvironment::addTabControl(const core::rect<s32>& rectangle,
	boost::shared_ptr<IGUIElement> parent, bool fillbackground, bool border, s32 id)
{
	boost::shared_ptr<IGUITabControl> t = boost::make_shared<CGUITabControl>(getSharedEnvironment(), parent ? parent : getSharedThis(),
		rectangle, fillbackground, border, id);
	t->setWeakThis(t);

	return t;
}


//! Adds tab to the environment.
boost::shared_ptr<IGUITab> CGUIEnvironment::addTab(const core::rect<s32>& rectangle,
	boost::shared_ptr<IGUIElement> parent, s32 id)
{
	boost::shared_ptr<IGUITab> t = boost::make_shared<CGUITab>(-1, getSharedEnvironment(), parent ? parent : getSharedThis(),
		rectangle, id);
	t->setWeakThis(t);

	return t;
}


//! Adds a context menu to the environment.
boost::shared_ptr<IGUIContextMenu> CGUIEnvironment::addContextMenu(const core::rect<s32>& rectangle,
	boost::shared_ptr<IGUIElement> parent, s32 id)
{
	boost::shared_ptr<IGUIContextMenu> c = boost::make_shared<CGUIContextMenu>(getSharedEnvironment(),
		parent ? parent : getSharedThis(), id, rectangle, true);
	c->setWeakThis(c);

	return c;
}


//! Adds a menu to the environment.
boost::shared_ptr<IGUIContextMenu> CGUIEnvironment::addMenu(boost::shared_ptr<IGUIElement> parent, s32 id)
{
	if (!parent)
		parent = getSharedThis();

	boost::shared_ptr<IGUIContextMenu> c = boost::make_shared<CGUIMenu>(getSharedEnvironment(),
		parent, id, core::rect<s32>(0,0,
				parent->getAbsolutePosition().getWidth(),
				parent->getAbsolutePosition().getHeight()));
	c->setWeakThis(c);

	return c;
}


//! Adds a toolbar to the environment. It is like a menu is always placed on top
//! in its parent, and contains buttons.
boost::shared_ptr<IGUIToolBar> CGUIEnvironment::addToolBar(boost::shared_ptr<IGUIElement> parent, s32 id)
{
	if (!parent)
		parent = getSharedThis();

	boost::shared_ptr<IGUIToolBar> b = boost::make_shared<CGUIToolBar>(getSharedEnvironment(), parent, id, core::rect<s32>(0,0,10,10));
	b->setWeakThis(b);
	return b;
}


//! Adds an element for fading in or out.
boost::shared_ptr<IGUIInOutFader> CGUIEnvironment::addInOutFader(const core::rect<s32>* rectangle, boost::shared_ptr<IGUIElement> parent, s32 id)
{
	core::rect<s32> rect;

	boost::shared_ptr<video::IVideoDriver> lockedDriver = getVideoDriver();

	if (rectangle)
		rect = *rectangle;
	else if (lockedDriver)
		rect = core::rect<s32>(core::position2d<s32>(0,0), core::dimension2di(lockedDriver->getScreenSize()));

	if (!parent)
		parent = getSharedThis();

	boost::shared_ptr<IGUIInOutFader> fader = boost::make_shared<CGUIInOutFader>(getSharedEnvironment(), parent, id, rect);
	fader->setWeakThis(fader);

	return fader;
}


//! Adds a combo box to the environment.
boost::shared_ptr<IGUIComboBox> CGUIEnvironment::addComboBox(const core::rect<s32>& rectangle,
	boost::shared_ptr<IGUIElement> parent, s32 id)
{
	boost::shared_ptr<IGUIComboBox> t = boost::make_shared<CGUIComboBox>(getSharedEnvironment(), parent ? parent : getSharedThis(),
		id, rectangle);
	t->setWeakThis(t);

	return t;
}


//! returns the font
boost::shared_ptr<IGUIFont> CGUIEnvironment::getFont(const io::path& filename)
{
	// search existing font

	SFont f;
	f.NamedPath.setPath(filename);

	s32 index = Fonts.binary_search(f);
	if (index != -1)
		return Fonts[index].Font;

	// font doesn't exist, attempt to load it

	// does the file exist?

	if (!FileSystem->existFile(filename))
	{
		os::Printer::log("Could not load font because the file does not exist", f.NamedPath.getPath(), ELL_ERROR);
		return 0;
	}

	boost::shared_ptr<IGUIEnvironment> lockedEnvironment = getSharedEnvironment();
	boost::shared_ptr<IGUIFont> ifont=0;
	io::IXMLReader *xml = FileSystem->createXMLReader(filename );
	if (xml)
	{
		// this is an XML font, but we need to know what type
		EGUI_FONT_TYPE t = EGFT_CUSTOM;

		bool found=false;
		while(!found && xml->read())
		{
			if (xml->getNodeType() == io::EXN_ELEMENT)
			{
				if (core::stringw(L"font") == xml->getNodeName())
				{
					if (core::stringw(L"vector") == xml->getAttributeValue(L"type"))
					{
						t = EGFT_VECTOR;
						found=true;
					}
					else if (core::stringw(L"bitmap") == xml->getAttributeValue(L"type"))
					{
						t = EGFT_BITMAP;
						found=true;
					}
					else found=true;
				}
			}
		}

		if (t==EGFT_BITMAP)
		{
			boost::shared_ptr<CGUIFont> font = boost::make_shared<CGUIFont>(lockedEnvironment, filename);

			ifont = (boost::shared_ptr<IGUIFont>)font;
			// change working directory, for loading textures
			io::path workingDir = FileSystem->getWorkingDirectory();
			FileSystem->changeWorkingDirectoryTo(FileSystem->getFileDir(f.NamedPath.getPath()));

			// load the font
			if (!font->load(xml))
			{
				font  = 0;
				ifont = 0;
			}
			// change working dir back again
			FileSystem->changeWorkingDirectoryTo( workingDir );
		}
		else if (t==EGFT_VECTOR)
		{
			// todo: vector fonts
			os::Printer::log("Unable to load font, XML vector fonts are not supported yet", f.NamedPath, ELL_ERROR);

			//CGUIFontVector* font = new CGUIFontVector(Driver);
			//ifont = (boost::shared_ptr<IGUIFont>)font;
			//if (!font->load(xml))
		}
		xml->drop();
	}


	if (!ifont)
	{

		boost::shared_ptr<CGUIFont> font = boost::make_shared<CGUIFont>(lockedEnvironment, f.NamedPath.getPath());

		ifont = (boost::shared_ptr<IGUIFont>)font;
		if (!font->load(f.NamedPath.getPath()))
		{
			return 0;
		}
	}

	// add to fonts.

	f.Font = ifont;
	Fonts.push_back(f);

	return ifont;
}


//! add an externally loaded font
boost::shared_ptr<IGUIFont> CGUIEnvironment::addFont(const io::path& name, boost::shared_ptr<IGUIFont> font)
{
	if (font)
	{
		SFont f;
		f.NamedPath.setPath(name);
		s32 index = Fonts.binary_search(f);
		if (index != -1)
			return Fonts[index].Font;
		f.Font = font;
		Fonts.push_back(f);
	}
	return font;
}

//! remove loaded font
void CGUIEnvironment::removeFont(boost::shared_ptr<IGUIFont> font)
{
	if ( !font )
		return;
	for ( u32 i=0; i<Fonts.size(); ++i )
	{
		if ( Fonts[i].Font == font )
		{
			Fonts.erase(i);
			return;
		}
	}
}

//! returns default font
boost::shared_ptr<IGUIFont> CGUIEnvironment::getBuiltInFont() const
{
	if (Fonts.empty())
		return 0;

	return Fonts[0].Font;
}


boost::shared_ptr<IGUISpriteBank> CGUIEnvironment::getSpriteBank(const io::path& filename)
{
	// search for the file name

	SSpriteBank b;
	b.NamedPath.setPath(filename);

	s32 index = Banks.binary_search(b);
	if (index != -1)
		return Banks[index].Bank;

	// we don't have this sprite bank, we should load it
	if (!FileSystem->existFile(b.NamedPath.getPath()))
	{
		if ( filename != DefaultFontName )
		{
			os::Printer::log("Could not load sprite bank because the file does not exist", b.NamedPath.getPath(), ELL_DEBUG);
		}
		return 0;
	}

	// todo: load it!

	return 0;
}


boost::shared_ptr<IGUISpriteBank> CGUIEnvironment::addEmptySpriteBank(const io::path& name)
{
	// no duplicate names allowed

	SSpriteBank b;
	b.NamedPath.setPath(name);

	const s32 index = Banks.binary_search(b);
	if (index != -1)
		return 0;

	// create a new sprite bank

	b.Bank = boost::make_shared<CGUISpriteBank>(getSharedEnvironment());
	Banks.push_back(b);

	return b.Bank;
}


//! Creates the image list from the given texture.
boost::shared_ptr<IGUIImageList> CGUIEnvironment::createImageList(  boost::shared_ptr<video::ITexture> texture,
					core::dimension2d<s32>	imageSize, bool useAlphaChannel )
{
	boost::shared_ptr<CGUIImageList> imageList = boost::make_shared<CGUIImageList>(getVideoDriver());
	if( !imageList->createImageList( texture, imageSize, useAlphaChannel ) )
	{
		return nullptr;
	}

	return imageList;
}

//! Returns the root gui element.
boost::shared_ptr<IGUIElement> CGUIEnvironment::getRootGUIElement()
{
	return getSharedThis();
}


//! Returns the next element in the tab group starting at the focused element
boost::shared_ptr<IGUIElement> CGUIEnvironment::getNextElement(bool reverse, bool group)
{
	// start the search at the root of the current tab group
	boost::shared_ptr<IGUIElement> startPos = Focus ? Focus->getTabGroup() : 0;
	s32 startOrder = -1;

	// if we're searching for a group
	if (group && startPos)
	{
		startOrder = startPos->getTabOrder();
	}
	else
	if (!group && Focus && !Focus->isTabGroup())
	{
		startOrder = Focus->getTabOrder();
		if (startOrder == -1)
		{
			// this element is not part of the tab cycle,
			// but its parent might be...
			boost::shared_ptr<IGUIElement> el = Focus;
			while (el && el->getParent() && startOrder == -1)
			{
				el = el->getParent();
				startOrder = el->getTabOrder();
			}

		}
	}

	if (group || !startPos)
		startPos = getSharedThis(); // start at the root

	// find the element
	boost::shared_ptr<IGUIElement> closest = 0;
	boost::shared_ptr<IGUIElement> first = 0;
	startPos->getNextElement(startOrder, reverse, group, first, closest);

	if (closest)
		return closest; // we found an element
	else if (first)
		return first; // go to the end or the start
	else if (group)
		return getSharedThis(); // no group found? root group
	else
		return 0;
}


//! creates an GUI Environment
boost::shared_ptr<IGUIEnvironment> createGUIEnvironment(boost::shared_ptr<io::IFileSystem> fs,
					boost::shared_ptr<video::IVideoDriver> Driver,
					boost::shared_ptr<IOSOperator> op)
{
	boost::shared_ptr<CGUIEnvironment> env = boost::make_shared<CGUIEnvironment>(fs, Driver, op);
	env->setWeakThis(env);

	return env;
}


} // end namespace gui
} // end namespace irr

#endif // _IRR_COMPILE_WITH_GUI_

