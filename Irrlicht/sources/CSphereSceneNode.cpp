// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "CSphereSceneNode.h"
#include "IVideoDriver.h"
#include "ISceneManager.h"
#include "S3DVertex.h"
#include "os.h"
#include "CShadowVolumeSceneNode.h"

namespace irr
{
namespace scene
{

//! constructor
CSphereSceneNode::CSphereSceneNode(f32 radius, u32 polyCountX, u32 polyCountY, boost::shared_ptr<ISceneNode> parent, boost::shared_ptr<scene::ISceneManager> mgr, s32 id,
			const core::vector3df& position, const core::vector3df& rotation, const core::vector3df& scale)
: IMeshSceneNode(parent, mgr, id, position, rotation, scale), Mesh(0), Shadow(0),
	Radius(radius), PolyCountX(polyCountX), PolyCountY(polyCountY)
{
	#ifdef _DEBUG
	setDebugName("CSphereSceneNode");
	#endif

	Mesh = getSceneManager()->getGeometryCreator()->createSphereMesh(radius, polyCountX, polyCountY);
}



//! destructor
CSphereSceneNode::~CSphereSceneNode()
{
}


//! renders the node.
void CSphereSceneNode::render()
{
	boost::shared_ptr<video::IVideoDriver> driver = getSceneManager()->getVideoDriver();

	if (Mesh && driver)
	{
		driver->setMaterial(Mesh->getMeshBuffer(0)->getMaterial());
		driver->setTransform(video::ETS_WORLD, AbsoluteTransformation);
		if (Shadow)
			Shadow->updateShadowVolumes();

		driver->drawMeshBuffer(Mesh->getMeshBuffer(0));
		if ( DebugDataVisible & scene::EDS_BBOX )
		{
			video::SMaterial m;
			m.Lighting = false;
			driver->setMaterial(m);
			driver->draw3DBox(Mesh->getMeshBuffer(0)->getBoundingBox(), video::SColor(255,255,255,255));
		}
	}
}


//! Removes a child from this scene node.
//! Implemented here, to be able to remove the shadow properly, if there is one,
//! or to remove attached childs.
bool CSphereSceneNode::removeChild(boost::shared_ptr<ISceneNode> child)
{
	if (child && Shadow == child)
	{
		Shadow = 0;
	}

	return ISceneNode::removeChild(child);
}


//! Creates shadow volume scene node as child of this node
//! and returns a pointer to it.
boost::shared_ptr<IShadowVolumeSceneNode> CSphereSceneNode::addShadowVolumeSceneNode(
		boost::shared_ptr<const IMesh> shadowMesh, s32 id, bool zfailmethod, f32 infinity)
{
	boost::shared_ptr<scene::ISceneManager> lockedSceneManager = getSceneManager();
	if (!lockedSceneManager->getVideoDriver()->queryFeature(video::EVDF_STENCIL_BUFFER))
		return 0;

	if (!shadowMesh)
		shadowMesh = Mesh; // if null is given, use the mesh of node

	Shadow = boost::make_shared<CShadowVolumeSceneNode>(shadowMesh, getSharedThis(), lockedSceneManager, id, zfailmethod, infinity);
	return Shadow;
}


//! returns the axis aligned bounding box of this node
const core::aabbox3d<f32>& CSphereSceneNode::getBoundingBox() const
{
	return Mesh ? Mesh->getBoundingBox() : Box;
}


void CSphereSceneNode::OnRegisterSceneNode()
{
	if (IsVisible)
		getSceneManager()->registerNodeForRendering(getSharedThis());

	ISceneNode::OnRegisterSceneNode();
}


//! returns the material based on the zero based index i. To get the amount
//! of materials used by this scene node, use getMaterialCount().
//! This function is needed for inserting the node into the scene hirachy on a
//! optimal position for minimizing renderstate changes, but can also be used
//! to directly modify the material of a scene node.
video::SMaterial& CSphereSceneNode::getMaterial(u32 i)
{
	if (i>0 || !Mesh)
		return ISceneNode::getMaterial(i);
	else
		return Mesh->getMeshBuffer(i)->getMaterial();
}


//! returns amount of materials used by this scene node.
u32 CSphereSceneNode::getMaterialCount() const
{
	return 1;
}


//! Writes attributes of the scene node.
void CSphereSceneNode::serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options)
{
	ISceneNode::serializeAttributes(out, options);

	out->addFloat("Radius", Radius);
	out->addInt("PolyCountX", PolyCountX);
	out->addInt("PolyCountY", PolyCountY);
}


//! Reads attributes of the scene node.
void CSphereSceneNode::deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options)
{
	f32 oldRadius = Radius;
	u32 oldPolyCountX = PolyCountX;
	u32 oldPolyCountY = PolyCountY;

	Radius = in->getAttributeAsFloat("Radius");
	PolyCountX = in->getAttributeAsInt("PolyCountX");
	PolyCountY = in->getAttributeAsInt("PolyCountY");
	// legacy values read for compatibility with older versions
	u32 polyCount = in->getAttributeAsInt("PolyCount");
	if (PolyCountX ==0 && PolyCountY == 0)
		PolyCountX = PolyCountY = polyCount;

	Radius = core::max_(Radius, 0.0001f);

	if ( !core::equals(Radius, oldRadius) || PolyCountX != oldPolyCountX || PolyCountY != oldPolyCountY)
	{
		Mesh = getSceneManager()->getGeometryCreator()->createSphereMesh(Radius, PolyCountX, PolyCountY);
	}

	ISceneNode::deserializeAttributes(in, options);
}

//! Creates a clone of this scene node and its children.
boost::shared_ptr<ISceneNode> CSphereSceneNode::clone(boost::shared_ptr<ISceneNode> newParent, boost::shared_ptr<scene::ISceneManager> newManager)
{
	if (!newParent)
		newParent = Parent.lock();
	if (!newManager)
		newManager = getSceneManager();

	boost::shared_ptr<CSphereSceneNode> nb = boost::make_shared<CSphereSceneNode>(Radius, PolyCountX, PolyCountY, newParent,
		newManager, ID, RelativeTranslation);
	nb->setWeakThis(nb);

	nb->cloneMembers(getSharedThis(), newManager);
	nb->getMaterial(0) = Mesh->getMeshBuffer(0)->getMaterial();
	nb->Shadow = Shadow;

	return nb;
}

} // end namespace scene
} // end namespace irr

