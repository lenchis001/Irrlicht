// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "CDummyTransformationSceneNode.h"
#include "os.h"

namespace irr
{
namespace scene
{

//! constructor
CDummyTransformationSceneNode::CDummyTransformationSceneNode(
	boost::shared_ptr<ISceneNode> parent, boost::shared_ptr<scene::ISceneManager> mgr, s32 id)
	: IDummyTransformationSceneNode(parent, mgr, id)
{
	#ifdef _DEBUG
	setDebugName("CDummyTransformationSceneNode");
	#endif

	setAutomaticCulling(scene::EAC_OFF);
}


//! returns the axis aligned bounding box of this node
const core::aabbox3d<f32>& CDummyTransformationSceneNode::getBoundingBox() const
{
	return Box;
}


//! Returns a reference to the current relative transformation matrix.
//! This is the matrix, this scene node uses instead of scale, translation
//! and rotation.
core::matrix4& CDummyTransformationSceneNode::getRelativeTransformationMatrix()
{
	return RelativeTransformationMatrix;
}


//! Returns the relative transformation of the scene node.
core::matrix4 CDummyTransformationSceneNode::getRelativeTransformation() const
{
	return RelativeTransformationMatrix;
}

//! Creates a clone of this scene node and its children.
boost::shared_ptr<ISceneNode> CDummyTransformationSceneNode::clone(boost::shared_ptr<ISceneNode> newParent, boost::shared_ptr<scene::ISceneManager> newManager)
{
	if (!newParent)
		newParent = Parent.lock();
	if (!newManager)
		newManager = SceneManager;

	boost::shared_ptr<CDummyTransformationSceneNode> nb = boost::make_shared<CDummyTransformationSceneNode>(newParent,
		newManager, ID);
	nb->setWeakThis(nb);

	nb->cloneMembers(getSharedThis(), newManager);
	nb->RelativeTransformationMatrix = RelativeTransformationMatrix;
	nb->Box = Box;

	return nb;
}

const core::vector3df& CDummyTransformationSceneNode::getScale() const
{
	os::Printer::log("CDummyTransformationSceneNode::getScale() does not contain the relative transformation.", ELL_DEBUG);
	return RelativeScale;
}

void CDummyTransformationSceneNode::setScale(const core::vector3df& scale)
{
	os::Printer::log("CDummyTransformationSceneNode::setScale() does not affect the relative transformation.", ELL_DEBUG);
	RelativeScale = scale;
}

const core::vector3df& CDummyTransformationSceneNode::getRotation() const
{
	os::Printer::log("CDummyTransformationSceneNode::getRotation() does not contain the relative transformation.", ELL_DEBUG);
	return RelativeRotation;
}

void CDummyTransformationSceneNode::setRotation(const core::vector3df& rotation)
{
	os::Printer::log("CDummyTransformationSceneNode::setRotation() does not affect the relative transformation.", ELL_DEBUG);
	RelativeRotation = rotation;
}

const core::vector3df& CDummyTransformationSceneNode::getPosition() const
{
	os::Printer::log("CDummyTransformationSceneNode::getPosition() does not contain the relative transformation.", ELL_DEBUG);
	return RelativeTranslation;
}

void CDummyTransformationSceneNode::setPosition(const core::vector3df& newpos)
{
	os::Printer::log("CDummyTransformationSceneNode::setPosition() does not affect the relative transformation.", ELL_DEBUG);
	RelativeTranslation = newpos;
}

} // end namespace scene
} // end namespace irr
