// Copyright (C) 2002-2012 Nikolaus Gebhardt / Thomas Alten
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_QUAKE3_SCENE_NODE_H_INCLUDED__
#define __C_QUAKE3_SCENE_NODE_H_INCLUDED__

#include "IMeshSceneNode.h"
#include "IQ3Shader.h"
#include "IFileSystem.h"
#include "SMeshBuffer.h"
#include "SMeshBufferLightMap.h"
#include "SMesh.h"
#include "ISceneManager.h"

namespace irr
{
namespace scene
{

//! Scene node which is a quake3 shader.
class CQuake3ShaderSceneNode : public scene::IMeshSceneNode
{
public:

	CQuake3ShaderSceneNode( boost::shared_ptr<ISceneNode> parent, boost::shared_ptr<scene::ISceneManager> mgr, s32 id,
				boost::shared_ptr<io::IFileSystem> fileSystem,
				const IMeshBuffer* original,
				const boost::shared_ptr<quake3::IShader> shader
			);

	virtual ~CQuake3ShaderSceneNode();

	virtual void OnRegisterSceneNode();
	virtual void render();
	virtual void OnAnimate(u32 timeMs);
	virtual const core::aabbox3d<f32>& getBoundingBox() const;

	virtual u32 getMaterialCount() const;
	virtual video::SMaterial& getMaterial(u32 i);

	//! Returns type of the scene node
	virtual ESCENE_NODE_TYPE getType() const { return ESNT_Q3SHADER_SCENE_NODE; }

	virtual void setMesh(boost::shared_ptr<IMesh> mesh){}
	virtual boost::shared_ptr<IMesh> getMesh() { return Mesh; }
	virtual void setReadOnlyMaterials(bool readonly) {}
	virtual bool isReadOnlyMaterials() const { return true; }

	//! Creates shadow volume scene node as child of this node
	//! and returns a pointer to it.
	virtual boost::shared_ptr<IShadowVolumeSceneNode> addShadowVolumeSceneNode(boost::shared_ptr<const IMesh> shadowMesh,
		s32 id, bool zfailmethod=true, f32 infinity=10000.0f);

	//! Removes a child from this scene node.
	//! Implemented here, to be able to remove the shadow properly, if there is one,
	//! or to remove attached childs.
	virtual bool removeChild(boost::shared_ptr<ISceneNode> child);

private:
	const boost::shared_ptr<quake3::IShader> Shader;
	boost::shared_ptr<SMesh> Mesh;
	boost::shared_ptr<IShadowVolumeSceneNode> Shadow;
	const SMeshBufferLightMap* Original;
	SMeshBuffer* MeshBuffer;
	core::vector3df MeshOffset;

	struct SQ3Texture
	{
		SQ3Texture () :
			TextureIndex ( 0 ),
			TextureFrequency(0.f),
			TextureAddressMode( video::ETC_REPEAT )
			{
				Texture.setAllocStrategy ( core::ALLOC_STRATEGY_SAFE );
			}

		quake3::tTexArray Texture;

		u32 TextureIndex;
		f32 TextureFrequency;
		video::E_TEXTURE_CLAMP TextureAddressMode;	// Wrapping/Clamping
	};

	core::array< SQ3Texture > Q3Texture;

	void loadTextures ( boost::shared_ptr<io::IFileSystem>  fileSystem );
	void addBuffer ( scene::SMeshBufferLightMap * buffer );
	void cloneBuffer ( scene::SMeshBuffer *dest, const scene::SMeshBufferLightMap * buffer, bool translateCenter );

	void deformvertexes_wave ( f32 dt, quake3::SModifierFunction &function );
	void deformvertexes_move ( f32 dt, quake3::SModifierFunction &function );
	void deformvertexes_bulge( f32 dt, quake3::SModifierFunction &function );
	void deformvertexes_autosprite( f32 dt, quake3::SModifierFunction &function );
	void deformvertexes_autosprite2( f32 dt, quake3::SModifierFunction &function );
	void deformvertexes_normal ( f32 dt, quake3::SModifierFunction &function );

	void vertextransform_tcgen ( f32 dt, quake3::SModifierFunction &function );
	void vertextransform_rgbgen ( f32 dt, quake3::SModifierFunction &function );
	void vertextransform_alphagen ( f32 dt, quake3::SModifierFunction &function );

	void transformtex ( const core::matrix4 &m, const u32 clamp );

	f32 TimeAbs;

	void animate( u32 stage, core::matrix4 &texture );

	E_SCENE_NODE_RENDER_PASS getRenderStage() const;

};


} // end namespace scene
} // end namespace irr


#endif

