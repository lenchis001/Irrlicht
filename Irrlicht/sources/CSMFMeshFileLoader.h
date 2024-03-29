// Copyright (C) 2010-2012 Gaz Davidson
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_SMF_MESH_LOADER_H_INCLUDED__
#define __C_SMF_MESH_LOADER_H_INCLUDED__

#include "IMeshLoader.h"
#include "SMesh.h"
#include "VideoDriverAwareMixin.h"

namespace irr
{

namespace video
{
	class IVideoDriver;
}

namespace scene
{

//! Class which can load
class CSMFMeshFileLoader : public virtual IMeshLoader, public video::VideoDriverAwareMixin<>
{
public:

	CSMFMeshFileLoader(boost::shared_ptr<video::IVideoDriver> driver);

	//! Returns true if the file might be loaded by this class.
	virtual bool isALoadableFileExtension(const io::path& filename) const;

	//! Creates/loads an animated mesh from the file.
	virtual boost::shared_ptr<IAnimatedMesh> createMesh(boost::shared_ptr<io::IReadFile> file);
private:

	void loadLimb(boost::shared_ptr<io::IReadFile> file, boost::shared_ptr<scene::SMesh> mesh, const core::matrix4 &parentTransformation);
};

} // end namespace scene

namespace io
{
	class BinaryFile
	{
	public:
		//! reads most types from the given file, moving the file pointer along
		template <class T>
		static void read(boost::shared_ptr<io::IReadFile> file, T &out, bool bigEndian=false);

		//! reads a 3d vector from the file, moving the file pointer along
		static void read(boost::shared_ptr<io::IReadFile> file, core::vector3df &outVector2d, bool bigEndian=false);

		//! reads a 2d vector from the file, moving the file pointer along
		static void read(boost::shared_ptr<io::IReadFile> file, core::vector2df &outVector2d, bool bigEndian=false);

		//! reads a null terminated string from the file, moving the file pointer along
		static void read(boost::shared_ptr<io::IReadFile> file, core::stringc &outString, bool bigEndian=false);

	};
}

} // end namespace irr

#endif // __C_SMF_MESH_LOADER_H_INCLUDED__

