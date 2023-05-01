// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_GUI_ENVIRONMENT_H_INCLUDED__
#define __C_GUI_ENVIRONMENT_H_INCLUDED__

#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_GUI_

#include "IGUIEnvironment.h"
#include "IGUIElement.h"
#include "irrArray.h"
#include "IFileSystem.h"
#include "IOSOperator.h"

namespace irr
{
namespace io
{
	class IXMLWriter;
}
namespace gui
{

class CGUIEnvironment : public IGUIEnvironment, public IGUIElement
{
public:

	//! constructor
	CGUIEnvironment(boost::shared_ptr<io::IFileSystem> fs, boost::shared_ptr<video::IVideoDriver> driver, boost::shared_ptr<IOSOperator> op);

	//! destructor
	virtual ~CGUIEnvironment();

	//! draws all gui elements
	virtual void drawAll();

	//! returns pointer to the filesystem
	virtual boost::shared_ptr<io::IFileSystem> getFileSystem() const;

	//! returns a pointer to the OS operator
	virtual boost::shared_ptr<IOSOperator> getOSOperator() const;

	//! posts an input event to the environment
	virtual bool postEventFromUser(const SEvent& event);

	//! This sets a new event receiver for gui events. Usually you do not have to
	//! use this method, it is used by the internal engine.
	virtual void setUserEventReceiver(IEventReceiver* evr);

	//! removes all elements from the environment
	virtual void clear();

	//! called if an event happened.
	virtual bool OnEvent(const SEvent& event);

	//! returns the current gui skin
	virtual boost::shared_ptr<IGUISkin> getSkin() const;

	//! Sets a new GUI Skin
	virtual void setSkin(boost::shared_ptr<IGUISkin> skin);

	//! Creates a new GUI Skin based on a template.
	/** \return Returns a pointer to the created skin.
	If you no longer need the skin, you should call IGUISkin::drop().
	See IReferenceCounted::drop() for more information. */
	virtual boost::shared_ptr<IGUISkin> createSkin(EGUI_SKIN_TYPE type);

	//! Creates the image list from the given texture.
	virtual boost::shared_ptr<IGUIImageList> createImageList( boost::shared_ptr<video::ITexture> texture,
					core::dimension2d<s32> imageSize, bool useAlphaChannel );

	//! returns the font
	virtual boost::shared_ptr<IGUIFont> getFont(const io::path& filename);

	//! add an externally loaded font
	virtual boost::shared_ptr<IGUIFont> addFont(const io::path& name, boost::shared_ptr<IGUIFont> font);

	//! remove loaded font
	virtual void removeFont(boost::shared_ptr<IGUIFont> font);

	//! returns default font
	virtual boost::shared_ptr<IGUIFont> getBuiltInFont() const;

	//! returns the sprite bank
	virtual boost::shared_ptr<IGUISpriteBank> getSpriteBank(const io::path& filename);

	//! returns the sprite bank
	virtual boost::shared_ptr<IGUISpriteBank> addEmptySpriteBank(const io::path& name);

	//! adds an button. The returned pointer must not be dropped.
	virtual boost::shared_ptr<IGUIButton> addButton(const core::rect<s32>& rectangle, boost::shared_ptr<IGUIElement> parent=0, s32 id=-1, const wchar_t* text=0,const wchar_t* tooltiptext = 0);

	//! adds a window. The returned pointer must not be dropped.
	virtual boost::shared_ptr<IGUIWindow> addWindow(const core::rect<s32>& rectangle, bool modal = false,
		const wchar_t* text=0, boost::shared_ptr<IGUIElement> parent=0, s32 id=-1);

	//! adds a modal screen. The returned pointer must not be dropped.
	virtual boost::shared_ptr<IGUIElement> addModalScreen(boost::shared_ptr<IGUIElement> parent);

	//! Adds a message box.
	virtual boost::shared_ptr<IGUIWindow> addMessageBox(const wchar_t* caption, const wchar_t* text=0,
		bool modal = true, s32 flag = EMBF_OK, boost::shared_ptr<IGUIElement> parent=0, s32 id=-1, boost::shared_ptr<video::ITexture> image=0);

	//! adds a scrollbar. The returned pointer must not be dropped.
	virtual boost::shared_ptr<IGUIScrollBar> addScrollBar(bool horizontal, const core::rect<s32>& rectangle,
		boost::shared_ptr<IGUIElement> parent=0, s32 id=-1);

	//! Adds an image element.
	virtual boost::shared_ptr<IGUIImage> addImage(boost::shared_ptr<video::ITexture> image, core::position2d<s32> pos,
		bool useAlphaChannel=true, boost::shared_ptr<IGUIElement> parent=0, s32 id=-1, const wchar_t* text=0);

	//! adds an image. The returned pointer must not be dropped.
	virtual boost::shared_ptr<IGUIImage> addImage(const core::rect<s32>& rectangle,
		boost::shared_ptr<IGUIElement> parent=0, s32 id=-1, const wchar_t* text=0, bool useAlphaChannel=true);

