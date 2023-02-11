// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "CCubeSceneNode.h"
#include "IVideoDriver.h"
#include "ISceneManager.h"
#include "S3DVertex.h"
#include "SMeshBuffer.h"
#include "os.h"
#include "CShadowVolumeSceneNode.h"

namespace irr
{
namespace scene
{

	/*
        011         111
          /6,8------/5        y
         /  |      / |        ^  z
        /   |     /  |        | /
    010 3,9-------2  |        |/
        |   7- - -10,4 101     *---->x
        |  /      |  /
        |/        | /
        0------11,1/
       000       100
	*/

//! constructor
CCubeSceneNode::CCubeSceneNode(f32 size, boost::shared_ptr<ISceneNode> parent, boost::shared_ptr<scene::ISceneManager> mgr,
		s32 id, const core::vector3df& position,
		const core::vector3df& rotation, const core::vector3df& scale)
	: IMeshSceneNode(parent, mgr, id, position, rotation, scale),
	Mesh(0), Shadow(0), Size(size)
{
	#ifdef _DEBUG
	setDebugName("CCubeSceneNode");
	#endif

	setSize();
}


CCubeSceneNode::~CCubeSceneNode()
{
}


void CCubeSceneNode::setSize()
{
	Mesh = SceneManager->getGeometryCreator()->createCubeMesh(core::vector3df(Size));
}


//! renders the node.
void CCubeSceneNode::render()
{
	video::IVideoDriver* driver = SceneManager->getVideoDriver();
	driver->setTransform(video::ETS_WORLD, AbsoluteTransformation);

	if (Shadow)
		Shadow->updateShadowVolumes();

	// for debug purposes only:
	video::SMaterial mat = Mesh->getMeshBuffer(0)->getMaterial();

	// overwrite half transparency
	if (DebugDataVisible & scene::EDS_HALF_TRANSPARENCY)
		mat.MaterialType = video::EMT_TRANSPARENT_ADD_COLOR;
	driver->setMaterial(mat);
	driver->drawMeshBuffer(Mesh->getMeshBuffer(0));

	// for debug purposes only:
	if (DebugDataVisible)
	{
		video::SMaterial m;
		m.Lighting = false;
		m.AntiAliasing=0;
		driver->setMaterial(m);

		if (DebugDataVisible & scene::EDS_BBOX)
		{
			driver->draw3DBox(Mesh->getMeshBuffer(0)->getBoundingBox(), video::SColor(255,255,255,255));
		}
		if (DebugDataVisible & scene::EDS_BBOX_BUFFERS)
		{
			driver->draw3DBox(Mesh->getMeshBuffer(0)->getBoundingBox(),
					video::SColor(255,190,128,128));
		}
		if (DebugDataVisible & scene::EDS_NORMALS)
		{
			// draw normals
			const f32 debugNormalLength = SceneManager->getParameters()->getAttributeAsFloat(DEBUG_NORMAL_LENGTH);
			const video::SColor debugNormalColor = SceneManager->getParameters()->getAttributeAsColor(DEBUG_NORMAL_COLOR);
			const u32 count = Mesh->getMeshBufferCount();

			for (u32 i=0; i != count; ++i)
			{
				driver->drawMeshBufferNormals(Mesh->getMeshBuffer(i), debugNormalLength, debugNormalColor);
			}
		}

		// show mesh
		if (DebugDataVisible & scene::EDS_MESH_WIRE_OVERLAY)
		{
			m.Wireframe = true;
			driver->setMaterial(m);

			driver->drawMeshBuffer(Mesh->getMeshBuffer(0));
		}
	}
}


//! returns the axis aligned bounding box of this node
const core::aabbox3d<f32>& CCubeSceneNode::getBoundingBox() const
{
	return Mesh->getMeshBuffer(0)->getBoundingBox();
}


//! Removes a child from this scene node.
//! Implemented here, to be able to remove the shadow properly, if there is one,
//! or to remove attached childs.
bool CCubeSceneNode::removeChild(boost::shared_ptr<ISceneNode> child)
{
	if (child && Shadow == child)
	{
		Shadow = 0;
	}

	return ISceneNode::removeChild(child);
}


//! Creates shadow volume scene node as child of this node
//! and returns a pointer to it.
boost::shared_ptr<IShadowVolumeSceneNode> CCubeSceneNode::addShadowVolumeSceneNode(
		boost::shared_ptr<const IMesh> shadowMesh, s32 id, bool zfailmethod, f32 infinity)
{
	if (!SceneManager->getVideoDriver()->queryFeature(video::EVDF_STENCIL_BUFFER))
		return 0;

	if (!shadowMesh)
		shadowMesh = Mesh; // if null is given, use the mesh of node

	Shadow = boost::make_shared<CShadowVolumeSceneNode>(shadowMesh, getSharedThis(), SceneManager, id, zfailmethod, infinity);
	return Shadow;
}


void CCubeSceneNode::OnRegisterSceneNode()
{
	if (IsVisible)
		SceneManager->registerNodeForRendering(getSharedThis());
	ISceneNode::OnRegisterSceneNode();
}


//! returns the material based on the zero based index i.
video::SMaterial& CCubeSceneNode::getMaterial(u32 i)
{
	return Mesh->getMeshBuffer(0)->getMaterial();
}


//! returns amount of materials used by this scene node.
u32 CCubeSceneNode::getMaterialCount() const
{
	return 1;
}


//! Writes attributes of the scene node.
void CCubeSceneNode::serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options) const
{
	ISceneNode::serializeAttributes(out, options);

	out->addFloat("Size", Size);
}


//! Reads attributes of the scene node.
void CCubeSceneNode::deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options)
{
	f32 newSize = in->getAttributeAsFloat("Size");
	newSize = core::max_(newSize, 0.0001f);
	if (newSize != Size)
	{
		Size = newSize;
		setSize();
	}

	ISceneNode::deserializeAttributes(in, options);
}


//! Creates a clone of this scene node and its children.
boost::shared_ptr<ISceneNode> CCubeSceneNode::clone(boost::shared_ptr<ISceneNode> newParent, boost::shared_ptr<scene::ISceneManager> newManager)
{
	if (!newParent)
		newParent = Parent.lock();
	if (!newManager)
		newManager = SceneManager;

	boost::shared_ptr<CCubeSceneNode> nb = boost::make_shared<CCubeSceneNode>(Size, newParent,
		newManager, ID, RelativeTranslation);
	nb->setWeakThis(nb);

	nb->cloneMembers(getSharedThis(), newManager);
	nb->getMaterial(0) = getMaterial(0);
	nb->Shadow = Shadow;

	return nb;
}


} // end namespace scene
} // end namespace irr

