// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_TRIANGLE_SELECTOR_H_INCLUDED__
#define __C_TRIANGLE_SELECTOR_H_INCLUDED__

#include "ITriangleSelector.h"
#include "IMesh.h"
#include "irrArray.h"
#include "aabbox3d.h"
#include "SharedThisMixin.h"

namespace irr
{
namespace scene
{

class ISceneNode;
class IAnimatedMeshSceneNode;

//! Stupid triangle selector without optimization
class CTriangleSelector : public ITriangleSelector, public SharedThisMixin<CTriangleSelector>
{
public:

	//! Constructs a selector based on a mesh
	CTriangleSelector(boost::shared_ptr<ISceneNode> node);

	//! Constructs a selector based on a mesh
	CTriangleSelector(const boost::shared_ptr<IMesh> mesh, boost::shared_ptr<ISceneNode> node);

	//! Constructs a selector based on an animated mesh scene node
	//!\param node An animated mesh scene node, which must have a valid mesh
	CTriangleSelector(boost::shared_ptr<scene::IAnimatedMeshSceneNode> node);

	//! Constructs a selector based on a bounding box
	CTriangleSelector(const core::aabbox3d<f32>& box, boost::shared_ptr<ISceneNode> node);

	//! Gets all triangles.
	void getTriangles(core::triangle3df* triangles, s32 arraySize, s32& outTriangleCount,
		const core::matrix4* transform=0) const;

	//! Gets all triangles which lie within a specific bounding box.
	void getTriangles(core::triangle3df* triangles, s32 arraySize, s32& outTriangleCount,
		const core::aabbox3d<f32>& box, const core::matrix4* transform=0) const;

	//! Gets all triangles which have or may have contact with a 3d line.
	virtual void getTriangles(core::triangle3df* triangles, s32 arraySize,
		s32& outTriangleCount, const core::line3d<f32>& line,
		const core::matrix4* transform=0) const;

	//! Returns amount of all available triangles in this selector
	virtual s32 getTriangleCount() const;

	//! Return the scene node associated with a given triangle.
	virtual boost::shared_ptr<ISceneNode> getSceneNodeForTriangle(u32 triangleIndex) const { return SceneNode; }

	// Get the number of TriangleSelectors that are part of this one
	virtual u32 getSelectorCount() const;

	// Get the TriangleSelector based on index based on getSelectorCount
	virtual boost::shared_ptr<ITriangleSelector> getSelector(u32 index);

protected:
	//! Create from a mesh
	virtual void createFromMesh(const boost::shared_ptr<IMesh> mesh);

	//! Update when the mesh has changed
	virtual void updateFromMesh(const boost::shared_ptr<IMesh> mesh) const;

	//! Update the triangle selector, which will only have an effect if it
	//! was built from an animated mesh and that mesh's frame has changed
	//! since the last time it was updated.
	virtual void update(void) const;

	boost::shared_ptr<ISceneNode> SceneNode;
	mutable core::array<core::triangle3df> Triangles; // (mutable for CTriangleBBSelector)
	mutable core::aabbox3df BoundingBox; // Allows for trivial rejection

	boost::shared_ptr<scene::IAnimatedMeshSceneNode> AnimatedNode;
	mutable u32 LastMeshFrame;
};

} // end namespace scene
} // end namespace irr


#endif

