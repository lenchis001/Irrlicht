// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "CGUIFileOpenDialog.h"
#ifdef _IRR_COMPILE_WITH_GUI_

#include <locale.h>

#include "IGUISkin.h"
#include "IGUIEnvironment.h"
#include "IVideoDriver.h"
#include "IGUIButton.h"
#include "IGUIStaticText.h"
#include "IGUIFont.h"
#include "IGUIFontBitmap.h"
#include "IFileList.h"
#include "os.h"

namespace irr
{
namespace gui
{

const s32 FOD_WIDTH = 350;
const s32 FOD_HEIGHT = 250;


//! constructor
CGUIFileOpenDialog::CGUIFileOpenDialog(const wchar_t* title,
		boost::shared_ptr<IGUIEnvironment> environment, boost::shared_ptr<IGUIElement> parent, s32 id,
		bool restoreCWD, io::path::char_type* startDir)
: IGUIFileOpenDialog(environment, parent, id,
		core::rect<s32>((parent->getAbsolutePosition().getWidth()-FOD_WIDTH)/2,
					(parent->getAbsolutePosition().getHeight()-FOD_HEIGHT)/2,
					(parent->getAbsolutePosition().getWidth()-FOD_WIDTH)/2+FOD_WIDTH,
					(parent->getAbsolutePosition().getHeight()-FOD_HEIGHT)/2+FOD_HEIGHT)),
	FileNameText(0), FileList(0), Dragging(false)
{
	#ifdef _DEBUG
	IGUIElement::setDebugName("CGUIFileOpenDialog");
	#endif

	Text = title;

	_restoreCWD = restoreCWD;
	_startDir = startDir;
}


//! destructor
CGUIFileOpenDialog::~CGUIFileOpenDialog()
{
	if (FileSystem)
	{
		// revert to original CWD if path was set in constructor
		if (RestoreDirectory.size())
			FileSystem->changeWorkingDirectoryTo(RestoreDirectory);
	}
}


//! returns the filename of the selected file. Returns NULL, if no file was selected.
const wchar_t* CGUIFileOpenDialog::getFileName() const
{
	return FileName.c_str();
}

//! Returns the directory of the selected file. Returns NULL, if no directory was selected.
const io::path& CGUIFileOpenDialog::getDirectoryName()
{
	FileSystem->flattenFilename ( FileDirectory );
	return FileDirectory;
}


//! called if an event happened.
bool CGUIFileOpenDialog::OnEvent(const SEvent& event)
{
	if (isEnabled())
	{
		switch(event.EventType)
		{
		case EET_GUI_EVENT:
			switch(event.GUIEvent.EventType)
			{
			case EGET_ELEMENT_FOCUS_LOST:
				Dragging = false;
				break;
			case EGET_BUTTON_CLICKED:
				if (event.GUIEvent.Caller == CloseButton ||
					event.GUIEvent.Caller == CancelButton)
				{
					sendCancelEvent();
					remove();
					return true;
				}
				else
				if (event.GUIEvent.Caller == OKButton )
				{
					if ( FileDirectory != L"" )
					{
						sendSelectedEvent( EGET_DIRECTORY_SELECTED );
					}
					if ( FileName != L"" )
					{
						sendSelectedEvent( EGET_FILE_SELECTED );
						remove();
						return true;
					}
				}
				break;

			case EGET_LISTBOX_CHANGED:
				{
					s32 selected = FileBox->getSelected();
					if (FileList && FileSystem)
					{
						if (FileList->isDirectory(selected))
						{
							FileName = L"";
							FileDirectory = FileList->getFullFileName(selected);
						}
						else
						{
							FileDirectory = L"";
							FileName = FileList->getFullFileName(selected);
						}
						return true;
					}
				}
				break;

			case EGET_LISTBOX_SELECTED_AGAIN:
				{
					const s32 selected = FileBox->getSelected();
					if (FileList && FileSystem)
					{
						if (FileList->isDirectory(selected))
						{
							FileDirectory = FileList->getFullFileName(selected);
							FileSystem->changeWorkingDirectoryTo(FileList->getFileName(selected));
							fillListBox();
							FileName = "";
						}
						else
						{
							FileName = FileList->getFullFileName(selected);
						}
						return true;
					}
				}
				break;
			case EGET_EDITBOX_ENTER:
				if (event.GUIEvent.Caller == FileNameText)
				{
					io::path dir( FileNameText->getText () );
					if ( FileSystem->changeWorkingDirectoryTo( dir ) )
					{
						fillListBox();
						FileName = L"";
					}
					return true;
				}
			break;
			default:
				break;
			}
			break;
		case EET_MOUSE_INPUT_EVENT:
			switch(event.MouseInput.Event)
			{
			case EMIE_MOUSE_WHEEL:
				return FileBox->OnEvent(event);
			case EMIE_LMOUSE_PRESSED_DOWN:
				DragStart.X = event.MouseInput.X;
				DragStart.Y = event.MouseInput.Y;
				Dragging = true;
				getSharedEnvironment()->setFocus(getSharedThis());
				return true;
			case EMIE_LMOUSE_LEFT_UP:
				Dragging = false;
				return true;
			case EMIE_MOUSE_MOVED:

				if ( !event.MouseInput.isLeftPressed () )
					Dragging = false;

				if (Dragging)
				{
					boost::shared_ptr<IGUIElement> lockedParent = getParent();

					// gui window should not be dragged outside its parent
					if (lockedParent)
						if (event.MouseInput.X < lockedParent->getAbsolutePosition().UpperLeftCorner.X +1 ||
							event.MouseInput.Y < lockedParent->getAbsolutePosition().UpperLeftCorner.Y +1 ||
							event.MouseInput.X > lockedParent->getAbsolutePosition().LowerRightCorner.X -1 ||
							event.MouseInput.Y > lockedParent->getAbsolutePosition().LowerRightCorner.Y -1)

							return true;

					move(core::position2d<s32>(event.MouseInput.X - DragStart.X, event.MouseInput.Y - DragStart.Y));
					DragStart.X = event.MouseInput.X;
					DragStart.Y = event.MouseInput.Y;
					return true;
				}
				break;
			default:
				break;
			}
		default:
			break;
		}
	}

	return IGUIElement::OnEvent(event);
}


//! draws the element and its children
void CGUIFileOpenDialog::draw()
{
	if (!IsVisible)
		return;

	boost::shared_ptr<IGUISkin> skin = getSharedEnvironment()->getSkin();

	core::rect<s32> rect = AbsoluteRect;

	rect = skin->draw3DWindowBackground(getSharedThis(), true, skin->getColor(EGDC_ACTIVE_BORDER),
		rect, &AbsoluteClippingRect);

	if (Text.size())
	{
		rect.UpperLeftCorner.X += 2;
		rect.LowerRightCorner.X -= skin->getSize(EGDS_WINDOW_BUTTON_WIDTH) + 5;

		boost::shared_ptr<IGUIFont> font = skin->getFont(EGDF_WINDOW);
		if (font)
			font->draw(Text.c_str(), rect,
					skin->getColor(EGDC_ACTIVE_CAPTION),
					false, true, &AbsoluteClippingRect);
	}

	IGUIElement::draw();
}


//! Writes attributes of the element.
/* Not sure if this will really work out properly. Saving paths can be
rather problematic. */
void CGUIFileOpenDialog::serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options) const
{
	IGUIFileOpenDialog::serializeAttributes(out,options);

	out->addString("StartDirectory", StartDirectory.c_str());
	out->addBool("RestoreDirectory", (RestoreDirectory.size()!=0));
}


//! Reads attributes of the element
/* Note that thiese paths changes will happen at arbitrary places upon
load of the gui description. This may be undesired. */
void CGUIFileOpenDialog::deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options)
{
	StartDirectory = in->getAttributeAsString("StartDirectory");
	const bool restore = in->getAttributeAsBool("RestoreDirectory");
	if (restore)
		RestoreDirectory = FileSystem->getWorkingDirectory();
	else
		RestoreDirectory = "";
	if (StartDirectory.size())
		FileSystem->changeWorkingDirectoryTo(StartDirectory);

	IGUIFileOpenDialog::deserializeAttributes(in,options);
}

