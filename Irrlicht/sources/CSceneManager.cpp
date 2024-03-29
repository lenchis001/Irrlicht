// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "IrrCompileConfig.h"
#include "CSceneManager.h"
#include "IVideoDriver.h"
#include "IFileSystem.h"
#include "SAnimatedMesh.h"
#include "CMeshCache.h"
#include "IXMLWriter.h"
#include "ISceneUserDataSerializer.h"
#include "IGUIEnvironment.h"
#include "IMaterialRenderer.h"
#include "IReadFile.h"
#include "IWriteFile.h"
#include "ISceneLoader.h"

#include "os.h"

// We need this include for the case of skinned mesh support without
// any such loader
#ifdef _IRR_COMPILE_WITH_SKINNED_MESH_SUPPORT_
#include "CSkinnedMesh.h"
#endif

#ifdef _IRR_COMPILE_WITH_IRR_MESH_LOADER_
#include "CIrrMeshFileLoader.h"
#endif

#ifdef _IRR_COMPILE_WITH_BSP_LOADER_
#include "CBSPMeshFileLoader.h"
#endif

#ifdef _IRR_COMPILE_WITH_MD2_LOADER_
#include "CMD2MeshFileLoader.h"
#endif

#ifdef _IRR_COMPILE_WITH_HALFLIFE_LOADER_
#include "CAnimatedMeshHalfLife.h"
#endif

#ifdef _IRR_COMPILE_WITH_MS3D_LOADER_
#include "CMS3DMeshFileLoader.h"
#endif

#ifdef _IRR_COMPILE_WITH_3DS_LOADER_
#include "C3DSMeshFileLoader.h"
#endif

#ifdef _IRR_COMPILE_WITH_X_LOADER_
#include "CXMeshFileLoader.h"
#endif

#ifdef _IRR_COMPILE_WITH_OCT_LOADER_
#include "COCTLoader.h"
#endif

#ifdef _IRR_COMPILE_WITH_CSM_LOADER_
#include "CCSMLoader.h"
#endif

#ifdef _IRR_COMPILE_WITH_LMTS_LOADER_
#include "CLMTSMeshFileLoader.h"
#endif

#ifdef _IRR_COMPILE_WITH_MY3D_LOADER_
#include "CMY3DMeshFileLoader.h"
#endif

#ifdef _IRR_COMPILE_WITH_COLLADA_LOADER_
#include "CColladaFileLoader.h"
#endif

#ifdef _IRR_COMPILE_WITH_DMF_LOADER_
#include "CDMFLoader.h"
#endif

#ifdef _IRR_COMPILE_WITH_OGRE_LOADER_
#include "COgreMeshFileLoader.h"
#endif

#ifdef _IRR_COMPILE_WITH_OBJ_LOADER_
#include "COBJMeshFileLoader.h"
#endif

#ifdef _IRR_COMPILE_WITH_MD3_LOADER_
#include "CMD3MeshFileLoader.h"
#endif

#ifdef _IRR_COMPILE_WITH_B3D_LOADER_
#include "CB3DMeshFileLoader.h"
#endif

#ifdef _IRR_COMPILE_WITH_LWO_LOADER_
#include "CLWOMeshFileLoader.h"
#endif

#ifdef _IRR_COMPILE_WITH_STL_LOADER_
#include "CSTLMeshFileLoader.h"
#endif

#ifdef _IRR_COMPILE_WITH_PLY_LOADER_
#include "CPLYMeshFileLoader.h"
#endif

#ifdef _IRR_COMPILE_WITH_SMF_LOADER_
#include "CSMFMeshFileLoader.h"
#endif

#ifdef _IRR_COMPILE_WITH_IRR_SCENE_LOADER_
#include "CSceneLoaderIrr.h"
#endif

#ifdef _IRR_COMPILE_WITH_COLLADA_WRITER_
#include "CColladaMeshWriter.h"
#endif

#ifdef _IRR_COMPILE_WITH_IRR_WRITER_
#include "CIrrMeshWriter.h"
#endif

#ifdef _IRR_COMPILE_WITH_STL_WRITER_
#include "CSTLMeshWriter.h"
#endif

#ifdef _IRR_COMPILE_WITH_OBJ_WRITER_
#include "COBJMeshWriter.h"
#endif

#ifdef _IRR_COMPILE_WITH_PLY_WRITER_
#include "CPLYMeshWriter.h"
#endif

#include "CCubeSceneNode.h"
#include "CSphereSceneNode.h"
#include "CAnimatedMeshSceneNode.h"
#include "COctreeSceneNode.h"
#include "CCameraSceneNode.h"
#include "CLightSceneNode.h"
#include "CBillboardSceneNode.h"
#include "CMeshSceneNode.h"
#include "CSkyBoxSceneNode.h"
#include "CSkyDomeSceneNode.h"
#include "CParticleSystemSceneNode.h"
#include "CDummyTransformationSceneNode.h"
#include "CWaterSurfaceSceneNode.h"
#include "CTerrainSceneNode.h"
#include "CEmptySceneNode.h"
#include "CTextSceneNode.h"
#include "CQuake3ShaderSceneNode.h"
#include "CVolumeLightSceneNode.h"

#include "CDefaultSceneNodeFactory.h"

#include "CSceneCollisionManager.h"
#include "CTriangleSelector.h"
#include "COctreeTriangleSelector.h"
#include "CTriangleBBSelector.h"
#include "CMetaTriangleSelector.h"
#include "CTerrainTriangleSelector.h"

#include "CSceneNodeAnimatorRotation.h"
#include "CSceneNodeAnimatorFlyCircle.h"
#include "CSceneNodeAnimatorFlyStraight.h"
#include "CSceneNodeAnimatorTexture.h"
#include "CSceneNodeAnimatorCollisionResponse.h"
#include "CSceneNodeAnimatorDelete.h"
#include "CSceneNodeAnimatorFollowSpline.h"
#include "CSceneNodeAnimatorCameraFPS.h"
#include "CSceneNodeAnimatorCameraEditor.h"
#include "CSceneNodeAnimatorCameraThirdPerson.h"
#include "CSceneNodeAnimatorCameraMaya.h"
#include "CDefaultSceneNodeAnimatorFactory.h"

#include "CGeometryCreator.h"

