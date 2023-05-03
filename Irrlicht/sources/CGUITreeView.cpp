// This file is part of the "Irrlicht Engine".
// written by Reinhard Ostermeier, reinhard@nospam.r-ostermeier.de
// expanded by burningwater

#include "CGUITreeView.h"

#ifdef _IRR_COMPILE_WITH_GUI_

#include "IGUISkin.h"
#include "IGUIEnvironment.h"
#include "IVideoDriver.h"
#include "IGUIFont.h"
#include "CGUIScrollBar.h"
#include "os.h"

namespace irr
{
namespace gui
{

CGUITreeViewNode::CGUITreeViewNode( boost::shared_ptr<CGUITreeView> owner, boost::shared_ptr<CGUITreeViewNode> parent )
	: Owner(owner), Parent(parent), ImageIndex(-1), SelectedImageIndex(-1),
	Data(0), Data2(0), Expanded(false)
{
#ifdef _DEBUG
	setDebugName( "CGUITreeView" );
#endif
}

CGUITreeViewNode::~CGUITreeViewNode()
{
	if( Owner && this == Owner->getSelected().get() )
	{
		setSelected( false );
	}

	clearChildren();

	if( Data2 )
	{
		Data2->drop();
	}
}

boost::shared_ptr<IGUITreeView> CGUITreeViewNode::getOwner() const
{
	return Owner;
}

boost::shared_ptr<IGUITreeViewNode> CGUITreeViewNode::getParent() const
{
	return Parent.lock();
}

void CGUITreeViewNode::setText( const wchar_t* text )
{
	Text = text;
}

void CGUITreeViewNode::setIcon( const wchar_t* icon )
{
	Icon = icon;
}

void CGUITreeViewNode::clearChildren()
{
	core::list<boost::shared_ptr<CGUITreeViewNode>>::Iterator	it;

	Children.clear();
}

boost::shared_ptr<IGUITreeViewNode> CGUITreeViewNode::addChildBack(
	const wchar_t*		text,
	const wchar_t*		icon /*= 0*/,
	s32					imageIndex /*= -1*/,
	s32					selectedImageIndex /*= -1*/,
	void*					data /*= 0*/,
	IReferenceCounted*			data2 /*= 0*/ )
{
	boost::shared_ptr<CGUITreeViewNode>	newChild = boost::make_shared<CGUITreeViewNode>( Owner, boost::static_pointer_cast<CGUITreeViewNode>(getSharedThis()));
	newChild->setWeakThis(newChild);

	Children.push_back( newChild );
	newChild->Text = text;
	newChild->Icon = icon;
	newChild->ImageIndex = imageIndex;
	newChild->SelectedImageIndex = selectedImageIndex;
	newChild->Data = data;
	newChild->Data2 = data2;
	if( data2 )
	{
		data2->grab();
	}
	return newChild;
}

boost::shared_ptr<IGUITreeViewNode> CGUITreeViewNode::addChildFront(
	const wchar_t*		text,
	const wchar_t*		icon /*= 0*/,
	s32					imageIndex /*= -1*/,
	s32					selectedImageIndex /*= -1*/,
	void*					data /*= 0*/,
	IReferenceCounted*			data2 /*= 0*/ )
{
	boost::shared_ptr<CGUITreeViewNode>	newChild = boost::make_shared<CGUITreeViewNode>( Owner, getSharedThis() );
	newChild->setWeakThis(newChild);

	Children.push_front( newChild );
	newChild->Text = text;
	newChild->Icon = icon;
	newChild->ImageIndex = imageIndex;
	newChild->SelectedImageIndex = selectedImageIndex;
	newChild->Data = data;
	newChild->Data2 = data2;
	if( data2 )
	{
		data2->grab();
	}
	return newChild;
}

boost::shared_ptr<IGUITreeViewNode> CGUITreeViewNode::insertChildAfter(
	boost::shared_ptr<IGUITreeViewNode>	other,
	const wchar_t*		text,
	const wchar_t*		icon /*= 0*/,
	s32					imageIndex /*= -1*/,
	s32					selectedImageIndex /*= -1*/,
	void*					data /*= 0*/,
	IReferenceCounted*			data2/* = 0*/ )
{
	core::list<boost::shared_ptr<CGUITreeViewNode>>::Iterator	itOther;
	boost::shared_ptr<CGUITreeViewNode>									newChild = 0;

	for( itOther = Children.begin(); itOther != Children.end(); itOther++ )
	{
		if( other == *itOther )
		{
			newChild = boost::make_shared<CGUITreeViewNode>( Owner, getSharedThis() );
			newChild->setWeakThis(newChild);

			newChild->Text = text;
			newChild->Icon = icon;
			newChild->ImageIndex = imageIndex;
			newChild->SelectedImageIndex = selectedImageIndex;
			newChild->Data = data;
			newChild->Data2 = data2;
			if( data2 )
			{
				data2->grab();
			}
			Children.insert_after( itOther, newChild );
			break;
		}
	}
	return newChild;
}

boost::shared_ptr<IGUITreeViewNode> CGUITreeViewNode::insertChildBefore(
	boost::shared_ptr<IGUITreeViewNode>	other,
	const wchar_t*		text,
	const wchar_t*		icon /*= 0*/,
	s32					imageIndex /*= -1*/,
	s32					selectedImageIndex /*= -1*/,
	void*					data /*= 0*/,
	IReferenceCounted*			data2/* = 0*/ )
{
	core::list<boost::shared_ptr<CGUITreeViewNode>>::Iterator	itOther;
	boost::shared_ptr<CGUITreeViewNode>									newChild = 0;

	for( itOther = Children.begin(); itOther != Children.end(); itOther++ )
	{
		if( other == *itOther )
		{
			newChild = boost::make_shared<CGUITreeViewNode>( Owner, getSharedThis() );
			newChild->setWeakThis(newChild);

			newChild->Text = text;
			newChild->Icon = icon;
			newChild->ImageIndex = imageIndex;
			newChild->SelectedImageIndex = selectedImageIndex;
			newChild->Data = data;
			newChild->Data2 = data2;
			if( data2 )
			{
				data2->grab();
			}
			Children.insert_before( itOther, newChild );
			break;
		}
	}
	return newChild;
}

boost::shared_ptr<IGUITreeViewNode> CGUITreeViewNode::getFirstChild() const
{
	if( Children.empty() )
	{
		return 0;
	}
	else
	{
		return *( Children.begin() );
	}
}

boost::shared_ptr<IGUITreeViewNode> CGUITreeViewNode::getLastChild() const
{
	if( Children.empty() )
	{
		return 0;
	}
	else
	{
		return *( Children.getLast() );
	}
}

boost::shared_ptr<IGUITreeViewNode> CGUITreeViewNode::getPrevSibling() const
{
	core::list<boost::shared_ptr<CGUITreeViewNode>>::Iterator	itThis;
	core::list<boost::shared_ptr<CGUITreeViewNode>>::Iterator	itOther;
	boost::shared_ptr<CGUITreeViewNode>									other = 0;

	if( !Parent.expired() )
	{
		boost::shared_ptr<CGUITreeViewNode> lockedParent = boost::static_pointer_cast<CGUITreeViewNode>(getParent());

		for( itThis = lockedParent->Children.begin(); itThis != lockedParent->Children.end(); itThis++ )
		{
			if( this == (*itThis).get())
			{
				if( itThis != lockedParent->Children.begin() )
				{
					other = *itOther;
				}
				break;
			}
			itOther = itThis;
		}
	}
	return other;
}

boost::shared_ptr<IGUITreeViewNode> CGUITreeViewNode::getNextSibling() const
{
	core::list<boost::shared_ptr<CGUITreeViewNode>>::Iterator	itThis;
	boost::shared_ptr<CGUITreeViewNode>									other = 0;

	if( !Parent.expired() )
	{
		boost::shared_ptr<CGUITreeViewNode> lockedParent = boost::static_pointer_cast<CGUITreeViewNode>(getParent());

		for( itThis = lockedParent->Children.begin(); itThis != lockedParent->Children.end(); itThis++ )
		{
			if( this == (*itThis).get())
			{
				if( itThis != lockedParent->Children.getLast() )
				{
					other = *( ++itThis );
				}
				break;
			}
		}
	}
	return other;
}

boost::shared_ptr<IGUITreeViewNode> CGUITreeViewNode::getNextVisible() const
{
	boost::shared_ptr<IGUITreeViewNode>	next = 0;
	boost::shared_ptr<IGUITreeViewNode>	node = 0;

	node = getSharedThis();

	if( node->getExpanded() && node->hasChildren() )
	{
		next = node->getFirstChild();
	}
	else
	{
		next = node->getNextSibling();
	}
	while( !next && node->getParent() )
	{
		next = node->getParent()->getNextSibling();
		if( !next )
		{
			node = node->getParent();
		}
	}

	return next;
}

bool CGUITreeViewNode::deleteChild( boost::shared_ptr<IGUITreeViewNode> child )
{
	core::list<boost::shared_ptr<CGUITreeViewNode>>::Iterator	itChild;
	bool	deleted = false;

	for( itChild = Children.begin(); itChild != Children.end(); itChild++ )
	{
		if( child == *itChild )
		{
			Children.erase( itChild );
			deleted = true;
			break;
		}
	}
	return deleted;
}

bool CGUITreeViewNode::moveChildUp( boost::shared_ptr<IGUITreeViewNode> child )
{
	core::list<boost::shared_ptr<CGUITreeViewNode>>::Iterator	itChild;
	core::list<boost::shared_ptr<CGUITreeViewNode>>::Iterator	itOther;
	boost::shared_ptr<CGUITreeViewNode>									nodeTmp;
	bool													moved = false;

	for( itChild = Children.begin(); itChild != Children.end(); itChild++ )
	{
		if( child == *itChild )
		{
			if( itChild != Children.begin() )
			{
				nodeTmp = *itChild;
				*itChild = *itOther;
				*itOther = nodeTmp;
				moved = true;
			}
			break;
		}
		itOther = itChild;
	}
	return moved;
}

bool CGUITreeViewNode::moveChildDown( boost::shared_ptr<IGUITreeViewNode> child )
{
	core::list<boost::shared_ptr<CGUITreeViewNode>>::Iterator	itChild;
	core::list<boost::shared_ptr<CGUITreeViewNode>>::Iterator	itOther;
	boost::shared_ptr<CGUITreeViewNode>									nodeTmp;
	bool													moved = false;

	for( itChild = Children.begin(); itChild != Children.end(); itChild++ )
	{
		if( child == *itChild )
		{
			if( itChild != Children.getLast() )
			{
				itOther = itChild;
				++itOther;
				nodeTmp = *itChild;
				*itChild = *itOther;
				*itOther = nodeTmp;
				moved = true;
			}
			break;
		}
	}
	return moved;
}

void CGUITreeViewNode::setExpanded( bool expanded )
{
	Expanded = expanded;
}

void CGUITreeViewNode::setSelected( bool selected )
{
	if( Owner )
	{
		if( selected )
		{
			Owner->Selected = getSharedThis();
		}
		else
		{
			if( Owner->Selected.get() == this)
			{
				Owner->Selected = 0;
			}
		}
	}
}

bool CGUITreeViewNode::getSelected() const
{
	if( Owner )
	{
		return Owner->Selected == getSharedThis();
	}
	else
	{
		return false;
	}
}

bool CGUITreeViewNode::isRoot() const
{
	return ( Owner && ( this == Owner->Root.get() ) );
}

s32 CGUITreeViewNode::getLevel() const
{
	if( Parent.expired() )
	{
		return 0;
	}
	else
	{
		boost::shared_ptr<IGUITreeViewNode> lockedParent = getParent();
		return lockedParent->getLevel() + 1;
	}
}

bool CGUITreeViewNode::isVisible() const
{
	if( Parent.expired() )
	{
		return true;
	}
	else
	{
		boost::shared_ptr<IGUITreeViewNode> lockedParent = getParent();

		return lockedParent->getExpanded() && lockedParent->isVisible();
	}
}

void CGUITreeViewNode::setWeakThis(boost::shared_ptr<CGUITreeViewNode> value)
{
#ifdef _DEBUG
	assert(this == value.get());
#endif
	WeakThis = value;
}

inline boost::shared_ptr<CGUITreeViewNode> CGUITreeViewNode::getSharedThis() const
{
	assert(!WeakThis.expired());

	return WeakThis.lock();
}


//! constructor
CGUITreeView::CGUITreeView(boost::shared_ptr<IGUIEnvironment> environment, boost::shared_ptr<IGUIElement> parent,
	s32 id, core::rect<s32> rectangle, bool clip,
	bool drawBack, bool scrollBarVertical, bool scrollBarHorizontal)
	: IGUITreeView( environment, parent, id, rectangle ),
	Root(0), Selected(0),
	ItemHeight( 0 ),
	IndentWidth( 0 ),
	TotalItemHeight( 0 ),
	TotalItemWidth ( 0 ),
	Font( 0 ),
	IconFont( 0 ),
	ScrollBarH( 0 ),
	ScrollBarV( 0 ),
	ImageList( 0 ),
	LastEventNode( 0 ),
	LinesVisible( true ),
	Selecting( false ),
	Clip( clip ),
	DrawBack( drawBack ),
	ImageLeftOfIcon( true )
{
#ifdef _DEBUG
	setDebugName( "CGUITreeView" );
#endif

	_scrollBarHorizontal = scrollBarHorizontal;
	_scrollBarVertical = scrollBarVertical;
	_clip = clip;
}


//! destructor
CGUITreeView::~CGUITreeView()
{
}

void CGUITreeView::setWeakThis(boost::shared_ptr<IGUIElement> value)
{
	IGUIElement::setWeakThis(value);

	boost::shared_ptr<IGUISkin> skin = getSharedEnvironment()->getSkin();
	s32 s = skin->getSize(EGDS_SCROLLBAR_SIZE);

	if (_scrollBarVertical)
	{
		ScrollBarV = boost::make_shared<CGUIScrollBar>(false, getSharedEnvironment(), getSharedThis(), 0,
			core::rect<s32>(RelativeRect.getWidth() - s,
				0,
				RelativeRect.getWidth(),
				RelativeRect.getHeight() - (_scrollBarHorizontal ? s : 0)
				),
			!_clip);

		ScrollBarV->setSubElement(true);
		ScrollBarV->setPos(0);
	}

	if (_scrollBarHorizontal)
	{
		ScrollBarH = boost::make_shared<CGUIScrollBar>(true, getSharedEnvironment(), getSharedThis(), 1,
			core::rect<s32>(0, RelativeRect.getHeight() - s, RelativeRect.getWidth() - s, RelativeRect.getHeight()),
			!_clip);

		ScrollBarH->setSubElement(true);
		ScrollBarH->setPos(0);
	}

	Root = boost::make_shared<CGUITreeViewNode>(getSharedThis<CGUITreeView>(), nullptr);
	Root->setWeakThis(Root);

	Root->Expanded = true;

	recalculateItemHeight();
}

void CGUITreeView::recalculateItemHeight()
{
	boost::shared_ptr<IGUISkin> skin = getSharedEnvironment()->getSkin();
	boost::shared_ptr<IGUITreeViewNode>	node;

	if( Font != skin->getFont() )
	{
		Font = skin->getFont();
		ItemHeight = 0;

		if( Font )
		{
			ItemHeight = Font->getDimension( L"A" ).Height + 4;
		}

		if( IconFont )
		{
			s32 height = IconFont->getDimension( L" " ).Height;
			if( height > ItemHeight )
			{
				ItemHeight = height;
			}
		}
		if( ImageList )
		{
			if( ImageList->getImageSize().Height + 1 > ItemHeight )
			{
				ItemHeight = ImageList->getImageSize().Height + 1;
			}
		}
	}

	IndentWidth = ItemHeight;
	if( IndentWidth < 9 )
	{
		IndentWidth = 9;
	}
	else if( IndentWidth > 15 )
	{
		IndentWidth = 15;
	}
	else
	{
		if( ( ( IndentWidth >> 1 ) << 1 ) - IndentWidth == 0 )
		{
			--IndentWidth;
		}
	}

	TotalItemHeight = 0;
	TotalItemWidth = AbsoluteRect.getWidth() * 2;
	node = Root->getFirstChild();
	while( node )
	{
		TotalItemHeight += ItemHeight;
		node = node->getNextVisible();
	}

	if ( ScrollBarV )
		ScrollBarV->setMax( core::max_(0,TotalItemHeight - AbsoluteRect.getHeight()) );

	if ( ScrollBarH )
		ScrollBarH->setMax( core::max_(0, TotalItemWidth - AbsoluteRect.getWidth()) );

}

//! called if an event happened.
bool CGUITreeView::OnEvent( const SEvent &event )
{
	if ( isEnabled() )
	{
		switch( event.EventType )
		{
		case EET_GUI_EVENT:
			switch( event.GUIEvent.EventType )
			{
			case gui::EGET_SCROLL_BAR_CHANGED:
				if( event.GUIEvent.Caller == ScrollBarV || event.GUIEvent.Caller == ScrollBarH )
				{
					//s32 pos = ( ( boost::shared_ptr<gui::IGUIScrollBar> )event.GUIEvent.Caller )->getPos();
					return true;
				}
				break;
			case gui::EGET_ELEMENT_FOCUS_LOST:
				{
					Selecting = false;
					return false;
				}
				break;
			default:
				break;
			}
			break;
		case EET_MOUSE_INPUT_EVENT:
			{
				core::position2d<s32> p( event.MouseInput.X, event.MouseInput.Y );
				boost::shared_ptr<IGUIElement> lockedThis = getSharedThis();
				boost::shared_ptr<IGUIEnvironment> lockedEnvironment = getSharedEnvironment();

				switch( event.MouseInput.Event )
				{
				case EMIE_MOUSE_WHEEL:
					if ( ScrollBarV )
						ScrollBarV->setPos( ScrollBarV->getPos() + (event.MouseInput.Wheel < 0 ? -1 : 1) * -10 );
					return true;
					break;

				case EMIE_LMOUSE_PRESSED_DOWN:

					if (lockedEnvironment->hasFocus(lockedThis) && !AbsoluteClippingRect.isPointInside(p) )
					{
						lockedEnvironment->removeFocus(lockedThis);
						return false;
					}

					if(lockedEnvironment->hasFocus(lockedThis) &&
						(	( ScrollBarV && ScrollBarV->getAbsolutePosition().isPointInside( p ) && ScrollBarV->OnEvent( event ) ) ||
						( ScrollBarH && ScrollBarH->getAbsolutePosition().isPointInside( p ) &&	ScrollBarH->OnEvent( event ) )
						)
						)
					{
						return true;
					}

					Selecting = true;
					lockedEnvironment->setFocus(lockedThis);
					return true;
					break;

				case EMIE_LMOUSE_LEFT_UP:
					if(lockedEnvironment->hasFocus(lockedThis) &&
						(	( ScrollBarV && ScrollBarV->getAbsolutePosition().isPointInside( p ) && ScrollBarV->OnEvent( event ) ) ||
						( ScrollBarH && ScrollBarH->getAbsolutePosition().isPointInside( p ) &&	ScrollBarH->OnEvent( event ) )
						)
						)
					{
						return true;
					}

					Selecting = false;
					lockedEnvironment->removeFocus(lockedThis);
					mouseAction( event.MouseInput.X, event.MouseInput.Y );
					return true;
					break;

				case EMIE_MOUSE_MOVED:
					if( Selecting )
					{
						if( getAbsolutePosition().isPointInside( p ) )
						{
							mouseAction( event.MouseInput.X, event.MouseInput.Y, true );
							return true;
						}
					}
					break;
				default:
					break;
				}
			}
			break;
		default:
			break;
		}
	}

	return Parent.expired() ? false : getParent()->OnEvent(event);
}

/*!
*/
void CGUITreeView::mouseAction( s32 xpos, s32 ypos, bool onlyHover /*= false*/ )
{
	boost::shared_ptr<IGUITreeViewNode>		oldSelected = Selected;
	boost::shared_ptr<IGUITreeViewNode>		hitNode = 0;
	s32						selIdx=-1;
	s32						n;
	boost::shared_ptr<IGUITreeViewNode>		node;
	SEvent					event;

	event.EventType			= EET_GUI_EVENT;
	event.GUIEvent.Caller	= getSharedThis();
	event.GUIEvent.Element = 0;

	xpos -= AbsoluteRect.UpperLeftCorner.X;
	ypos -= AbsoluteRect.UpperLeftCorner.Y;

	// find new selected item.
	if( ItemHeight != 0 && ScrollBarV )
	{
		selIdx = ( ( ypos - 1 ) + ScrollBarV->getPos() ) / ItemHeight;
	}

	hitNode = 0;
	node = Root->getFirstChild();
	n = 0;
	while( node )
	{
		if( selIdx == n )
		{
			hitNode = node;
			break;
		}
		node = node->getNextVisible();
		++n;
	}

	if( hitNode && xpos > hitNode->getLevel() * IndentWidth )
	{
		Selected = hitNode;
	}

	boost::shared_ptr<IGUIElement> lockedParent = getParent();

	if( hitNode && !onlyHover
		&& xpos < hitNode->getLevel() * IndentWidth
		&& xpos > ( hitNode->getLevel() - 1 ) * IndentWidth
		&& hitNode->hasChildren() )
	{
		hitNode->setExpanded( !hitNode->getExpanded() );

		// post expand/collaps news
		if( hitNode->getExpanded() )
		{
			event.GUIEvent.EventType = EGET_TREEVIEW_NODE_EXPAND;
		}
		else
		{
			event.GUIEvent.EventType = EGET_TREEVIEW_NODE_COLLAPS;
		}
		LastEventNode = hitNode;
		lockedParent->OnEvent( event );
		LastEventNode = 0;
	}

	if( Selected && !Selected->isVisible() )
	{
		Selected = 0;
	}

	// post selection news

	if(lockedParent && !onlyHover && Selected != oldSelected )
	{
		if( oldSelected )
		{
			event.GUIEvent.EventType = EGET_TREEVIEW_NODE_DESELECT;
			LastEventNode = oldSelected;
			lockedParent->OnEvent( event );
			LastEventNode = 0;
		}
		if( Selected )
		{
			event.GUIEvent.EventType = EGET_TREEVIEW_NODE_SELECT;
			LastEventNode = Selected;
			lockedParent->OnEvent( event );
			LastEventNode = 0;
		}
	}
}


//! draws the element and its children
void CGUITreeView::draw()
{
	if( !IsVisible )
	{
		return;
	}

	recalculateItemHeight(); // if the font changed

	boost::shared_ptr<IGUIEnvironment> lockedEnvironment = getSharedEnvironment();
	boost::shared_ptr<IGUISkin> skin = lockedEnvironment->getSkin();
	boost::shared_ptr<irr::video::IVideoDriver> driver = lockedEnvironment->getVideoDriver();

	core::rect<s32>* clipRect = 0;
	if( Clip )
	{
		clipRect = &AbsoluteClippingRect;
	}

	// draw background
	core::rect<s32> frameRect( AbsoluteRect );

	if( DrawBack )
	{
		driver->draw2DRectangle( skin->getColor( EGDC_3D_HIGH_LIGHT ), frameRect,
			clipRect );
	}

	// draw the border
	frameRect.LowerRightCorner.Y = frameRect.UpperLeftCorner.Y + 1;
	driver->draw2DRectangle( skin->getColor( EGDC_3D_SHADOW ), frameRect,
		clipRect );

	frameRect.LowerRightCorner.Y = AbsoluteRect.LowerRightCorner.Y;
	frameRect.LowerRightCorner.X = frameRect.UpperLeftCorner.X + 1;
	driver->draw2DRectangle( skin->getColor( EGDC_3D_SHADOW ), frameRect,
		clipRect );

	frameRect = AbsoluteRect;
	frameRect.UpperLeftCorner.X = frameRect.LowerRightCorner.X - 1;
	driver->draw2DRectangle( skin->getColor( EGDC_3D_HIGH_LIGHT ), frameRect,
		clipRect );

	frameRect = AbsoluteRect;
	frameRect.UpperLeftCorner.Y = AbsoluteRect.LowerRightCorner.Y - 1;
	frameRect.LowerRightCorner.Y = AbsoluteRect.LowerRightCorner.Y;
	driver->draw2DRectangle( skin->getColor( EGDC_3D_HIGH_LIGHT ), frameRect,
		clipRect );


	// draw items

	core::rect<s32> clientClip( AbsoluteRect );
	clientClip.UpperLeftCorner.Y += 1;
	clientClip.UpperLeftCorner.X += 1;
	clientClip.LowerRightCorner.X = AbsoluteRect.LowerRightCorner.X;
	clientClip.LowerRightCorner.Y -= 1;

	if ( ScrollBarV )
		clientClip.LowerRightCorner.X -= skin->getSize( EGDS_SCROLLBAR_SIZE );
	if ( ScrollBarH )
		clientClip.LowerRightCorner.Y -= skin->getSize( EGDS_SCROLLBAR_SIZE );

	if( clipRect )
	{
		clientClip.clipAgainst( *clipRect );
	}

	frameRect = AbsoluteRect;
	frameRect.LowerRightCorner.X = AbsoluteRect.LowerRightCorner.X - skin->getSize( EGDS_SCROLLBAR_SIZE );
	frameRect.LowerRightCorner.Y = AbsoluteRect.UpperLeftCorner.Y + ItemHeight;

	if ( ScrollBarV )
	{
		frameRect.UpperLeftCorner.Y -= ScrollBarV->getPos();
		frameRect.LowerRightCorner.Y -= ScrollBarV->getPos();
	}

	if ( ScrollBarH )
	{
		frameRect.UpperLeftCorner.X -= ScrollBarH->getPos();
		frameRect.LowerRightCorner.X -= ScrollBarH->getPos();
	}

	boost::shared_ptr<IGUITreeViewNode> node = Root->getFirstChild();
	while( node )
	{
		frameRect.UpperLeftCorner.X = AbsoluteRect.UpperLeftCorner.X + 1 + node->getLevel() * IndentWidth;

		if( frameRect.LowerRightCorner.Y >= AbsoluteRect.UpperLeftCorner.Y
			&& frameRect.UpperLeftCorner.Y <= AbsoluteRect.LowerRightCorner.Y )
		{
			if( node == Selected )
			{
				driver->draw2DRectangle( skin->getColor( EGDC_HIGH_LIGHT ), frameRect, &clientClip );
			}

			if( node->hasChildren() )
			{
				core::rect<s32> rc;
				core::rect<s32> expanderRect;

				expanderRect.UpperLeftCorner.X = frameRect.UpperLeftCorner.X - IndentWidth + 2;
				expanderRect.UpperLeftCorner.Y = frameRect.UpperLeftCorner.Y + ( ( frameRect.getHeight() - ( IndentWidth - 4 ) ) >> 1 );
				expanderRect.LowerRightCorner.X = expanderRect.UpperLeftCorner.X + IndentWidth - 4;
				expanderRect.LowerRightCorner.Y = expanderRect.UpperLeftCorner.Y + IndentWidth - 4;

				// box upper line
				rc.UpperLeftCorner.X = expanderRect.UpperLeftCorner.X;
				rc.UpperLeftCorner.Y = expanderRect.UpperLeftCorner.Y;
				rc.LowerRightCorner.X = expanderRect.LowerRightCorner.X;
				rc.LowerRightCorner.Y = rc.UpperLeftCorner.Y + 1;
				driver->draw2DRectangle( skin->getColor( EGDC_3D_SHADOW ), rc,
					clipRect );

				// box left line
				rc.UpperLeftCorner.X = expanderRect.UpperLeftCorner.X;
				rc.UpperLeftCorner.Y = expanderRect.UpperLeftCorner.Y;
				rc.LowerRightCorner.X = rc.UpperLeftCorner.X + 1;
				rc.LowerRightCorner.Y = expanderRect.LowerRightCorner.Y;
				driver->draw2DRectangle( skin->getColor( EGDC_3D_SHADOW ), rc,
					clipRect );

				// box right line
				rc.UpperLeftCorner.X = expanderRect.LowerRightCorner.X - 1;
				rc.UpperLeftCorner.Y = expanderRect.UpperLeftCorner.Y;
				rc.LowerRightCorner.X = rc.UpperLeftCorner.X + 1;
				rc.LowerRightCorner.Y = expanderRect.LowerRightCorner.Y;
				driver->draw2DRectangle( skin->getColor( EGDC_3D_SHADOW ), rc,
					clipRect );

				// box bottom line
				rc.UpperLeftCorner.X = expanderRect.UpperLeftCorner.X;
				rc.UpperLeftCorner.Y = expanderRect.LowerRightCorner.Y - 1;
				rc.LowerRightCorner.X = expanderRect.LowerRightCorner.X;
				rc.LowerRightCorner.Y = rc.UpperLeftCorner.Y + 1;
				driver->draw2DRectangle( skin->getColor( EGDC_3D_SHADOW ), rc,
					clipRect );

				// horizontal '-' line
				rc.UpperLeftCorner.X = expanderRect.UpperLeftCorner.X + 2;
				rc.UpperLeftCorner.Y = expanderRect.UpperLeftCorner.Y + ( expanderRect.getHeight() >> 1 );
				rc.LowerRightCorner.X = rc.UpperLeftCorner.X + expanderRect.getWidth() - 4;
				rc.LowerRightCorner.Y = rc.UpperLeftCorner.Y + 1;
				driver->draw2DRectangle( skin->getColor( EGDC_BUTTON_TEXT ), rc,
					clipRect );

				if( !node->getExpanded() )
				{
					// vertical '+' line
					rc.UpperLeftCorner.X = expanderRect.UpperLeftCorner.X + ( expanderRect.getWidth() >> 1 );
					rc.UpperLeftCorner.Y = expanderRect.UpperLeftCorner.Y + 2;
					rc.LowerRightCorner.X = rc.UpperLeftCorner.X + 1;
					rc.LowerRightCorner.Y = rc.UpperLeftCorner.Y + expanderRect.getHeight() - 4;
					driver->draw2DRectangle( skin->getColor( EGDC_BUTTON_TEXT ), rc,
						clipRect );
				}
			}

			core::rect<s32> textRect = frameRect;

			if( Font )
			{
				EGUI_DEFAULT_COLOR textCol = EGDC_GRAY_TEXT;
				if ( isEnabled() )
					textCol = ( node == Selected ) ? EGDC_HIGH_LIGHT_TEXT : EGDC_BUTTON_TEXT;

				s32 iconWidth = 0;
				for( s32 n = 0; n < 2; ++n )
				{
					s32 index = node->getImageIndex();
					if( ( ImageList && index >= 0 )
						&& ( ( ImageLeftOfIcon && n == 0 )
						|| ( !ImageLeftOfIcon && n == 1 ) ) )
					{
						index = node->getSelectedImageIndex();
						if( node != Selected || index < 0 )
						{
							index = node->getImageIndex();
						}
						ImageList->draw(
							index,
							core::position2d<s32>(
							textRect.UpperLeftCorner.X,
							textRect.UpperLeftCorner.Y + ( ( textRect.getHeight() - ImageList->getImageSize().Height ) >> 1 ) ),
							&clientClip );
						iconWidth += ImageList->getImageSize().Width + 3;
						textRect.UpperLeftCorner.X += ImageList->getImageSize().Width + 3;
					}
					else if( ( IconFont && boost::reinterpret_pointer_cast<CGUITreeViewNode>(node)->Icon.size() )
						&& ( ( ImageLeftOfIcon && n == 1 )
						|| ( !ImageLeftOfIcon && n == 0 ) ) )
					{
						IconFont->draw( node->getIcon(), textRect, skin->getColor(textCol), false, true, &clientClip );
						iconWidth += IconFont->getDimension( node->getIcon() ).Width + 3;
						textRect.UpperLeftCorner.X += IconFont->getDimension( node->getIcon() ).Width + 3;
					}
				}

				Font->draw( node->getText(), textRect, skin->getColor(textCol), false, true, &clientClip );

				textRect.UpperLeftCorner.X -= iconWidth;
			}

			// draw the lines if neccessary
			if( LinesVisible )
			{
				core::rect<s32> rc;

				// horizontal line
				rc.UpperLeftCorner.X = frameRect.UpperLeftCorner.X - IndentWidth - ( IndentWidth >> 1 ) - 1;
				rc.UpperLeftCorner.Y = frameRect.UpperLeftCorner.Y + ( ( frameRect.getHeight() ) >> 1 );
				if( node->hasChildren() )
				{
					rc.LowerRightCorner.X = frameRect.UpperLeftCorner.X - IndentWidth;
				}
				else
				{
					rc.LowerRightCorner.X = frameRect.UpperLeftCorner.X - 2;
				}
				rc.LowerRightCorner.Y = rc.UpperLeftCorner.Y + 1;
				driver->draw2DRectangle( skin->getColor( EGDC_3D_SHADOW ), rc,
					clipRect );

				if( node->getParent() != Root )
				{
					// vertical line
					if( node == node->getParent()->getFirstChild() )
					{
						rc.UpperLeftCorner.Y = frameRect.UpperLeftCorner.Y - ( ( frameRect.getHeight() - IndentWidth ) >> 1 );
					}
					else
					{
						rc.UpperLeftCorner.Y = frameRect.UpperLeftCorner.Y - ( frameRect.getHeight() >> 1 );
					}
					rc.LowerRightCorner.X = rc.UpperLeftCorner.X + 1;
					driver->draw2DRectangle( skin->getColor( EGDC_3D_SHADOW ), rc,
						clipRect );

					// the vertical lines of all parents
					boost::shared_ptr<IGUITreeViewNode> nodeTmp = node->getParent();
					rc.UpperLeftCorner.Y = frameRect.UpperLeftCorner.Y - ( frameRect.getHeight() >> 1 );
					for( s32 n = 0; n < node->getLevel() - 2; ++n )
					{
						rc.UpperLeftCorner.X -= IndentWidth;
						rc.LowerRightCorner.X -= IndentWidth;
						if( nodeTmp != nodeTmp->getParent()->getLastChild() )
						{
							driver->draw2DRectangle( skin->getColor( EGDC_3D_SHADOW ), rc,
								clipRect );
						}
						nodeTmp = nodeTmp->getParent();
					}
				}
			}
		}

		frameRect.UpperLeftCorner.Y += ItemHeight;
		frameRect.LowerRightCorner.Y += ItemHeight;

		node = node->getNextVisible();
	}

	IGUIElement::draw();
}

//! Sets the font which should be used as icon font. This font is set to the Irrlicht engine
//! built-in-font by default. Icons can be displayed in front of every list item.
//! An icon is a string, displayed with the icon font. When using the build-in-font of the
//! Irrlicht engine as icon font, the icon strings defined in GUIIcons.h can be used.
void CGUITreeView::setIconFont( boost::shared_ptr<IGUIFont> font )
{
	s32	height;

	IconFont = font;
	if( IconFont )
	{
		height = IconFont->getDimension( L" " ).Height;
		if( height > ItemHeight )
		{
			ItemHeight = height;
		}
	}
}

//! Sets the image list which should be used for the image and selected image of every node.
//! The default is 0 (no images).
void CGUITreeView::setImageList( boost::shared_ptr<IGUIImageList> imageList )
{
	ImageList = imageList;
	if( ImageList )
	{
		if( ImageList->getImageSize().Height + 1 > ItemHeight )
		{
			ItemHeight = ImageList->getImageSize().Height + 1;
		}
	}
}

} // end namespace gui
} // end namespace irr


#endif // _IRR_COMPILE_WITH_GUI_