void CGUIFileOpenDialog::setWeakThis(boost::shared_ptr<IGUIElement> value)
{
	IGUIElement::setWeakThis(value);

	boost::shared_ptr<IGUIEnvironment> lockedEnvironment = getSharedEnvironment();
	FileSystem = lockedEnvironment ? lockedEnvironment->getFileSystem() : 0;

	if (FileSystem)
	{
		if (_restoreCWD)
			RestoreDirectory = FileSystem->getWorkingDirectory();
		if (_startDir)
		{
			StartDirectory = _startDir;
			FileSystem->changeWorkingDirectoryTo(_startDir);
		}
	}
	else
		return;

	boost::shared_ptr<IGUISpriteBank> sprites = 0;
	video::SColor color(255, 255, 255, 255);
	boost::shared_ptr<IGUISkin> skin = lockedEnvironment->getSkin();
	if (skin)
	{
		sprites = skin->getSpriteBank();
		color = skin->getColor(EGDC_WINDOW_SYMBOL);
	}

	const s32 buttonw = skin->getSize(EGDS_WINDOW_BUTTON_WIDTH);
	const s32 posx = RelativeRect.getWidth() - buttonw - 4;

	boost::shared_ptr<IGUIElement> lockedThis = getSharedThis();

	CloseButton = lockedEnvironment->addButton(core::rect<s32>(posx, 3, posx + buttonw, 3 + buttonw), lockedThis, -1,
		L"", skin ? skin->getDefaultText(EGDT_WINDOW_CLOSE) : L"Close");
	CloseButton->setSubElement(true);
	CloseButton->setTabStop(false);
	if (sprites)
	{
		CloseButton->setSpriteBank(sprites);
		CloseButton->setSprite(EGBS_BUTTON_UP, skin->getIcon(EGDI_WINDOW_CLOSE), color);
		CloseButton->setSprite(EGBS_BUTTON_DOWN, skin->getIcon(EGDI_WINDOW_CLOSE), color);
	}
	CloseButton->setAlignment(EGUIA_LOWERRIGHT, EGUIA_LOWERRIGHT, EGUIA_UPPERLEFT, EGUIA_UPPERLEFT);

	OKButton = lockedEnvironment->addButton(
		core::rect<s32>(RelativeRect.getWidth() - 80, 30, RelativeRect.getWidth() - 10, 50),
		lockedThis, -1, skin ? skin->getDefaultText(EGDT_MSG_BOX_OK) : L"OK");
	OKButton->setSubElement(true);
	OKButton->setAlignment(EGUIA_LOWERRIGHT, EGUIA_LOWERRIGHT, EGUIA_UPPERLEFT, EGUIA_UPPERLEFT);

	CancelButton = lockedEnvironment->addButton(
		core::rect<s32>(RelativeRect.getWidth() - 80, 55, RelativeRect.getWidth() - 10, 75),
		lockedThis, -1, skin ? skin->getDefaultText(EGDT_MSG_BOX_CANCEL) : L"Cancel");
	CancelButton->setSubElement(true);
	CancelButton->setAlignment(EGUIA_LOWERRIGHT, EGUIA_LOWERRIGHT, EGUIA_UPPERLEFT, EGUIA_UPPERLEFT);

	FileBox = lockedEnvironment->addListBox(core::rect<s32>(10, 55, RelativeRect.getWidth() - 90, 230), lockedThis, -1, true);
	FileBox->setSubElement(true);
	FileBox->setAlignment(EGUIA_UPPERLEFT, EGUIA_LOWERRIGHT, EGUIA_UPPERLEFT, EGUIA_LOWERRIGHT);

	FileNameText = lockedEnvironment->addEditBox(0, core::rect<s32>(10, 30, RelativeRect.getWidth() - 90, 50), true, lockedThis);
	FileNameText->setSubElement(true);
	FileNameText->setAlignment(EGUIA_UPPERLEFT, EGUIA_LOWERRIGHT, EGUIA_UPPERLEFT, EGUIA_UPPERLEFT);

	setTabGroup(true);

	fillListBox();
}