namespace irr
{
namespace scene
{

//! constructor
CSceneManager::CSceneManager(boost::shared_ptr<video::IVideoDriver> driver, boost::shared_ptr<io::IFileSystem> fs,
		boost::shared_ptr<gui::ICursorControl> cursorControl, boost::shared_ptr<IMeshCache> cache,
		boost::shared_ptr<gui::IGUIEnvironment> gui)
	: ISceneManager(driver), ISceneNode(0, 0), FileSystem(fs), GUIEnvironment(gui),
	CursorControl(cursorControl), CollisionManager(0),
	ActiveCamera(0), ShadowColor(150,0,0,0), AmbientLight(0,0,0,0),
	MeshCache(cache), CurrentRendertime(ESNRP_NONE), LightManager(0),
	IRR_XML_FORMAT_SCENE(L"irr_scene"), IRR_XML_FORMAT_NODE(L"node"), IRR_XML_FORMAT_NODE_ATTR_TYPE(L"type")
{
	#ifdef _DEBUG
	ISceneManager::setDebugName("CSceneManager ISceneManager");
	ISceneNode::setDebugName("CSceneManager ISceneNode");
	#endif
}

void CSceneManager::setWeakThis(boost::shared_ptr<CSceneManager> value)
{
	ISceneNode::setWeakThis(value);

	// root node's scene manager
	getSceneManager() = value;

	// set scene parameters
	Parameters.setAttribute(DEBUG_NORMAL_LENGTH, 1.f);
	Parameters.setAttribute(DEBUG_NORMAL_COLOR, video::SColor(255, 34, 221, 221));

	// create mesh cache if not there already
	if (!MeshCache)
		MeshCache = boost::make_shared<CMeshCache>();

	boost::shared_ptr<video::IVideoDriver> lockedDriver = getVideoDriver();
	boost::shared_ptr<ISceneManager> lockedSceneManager = getSceneManager();

	// create collision manager
	CollisionManager = boost::make_shared<CSceneCollisionManager>(lockedSceneManager, lockedDriver);

	// create geometry creator
	GeometryCreator = boost::make_shared<CGeometryCreator>();

	// add file format loaders. add the least commonly used ones first,
	// as these are checked last

	// TODO: now that we have multiple scene managers, these should be
	// shallow copies from the previous manager if there is one.

	#ifdef _IRR_COMPILE_WITH_STL_LOADER_
		MeshLoaderList.push_back(boost::make_shared<CSTLMeshFileLoader>());
	#endif
	#ifdef _IRR_COMPILE_WITH_PLY_LOADER_
		MeshLoaderList.push_back(boost::make_shared<CPLYMeshFileLoader>(lockedSceneManager));
	#endif
	#ifdef _IRR_COMPILE_WITH_SMF_LOADER_
		MeshLoaderList.push_back(boost::make_shared<CSMFMeshFileLoader>(lockedDriver));
	#endif
	#ifdef _IRR_COMPILE_WITH_OCT_LOADER_
		MeshLoaderList.push_back(boost::make_shared<COCTLoader>(lockedSceneManager, FileSystem));
	#endif
	#ifdef _IRR_COMPILE_WITH_CSM_LOADER_
		MeshLoaderList.push_back(boost::make_shared<CCSMLoader>(lockedSceneManager, FileSystem));
	#endif
	#ifdef _IRR_COMPILE_WITH_LMTS_LOADER_
		MeshLoaderList.push_back(boost::make_shared<CLMTSMeshFileLoader>(FileSystem, lockedDriver, &Parameters));
	#endif
	#ifdef _IRR_COMPILE_WITH_MY3D_LOADER_
		MeshLoaderList.push_back(boost::make_shared<CMY3DMeshFileLoader>(lockedSceneManager, FileSystem));
	#endif
	#ifdef _IRR_COMPILE_WITH_DMF_LOADER_
		MeshLoaderList.push_back(boost::make_shared<CDMFLoader>(lockedSceneManager, FileSystem));
	#endif
	#ifdef _IRR_COMPILE_WITH_OGRE_LOADER_
		MeshLoaderList.push_back(boost::make_shared<COgreMeshFileLoader>(FileSystem, lockedDriver));
	#endif
	#ifdef _IRR_COMPILE_WITH_HALFLIFE_LOADER_
		MeshLoaderList.push_back(boost::make_shared<CHalflifeMDLMeshFileLoader>(lockedSceneManager));
	#endif
	#ifdef _IRR_COMPILE_WITH_MD3_LOADER_
		MeshLoaderList.push_back(boost::make_shared<CMD3MeshFileLoader>(lockedSceneManager));
	#endif
	#ifdef _IRR_COMPILE_WITH_LWO_LOADER_
		MeshLoaderList.push_back(boost::make_shared<CLWOMeshFileLoader>(lockedSceneManager, FileSystem));
	#endif
	#ifdef _IRR_COMPILE_WITH_MD2_LOADER_
		MeshLoaderList.push_back(boost::make_shared<CMD2MeshFileLoader>());
	#endif
	#ifdef _IRR_COMPILE_WITH_IRR_MESH_LOADER_
		MeshLoaderList.push_back(boost::make_shared<CIrrMeshFileLoader>(lockedSceneManager, FileSystem));
	#endif
	#ifdef _IRR_COMPILE_WITH_BSP_LOADER_
		MeshLoaderList.push_back(boost::make_shared<CBSPMeshFileLoader>(lockedSceneManager, FileSystem));
	#endif
	#ifdef _IRR_COMPILE_WITH_COLLADA_LOADER_
		MeshLoaderList.push_back(boost::make_shared<CColladaFileLoader>(lockedSceneManager, FileSystem));
	#endif
	#ifdef _IRR_COMPILE_WITH_3DS_LOADER_
		MeshLoaderList.push_back(boost::make_shared<C3DSMeshFileLoader>(lockedSceneManager, FileSystem));
	#endif
	#ifdef _IRR_COMPILE_WITH_X_LOADER_
		MeshLoaderList.push_back(boost::make_shared<CXMeshFileLoader>(lockedSceneManager, FileSystem));
	#endif
	#ifdef _IRR_COMPILE_WITH_MS3D_LOADER_
		MeshLoaderList.push_back(boost::make_shared<CMS3DMeshFileLoader>(lockedDriver));
	#endif
	#ifdef _IRR_COMPILE_WITH_OBJ_LOADER_
		MeshLoaderList.push_back(boost::make_shared<COBJMeshFileLoader>(lockedSceneManager, FileSystem));
	#endif
	#ifdef _IRR_COMPILE_WITH_B3D_LOADER_
		MeshLoaderList.push_back(boost::make_shared<CB3DMeshFileLoader>(lockedSceneManager));
	#endif

		// scene loaders
	#ifdef _IRR_COMPILE_WITH_IRR_SCENE_LOADER_
		SceneLoaderList.push_back(boost::make_shared<CSceneLoaderIrr>(lockedSceneManager, FileSystem));
	#endif


	// factories
	boost::shared_ptr<ISceneNodeFactory> factory = boost::make_shared<CDefaultSceneNodeFactory>(lockedSceneManager);
	registerSceneNodeFactory(factory);

	ISceneNodeAnimatorFactory* animatorFactory = new CDefaultSceneNodeAnimatorFactory(lockedSceneManager, CursorControl);
	registerSceneNodeAnimatorFactory(animatorFactory);
	animatorFactory->drop();
}


//! destructor
CSceneManager::~CSceneManager()
{
	clearDeletionList();

	//! force to remove hardwareTextures from the driver
	//! because Scenes may hold internally data bounded to sceneNodes
	//! which may be destroyed twice

	boost::shared_ptr<video::IVideoDriver> lockedDriver = getVideoDriver();

	if (lockedDriver)
		lockedDriver->removeAllHardwareBuffers();

	ActiveCamera = 0;

	for (u32 i=0; i<SceneNodeAnimatorFactoryList.size(); ++i)
		SceneNodeAnimatorFactoryList[i]->drop();

	// remove all nodes and animators before dropping the driver
	// as render targets may be destroyed twice

	removeAll();
	removeAnimators();
}


//! gets an animateable mesh. loads it if needed. returned pointer must not be dropped.
boost::shared_ptr<IAnimatedMesh> CSceneManager::getMesh(const io::path& filename)
{
	boost::shared_ptr<IAnimatedMesh> msh = MeshCache->getMeshByName(filename);
	if (msh)
		return msh;

	boost::shared_ptr<io::IReadFile> file = FileSystem->createAndOpenFile(filename);
	if (!file)
	{
		os::Printer::log("Could not load mesh, because file could not be opened: ", filename, ELL_ERROR);
		return 0;
	}

	// iterate the list in reverse order so user-added loaders can override the built-in ones
	s32 count = MeshLoaderList.size();
	for (s32 i=count-1; i>=0; --i)
	{
		if (MeshLoaderList[i]->isALoadableFileExtension(filename))
		{
			// reset file to avoid side effects of previous calls to createMesh
			file->seek(0);
			msh = MeshLoaderList[i]->createMesh(file);
			if (msh)
			{
				MeshCache->addMesh(filename, msh);
				break;
			}
		}
	}

	if (!msh)
		os::Printer::log("Could not load mesh, file format seems to be unsupported", filename, ELL_ERROR);
	else
		os::Printer::log("Loaded mesh", filename, ELL_INFORMATION);

	return msh;
}


//! gets an animateable mesh. loads it if needed. returned pointer must not be dropped.
boost::shared_ptr<IAnimatedMesh> CSceneManager::getMesh(boost::shared_ptr<io::IReadFile> file)
{
	if (!file)
		return 0;

	io::path name = file->getFileName();
	boost::shared_ptr<IAnimatedMesh> msh = MeshCache->getMeshByName(file->getFileName());
	if (msh)
		return msh;

	// iterate the list in reverse order so user-added loaders can override the built-in ones
	s32 count = MeshLoaderList.size();
	for (s32 i=count-1; i>=0; --i)
	{
		if (MeshLoaderList[i]->isALoadableFileExtension(name))
		{
			// reset file to avoid side effects of previous calls to createMesh
			file->seek(0);
			msh = MeshLoaderList[i]->createMesh(file);
			if (msh)
			{
				MeshCache->addMesh(file->getFileName(), msh);
				break;
			}
		}
	}

	if (!msh)
		os::Printer::log("Could not load mesh, file format seems to be unsupported", file->getFileName(), ELL_ERROR);
	else
		os::Printer::log("Loaded mesh", file->getFileName(), ELL_INFORMATION);

	return msh;
}


//! returns the GUI Environment
boost::shared_ptr<gui::IGUIEnvironment> CSceneManager::getGUIEnvironment()
{
	return GUIEnvironment;
}

//! Get the active FileSystem
/** \return Pointer to the FileSystem
This pointer should not be dropped. See IReferenceCounted::drop() for more information. */
boost::shared_ptr<io::IFileSystem> CSceneManager::getFileSystem()
{
	return FileSystem;
}

//! Adds a text scene node, which is able to display
//! 2d text at a position in three dimensional space
boost::shared_ptr<ITextSceneNode> CSceneManager::addTextSceneNode(boost::shared_ptr<gui::IGUIFont> font,
		const wchar_t* text, video::SColor color, boost::shared_ptr<ISceneNode> parent,
		const core::vector3df& position, s32 id)
{
	if (!font)
		return 0;

	if (!parent)
		parent = getSharedThis();

	boost::shared_ptr<ITextSceneNode> t = boost::make_shared<CTextSceneNode>(parent, getSceneManager(), id, font,
		getSceneCollisionManager(), position, text, color);
	t->setWeakThis(t);

	return t;
}


//! Adds a text scene node, which uses billboards
boost::shared_ptr<IBillboardTextSceneNode> CSceneManager::addBillboardTextSceneNode(boost::shared_ptr<gui::IGUIFont> font,
		const wchar_t* text, boost::shared_ptr<ISceneNode> parent,
		const core::dimension2d<f32>& size,
		const core::vector3df& position, s32 id,
		video::SColor colorTop, video::SColor colorBottom)
{
	if (!font && GUIEnvironment)
		font = GUIEnvironment->getBuiltInFont();

	if (!font)
		return 0;

	if (!parent)
		parent = getSharedThis();

	boost::shared_ptr<IBillboardTextSceneNode> node = boost::make_shared<CBillboardTextSceneNode>(parent, getSceneManager(), id, font, text, position, size,
		colorTop, colorBottom);
	node->setWeakThis(node);

	return node;

}


//! Adds a scene node, which can render a quake3 shader
boost::shared_ptr<IMeshSceneNode> CSceneManager::addQuake3SceneNode(const IMeshBuffer* meshBuffer,
					const boost::shared_ptr<quake3::IShader>  shader,
					boost::shared_ptr<ISceneNode> parent, s32 id )
{
#ifdef _IRR_COMPILE_WITH_BSP_LOADER_
	if (!shader)
		return 0;

	if (!parent)
		parent = getSharedThis();

	boost::shared_ptr<CQuake3ShaderSceneNode> node = boost::make_shared<CQuake3ShaderSceneNode>(parent,
		getSceneManager(), id, FileSystem,
		meshBuffer, shader );
	node->setWeakThis(node);

	return node;
#else
	return 0;
#endif
}


//! adds Volume Lighting Scene Node.
//! the returned pointer must not be dropped.
boost::shared_ptr<IVolumeLightSceneNode> CSceneManager::addVolumeLightSceneNode(
		boost::shared_ptr<ISceneNode> parent, s32 id,
		const u32 subdivU, const u32 subdivV,
		const video::SColor foot, const video::SColor tail,
		const core::vector3df& position, const core::vector3df& rotation, const core::vector3df& scale)
{
	if (!parent)
		parent = getSharedThis();

	boost::shared_ptr<IVolumeLightSceneNode> node = boost::make_shared<CVolumeLightSceneNode>(parent, getSceneManager(), id, subdivU, subdivV, foot, tail, position, rotation, scale);
	node->setWeakThis(node);

	return node;
}


//! adds a test scene node for test purposes to the scene. It is a simple cube of (1,1,1) size.
//! the returned pointer must not be dropped.
boost::shared_ptr<IMeshSceneNode> CSceneManager::addCubeSceneNode(f32 size, boost::shared_ptr<ISceneNode> parent,
		s32 id, const core::vector3df& position,
		const core::vector3df& rotation, const core::vector3df& scale)
{
	if (!parent)
		parent = getSharedThis();

	boost::shared_ptr<IMeshSceneNode> node = boost::make_shared<CCubeSceneNode>(size, parent, getSceneManager(), id, position, rotation, scale);
	node->setWeakThis(node);

	return node;
}


//! Adds a sphere scene node for test purposes to the scene.
boost::shared_ptr<IMeshSceneNode> CSceneManager::addSphereSceneNode(f32 radius, s32 polyCount,
		boost::shared_ptr<ISceneNode> parent, s32 id, const core::vector3df& position,
		const core::vector3df& rotation, const core::vector3df& scale)
{
	if (!parent)
		parent = getSharedThis();

	boost::shared_ptr<IMeshSceneNode> node = boost::make_shared<CSphereSceneNode>(radius, polyCount, polyCount, parent, getSceneManager(), id, position, rotation, scale);
	node->setWeakThis(node);

	return node;
}


//! adds a scene node for rendering a static mesh
//! the returned pointer must not be dropped.
boost::shared_ptr<IMeshSceneNode> CSceneManager::addMeshSceneNode(boost::shared_ptr<IMesh> mesh, boost::shared_ptr<ISceneNode> parent, s32 id,
	const core::vector3df& position, const core::vector3df& rotation,
	const core::vector3df& scale, bool alsoAddIfMeshPointerZero)
{
	if (!alsoAddIfMeshPointerZero && !mesh)
		return 0;

	if (!parent)
		parent = getSharedThis();

	boost::shared_ptr<IMeshSceneNode> node = boost::make_shared<CMeshSceneNode>(mesh, parent, getSceneManager(), id, position, rotation, scale);
	node->setWeakThis(node);

	return node;
}


//! Adds a scene node for rendering a animated water surface mesh.
boost::shared_ptr<ISceneNode> CSceneManager::addWaterSurfaceSceneNode(boost::shared_ptr<IMesh> mesh, f32 waveHeight, f32 waveSpeed, f32 waveLength,
	boost::shared_ptr<ISceneNode> parent, s32 id, const core::vector3df& position,
	const core::vector3df& rotation, const core::vector3df& scale)
{
	if (!parent)
		parent = getSharedThis();

	boost::shared_ptr<ISceneNode> node = boost::make_shared<CWaterSurfaceSceneNode>(waveHeight, waveSpeed, waveLength,
		mesh, parent, getSceneManager(), id, position, rotation, scale);
	node->setWeakThis(node);

	return node;
}


//! adds a scene node for rendering an animated mesh model
boost::shared_ptr<scene::IAnimatedMeshSceneNode> CSceneManager::addAnimatedMeshSceneNode(boost::shared_ptr<IAnimatedMesh> mesh, boost::shared_ptr<ISceneNode> parent, s32 id,
	const core::vector3df& position, const core::vector3df& rotation,
	const core::vector3df& scale, bool alsoAddIfMeshPointerZero)
{
	if (!alsoAddIfMeshPointerZero && !mesh)
		return 0;

	if (!parent)
		parent = getSharedThis();

	boost::shared_ptr<scene::IAnimatedMeshSceneNode> node =
		boost::make_shared<CAnimatedMeshSceneNode>(mesh, parent, getSceneManager(), id, position, rotation, scale);
	node->setWeakThis(node);

	return node;
}


//! Adds a scene node for rendering using a octree to the scene graph. This a good method for rendering
//! scenes with lots of geometry. The Octree is built on the fly from the mesh, much
//! faster then a bsp tree.
boost::shared_ptr<IMeshSceneNode> CSceneManager::addOctreeSceneNode(boost::shared_ptr<IAnimatedMesh> mesh, boost::shared_ptr<ISceneNode> parent,
			s32 id, s32 minimalPolysPerNode, bool alsoAddIfMeshPointerZero)
{
	if (!alsoAddIfMeshPointerZero && (!mesh || !mesh->getFrameCount()))
		return 0;

	return addOctreeSceneNode(mesh ? mesh->getMesh(0) : 0,
				parent, id, minimalPolysPerNode,
				alsoAddIfMeshPointerZero);
}


//! Adds a scene node for rendering using a octree. This a good method for rendering
//! scenes with lots of geometry. The Octree is built on the fly from the mesh, much
//! faster then a bsp tree.
boost::shared_ptr<IMeshSceneNode> CSceneManager::addOctreeSceneNode(boost::shared_ptr<IMesh> mesh, boost::shared_ptr<ISceneNode> parent,
		s32 id, s32 minimalPolysPerNode, bool alsoAddIfMeshPointerZero)
{
	if (!alsoAddIfMeshPointerZero && !mesh)
		return 0;

	if (!parent)
		parent = getSharedThis();

	boost::shared_ptr<COctreeSceneNode> node = boost::make_shared<COctreeSceneNode>(parent, getSceneManager(), id, minimalPolysPerNode);
	node->setWeakThis(node);

	if (node)
	{
		node->setMesh(mesh);
	}

	return node;
}


//! Adds a camera scene node to the tree and sets it as active camera.
//! \param position: Position of the space relative to its parent where the camera will be placed.
//! \param lookat: Position where the camera will look at. Also known as target.
//! \param parent: Parent scene node of the camera. Can be null. If the parent moves,
//! the camera will move too.
//! \return Returns pointer to interface to camera
boost::shared_ptr<ICameraSceneNode> CSceneManager::addCameraSceneNode(boost::shared_ptr<ISceneNode> parent,
	const core::vector3df& position, const core::vector3df& lookat, s32 id,
	bool makeActive)
{
	if (!parent)
		parent = getSharedThis();

	boost::shared_ptr<ICameraSceneNode> node = boost::make_shared<CCameraSceneNode>(parent, getSceneManager(), id, position, lookat);
	node->setWeakThis(node);

	if (makeActive)
		setActiveCamera(node);

	return node;
}


//! Adds a camera scene node which is able to be controlled with the mouse similar
//! to in the 3D Software Maya by Alias Wavefront.
//! The returned pointer must not be dropped.
boost::shared_ptr<ICameraSceneNode> CSceneManager::addCameraSceneNodeMaya(boost::shared_ptr<ISceneNode> parent,
	f32 rotateSpeed, f32 zoomSpeed, f32 translationSpeed, s32 id, f32 distance,
	bool makeActive)
{
	boost::shared_ptr<ICameraSceneNode> node = addCameraSceneNode(parent, core::vector3df(),
			core::vector3df(0,0,100), id, makeActive);
	if (node)
	{
		boost::shared_ptr<ISceneNodeAnimator> anm = boost::make_shared<CSceneNodeAnimatorCameraMaya>(CursorControl,
			rotateSpeed, zoomSpeed, translationSpeed, distance);

		node->addAnimator(anm);
	}

	return node;
}


//! Adds a camera scene node which is able to be controlled with the mouse and keys
//! like in most first person shooters (FPS):
boost::shared_ptr<ICameraSceneNode> CSceneManager::addCameraSceneNodeFPS(boost::shared_ptr<ISceneNode> parent,
	f32 rotateSpeed, f32 moveSpeed, s32 id, SKeyMap* keyMapArray,
	s32 keyMapSize, bool noVerticalMovement, f32 jumpSpeed,
	bool invertMouseY, bool makeActive)
{
	boost::shared_ptr<ICameraSceneNode> node = addCameraSceneNode(parent, core::vector3df(),
			core::vector3df(0,0,100), id, makeActive);
	if (node)
	{
		boost::shared_ptr<ISceneNodeAnimator> anm = boost::make_shared<CSceneNodeAnimatorCameraFPS>(CursorControl,
				rotateSpeed, moveSpeed, jumpSpeed,
				keyMapArray, keyMapSize, noVerticalMovement, invertMouseY);

		// Bind the node's rotation to its target. This is consistent with 1.4.2 and below.
		node->bindTargetAndRotation(true);
		node->addAnimator(anm);
	}

	return node;
}

boost::shared_ptr<ICameraSceneNode> CSceneManager::addCameraSceneNodeEditor() {
	auto camera = addCameraSceneNode();
	camera->setName(L"Internal camera");

	camera->setPosition(irr::core::vector3df(0, 50, 100));
	camera->setTarget(irr::core::vector3df(0, 0, 0));

	camera->bindTargetAndRotation(false);
	camera->addAnimator(boost::make_shared<CSceneNodeAnimatorCameraEditor>());

	return camera;
}

boost::shared_ptr<ICameraSceneNode> CSceneManager::addCameraSceneNodeThirdPerson(boost::shared_ptr<ISceneNode> parent) {
	auto camera = addCameraSceneNode(parent);
	camera->bindTargetAndRotation(false);

	camera->addAnimator(boost::make_shared<CSceneNodeAnimatorCameraThirdPerson>(CursorControl));

	return camera;
}

//! Adds a dynamic light scene node. The light will cast dynamic light on all
//! other scene nodes in the scene, which have the material flag video::MTF_LIGHTING
//! turned on. (This is the default setting in most scene nodes).
boost::shared_ptr<scene::ILightSceneNode> CSceneManager::addLightSceneNode(boost::shared_ptr<ISceneNode> parent,
	const core::vector3df& position, video::SColorf color, f32 range, s32 id)
{
	if (!parent)
		parent = getSharedThis();

	boost::shared_ptr<scene::ILightSceneNode> node = boost::make_shared<CLightSceneNode>(parent, getSceneManager(), id, position, color, range);
	node->setWeakThis(node);

	return node;
}


//! Adds a billboard scene node to the scene. A billboard is like a 3d sprite: A 2d element,
//! which always looks to the camera. It is usually used for things like explosions, fire,
//! lensflares and things like that.
boost::shared_ptr<IBillboardSceneNode> CSceneManager::addBillboardSceneNode(boost::shared_ptr<ISceneNode> parent,
	const core::dimension2d<f32>& size, const core::vector3df& position, s32 id,
	video::SColor colorTop, video::SColor colorBottom
	)
{
	if (!parent)
		parent = getSharedThis();

	boost::shared_ptr<IBillboardSceneNode> node = boost::make_shared<CBillboardSceneNode>(parent, getSceneManager(), id, position, size,
		colorTop, colorBottom);
	node->setWeakThis(node);

	return node;
}


//! Adds a skybox scene node. A skybox is a big cube with 6 textures on it and
//! is drawn around the camera position.
boost::shared_ptr<ISceneNode> CSceneManager::addSkyBoxSceneNode(boost::shared_ptr<video::ITexture> top, boost::shared_ptr<video::ITexture> bottom,
	boost::shared_ptr<video::ITexture> left, boost::shared_ptr<video::ITexture> right, boost::shared_ptr<video::ITexture> front,
	boost::shared_ptr<video::ITexture> back, boost::shared_ptr<ISceneNode> parent, s32 id)
{
	if (!parent)
		parent = getSharedThis();

	boost::shared_ptr<ISceneNode> node = boost::make_shared<CSkyBoxSceneNode>(top, bottom, left, right,
			front, back, parent, getSceneManager(), id);
	node->setWeakThis(node);

	return node;
}


//! Adds a skydome scene node. A skydome is a large (half-) sphere with a
//! panoramic texture on it and is drawn around the camera position.
boost::shared_ptr<ISceneNode> CSceneManager::addSkyDomeSceneNode(boost::shared_ptr<video::ITexture> texture,
	u32 horiRes, u32 vertRes, f32 texturePercentage,f32 spherePercentage, f32 radius,
	boost::shared_ptr<ISceneNode> parent, s32 id)
{
	if (!parent)
		parent = getSharedThis();

	boost::shared_ptr<ISceneNode> node = boost::make_shared<CSkyDomeSceneNode>(texture, horiRes, vertRes,
		texturePercentage, spherePercentage, radius, parent, getSceneManager(), id);
	node->setWeakThis(node);

	return node;
}


//! Adds a particle system scene node.
boost::shared_ptr<IParticleSystemSceneNode> CSceneManager::addParticleSystemSceneNode(
	bool withDefaultEmitter, boost::shared_ptr<ISceneNode> parent, s32 id,
	const core::vector3df& position, const core::vector3df& rotation,
	const core::vector3df& scale)
{
	if (!parent)
		parent = getSharedThis();

	boost::shared_ptr<IParticleSystemSceneNode> node = boost::make_shared<CParticleSystemSceneNode>(withDefaultEmitter,
		parent, getSceneManager(), id, position, rotation, scale);
	node->setWeakThis(node);

	return node;
}


//! Adds a terrain scene node to the scene graph.
boost::shared_ptr<ITerrainSceneNode> CSceneManager::addTerrainSceneNode(
	const io::path& heightMapFileName,
	boost::shared_ptr<ISceneNode> parent, s32 id,
	const core::vector3df& position,
	const core::vector3df& rotation,
	const core::vector3df& scale,
	video::SColor vertexColor,
	s32 maxLOD, E_TERRAIN_PATCH_SIZE patchSize, s32 smoothFactor,
	bool addAlsoIfHeightmapEmpty)
{
	boost::shared_ptr<io::IReadFile> file = FileSystem->createAndOpenFile(heightMapFileName);

	if (!file && !addAlsoIfHeightmapEmpty)
	{
		os::Printer::log("Could not load terrain, because file could not be opened.",
		heightMapFileName, ELL_ERROR);
		return 0;
	}

	boost::shared_ptr<ITerrainSceneNode> terrain = addTerrainSceneNode(file, parent, id,
		position, rotation, scale, vertexColor, maxLOD, patchSize,
		smoothFactor, addAlsoIfHeightmapEmpty);

	return terrain;
}

//! Adds a terrain scene node to the scene graph.
boost::shared_ptr<ITerrainSceneNode> CSceneManager::addTerrainSceneNode(
	boost::shared_ptr<io::IReadFile> heightMapFile,
	boost::shared_ptr<ISceneNode> parent, s32 id,
	const core::vector3df& position,
	const core::vector3df& rotation,
	const core::vector3df& scale,
	video::SColor vertexColor,
	s32 maxLOD, E_TERRAIN_PATCH_SIZE patchSize,
	s32 smoothFactor,
	bool addAlsoIfHeightmapEmpty)
{
	if (!parent)
		parent = getSharedThis();

	if (!heightMapFile && !addAlsoIfHeightmapEmpty)
	{
		os::Printer::log("Could not load terrain, because file could not be opened.", ELL_ERROR);
		return 0;
	}

	boost::shared_ptr<CTerrainSceneNode> node = boost::make_shared<CTerrainSceneNode>(parent, getSceneManager(), FileSystem, id,
		maxLOD, patchSize, position, rotation, scale);
	node->setWeakThis(node);

	if (!node->loadHeightMap(heightMapFile, vertexColor, smoothFactor))
	{
		if (!addAlsoIfHeightmapEmpty)
		{
			node->remove();
			return 0;
		}
	}

	return node;
}


//! Adds an empty scene node.
boost::shared_ptr<ISceneNode> CSceneManager::addEmptySceneNode(boost::shared_ptr<ISceneNode> parent, s32 id)
{
	if (!parent)
		parent = getSharedThis();

	boost::shared_ptr<ISceneNode> node = boost::make_shared<CEmptySceneNode>(parent, getSceneManager(), id);
	node->setWeakThis(node);

	return node;
}


//! Adds a dummy transformation scene node to the scene graph.
boost::shared_ptr<IDummyTransformationSceneNode> CSceneManager::addDummyTransformationSceneNode(
	boost::shared_ptr<ISceneNode> parent, s32 id)
{
	if (!parent)
		parent = getSharedThis();

	boost::shared_ptr<IDummyTransformationSceneNode> node = boost::make_shared<CDummyTransformationSceneNode>(
		parent, getSceneManager(), id);
	node->setWeakThis(node);

	return node;
}

//! Adds a Hill Plane mesh to the mesh pool. The mesh is generated on the fly
//! and looks like a plane with some hills on it. You can specify how many hills
//! there should be on the plane and how high they should be. Also you must
//! specify a name for the mesh, because the mesh is added to the mesh pool,
//! and can be retrieved again using ISceneManager::getMesh with the name as
//! parameter.
boost::shared_ptr<IAnimatedMesh> CSceneManager::addHillPlaneMesh(const io::path& name,
		const core::dimension2d<f32>& tileSize,
		const core::dimension2d<u32>& tileCount,
		video::SMaterial* material, f32 hillHeight,
		const core::dimension2d<f32>& countHills,
		const core::dimension2d<f32>& textureRepeatCount)
{
	if (MeshCache->isMeshLoaded(name))
		return MeshCache->getMeshByName(name);

	boost::shared_ptr<IMesh> mesh = GeometryCreator->createHillPlaneMesh(tileSize,
			tileCount, material, hillHeight, countHills,
			textureRepeatCount);
	if (!mesh)
		return 0;

	boost::shared_ptr<SAnimatedMesh> animatedMesh = boost::make_shared<SAnimatedMesh>();
	if (!animatedMesh)
	{
		return 0;
	}

	animatedMesh->addMesh(mesh);
	animatedMesh->recalculateBoundingBox();

	MeshCache->addMesh(name, animatedMesh);

	return animatedMesh;
}


//! Adds a terrain mesh to the mesh pool.
boost::shared_ptr<IAnimatedMesh> CSceneManager::addTerrainMesh(const io::path& name,
	boost::shared_ptr<video::IImage> texture, boost::shared_ptr<video::IImage> heightmap,
	const core::dimension2d<f32>& stretchSize,
	f32 maxHeight,
	const core::dimension2d<u32>& defaultVertexBlockSize)
{
	if (MeshCache->isMeshLoaded(name))
		return MeshCache->getMeshByName(name);

	const bool debugBorders=false;
	boost::shared_ptr<IMesh> mesh = GeometryCreator->createTerrainMesh(texture, heightmap,
			stretchSize, maxHeight, getVideoDriver(),
			defaultVertexBlockSize, debugBorders);
	if (!mesh)
		return 0;

	boost::shared_ptr<SAnimatedMesh> animatedMesh = boost::make_shared<SAnimatedMesh>();
	if (!animatedMesh)
	{
		return 0;
	}

	animatedMesh->addMesh(mesh);
	animatedMesh->recalculateBoundingBox();

	MeshCache->addMesh(name, animatedMesh);

	return animatedMesh;
}


//! Adds an arrow mesh to the mesh pool.
boost::shared_ptr<IAnimatedMesh> CSceneManager::addArrowMesh(const io::path& name,
		video::SColor vtxColor0, video::SColor vtxColor1,
		u32 tesselationCylinder, u32 tesselationCone, f32 height,
		f32 cylinderHeight, f32 width0,f32 width1)
{
	if (MeshCache->isMeshLoaded(name))
		return MeshCache->getMeshByName(name);

	boost::shared_ptr<IMesh> mesh = GeometryCreator->createArrowMesh( tesselationCylinder,
			tesselationCone, height, cylinderHeight, width0,width1,
			vtxColor0, vtxColor1);
	if (!mesh)
		return 0;

	boost::shared_ptr<SAnimatedMesh> animatedMesh = boost::make_shared<SAnimatedMesh>();
	if (!animatedMesh)
	{
		return 0;
	}

	animatedMesh->addMesh(mesh);
	animatedMesh->recalculateBoundingBox();

	MeshCache->addMesh(name, animatedMesh);

	return animatedMesh;
}


//! Adds a static sphere mesh to the mesh pool.
boost::shared_ptr<IAnimatedMesh> CSceneManager::addSphereMesh(const io::path& name,
		f32 radius, u32 polyCountX, u32 polyCountY)
{
	if (MeshCache->isMeshLoaded(name))
		return MeshCache->getMeshByName(name);

	boost::shared_ptr<IMesh> mesh = GeometryCreator->createSphereMesh(radius, polyCountX, polyCountY);
	if (!mesh)
		return 0;

	boost::shared_ptr<SAnimatedMesh> animatedMesh = boost::make_shared<SAnimatedMesh>();
	if (!animatedMesh)
	{
		return 0;
	}

	animatedMesh->addMesh(mesh);
	animatedMesh->recalculateBoundingBox();

	MeshCache->addMesh(name, animatedMesh);

	return animatedMesh;
}



//! Adds a static volume light mesh to the mesh pool.
boost::shared_ptr<IAnimatedMesh> CSceneManager::addVolumeLightMesh(const io::path& name,
		const u32 SubdivideU, const u32 SubdivideV,
		const video::SColor FootColor, const video::SColor TailColor)
{
	if (MeshCache->isMeshLoaded(name))
		return MeshCache->getMeshByName(name);

	boost::shared_ptr<IMesh> mesh = GeometryCreator->createVolumeLightMesh(SubdivideU, SubdivideV, FootColor, TailColor);
	if (!mesh)
		return 0;

	boost::shared_ptr<SAnimatedMesh> animatedMesh = boost::make_shared<SAnimatedMesh>();
	if (!animatedMesh)
	{
		return 0;
	}

	animatedMesh->addMesh(mesh);
	animatedMesh->recalculateBoundingBox();

	MeshCache->addMesh(name, animatedMesh);

	return animatedMesh;
}


//! Returns the root scene node. This is the scene node wich is parent
//! of all scene nodes. The root scene node is a special scene node which
//! only exists to manage all scene nodes. It is not rendered and cannot
//! be removed from the scene.
//! \return Returns a pointer to the root scene node.
boost::shared_ptr<ISceneNode> CSceneManager::getRootSceneNode()
{
	return getSharedThis();
}


//! Returns the current active camera.
//! \return The active camera is returned. Note that this can be NULL, if there
//! was no camera created yet.
boost::shared_ptr<ICameraSceneNode> CSceneManager::getActiveCamera() const
{
	return ActiveCamera;
}


//! Sets the active camera. The previous active camera will be deactivated.
//! \param camera: The new camera which should be active.
void CSceneManager::setActiveCamera(boost::shared_ptr<ICameraSceneNode> camera)
{
	ActiveCamera = camera;
}


//! renders the node.
void CSceneManager::render()
{
}


//! returns the axis aligned bounding box of this node
const core::aabbox3d<f32>& CSceneManager::getBoundingBox() const
{
	_IRR_DEBUG_BREAK_IF(true) // Bounding Box of Scene Manager wanted.

	// should never be used.
	return *((core::aabbox3d<f32>*)0);
}


//! returns if node is culled
bool CSceneManager::isCulled(const boost::shared_ptr<ISceneNode> node)
{
	const boost::shared_ptr<ICameraSceneNode> cam = getActiveCamera();
	if (!cam)
	{
		_IRR_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX;
		return false;
	}
	bool result = false;

	// has occlusion query information
	if (node->getAutomaticCulling() & scene::EAC_OCC_QUERY)
	{
		result = (getVideoDriver()->getOcclusionQueryResult(node) == 0);
	}

	// can be seen by a bounding box ?
	if (!result && (node->getAutomaticCulling() & scene::EAC_BOX))
	{
		core::aabbox3d<f32> tbox = node->getBoundingBox();
		node->getAbsoluteTransformation().transformBoxEx(tbox);
		result = !(tbox.intersectsWithBox(cam->getViewFrustum()->getBoundingBox() ));
	}

	// can be seen by a bounding sphere
	if (!result && (node->getAutomaticCulling() & scene::EAC_FRUSTUM_SPHERE))
	{ // requires bbox diameter
	}

	// can be seen by cam pyramid planes ?
	if (!result && (node->getAutomaticCulling() & scene::EAC_FRUSTUM_BOX))
	{
		SViewFrustum frust = *cam->getViewFrustum();

		//transform the frustum to the node's current absolute transformation
		core::matrix4 invTrans(node->getAbsoluteTransformation(), core::matrix4::EM4CONST_INVERSE);
		//invTrans.makeInverse();
		frust.transform(invTrans);

		core::vector3df edges[8];
		node->getBoundingBox().getEdges(edges);

		for (s32 i=0; i<scene::SViewFrustum::VF_PLANE_COUNT; ++i)
		{
			bool boxInFrustum=false;
			for (u32 j=0; j<8; ++j)
			{
				if (frust.planes[i].classifyPointRelation(edges[j]) != core::ISREL3D_FRONT)
				{
					boxInFrustum=true;
					break;
				}
			}

			if (!boxInFrustum)
			{
				result = true;
				break;
			}
		}
	}

	_IRR_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX;
	return result;
}


//! registers a node for rendering it at a specific time.
u32 CSceneManager::registerNodeForRendering(boost::shared_ptr<ISceneNode> node, E_SCENE_NODE_RENDER_PASS pass)
{
	u32 taken = 0;

	switch(pass)
	{
		// take camera if it is not already registered
	case ESNRP_CAMERA:
		{
			taken = 1;
			for (u32 i = 0; i != CameraList.size(); ++i)
			{
				if (CameraList[i] == node)
				{
					taken = 0;
					break;
				}
			}
			if (taken)
			{
				CameraList.push_back(node);
			}
		}
		break;

	case ESNRP_LIGHT:
		// TODO: Point Light culling..
		// Lighting model in irrlicht has to be redone..
		//if (!isCulled(node))
		{
			LightList.push_back(node);
			taken = 1;
		}
		break;

	case ESNRP_SKY_BOX:
		SkyBoxList.push_back(node);
		taken = 1;
		break;
	case ESNRP_SOLID:
		if (!isCulled(node))
		{
			SolidNodeList.push_back(node);
			taken = 1;
		}
		break;
	case ESNRP_TRANSPARENT:
		if (!isCulled(node))
		{
			TransparentNodeList.push_back(TransparentNodeEntry(node, camWorldPos));
			taken = 1;
		}
		break;
	case ESNRP_TRANSPARENT_EFFECT:
		if (!isCulled(node))
		{
			TransparentEffectNodeList.push_back(TransparentNodeEntry(node, camWorldPos));
			taken = 1;
		}
		break;
	case ESNRP_AUTOMATIC:
		if (!isCulled(node))
		{
			const u32 count = node->getMaterialCount();

			taken = 0;
			for (u32 i=0; i<count; ++i)
			{
				boost::shared_ptr<video::IMaterialRenderer> rnd =
					getVideoDriver()->getMaterialRenderer(node->getMaterial(i).MaterialType);
				if (rnd && rnd->isTransparent())
				{
					// register as transparent node
					TransparentNodeEntry e(node, camWorldPos);
					TransparentNodeList.push_back(e);
					taken = 1;
					break;
				}
			}

			// not transparent, register as solid
			if (!taken)
			{
				SolidNodeList.push_back(node);
				taken = 1;
			}
		}
		break;
	case ESNRP_SHADOW:
		if (!isCulled(node))
		{
			ShadowNodeList.push_back(node);
			taken = 1;
		}
		break;

	case ESNRP_NONE: // ignore this one
		break;
	}

#ifdef _IRR_SCENEMANAGER_DEBUG
	s32 index = Parameters.findAttribute ( "calls" );
	Parameters.setAttribute ( index, Parameters.getAttributeAsInt ( index ) + 1 );

	if (!taken)
	{
		index = Parameters.findAttribute ( "culled" );
		Parameters.setAttribute ( index, Parameters.getAttributeAsInt ( index ) + 1 );
	}
#endif

	return taken;
}


//! This method is called just before the rendering process of the whole scene.
//! draws all scene nodes
void CSceneManager::drawAll()
{
	boost::shared_ptr<video::IVideoDriver> lockedDriver = getVideoDriver();

	if (!lockedDriver)
		return;

#ifdef _IRR_SCENEMANAGER_DEBUG
	// reset attributes
	Parameters.setAttribute ( "culled", 0 );
	Parameters.setAttribute ( "calls", 0 );
	Parameters.setAttribute ( "drawn_solid", 0 );
	Parameters.setAttribute ( "drawn_transparent", 0 );
	Parameters.setAttribute ( "drawn_transparent_effect", 0 );
#endif

	u32 i; // new ISO for scoping problem in some compilers

	// reset all transforms
	lockedDriver->setMaterial(video::SMaterial());
	lockedDriver->setTransform ( video::ETS_PROJECTION, core::IdentityMatrix );
	lockedDriver->setTransform ( video::ETS_VIEW, core::IdentityMatrix );
	lockedDriver->setTransform ( video::ETS_WORLD, core::IdentityMatrix );
	for (i=video::ETS_COUNT-1; i>=video::ETS_TEXTURE_0; --i)
		lockedDriver->setTransform ( (video::E_TRANSFORMATION_STATE)i, core::IdentityMatrix );

	// TODO: This should not use an attribute here but a real parameter when necessary (too slow!)
	lockedDriver->setAllowZWriteOnTransparent(Parameters.getAttributeAsBool( ALLOW_ZWRITE_ON_TRANSPARENT) );

	// do animations and other stuff.
	OnAnimate(os::Timer::getTime());

	/*!
		First Scene Node for prerendering should be the active camera
		consistent Camera is needed for culling
	*/
	camWorldPos.set(0,0,0);
	if (ActiveCamera)
	{
		ActiveCamera->render();
		camWorldPos = ActiveCamera->getAbsolutePosition();
	}

	// let all nodes register themselves
	OnRegisterSceneNode();

	if (LightManager)
		LightManager->OnPreRender(LightList);

	//render camera scenes
	{
		CurrentRendertime = ESNRP_CAMERA;
		lockedDriver->getOverrideMaterial().Enabled = ((lockedDriver->getOverrideMaterial().EnablePasses & CurrentRendertime) != 0);

		if (LightManager)
			LightManager->OnRenderPassPreRender(CurrentRendertime);

		for (i=0; i<CameraList.size(); ++i)
			CameraList[i]->render();

		CameraList.set_used(0);

		if (LightManager)
			LightManager->OnRenderPassPostRender(CurrentRendertime);
	}

	//render lights scenes
	{
		CurrentRendertime = ESNRP_LIGHT;
		lockedDriver->getOverrideMaterial().Enabled = ((lockedDriver->getOverrideMaterial().EnablePasses & CurrentRendertime) != 0);

		if (LightManager)
		{
			LightManager->OnRenderPassPreRender(CurrentRendertime);
		}
		else
		{
			// Sort the lights by distance from the camera
			core::vector3df camWorldPos(0, 0, 0);
			if (ActiveCamera)
				camWorldPos = ActiveCamera->getAbsolutePosition();

			core::array<DistanceNodeEntry> SortedLights;
			SortedLights.set_used(LightList.size());
			for (s32 light = (s32)LightList.size() - 1; light >= 0; --light)
				SortedLights[light].setNodeAndDistanceFromPosition(LightList[light], camWorldPos);

			SortedLights.set_sorted(false);
			SortedLights.sort();

			for(s32 light = (s32)LightList.size() - 1; light >= 0; --light)
				LightList[light] = SortedLights[light].Node;
		}

		lockedDriver->deleteAllDynamicLights();

		lockedDriver->setAmbientLight(AmbientLight);

		u32 maxLights = LightList.size();

		if (!LightManager)
			maxLights = core::min_ (lockedDriver->getMaximalDynamicLightAmount(), maxLights);

		for (i=0; i< maxLights; ++i)
			LightList[i]->render();

		if (LightManager)
			LightManager->OnRenderPassPostRender(CurrentRendertime);
	}

	// render skyboxes
	{
		CurrentRendertime = ESNRP_SKY_BOX;
		lockedDriver->getOverrideMaterial().Enabled = ((lockedDriver->getOverrideMaterial().EnablePasses & CurrentRendertime) != 0);

		if (LightManager)
		{
			LightManager->OnRenderPassPreRender(CurrentRendertime);
			for (i=0; i<SkyBoxList.size(); ++i)
			{
				boost::shared_ptr<ISceneNode> node = SkyBoxList[i];
				LightManager->OnNodePreRender(node);
				node->render();
				LightManager->OnNodePostRender(node);
			}
		}
		else
		{
			for (i=0; i<SkyBoxList.size(); ++i)
				SkyBoxList[i]->render();
		}

		SkyBoxList.set_used(0);

		if (LightManager)
			LightManager->OnRenderPassPostRender(CurrentRendertime);
	}


	// render default objects
	{
		CurrentRendertime = ESNRP_SOLID;
		lockedDriver->getOverrideMaterial().Enabled = ((lockedDriver->getOverrideMaterial().EnablePasses & CurrentRendertime) != 0);

		SolidNodeList.sort(); // sort by textures

		if (LightManager)
		{
			LightManager->OnRenderPassPreRender(CurrentRendertime);
			for (i=0; i<SolidNodeList.size(); ++i)
			{
				boost::shared_ptr<ISceneNode> node = SolidNodeList[i].Node;
				LightManager->OnNodePreRender(node);
				node->render();
				LightManager->OnNodePostRender(node);
			}
		}
		else
		{
			for (i=0; i<SolidNodeList.size(); ++i)
				SolidNodeList[i].Node->render();
		}

#ifdef _IRR_SCENEMANAGER_DEBUG
		Parameters.setAttribute("drawn_solid", (s32) SolidNodeList.size() );
#endif
		SolidNodeList.set_used(0);

		if (LightManager)
			LightManager->OnRenderPassPostRender(CurrentRendertime);
	}

	// render shadows
	{
		boost::shared_ptr<video::IVideoDriver> lockedDriver = getVideoDriver();

		CurrentRendertime = ESNRP_SHADOW;
		lockedDriver->getOverrideMaterial().Enabled = ((lockedDriver->getOverrideMaterial().EnablePasses & CurrentRendertime) != 0);

		if (LightManager)
		{
			LightManager->OnRenderPassPreRender(CurrentRendertime);
			for (i=0; i<ShadowNodeList.size(); ++i)
			{
				boost::shared_ptr<ISceneNode> node = ShadowNodeList[i];
				LightManager->OnNodePreRender(node);
				node->render();
				LightManager->OnNodePostRender(node);
			}
		}
		else
		{
			for (i=0; i<ShadowNodeList.size(); ++i)
				ShadowNodeList[i]->render();
		}

		if (!ShadowNodeList.empty())
			lockedDriver->drawStencilShadow(true,ShadowColor, ShadowColor,
				ShadowColor, ShadowColor);

		ShadowNodeList.set_used(0);

		if (LightManager)
			LightManager->OnRenderPassPostRender(CurrentRendertime);
	}

	// render transparent objects.
	{
		CurrentRendertime = ESNRP_TRANSPARENT;
		lockedDriver->getOverrideMaterial().Enabled = ((lockedDriver->getOverrideMaterial().EnablePasses & CurrentRendertime) != 0);

		TransparentNodeList.sort(); // sort by distance from camera
		if (LightManager)
		{
			LightManager->OnRenderPassPreRender(CurrentRendertime);

			for (i=0; i<TransparentNodeList.size(); ++i)
			{
				boost::shared_ptr<ISceneNode> node = TransparentNodeList[i].Node;
				LightManager->OnNodePreRender(node);
				node->render();
				LightManager->OnNodePostRender(node);
			}
		}
		else
		{
			for (i=0; i<TransparentNodeList.size(); ++i)
				TransparentNodeList[i].Node->render();
		}

#ifdef _IRR_SCENEMANAGER_DEBUG
		Parameters.setAttribute ( "drawn_transparent", (s32) TransparentNodeList.size() );
#endif
		TransparentNodeList.set_used(0);

		if (LightManager)
			LightManager->OnRenderPassPostRender(CurrentRendertime);
	}

	// render transparent effect objects.
	{
		CurrentRendertime = ESNRP_TRANSPARENT_EFFECT;
		lockedDriver->getOverrideMaterial().Enabled = ((lockedDriver->getOverrideMaterial().EnablePasses & CurrentRendertime) != 0);

		TransparentEffectNodeList.sort(); // sort by distance from camera

		if (LightManager)
		{
			LightManager->OnRenderPassPreRender(CurrentRendertime);

			for (i=0; i<TransparentEffectNodeList.size(); ++i)
			{
				boost::shared_ptr<ISceneNode> node = TransparentEffectNodeList[i].Node;
				LightManager->OnNodePreRender(node);
				node->render();
				LightManager->OnNodePostRender(node);
			}
		}
		else
		{
			for (i=0; i<TransparentEffectNodeList.size(); ++i)
				TransparentEffectNodeList[i].Node->render();
		}
#ifdef _IRR_SCENEMANAGER_DEBUG
		Parameters.setAttribute ( "drawn_transparent_effect", (s32) TransparentEffectNodeList.size() );
#endif
		TransparentEffectNodeList.set_used(0);
	}

	if (LightManager)
		LightManager->OnPostRender();

	LightList.set_used(0);
	clearDeletionList();

	CurrentRendertime = ESNRP_NONE;
}

void CSceneManager::setLightManager(boost::shared_ptr<ILightManager> lightManager)
{
	LightManager = lightManager;
}


//! Sets the color of stencil buffers shadows drawn by the scene manager.
void CSceneManager::setShadowColor(video::SColor color)
{
	ShadowColor = color;
}


//! Returns the current color of shadows.
video::SColor CSceneManager::getShadowColor() const
{
	return ShadowColor;
}


//! creates a rotation animator, which rotates the attached scene node around itself.
boost::shared_ptr<ISceneNodeAnimator> CSceneManager::createRotationAnimator(const core::vector3df& rotationPerSecond)
{
	boost::shared_ptr<ISceneNodeAnimator> anim = boost::make_shared<CSceneNodeAnimatorRotation>(os::Timer::getTime(),
		rotationPerSecond);

	return anim;
}


//! creates a fly circle animator, which lets the attached scene node fly around a center.
boost::shared_ptr<ISceneNodeAnimator> CSceneManager::createFlyCircleAnimator(
		const core::vector3df& center, f32 radius, f32 speed,
		const core::vector3df& direction,
		f32 startPosition,
		f32 radiusEllipsoid)
{
	const f32 orbitDurationMs = (core::DEGTORAD * 360.f) / speed;
	const u32 effectiveTime = os::Timer::getTime() + (u32)(orbitDurationMs * startPosition);

	boost::shared_ptr<ISceneNodeAnimator> anim = boost::make_shared<CSceneNodeAnimatorFlyCircle>(
			effectiveTime, center,
			radius, speed, direction,radiusEllipsoid);
	return anim;
}


//! Creates a fly straight animator, which lets the attached scene node
//! fly or move along a line between two points.
boost::shared_ptr<ISceneNodeAnimator> CSceneManager::createFlyStraightAnimator(const core::vector3df& startPoint,
					const core::vector3df& endPoint, u32 timeForWay, bool loop,bool pingpong)
{
	boost::shared_ptr<ISceneNodeAnimator> anim = boost::make_shared<CSceneNodeAnimatorFlyStraight>(startPoint,
		endPoint, timeForWay, loop, os::Timer::getTime(), pingpong);

	return anim;
}


//! Creates a texture animator, which switches the textures of the target scene
//! node based on a list of textures.
boost::shared_ptr<ISceneNodeAnimator> CSceneManager::createTextureAnimator(const core::array<boost::shared_ptr<video::ITexture>>& textures,
	s32 timePerFrame, bool loop)
{
	boost::shared_ptr<ISceneNodeAnimator> anim = boost::make_shared<CSceneNodeAnimatorTexture>(textures,
		timePerFrame, loop, os::Timer::getTime());

	return anim;
}


//! Creates a scene node animator, which deletes the scene node after
//! some time automaticly.
boost::shared_ptr<ISceneNodeAnimator> CSceneManager::createDeleteAnimator(u32 when)
{
	return boost::make_shared<CSceneNodeAnimatorDelete>(getSceneManager(), os::Timer::getTime() + when);
}


//! Creates a special scene node animator for doing automatic collision detection
//! and response.
boost::shared_ptr<ISceneNodeAnimatorCollisionResponse> CSceneManager::createCollisionResponseAnimator(
	boost::shared_ptr<ITriangleSelector> world, boost::shared_ptr<ISceneNode> sceneNode, const core::vector3df& ellipsoidRadius,
	const core::vector3df& gravityPerSecond,
	const core::vector3df& ellipsoidTranslation, f32 slidingValue)
{
	boost::shared_ptr<ISceneNodeAnimatorCollisionResponse> anim = 
		boost::make_shared<CSceneNodeAnimatorCollisionResponse>(getSceneManager(), world, sceneNode,
			ellipsoidRadius, gravityPerSecond,
			ellipsoidTranslation, slidingValue);

	return anim;
}


//! Creates a follow spline animator.
boost::shared_ptr<ISceneNodeAnimator> CSceneManager::createFollowSplineAnimator(s32 startTime,
	const core::array< core::vector3df >& points,
	f32 speed, f32 tightness, bool loop, bool pingpong)
{
	boost::shared_ptr<ISceneNodeAnimator> a = boost::make_shared<CSceneNodeAnimatorFollowSpline>(startTime, points,
		speed, tightness, loop, pingpong);
	return a;
}


//! Adds an external mesh loader.
void CSceneManager::addExternalMeshLoader(boost::shared_ptr<IMeshLoader> externalLoader)
{
	if (!externalLoader)
		return;

	MeshLoaderList.push_back(externalLoader);
}


//! Returns the number of mesh loaders supported by Irrlicht at this time
u32 CSceneManager::getMeshLoaderCount() const
{
	return MeshLoaderList.size();
}


//! Retrieve the given mesh loader
boost::shared_ptr<IMeshLoader> CSceneManager::getMeshLoader(u32 index) const
{
	if (index < MeshLoaderList.size())
		return MeshLoaderList[index];
	else
		return 0;
}


//! Adds an external scene loader.
void CSceneManager::addExternalSceneLoader(boost::shared_ptr<ISceneLoader> externalLoader)
{
	if (!externalLoader)
		return;

	SceneLoaderList.push_back(externalLoader);
}


//! Returns the number of scene loaders
u32 CSceneManager::getSceneLoaderCount() const
{
	return SceneLoaderList.size();
}


//! Retrieve the given scene loader
boost::shared_ptr<ISceneLoader> CSceneManager::getSceneLoader(u32 index) const
{
	if (index < SceneLoaderList.size())
		return SceneLoaderList[index];
	else
		return 0;
}


//! Returns a pointer to the scene collision manager.
boost::shared_ptr<ISceneCollisionManager> CSceneManager::getSceneCollisionManager()
{
	return CollisionManager;
}


//! Returns a pointer to the mesh manipulator.
boost::shared_ptr<IMeshManipulator> CSceneManager::getMeshManipulator()
{
	return getVideoDriver()->getMeshManipulator();
}


//! Creates a simple ITriangleSelector, based on a mesh.
boost::shared_ptr<ITriangleSelector> CSceneManager::createTriangleSelector(boost::shared_ptr<IMesh> mesh, boost::shared_ptr<ISceneNode> node)
{
	if (!mesh)
		return 0;

	boost::shared_ptr<CTriangleSelector> selector = boost::make_shared<CTriangleSelector>(mesh, node);
	selector->setWeakPtr(selector);

	return selector;
}


//! Creates a simple and updatable ITriangleSelector, based on a the mesh owned by an
//! animated scene node
boost::shared_ptr<ITriangleSelector> CSceneManager::createTriangleSelector(boost::shared_ptr<scene::IAnimatedMeshSceneNode> node)
{
	if (!node || !node->getMesh())
		return nullptr;

	boost::shared_ptr<CTriangleSelector> selector = boost::make_shared<CTriangleSelector>(node);
	selector->setWeakPtr(selector);

	return selector;
}


//! Creates a simple dynamic ITriangleSelector, based on a axis aligned bounding box.
boost::shared_ptr<ITriangleSelector> CSceneManager::createTriangleSelectorFromBoundingBox(boost::shared_ptr<ISceneNode> node)
{
	if (!node)
		return 0;

	return boost::make_shared<CTriangleBBSelector>(node);
}


//! Creates a simple ITriangleSelector, based on a mesh.
boost::shared_ptr<ITriangleSelector> CSceneManager::createOctreeTriangleSelector(boost::shared_ptr<IMesh> mesh,
							boost::shared_ptr<ISceneNode> node, s32 minimalPolysPerNode)
{
	if (!mesh)
		return 0;

	return boost::make_shared<COctreeTriangleSelector>(mesh, node, minimalPolysPerNode);
}


//! Creates a meta triangle selector.
boost::shared_ptr<IMetaTriangleSelector> CSceneManager::createMetaTriangleSelector()
{
	return boost::make_shared<CMetaTriangleSelector>();
}


//! Creates a triangle selector which can select triangles from a terrain scene node
boost::shared_ptr<ITriangleSelector> CSceneManager::createTerrainTriangleSelector(
	boost::shared_ptr<ITerrainSceneNode> node, s32 LOD)
{
	boost::shared_ptr<CTerrainTriangleSelector> selector = boost::make_shared<CTerrainTriangleSelector>(node, LOD);\
	selector->setWeakPtr(selector);

	return selector;
}



//! Adds a scene node to the deletion queue.
void CSceneManager::addToDeletionQueue(boost::shared_ptr<ISceneNode> node)
{
	if (!node)
		return;

	DeletionList.push_back(node);
}


//! clears the deletion list
void CSceneManager::clearDeletionList()
{
	if (DeletionList.empty())
		return;

	for (u32 i=0; i<DeletionList.size(); ++i)
	{
		DeletionList[i]->remove();
	}

	DeletionList.clear();
}


//! Returns the first scene node with the specified name.
boost::shared_ptr<ISceneNode> CSceneManager::getSceneNodeFromName(const char* name, boost::shared_ptr<ISceneNode> start)
{
	if (start == 0)
		start = getRootSceneNode();

	if (!strcmp(start->getName(),name))
		return start;

	boost::shared_ptr<ISceneNode> node = 0;

	const ISceneNodeList& list = start->getChildren();
	ISceneNodeList::ConstIterator it = list.begin();
	for (; it!=list.end(); ++it)
	{
		node = getSceneNodeFromName(name, *it);
		if (node)
			return node;
	}

	return 0;
}


//! Returns the first scene node with the specified id.
boost::shared_ptr<ISceneNode> CSceneManager::getSceneNodeFromId(s32 id, boost::shared_ptr<ISceneNode> start)
{
	if (start == 0)
		start = getRootSceneNode();

	if (start->getID() == id)
		return start;

	boost::shared_ptr<ISceneNode> node = 0;

	const ISceneNodeList& list = start->getChildren();
	ISceneNodeList::ConstIterator it = list.begin();
	for (; it!=list.end(); ++it)
	{
		node = getSceneNodeFromId(id, *it);
		if (node)
			return node;
	}

	return 0;
}


//! Returns the first scene node with the specified type.
boost::shared_ptr<ISceneNode> CSceneManager::getSceneNodeFromType(scene::ESCENE_NODE_TYPE type, boost::shared_ptr<ISceneNode> start)
{
	if (start == 0)
		start = getRootSceneNode();

	if (start->getType() == type || ESNT_ANY == type)
		return start;

	boost::shared_ptr<ISceneNode> node = 0;

	const ISceneNodeList& list = start->getChildren();
	ISceneNodeList::ConstIterator it = list.begin();
	for (; it!=list.end(); ++it)
	{
		node = getSceneNodeFromType(type, *it);
		if (node)
			return node;
	}

	return 0;
}


//! returns scene nodes by type.
void CSceneManager::getSceneNodesFromType(ESCENE_NODE_TYPE type, core::array<boost::shared_ptr<scene::ISceneNode>>& outNodes, boost::shared_ptr<ISceneNode> start)
{
	if (start == 0)
		start = getRootSceneNode();

	if (start->getType() == type || ESNT_ANY == type)
		outNodes.push_back(start);

	const ISceneNodeList& list = start->getChildren();
	ISceneNodeList::ConstIterator it = list.begin();

	for (; it!=list.end(); ++it)
	{
		getSceneNodesFromType(type, outNodes, *it);
	}
}


//! Posts an input event to the environment. Usually you do not have to
//! use this method, it is used by the internal engine.
bool CSceneManager::postEventFromUser(const SEvent& event)
{
	bool ret = false;
	boost::shared_ptr<ICameraSceneNode> cam = getActiveCamera();
	if (cam)
		ret = cam->OnEvent(event);

	_IRR_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX;
	return ret;
}


//! Removes all children of this scene node
void CSceneManager::removeAll()
{
	ISceneNode::removeAll();
	setActiveCamera(0);
	// Make sure the driver is reset, might need a more complex method at some point
	boost::shared_ptr<video::IVideoDriver> lockedDriver = getVideoDriver();

	if (lockedDriver)
		lockedDriver->setMaterial(video::SMaterial());
}


//! Clears the whole scene. All scene nodes are removed.
void CSceneManager::clear()
{
	removeAll();
}


//! Returns interface to the parameters set in this scene.
io::IAttributes* CSceneManager::getParameters()
{
	return &Parameters;
}


//! Returns current render pass.
E_SCENE_NODE_RENDER_PASS CSceneManager::getSceneNodeRenderPass() const
{
	return CurrentRendertime;
}


//! Returns an interface to the mesh cache which is shared beween all existing scene managers.
boost::shared_ptr<IMeshCache> CSceneManager::getMeshCache()
{
	return MeshCache;
}


//! Creates a new scene manager.
boost::shared_ptr<scene::ISceneManager> CSceneManager::createNewSceneManager(bool cloneContent)
{
	boost::shared_ptr<CSceneManager> manager = boost::make_shared<CSceneManager>(getVideoDriver(), FileSystem, CursorControl, MeshCache, GUIEnvironment);
	manager->setWeakThis(manager);

	if (cloneContent)
		manager->cloneMembers(getSharedThis(), manager);

	return manager;
}


//! Returns the default scene node factory which can create all built in scene nodes
boost::shared_ptr<ISceneNodeFactory> CSceneManager::getDefaultSceneNodeFactory()
{
	return getSceneNodeFactory(0);
}


//! Adds a scene node factory to the scene manager.
void CSceneManager::registerSceneNodeFactory(boost::shared_ptr<ISceneNodeFactory> factoryToAdd)
{
	if (factoryToAdd)
	{
		SceneNodeFactoryList.push_back(factoryToAdd);
	}
}


//! Returns amount of registered scene node factories.
u32 CSceneManager::getRegisteredSceneNodeFactoryCount() const
{
	return SceneNodeFactoryList.size();
}


//! Returns a scene node factory by index
boost::shared_ptr<ISceneNodeFactory> CSceneManager::getSceneNodeFactory(u32 index)
{
	if (index < SceneNodeFactoryList.size())
		return SceneNodeFactoryList[index];

	return 0;
}


//! Returns the default scene node animator factory which can create all built-in scene node animators
ISceneNodeAnimatorFactory* CSceneManager::getDefaultSceneNodeAnimatorFactory()
{
	return getSceneNodeAnimatorFactory(0);
}

//! Adds a scene node animator factory to the scene manager.
void CSceneManager::registerSceneNodeAnimatorFactory(ISceneNodeAnimatorFactory* factoryToAdd)
{
	if (factoryToAdd)
	{
		factoryToAdd->grab();
		SceneNodeAnimatorFactoryList.push_back(factoryToAdd);
	}
}


//! Returns amount of registered scene node animator factories.
u32 CSceneManager::getRegisteredSceneNodeAnimatorFactoryCount() const
{
	return SceneNodeAnimatorFactoryList.size();
}


//! Returns a scene node animator factory by index
ISceneNodeAnimatorFactory* CSceneManager::getSceneNodeAnimatorFactory(u32 index)
{
	if (index < SceneNodeAnimatorFactoryList.size())
		return SceneNodeAnimatorFactoryList[index];

	return 0;
}


//! Saves the current scene into a file.
//! \param filename: Name of the file .
bool CSceneManager::saveScene(const io::path& filename, ISceneUserDataSerializer* userDataSerializer, boost::shared_ptr<ISceneNode> node)
{
	bool ret = false;
	boost::shared_ptr<io::IWriteFile> file = FileSystem->createAndWriteFile(filename);
	if (file)
	{
		ret = saveScene(file, userDataSerializer, node);
	}
	else
		os::Printer::log("Unable to open file", filename, ELL_ERROR);

	_IRR_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX;
	return ret;
}


//! Saves the current scene into a file.
bool CSceneManager::saveScene(boost::shared_ptr<io::IWriteFile> file, ISceneUserDataSerializer* userDataSerializer, boost::shared_ptr<ISceneNode> node)
{
	if (!file)
	{
		return false;
	}

	bool result=false;
	io::IXMLWriter* writer = FileSystem->createXMLWriter(file);
	if (!writer)
	{
		os::Printer::log("Unable to create XML writer", file->getFileName(), ELL_ERROR);
	}
	else
	{
		result = saveScene(writer, FileSystem->getFileDir(FileSystem->getAbsolutePath(file->getFileName())), userDataSerializer, node);
		writer->drop();
	}
	return result;
}


//! Saves the current scene into a file.
bool CSceneManager::saveScene(io::IXMLWriter* writer, const io::path& currentPath, ISceneUserDataSerializer* userDataSerializer, boost::shared_ptr<ISceneNode> node)
{
	if (!writer)
		return false;

	if (!node)
		node=getSharedThis();

	writer->writeXMLHeader();
	writeSceneNode(writer, node, userDataSerializer, currentPath.c_str(), true);

	return true;
}


//! Loads a scene.
bool CSceneManager::loadScene(const io::path& filename, ISceneUserDataSerializer* userDataSerializer, boost::shared_ptr<ISceneNode> rootNode)
{
	boost::shared_ptr<io::IReadFile> file = FileSystem->createAndOpenFile(filename);
	if (!file)
	{
		os::Printer::log("Unable to open scene file", filename.c_str(), ELL_ERROR);
		return false;
	}

	const bool ret = loadScene(file, userDataSerializer, rootNode);

	return ret;
}


//! Loads a scene. Note that the current scene is not cleared before.
bool CSceneManager::loadScene(boost::shared_ptr<io::IReadFile> file, ISceneUserDataSerializer* userDataSerializer, boost::shared_ptr<ISceneNode> rootNode)
{
	if (!file)
	{
		os::Printer::log("Unable to open scene file", ELL_ERROR);
		return false;
	}

	bool ret = false;

	// try scene loaders in reverse order
	s32 i = SceneLoaderList.size()-1;
	for (; i >= 0 && !ret; --i)
		if (SceneLoaderList[i]->isALoadableFileFormat(file))
			ret = SceneLoaderList[i]->loadScene(file, userDataSerializer, rootNode);

	if (!ret)
		os::Printer::log("Could not load scene file, perhaps the format is unsupported: ", file->getFileName().c_str(), ELL_ERROR);

	return ret;
}


//! writes a scene node
void CSceneManager::writeSceneNode(io::IXMLWriter* writer, boost::shared_ptr<ISceneNode> node, ISceneUserDataSerializer* userDataSerializer,
		const fschar_t* currentPath, bool init)
{
	if (!writer || !node || node->isDebugObject())
		return;

	const wchar_t* name;
	boost::shared_ptr<ISceneNode> tmpNode=node;

	if (init)
	{
		name = IRR_XML_FORMAT_SCENE.c_str();
		writer->writeElement(name, false);
		node= getSharedThis();
	}
	else
	{
		name = IRR_XML_FORMAT_NODE.c_str();
		writer->writeElement(name, false, IRR_XML_FORMAT_NODE_ATTR_TYPE.c_str(),
			core::stringw(getSceneNodeTypeName(node->getType())).c_str());
	}

	writer->writeLineBreak();

	// write properties
	boost::shared_ptr<video::IVideoDriver> lockedDriver = getVideoDriver();

	io::IAttributes* attr = FileSystem->createEmptyAttributes(lockedDriver);
	io::SAttributeReadWriteOptions options;
	if (currentPath)
	{
		options.Filename=currentPath;
		options.Flags|=io::EARWF_USE_RELATIVE_PATHS;
	}
	node->serializeAttributes(attr, &options);

	if (attr->getAttributeCount() != 0)
	{
		attr->write(writer);
		writer->writeLineBreak();
	}

	// write materials

	if (node->getMaterialCount() && lockedDriver)
	{
		const wchar_t* materialElement = L"materials";

		writer->writeElement(materialElement);
		writer->writeLineBreak();

		for (u32 i=0; i < node->getMaterialCount(); ++i)
		{
			io::IAttributes* tmp_attr =
				lockedDriver->createAttributesFromMaterial(node->getMaterial(i), &options);
			tmp_attr->write(writer);
			tmp_attr->drop();
		}

		writer->writeClosingTag(materialElement);
		writer->writeLineBreak();
	}

	// write animators

	if (!node->getAnimators().empty())
	{
		const wchar_t* animatorElement = L"animators";
		writer->writeElement(animatorElement);
		writer->writeLineBreak();

		ISceneNodeAnimatorList::ConstIterator it = node->getAnimators().begin();
		for (; it != node->getAnimators().end(); ++it)
		{
			attr->clear();
			attr->addString("Type", getAnimatorTypeName((*it)->getType()));

			(*it)->serializeAttributes(attr);

			attr->write(writer);
		}

		writer->writeClosingTag(animatorElement);
		writer->writeLineBreak();
	}

	// write possible user data

	if (userDataSerializer)
	{
		io::IAttributes* userData = userDataSerializer->createUserData(node);
		if (userData)
		{
			const wchar_t* userDataElement = L"userData";

			writer->writeLineBreak();
			writer->writeElement(userDataElement);
			writer->writeLineBreak();

			userData->write(writer);

			writer->writeClosingTag(userDataElement);
			writer->writeLineBreak();
			writer->writeLineBreak();

			userData->drop();
		}
	}
	// reset to actual root node
	if (init)
		node=tmpNode;

	// write children once root node is written
	// if parent is not scene manager, we need to write out node first
	if (init && (node.get() != this))
	{
		writeSceneNode(writer, node, userDataSerializer, currentPath);
	}
	else
	{
		ISceneNodeList::ConstIterator it = node->getChildren().begin();
		for (; it != node->getChildren().end(); ++it)
			writeSceneNode(writer, (*it), userDataSerializer, currentPath);
	}

	attr->drop();

	writer->writeClosingTag(name);
	writer->writeLineBreak();
	writer->writeLineBreak();
}


//! Returns a typename from a scene node type or null if not found
const c8* CSceneManager::getSceneNodeTypeName(ESCENE_NODE_TYPE type)
{
	const char* name = 0;

	for (s32 i=(s32)SceneNodeFactoryList.size()-1; !name && i>=0; --i)
		name = SceneNodeFactoryList[i]->getCreateableSceneNodeTypeName(type);

	return name;
}

//! Adds a scene node to the scene by name
boost::shared_ptr<ISceneNode> CSceneManager::addSceneNode(const char* sceneNodeTypeName, boost::shared_ptr<ISceneNode> parent)
{
	boost::shared_ptr<ISceneNode> node = 0;

	for (s32 i=(s32)SceneNodeFactoryList.size()-1; i>=0 && !node; --i)
			node = SceneNodeFactoryList[i]->addSceneNode(sceneNodeTypeName, parent);

	return node;
}

boost::shared_ptr<ISceneNodeAnimator> CSceneManager::createSceneNodeAnimator(const char* typeName, boost::shared_ptr<ISceneNode> target)
{
	boost::shared_ptr<ISceneNodeAnimator> animator = 0;

	for (s32 i=(s32)SceneNodeAnimatorFactoryList.size()-1; i>=0 && !animator; --i)
		animator = SceneNodeAnimatorFactoryList[i]->createSceneNodeAnimator(typeName, target);

	return animator;
}


//! Returns a typename from a scene node animator type or null if not found
const c8* CSceneManager::getAnimatorTypeName(ESCENE_NODE_ANIMATOR_TYPE type)
{
	const char* name = 0;

	for (s32 i=SceneNodeAnimatorFactoryList.size()-1; !name && i >= 0; --i)
		name = SceneNodeAnimatorFactoryList[i]->getCreateableSceneNodeAnimatorTypeName(type);

	return name;
}


//! Writes attributes of the scene node.
void CSceneManager::serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options)
{
	out->addString	("Name", Name.c_str());
	out->addInt	("Id", ID );
	out->addColorf	("AmbientLight", AmbientLight);

	// fog attributes from video driver
	video::SColor color;
	video::E_FOG_TYPE fogType;
	f32 start, end, density;
	bool pixelFog, rangeFog;

	getVideoDriver()->getFog(color, fogType, start, end, density, pixelFog, rangeFog);

	out->addEnum("FogType", fogType, video::FogTypeNames);
	out->addColorf("FogColor", color);
	out->addFloat("FogStart", start);
	out->addFloat("FogEnd", end);
	out->addFloat("FogDensity", density);
	out->addBool("FogPixel", pixelFog);
	out->addBool("FogRange", rangeFog);
}

//! Reads attributes of the scene node.
void CSceneManager::deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options)
{
	Name = in->getAttributeAsString("Name");
	ID = in->getAttributeAsInt("Id");
	AmbientLight = in->getAttributeAsColorf("AmbientLight");

	// fog attributes
	video::SColor color;
	video::E_FOG_TYPE fogType;
	f32 start, end, density;
	bool pixelFog, rangeFog;
	if (in->existsAttribute("FogType"))
	{
		fogType = (video::E_FOG_TYPE) in->getAttributeAsEnumeration("FogType", video::FogTypeNames);
		color = in->getAttributeAsColorf("FogColor").toSColor();
		start = in->getAttributeAsFloat("FogStart");
		end = in->getAttributeAsFloat("FogEnd");
		density = in->getAttributeAsFloat("FogDensity");
		pixelFog = in->getAttributeAsBool("FogPixel");
		rangeFog = in->getAttributeAsBool("FogRange");
		getVideoDriver()->setFog(color, fogType, start, end, density, pixelFog, rangeFog);
	}

	RelativeTranslation.set(0,0,0);
	RelativeRotation.set(0,0,0);
	RelativeScale.set(1,1,1);
	IsVisible = true;
	AutomaticCullingState = scene::EAC_BOX;
	DebugDataVisible = scene::EDS_OFF;
	IsDebugObject = false;

	updateAbsolutePosition();
}


