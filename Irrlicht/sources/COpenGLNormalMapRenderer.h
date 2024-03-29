// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_OPENGL_NORMAL_MAP_RENDERER_H_INCLUDED__
#define __C_OPENGL_NORMAL_MAP_RENDERER_H_INCLUDED__

#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_OPENGL_

#include "COpenGLShaderMaterialRenderer.h"
#include "IShaderConstantSetCallBack.h"
#include "SharedThisMixin.h"

namespace irr
{
namespace video
{

//! Class for rendering normal maps with OpenGL
class COpenGLNormalMapRenderer : public COpenGLShaderMaterialRenderer, public IShaderConstantSetCallBack, public SharedThisMixin<COpenGLNormalMapRenderer>
{
public:

	//! Constructor
	COpenGLNormalMapRenderer(boost::shared_ptr<video::COpenGLDriver> driver,
		boost::shared_ptr<IMaterialRenderer> baseMaterial);

	//! Destructor
	~COpenGLNormalMapRenderer();

	//! Called by the engine when the vertex and/or pixel shader constants for an
	//! material renderer should be set.
	virtual void OnSetConstants(IMaterialRendererServices* services, s32 userData);

	//! Returns the render capability of the material.
	virtual s32 getRenderCapability();

	virtual void setWeakPtr(boost::shared_ptr<COpenGLNormalMapRenderer> sharedThis) override;
protected:

	bool CompiledShaders;
};


} // end namespace video
} // end namespace irr

#endif
#endif