	//! adds a checkbox
	virtual boost::shared_ptr<IGUICheckBox> addCheckBox(bool checked, const core::rect<s32>& rectangle, boost::shared_ptr<IGUIElement> parent=0, s32 id=-1, const wchar_t* text=0);

	//! adds a list box
	virtual boost::shared_ptr<IGUIListBox> addListBox(const core::rect<s32>& rectangle,
		boost::shared_ptr<IGUIElement> parent=0, s32 id=-1, bool drawBackground=false);

	//! adds a tree view
	virtual boost::shared_ptr<IGUITreeView> addTreeView(const core::rect<s32>& rectangle,
		boost::shared_ptr<IGUIElement> parent=0, s32 id=-1, bool drawBackground=false,
		bool scrollBarVertical = true, bool scrollBarHorizontal = false);

	//! adds an mesh viewer. The returned pointer must not be dropped.
	virtual boost::shared_ptr<IGUIMeshViewer> addMeshViewer(const core::rect<s32>& rectangle, boost::shared_ptr<IGUIElement> parent=0, s32 id=-1, const wchar_t* text=0);

	//! Adds a file open dialog.
	virtual boost::shared_ptr<IGUIFileOpenDialog> addFileOpenDialog(const wchar_t* title = 0,
			bool modal=true, boost::shared_ptr<IGUIElement> parent=0, s32 id=-1,
			bool restoreCWD=false, io::path::char_type* startDir=0);

	//! Adds a color select dialog.
	virtual boost::shared_ptr<IGUIColorSelectDialog> addColorSelectDialog(const wchar_t* title = 0, bool modal=true, boost::shared_ptr<IGUIElement> parent=0, s32 id=-1);

	//! adds a static text. The returned pointer must not be dropped.
	virtual boost::shared_ptr<IGUIStaticText> addStaticText(const wchar_t* text, const core::rect<s32>& rectangle,
		bool border=false, bool wordWrap=true, boost::shared_ptr<IGUIElement> parent=0, s32 id=-1, bool drawBackground = false);

	//! Adds an edit box. The returned pointer must not be dropped.
	virtual boost::shared_ptr<IGUIEditBox> addEditBox(const wchar_t* text, const core::rect<s32>& rectangle,
		bool border=false, boost::shared_ptr<IGUIElement> parent=0, s32 id=-1);

	//! Adds a spin box to the environment
	virtual boost::shared_ptr<IGUISpinBox> addSpinBox(const wchar_t* text, const core::rect<s32>& rectangle,
		bool border=false,boost::shared_ptr<IGUIElement> parent=0, s32 id=-1);

	//! Adds a tab control to the environment.
	virtual boost::shared_ptr<IGUITabControl> addTabControl(const core::rect<s32>& rectangle,
		boost::shared_ptr<IGUIElement> parent=0, bool fillbackground=false, bool border=true, s32 id=-1);

	//! Adds tab to the environment.
	virtual boost::shared_ptr<IGUITab> addTab(const core::rect<s32>& rectangle,
		boost::shared_ptr<IGUIElement> parent=0, s32 id=-1);

	//! Adds a context menu to the environment.
	virtual boost::shared_ptr<IGUIContextMenu> addContextMenu(const core::rect<s32>& rectangle,
		boost::shared_ptr<IGUIElement> parent=0, s32 id=-1);

	//! Adds a menu to the environment.
	virtual boost::shared_ptr<IGUIContextMenu> addMenu(boost::shared_ptr<IGUIElement> parent=0, s32 id=-1);

	//! Adds a toolbar to the environment. It is like a menu is always placed on top
	//! in its parent, and contains buttons.
	virtual boost::shared_ptr<IGUIToolBar> addToolBar(boost::shared_ptr<IGUIElement> parent=0, s32 id=-1);

	//! Adds a combo box to the environment.
	virtual boost::shared_ptr<IGUIComboBox> addComboBox(const core::rect<s32>& rectangle,
		boost::shared_ptr<IGUIElement> parent=0, s32 id=-1);

	//! Adds a table element.
	virtual boost::shared_ptr<IGUITable> addTable(const core::rect<s32>& rectangle,
		boost::shared_ptr<IGUIElement> parent=0, s32 id=-1, bool drawBackground=false);

	//! Adds a empty element to the environment
	boost::shared_ptr<IGUIElement> addEmpty(boost::shared_ptr<IGUIElement> parent=nullptr, s32 id=-1) override;

	//! sets the focus to an element
	virtual bool setFocus(boost::shared_ptr<IGUIElement> element);

	//! removes the focus from an element
	virtual bool removeFocus(boost::shared_ptr<IGUIElement> element);

	//! Returns if the element has focus
	virtual bool hasFocus(boost::shared_ptr<IGUIElement> element) const;

	//! Returns the element with the focus
	virtual boost::shared_ptr<IGUIElement> getFocus() const;