//! fills the listbox with files.
void CGUIFileOpenDialog::fillListBox()
{
	boost::shared_ptr<IGUISkin>skin = getSharedEnvironment()->getSkin();

	if (!FileSystem || !FileBox || !skin)
		return;

	FileBox->clear();

	FileList = FileSystem->createFileList();
	core::stringw s;

	setlocale(LC_ALL,"");

	if (FileList)
	{
		for (u32 i=0; i < FileList->getFileCount(); ++i)
		{
			#ifndef _IRR_WCHAR_FILESYSTEM
			const c8 *cs = (const c8 *)FileList->getFileName(i).c_str();
			wchar_t *ws = new wchar_t[strlen(cs) + 1];
			int len = mbstowcs(ws,cs,strlen(cs));
			ws[len] = 0;
			s = ws;
			delete [] ws;
			#else
			s = FileList->getFileName(i).c_str();
			#endif
			FileBox->addItem(s.c_str(), skin->getIcon(FileList->isDirectory(i) ? EGDI_DIRECTORY : EGDI_FILE));
		}
	}

	if (FileNameText)
	{
		#ifndef _IRR_WCHAR_FILESYSTEM
		const c8 *cs = (const c8 *)FileSystem->getWorkingDirectory().c_str();
		wchar_t *ws = new wchar_t[strlen(cs) + 1];
		int len = mbstowcs(ws,cs,strlen(cs));
		ws[len] = 0;
		s = ws;
		delete [] ws;
		#else
		s = FileSystem->getWorkingDirectory();
		#endif

		FileDirectory = s;
		FileNameText->setText(s.c_str());
	}
}


//! sends the event that the file has been selected.
void CGUIFileOpenDialog::sendSelectedEvent( EGUI_EVENT_TYPE type)
{
	SEvent event;
	event.EventType = EET_GUI_EVENT;
	event.GUIEvent.Caller = getSharedThis();
	event.GUIEvent.Element = 0;
	event.GUIEvent.EventType = type;
	getParent()->OnEvent(event);
}


//! sends the event that the file choose process has been canceld
void CGUIFileOpenDialog::sendCancelEvent()
{
	SEvent event;
	event.EventType = EET_GUI_EVENT;
	event.GUIEvent.Caller = getSharedThis();
	event.GUIEvent.Element = 0;
	event.GUIEvent.EventType = EGET_FILE_CHOOSE_DIALOG_CANCELLED;
	getParent()->OnEvent(event);
}

} // end namespace gui
} // end namespace irr

#endif // _IRR_COMPILE_WITH_GUI_