//! Sets ambient color of the scene
void CSceneManager::setAmbientLight(const video::SColorf &ambientColor)
{
	AmbientLight = ambientColor;
}


//! Returns ambient color of the scene
const video::SColorf& CSceneManager::getAmbientLight() const
{
	return AmbientLight;
}


//! Get a skinned mesh, which is not available as header-only code
boost::shared_ptr<ISkinnedMesh> CSceneManager::createSkinnedMesh()
{
#ifdef _IRR_COMPILE_WITH_SKINNED_MESH_SUPPORT_
	boost::shared_ptr<CSkinnedMesh> result = boost::make_shared<CSkinnedMesh>();
	result->setWeakThis(result);

	return result;
#else
	return 0;
#endif
}

//! Returns a mesh writer implementation if available
boost::shared_ptr<IMeshWriter> CSceneManager::createMeshWriter(EMESH_WRITER_TYPE type)
{
	boost::shared_ptr<video::IVideoDriver> lockedDriver = getVideoDriver();
	boost::shared_ptr<ISceneManager> lockedSceneManager = getSceneManager();

	switch(type)
	{
	case EMWT_IRR_MESH:
#ifdef _IRR_COMPILE_WITH_IRR_WRITER_
		return boost::make_shared<CIrrMeshWriter>(lockedDriver, FileSystem);
#else
		return nullptr;
#endif
	case EMWT_COLLADA:
#ifdef _IRR_COMPILE_WITH_COLLADA_WRITER_
		return boost::make_shared<CColladaMeshWriter>(lockedSceneManager, lockedDriver, FileSystem);
#else
		return nullptr;
#endif
	case EMWT_STL:
#ifdef _IRR_COMPILE_WITH_STL_WRITER_
		return boost::make_shared<CSTLMeshWriter>(lockedSceneManager);
#else
		return nullptr;
#endif
	case EMWT_OBJ:
#ifdef _IRR_COMPILE_WITH_OBJ_WRITER_
		return boost::make_shared<COBJMeshWriter>(lockedSceneManager, FileSystem);
#else
		return nullptr;
#endif

	case EMWT_PLY:
#ifdef _IRR_COMPILE_WITH_PLY_WRITER_
		return boost::make_shared<CPLYMeshWriter>();
#else
		return nullptr;
#endif
	}

	return nullptr;
}


// creates a scenemanager
boost::shared_ptr<scene::ISceneManager> createSceneManager(boost::shared_ptr<video::IVideoDriver> driver,
		boost::shared_ptr<io::IFileSystem> fs, boost::shared_ptr<gui::ICursorControl> cursorcontrol,
		boost::shared_ptr<gui::IGUIEnvironment> guiEnvironment)
{
	boost::shared_ptr<scene::CSceneManager> sceneManager = boost::make_shared<CSceneManager>(driver, fs, cursorcontrol, nullptr, guiEnvironment);
	sceneManager->setSceneManager(sceneManager);
	sceneManager->setWeakThis(sceneManager);

	return sceneManager;
}


} // end namespace scene
} // end namespace irr