	//! Returns the element last known to be under the mouse
	virtual boost::shared_ptr<IGUIElement> getHovered() const;

	//! Adds an element for fading in or out.
	virtual boost::shared_ptr<IGUIInOutFader> addInOutFader(const core::rect<s32>* rectangle=0, boost::shared_ptr<IGUIElement> parent=0, s32 id=-1);

	//! Returns the root gui element.
	virtual boost::shared_ptr<IGUIElement> getRootGUIElement();

	virtual void OnPostRender( u32 time );

	//! Returns the default element factory which can create all built in elements
	virtual boost::shared_ptr<IGUIElementFactory> getDefaultGUIElementFactory() const;

	//! Adds an element factory to the gui environment.
	/** Use this to extend the gui environment with new element types which it should be
	able to create automaticly, for example when loading data from xml files. */
	virtual void registerGUIElementFactory(boost::shared_ptr<IGUIElementFactory> factoryToAdd);

	//! Returns amount of registered scene node factories.
	virtual u32 getRegisteredGUIElementFactoryCount() const;

	//! Returns a scene node factory by index
	virtual boost::shared_ptr<IGUIElementFactory> getGUIElementFactory(u32 index) const;

	//! Adds a GUI Element by its name
	virtual boost::shared_ptr<IGUIElement> addGUIElement(const c8* elementName, boost::shared_ptr<IGUIElement> parent=0);

	//! Saves the current gui into a file.
	/** \param filename: Name of the file.
	\param start: The element to start saving from.
	if not specified, the root element will be used */
	virtual bool saveGUI( const io::path& filename, boost::shared_ptr<IGUIElement> start=0);

	//! Saves the current gui into a file.
	/** \param file: The file to save the GUI to.
	\param start: The element to start saving from.
	if not specified, the root element will be used */
	virtual bool saveGUI(boost::shared_ptr<io::IWriteFile> file, boost::shared_ptr<IGUIElement> start=0);

	//! Loads the gui. Note that the current gui is not cleared before.
	/** \param filename: Name of the file.
	\param parent: The parent of all loaded GUI elements,
	if not specified, the root element will be used */
	virtual bool loadGUI(const io::path& filename, boost::shared_ptr<IGUIElement> parent=0);

	//! Loads the gui. Note that the current gui is not cleared before.
	/** \param file: IReadFile to load the GUI from
	\param parent: The parent of all loaded GUI elements,
	if not specified, the root element will be used */
	virtual bool loadGUI(boost::shared_ptr<io::IReadFile> file, boost::shared_ptr<IGUIElement> parent=0);

	//! Writes attributes of the environment
	virtual void serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options=0) const;

	//! Reads attributes of the environment.
	virtual void deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options=0);

	//! writes an element
	virtual void writeGUIElement(io::IXMLWriter* writer, boost::shared_ptr<IGUIElement> node);

	//! reads an element
	virtual void readGUIElement(io::IXMLReader* reader, boost::shared_ptr<IGUIElement> node);

	virtual void setWeakThis(boost::shared_ptr<IGUIElement> value);
private:

	boost::shared_ptr<IGUIElement> getNextElement(bool reverse=false, bool group=false);

	void updateHoveredElement(core::position2d<s32> mousePos);

	void loadBuiltInFont();

	struct SFont
	{
		io::SNamedPath NamedPath;
		boost::shared_ptr<IGUIFont> Font;

		bool operator < (const SFont& other) const
		{
			return (NamedPath < other.NamedPath);
		}
	};

	struct SSpriteBank
	{
		io::SNamedPath NamedPath;
		boost::shared_ptr<IGUISpriteBank> Bank;

		bool operator < (const SSpriteBank& other) const
		{
			return (NamedPath < other.NamedPath);
		}
	};

	struct SToolTip
	{
		boost::shared_ptr<IGUIStaticText> Element;
		u32 LastTime;
		u32 EnterTime;
		u32 LaunchTime;
		u32 RelaunchTime;
	};

	SToolTip ToolTip;

	core::array<boost::shared_ptr<IGUIElementFactory>> GUIElementFactoryList;

	core::array<SFont> Fonts;
	core::array<SSpriteBank> Banks;
	boost::shared_ptr<IGUIElement> Hovered;
	boost::shared_ptr<IGUIElement> HoveredNoSubelement;	// subelements replaced by their parent, so you only have 'real' elements here
	boost::shared_ptr<IGUIElement> Focus;
	core::position2d<s32> LastHoveredMousePos;
	boost::shared_ptr<IGUISkin> CurrentSkin;
	boost::shared_ptr<io::IFileSystem> FileSystem;
	IEventReceiver* UserReceiver;
	boost::shared_ptr<IOSOperator> Operator;
	static const io::path DefaultFontName;
};

} // end namespace gui
} // end namespace irr

#endif // _IRR_COMPILE_WITH_GUI_

#endif // __C_GUI_ENVIRONMENT_H_INCLUDED__


