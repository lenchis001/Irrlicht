// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "CEmptySceneNode.h"
#include "ISceneManager.h"

namespace irr
{
namespace scene
{

//! constructor
CEmptySceneNode::CEmptySceneNode(boost::shared_ptr<ISceneNode> parent, boost::shared_ptr<scene::ISceneManager> mgr, s32 id)
: ISceneNode(parent, mgr, id)
{
	#ifdef _DEBUG
	setDebugName("CEmptySceneNode");
	#endif

	setAutomaticCulling(scene::EAC_OFF);
}


//! pre render event
void CEmptySceneNode::OnRegisterSceneNode()
{
	if (IsVisible)
		getSceneManager()->registerNodeForRendering(getSharedThis());

	ISceneNode::OnRegisterSceneNode();
}


//! render
void CEmptySceneNode::render()
{
	// do nothing
}


//! returns the axis aligned bounding box of this node
const core::aabbox3d<f32>& CEmptySceneNode::getBoundingBox() const
{
	return Box;
}


//! Creates a clone of this scene node and its children.
boost::shared_ptr<ISceneNode> CEmptySceneNode::clone(boost::shared_ptr<ISceneNode> newParent, boost::shared_ptr<scene::ISceneManager> newManager)
{
	if (!newParent)
		newParent = Parent.lock();
	if (!newManager)
		newManager = getSceneManager();

	boost::shared_ptr<CEmptySceneNode> nb = boost::make_shared<CEmptySceneNode>(newParent,
		newManager, ID);
	nb->setWeakThis(nb);

	nb->cloneMembers(getSharedThis(), newManager);
	nb->Box = Box;

	return nb;
}


} // end namespace scene
} // end namespace irr
