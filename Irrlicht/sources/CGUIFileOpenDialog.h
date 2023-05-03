// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_GUI_FILE_OPEN_DIALOG_H_INCLUDED__
#define __C_GUI_FILE_OPEN_DIALOG_H_INCLUDED__

#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_GUI_

#include "IGUIFileOpenDialog.h"
#include "IGUIButton.h"
#include "IGUIListBox.h"
#include "IGUIEditBox.h"
#include "IFileSystem.h"

namespace irr
{
namespace gui
{

	class CGUIFileOpenDialog : public IGUIFileOpenDialog
	{
	public:

		//! constructor
		CGUIFileOpenDialog(const wchar_t* title, boost::shared_ptr<IGUIEnvironment> environment,
				boost::shared_ptr<IGUIElement> parent, s32 id, bool restoreCWD=false,
				io::path::char_type* startDir=0);

		//! destructor
		virtual ~CGUIFileOpenDialog();

		//! returns the filename of the selected file. Returns NULL, if no file was selected.
		virtual const wchar_t* getFileName() const;

		//! Returns the directory of the selected file. Returns NULL, if no directory was selected.
		virtual const io::path& getDirectoryName();

		//! called if an event happened.
		virtual bool OnEvent(const SEvent& event);

		//! draws the element and its children
		virtual void draw();

		virtual void serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options=0) const;
		virtual void deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options=0);

		virtual void setWeakThis(boost::shared_ptr<IGUIElement> value) override;

	protected:

		//! fills the listbox with files.
		void fillListBox();

		//! sends the event that the file has been selected.
		void sendSelectedEvent( EGUI_EVENT_TYPE type );

		//! sends the event that the file choose process has been canceld
		void sendCancelEvent();

		core::position2d<s32> DragStart;
		core::stringw FileName;
		io::path FileDirectory;
		io::path RestoreDirectory;
		io::path StartDirectory;

		boost::shared_ptr<IGUIButton> CloseButton;
		boost::shared_ptr<IGUIButton> OKButton;
		boost::shared_ptr<IGUIButton> CancelButton;
		boost::shared_ptr<IGUIListBox> FileBox;
		boost::shared_ptr<IGUIEditBox> FileNameText;
		boost::shared_ptr<IGUIElement> EventParent;
		boost::shared_ptr<io::IFileSystem> FileSystem;
		boost::shared_ptr<io::IFileList> FileList;
		bool Dragging;
		bool _restoreCWD;
		io::path::char_type* _startDir;
	};


} // end namespace gui
} // end namespace irr

#endif // _IRR_COMPILE_WITH_GUI_

#endif // __C_GUI_FILE_OPEN_DIALOG_H_INCLUDED__
