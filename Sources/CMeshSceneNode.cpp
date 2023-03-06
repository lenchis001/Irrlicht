// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "CMeshSceneNode.h"
#include "IVideoDriver.h"
#include "ISceneManager.h"
#include "S3DVertex.h"
#include "ICameraSceneNode.h"
#include "IMeshCache.h"
#include "IAnimatedMesh.h"
#include "IMaterialRenderer.h"
#include "IFileSystem.h"
#include "CShadowVolumeSceneNode.h"

namespace irr
{
namespace scene
{



//! constructor
CMeshSceneNode::CMeshSceneNode(boost::shared_ptr<IMesh> mesh, boost::shared_ptr<ISceneNode> parent, boost::shared_ptr<scene::ISceneManager> mgr, s32 id,
			const core::vector3df& position, const core::vector3df& rotation,
			const core::vector3df& scale)
: IMeshSceneNode(parent, mgr, id, position, rotation, scale), Mesh(0), Shadow(0),
	PassCount(0), ReadOnlyMaterials(false)
{
	#ifdef _DEBUG
	setDebugName("CMeshSceneNode");
	#endif

	setMesh(mesh);
}


//! destructor
CMeshSceneNode::~CMeshSceneNode()
{
}


//! frame
void CMeshSceneNode::OnRegisterSceneNode()
{
	if (IsVisible)
	{
		// because this node supports rendering of mixed mode meshes consisting of
		// transparent and solid material at the same time, we need to go through all
		// materials, check of what type they are and register this node for the right
		// render pass according to that.
		boost::shared_ptr<scene::ISceneManager> lockedSceneManager = getSceneManager();
		boost::shared_ptr<video::IVideoDriver> lockedDriver = lockedSceneManager->getVideoDriver();

		PassCount = 0;
		int transparentCount = 0;
		int solidCount = 0;

		// count transparent and solid materials in this scene node
		if (ReadOnlyMaterials && Mesh)
		{
			// count mesh materials

			for (u32 i=0; i<Mesh->getMeshBufferCount(); ++i)
			{
				scene::IMeshBuffer* mb = Mesh->getMeshBuffer(i);
				video::IMaterialRenderer* rnd = mb ? lockedDriver->getMaterialRenderer(mb->getMaterial().MaterialType) : 0;

				if (rnd && rnd->isTransparent())
					++transparentCount;
				else
					++solidCount;

				if (solidCount && transparentCount)
					break;
			}
		}
		else
		{
			// count copied materials

			for (u32 i=0; i<Materials.size(); ++i)
			{
				video::IMaterialRenderer* rnd =
					lockedDriver->getMaterialRenderer(Materials[i].MaterialType);

				if (rnd && rnd->isTransparent())
					++transparentCount;
				else
					++solidCount;

				if (solidCount && transparentCount)
					break;
			}
		}

		// register according to material types counted

		if (solidCount)
			lockedSceneManager->registerNodeForRendering(getSharedThis(), scene::ESNRP_SOLID);

		if (transparentCount)
			lockedSceneManager->registerNodeForRendering(getSharedThis(), scene::ESNRP_TRANSPARENT);

		ISceneNode::OnRegisterSceneNode();
	}
}


//! renders the node.
void CMeshSceneNode::render()
{
	boost::shared_ptr<scene::ISceneManager> lockedSceneManager = getSceneManager();
	boost::shared_ptr<video::IVideoDriver> driver = lockedSceneManager->getVideoDriver();

	if (!Mesh || !driver)
		return;

	bool isTransparentPass =
		lockedSceneManager->getSceneNodeRenderPass() == scene::ESNRP_TRANSPARENT;

	++PassCount;

	driver->setTransform(video::ETS_WORLD, AbsoluteTransformation);
	Box = Mesh->getBoundingBox();

	if (Shadow && PassCount==1)
		Shadow->updateShadowVolumes();

	// for debug purposes only:

	bool renderMeshes = true;
	video::SMaterial mat;
	if (DebugDataVisible && PassCount==1)
	{
		// overwrite half transparency
		if (DebugDataVisible & scene::EDS_HALF_TRANSPARENCY)
		{
			for (u32 g=0; g<Mesh->getMeshBufferCount(); ++g)
			{
				mat = Materials[g];
				mat.MaterialType = video::EMT_TRANSPARENT_ADD_COLOR;
				driver->setMaterial(mat);
				driver->drawMeshBuffer(Mesh->getMeshBuffer(g));
			}
			renderMeshes = false;
		}
	}

	// render original meshes
	if (renderMeshes)
	{
		for (u32 i=0; i<Mesh->getMeshBufferCount(); ++i)
		{
			scene::IMeshBuffer* mb = Mesh->getMeshBuffer(i);
			if (mb)
			{
				const video::SMaterial& material = ReadOnlyMaterials ? mb->getMaterial() : Materials[i];

				video::IMaterialRenderer* rnd = driver->getMaterialRenderer(material.MaterialType);
				bool transparent = (rnd && rnd->isTransparent());

				// only render transparent buffer if this is the transparent render pass
				// and solid only in solid pass
				if (transparent == isTransparentPass)
				{
					driver->setMaterial(material);
					driver->drawMeshBuffer(mb);
				}
			}
		}
	}

	driver->setTransform(video::ETS_WORLD, AbsoluteTransformation);

	// for debug purposes only:
	if (DebugDataVisible && PassCount==1)
	{
		video::SMaterial m;
		m.Lighting = false;
		m.AntiAliasing=0;
		driver->setMaterial(m);

		if (DebugDataVisible & scene::EDS_BBOX)
		{
			driver->draw3DBox(Box, video::SColor(255,255,255,255));
		}
		if (DebugDataVisible & scene::EDS_BBOX_BUFFERS)
		{
			for (u32 g=0; g<Mesh->getMeshBufferCount(); ++g)
			{
				driver->draw3DBox(
					Mesh->getMeshBuffer(g)->getBoundingBox(),
					video::SColor(255,190,128,128));
			}
		}

		if (DebugDataVisible & scene::EDS_NORMALS)
		{
			// draw normals
			const f32 debugNormalLength = lockedSceneManager->getParameters()->getAttributeAsFloat(DEBUG_NORMAL_LENGTH);
			const video::SColor debugNormalColor = lockedSceneManager->getParameters()->getAttributeAsColor(DEBUG_NORMAL_COLOR);
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

			for (u32 g=0; g<Mesh->getMeshBufferCount(); ++g)
			{
				driver->drawMeshBuffer(Mesh->getMeshBuffer(g));
			}
		}
	}
}


//! Removes a child from this scene node.
//! Implemented here, to be able to remove the shadow properly, if there is one,
//! or to remove attached childs.
bool CMeshSceneNode::removeChild(boost::shared_ptr<ISceneNode> child)
{
	if (child && Shadow == child)
	{
		Shadow = 0;
	}

	return ISceneNode::removeChild(child);
}


//! returns the axis aligned bounding box of this node
const core::aabbox3d<f32>& CMeshSceneNode::getBoundingBox() const
{
	return Mesh ? Mesh->getBoundingBox() : Box;
}


//! returns the material based on the zero based index i. To get the amount
//! of materials used by this scene node, use getMaterialCount().
//! This function is needed for inserting the node into the scene hierarchy on a
//! optimal position for minimizing renderstate changes, but can also be used
//! to directly modify the material of a scene node.
video::SMaterial& CMeshSceneNode::getMaterial(u32 i)
{
	if (Mesh && ReadOnlyMaterials && i<Mesh->getMeshBufferCount())
	{
		ReadOnlyMaterial = Mesh->getMeshBuffer(i)->getMaterial();
		return ReadOnlyMaterial;
	}

	if (i >= Materials.size())
		return ISceneNode::getMaterial(i);

	return Materials[i];
}


//! returns amount of materials used by this scene node.
u32 CMeshSceneNode::getMaterialCount() const
{
	if (Mesh && ReadOnlyMaterials)
		return Mesh->getMeshBufferCount();

	return Materials.size();
}


//! Sets a new mesh
void CMeshSceneNode::setMesh(boost::shared_ptr<IMesh> mesh)
{
	if (mesh)
	{
		Mesh = mesh;
		copyMaterials();
	}
}


//! Creates shadow volume scene node as child of this node
//! and returns a pointer to it.
boost::shared_ptr<IShadowVolumeSceneNode> CMeshSceneNode::addShadowVolumeSceneNode(
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


void CMeshSceneNode::copyMaterials()
{
	Materials.clear();

	if (Mesh)
	{
		video::SMaterial mat;

		for (u32 i=0; i<Mesh->getMeshBufferCount(); ++i)
		{
			IMeshBuffer* mb = Mesh->getMeshBuffer(i);
			if (mb)
				mat = mb->getMaterial();

			Materials.push_back(mat);
		}
	}
}


//! Writes attributes of the scene node.
void CMeshSceneNode::serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options)
{
	IMeshSceneNode::serializeAttributes(out, options);

	boost::shared_ptr<scene::ISceneManager> lockedSceneManager = getSceneManager();

	if (options && (options->Flags&io::EARWF_USE_RELATIVE_PATHS) && options->Filename)
	{
		const io::path path = lockedSceneManager->getFileSystem()->getRelativeFilename(
			lockedSceneManager->getFileSystem()->getAbsolutePath(lockedSceneManager->getMeshCache()->getMeshName(Mesh).getPath()),
				options->Filename);
		out->addString("Mesh", path.c_str());
	}
	else
		out->addString("Mesh", lockedSceneManager->getMeshCache()->getMeshName(Mesh).getPath().c_str());
	out->addBool("ReadOnlyMaterials", ReadOnlyMaterials);
}


//! Reads attributes of the scene node.
void CMeshSceneNode::deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options)
{
	boost::shared_ptr<scene::ISceneManager> lockedSceneManager = getSceneManager();

	io::path oldMeshStr = lockedSceneManager->getMeshCache()->getMeshName(Mesh);
	io::path newMeshStr = in->getAttributeAsString("Mesh");
	ReadOnlyMaterials = in->getAttributeAsBool("ReadOnlyMaterials");

	if (newMeshStr != "" && oldMeshStr != newMeshStr)
	{
		boost::shared_ptr<IMesh> newMesh = 0;
		boost::shared_ptr<IAnimatedMesh> newAnimatedMesh = lockedSceneManager->getMesh(newMeshStr.c_str());

		if (newAnimatedMesh)
			newMesh = newAnimatedMesh->getMesh(0);

		if (newMesh)
			setMesh(newMesh);
	}

	// optional attribute to assign the hint to the whole mesh
	if (in->existsAttribute("HardwareMappingHint") &&
		in->existsAttribute("HardwareMappingBufferType"))
	{
		scene::E_HARDWARE_MAPPING mapping = scene::EHM_NEVER;
		scene::E_BUFFER_TYPE bufferType = scene::EBT_NONE;

		core::stringc smapping = in->getAttributeAsString("HardwareMappingHint");
		if (smapping.equals_ignore_case("static"))
			mapping = scene::EHM_STATIC;
		else if (smapping.equals_ignore_case("dynamic"))
			mapping = scene::EHM_DYNAMIC;
		else if (smapping.equals_ignore_case("stream"))
			mapping = scene::EHM_STREAM;

		core::stringc sbufferType = in->getAttributeAsString("HardwareMappingBufferType");
		if (sbufferType.equals_ignore_case("vertex"))
			bufferType = scene::EBT_VERTEX;
		else if (sbufferType.equals_ignore_case("index"))
			bufferType = scene::EBT_INDEX;
		else if (sbufferType.equals_ignore_case("vertexindex"))
			bufferType = scene::EBT_VERTEX_AND_INDEX;

		boost::shared_ptr<IMesh> mesh = getMesh();
		if (mesh)
			mesh->setHardwareMappingHint(mapping, bufferType);
	}

	IMeshSceneNode::deserializeAttributes(in, options);
}


//! Sets if the scene node should not copy the materials of the mesh but use them in a read only style.
/* In this way it is possible to change the materials a mesh causing all mesh scene nodes
referencing this mesh to change too. */
void CMeshSceneNode::setReadOnlyMaterials(bool readonly)
{
	ReadOnlyMaterials = readonly;
}


//! Returns if the scene node should not copy the materials of the mesh but use them in a read only style
bool CMeshSceneNode::isReadOnlyMaterials() const
{
	return ReadOnlyMaterials;
}


//! Creates a clone of this scene node and its children.
boost::shared_ptr<ISceneNode> CMeshSceneNode::clone(boost::shared_ptr<ISceneNode> newParent, boost::shared_ptr<scene::ISceneManager> newManager)
{
	if (!newParent)
		newParent = Parent.lock();
	if (!newManager)
		newManager = getSceneManager();

	boost::shared_ptr<CMeshSceneNode> nb = boost::make_shared<CMeshSceneNode>(Mesh, newParent,
		newManager, ID, RelativeTranslation, RelativeRotation, RelativeScale);
	nb->setWeakThis(nb);

	nb->cloneMembers(getSharedThis(), newManager);
	nb->ReadOnlyMaterials = ReadOnlyMaterials;
	nb->Materials = Materials;
	nb->Shadow = Shadow;

	return nb;
}


} // end namespace scene
} // end namespace irr

