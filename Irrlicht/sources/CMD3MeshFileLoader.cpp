// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_MD3_LOADER_

#include "CMD3MeshFileLoader.h"
#include "CAnimatedMeshMD3.h"
#include "irrString.h"

namespace irr
{
namespace scene
{

//! Constructor
CMD3MeshFileLoader::CMD3MeshFileLoader( boost::shared_ptr<scene::ISceneManager> smgr)
: SceneManagerAwareMixin(smgr)
{
}


//! destructor
CMD3MeshFileLoader::~CMD3MeshFileLoader()
{
}


//! returns true if the file maybe is able to be loaded by this class
//! based on the file extension (e.g. ".bsp")
bool CMD3MeshFileLoader::isALoadableFileExtension(const io::path& filename) const
{
	return core::hasFileExtension ( filename, "md3" );
}


boost::shared_ptr<IAnimatedMesh> CMD3MeshFileLoader::createMesh(boost::shared_ptr<io::IReadFile> file)
{
	boost::shared_ptr<CAnimatedMeshMD3> mesh = boost::make_shared<CAnimatedMeshMD3>();
	boost::shared_ptr<ISceneManager> lockedSceneManager = getSceneManager();

	if ( mesh->loadModelFile ( 0, file, lockedSceneManager->getFileSystem(), lockedSceneManager->getVideoDriver()))
		return mesh;

	return nullptr;
}


} // end namespace scene
} // end namespace irr

#endif // _IRR_COMPILE_WITH_MD3_LOADER_
