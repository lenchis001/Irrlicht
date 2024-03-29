// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_SCENE_MANAGER_H_INCLUDED__
#define __C_SCENE_MANAGER_H_INCLUDED__

#include "ISceneManager.h"
#include "ISceneNode.h"
#include "ICursorControl.h"
#include "irrString.h"
#include "irrArray.h"
#include "IMeshLoader.h"
#include "CAttributes.h"
#include "ILightManager.h"
#include "VideoDriverAwareMixin.h"

namespace irr
{
namespace io
{
	class IXMLWriter;
	class IFileSystem;
}
namespace scene
{
	class IMeshCache;
	class IGeometryCreator;

	/*!
		The Scene Manager manages scene nodes, mesh recources, cameras and all the other stuff.
	*/
	class CSceneManager : public ISceneManager, public ISceneNode
	{
	public:

		//! constructor
		CSceneManager(boost::shared_ptr<video::IVideoDriver> driver, boost::shared_ptr<io::IFileSystem> fs,
			boost::shared_ptr<gui::ICursorControl> cursorControl, boost::shared_ptr<IMeshCache> cache = 0,
			boost::shared_ptr<gui::IGUIEnvironment> guiEnvironment = 0);

		virtual void setWeakThis(boost::shared_ptr<CSceneManager> value);

		//! destructor
		virtual ~CSceneManager();

		//! gets an animateable mesh. loads it if needed. returned pointer must not be dropped.
		virtual boost::shared_ptr<IAnimatedMesh> getMesh(const io::path& filename);

		//! gets an animateable mesh. loads it if needed. returned pointer must not be dropped.
		virtual boost::shared_ptr<IAnimatedMesh> getMesh(boost::shared_ptr<io::IReadFile> file);

		//! Returns an interface to the mesh cache which is shared beween all existing scene managers.
		virtual boost::shared_ptr<IMeshCache> getMeshCache();

		//! return the gui environment
		virtual boost::shared_ptr<gui::IGUIEnvironment> getGUIEnvironment();

		//! return the filesystem
		virtual boost::shared_ptr<io::IFileSystem> getFileSystem();

		//! adds Volume Lighting Scene Node.
		//! the returned pointer must not be dropped.
		virtual boost::shared_ptr<IVolumeLightSceneNode> addVolumeLightSceneNode(boost::shared_ptr<ISceneNode> parent=0, s32 id=-1,
			const u32 subdivU = 32, const u32 subdivV = 32,
			const video::SColor foot = video::SColor(51, 0, 230, 180),
			const video::SColor tail = video::SColor(0, 0, 0, 0),
			const core::vector3df& position = core::vector3df(0,0,0),
			const core::vector3df& rotation = core::vector3df(0,0,0),
			const core::vector3df& scale = core::vector3df(1.0f, 1.0f, 1.0f));

		//! adds a cube scene node to the scene. It is a simple cube of (1,1,1) size.
		//! the returned pointer must not be dropped.
		virtual boost::shared_ptr<IMeshSceneNode> addCubeSceneNode(f32 size=10.0f, boost::shared_ptr<ISceneNode> parent=0, s32 id=-1,
			const core::vector3df& position = core::vector3df(0,0,0),	const core::vector3df& rotation = core::vector3df(0,0,0),	const core::vector3df& scale = core::vector3df(1.0f, 1.0f, 1.0f));

		//! Adds a sphere scene node to the scene.
		virtual boost::shared_ptr<IMeshSceneNode> addSphereSceneNode(f32 radius=5.0f, s32 polyCount=16, boost::shared_ptr<ISceneNode> parent=0, s32 id=-1,
			const core::vector3df& position = core::vector3df(0,0,0),
			const core::vector3df& rotation = core::vector3df(0,0,0),
			const core::vector3df& scale = core::vector3df(1.0f, 1.0f, 1.0f));

		//! adds a scene node for rendering an animated mesh model
		virtual boost::shared_ptr<scene::IAnimatedMeshSceneNode> addAnimatedMeshSceneNode(boost::shared_ptr<IAnimatedMesh> mesh, boost::shared_ptr<ISceneNode> parent=0, s32 id=-1,
			const core::vector3df& position = core::vector3df(0,0,0),
			const core::vector3df& rotation = core::vector3df(0,0,0),
			const core::vector3df& scale = core::vector3df(1.0f, 1.0f, 1.0f),
			bool alsoAddIfMeshPointerZero=false);

		//! adds a scene node for rendering a static mesh
		//! the returned pointer must not be dropped.
		virtual boost::shared_ptr<IMeshSceneNode> addMeshSceneNode(boost::shared_ptr<IMesh> mesh, boost::shared_ptr<ISceneNode> parent=0, s32 id=-1,
			const core::vector3df& position = core::vector3df(0,0,0),
			const core::vector3df& rotation = core::vector3df(0,0,0),
			const core::vector3df& scale = core::vector3df(1.0f, 1.0f, 1.0f),
			bool alsoAddIfMeshPointerZero=false);

		//! Adds a scene node for rendering a animated water surface mesh.
		virtual boost::shared_ptr<ISceneNode> addWaterSurfaceSceneNode(boost::shared_ptr<IMesh> mesh, f32 waveHeight, f32 waveSpeed, f32 wlenght, boost::shared_ptr<ISceneNode> parent=0, s32 id=-1,
			const core::vector3df& position = core::vector3df(0,0,0),
			const core::vector3df& rotation = core::vector3df(0,0,0),
			const core::vector3df& scale = core::vector3df(1.0f, 1.0f, 1.0f));

		//! renders the node.
		virtual void render();

		//! returns the axis aligned bounding box of this node
		virtual const core::aabbox3d<f32>& getBoundingBox() const;

		//! registers a node for rendering it at a specific time.
		virtual u32 registerNodeForRendering(boost::shared_ptr<ISceneNode> node, E_SCENE_NODE_RENDER_PASS pass = ESNRP_AUTOMATIC);

		//! draws all scene nodes
		virtual void drawAll();

		//! Adds a scene node for rendering using a octree to the scene graph. This a good method for rendering
		//! scenes with lots of geometry. The Octree is built on the fly from the mesh, much
		//! faster then a bsp tree.
		virtual boost::shared_ptr<IMeshSceneNode> addOctreeSceneNode(boost::shared_ptr<IAnimatedMesh> mesh, boost::shared_ptr<ISceneNode> parent=0,
			s32 id=-1, s32 minimalPolysPerNode=512, bool alsoAddIfMeshPointerZero=false);

		//! Adss a scene node for rendering using a octree. This a good method for rendering
		//! scenes with lots of geometry. The Octree is built on the fly from the mesh, much
		//! faster then a bsp tree.
		virtual boost::shared_ptr<IMeshSceneNode> addOctreeSceneNode(boost::shared_ptr<IMesh> mesh, boost::shared_ptr<ISceneNode> parent=0,
			s32 id=-1, s32 minimalPolysPerNode=128, bool alsoAddIfMeshPointerZero=false);

		//! Adds a camera scene node to the tree and sets it as active camera.
		//! \param position: Position of the space relative to its parent where the camera will be placed.
		//! \param lookat: Position where the camera will look at. Also known as target.
		//! \param parent: Parent scene node of the camera. Can be null. If the parent moves,
		//! the camera will move too.
		//! \return Pointer to interface to camera
		virtual boost::shared_ptr<ICameraSceneNode> addCameraSceneNode(boost::shared_ptr<ISceneNode> parent = 0,
			const core::vector3df& position = core::vector3df(0,0,0),
			const core::vector3df& lookat = core::vector3df(0,0,100),
			s32 id=-1, bool makeActive=true);

		//! Adds a camera scene node which is able to be controlle with the mouse similar
		//! like in the 3D Software Maya by Alias Wavefront.
		//! The returned pointer must not be dropped.
		virtual boost::shared_ptr<ICameraSceneNode> addCameraSceneNodeMaya(boost::shared_ptr<ISceneNode> parent=0,
			f32 rotateSpeed=-1500.f, f32 zoomSpeed=200.f,
			f32 translationSpeed=1500.f, s32 id=-1, f32 distance=70.f,
			bool makeActive=true);

		//! Adds a camera scene node which is able to be controled with the mouse and keys
		//! like in most first person shooters (FPS):
		virtual boost::shared_ptr<ICameraSceneNode> addCameraSceneNodeFPS(boost::shared_ptr<ISceneNode> parent = 0,
			f32 rotateSpeed = 100.0f, f32 moveSpeed = .5f, s32 id=-1,
			SKeyMap* keyMapArray=0, s32 keyMapSize=0,
			bool noVerticalMovement=false, f32 jumpSpeed = 0.f,
			bool invertMouseY=false, bool makeActive=true);

		boost::shared_ptr<ICameraSceneNode> addCameraSceneNodeEditor() override;

		boost::shared_ptr<ICameraSceneNode> addCameraSceneNodeThirdPerson(boost::shared_ptr<ISceneNode> parent) override;

		//! Adds a dynamic light scene node. The light will cast dynamic light on all
		//! other scene nodes in the scene, which have the material flag video::MTF_LIGHTING
		//! turned on. (This is the default setting in most scene nodes).
		virtual boost::shared_ptr<scene::ILightSceneNode> addLightSceneNode(boost::shared_ptr<ISceneNode> parent = 0,
			const core::vector3df& position = core::vector3df(0,0,0),
			video::SColorf color = video::SColorf(1.0f, 1.0f, 1.0f),
			f32 range=100.0f, s32 id=-1);

		//! Adds a billboard scene node to the scene. A billboard is like a 3d sprite: A 2d element,
		//! which always looks to the camera. It is usually used for things like explosions, fire,
		//! lensflares and things like that.
		virtual boost::shared_ptr<IBillboardSceneNode> addBillboardSceneNode(boost::shared_ptr<ISceneNode> parent = 0,
			const core::dimension2d<f32>& size = core::dimension2d<f32>(10.0f, 10.0f),
			const core::vector3df& position = core::vector3df(0,0,0), s32 id=-1,
			video::SColor shadeTop = 0xFFFFFFFF, video::SColor shadeBottom = 0xFFFFFFFF);

		//! Adds a skybox scene node. A skybox is a big cube with 6 textures on it and
		//! is drawn around the camera position.
		virtual boost::shared_ptr<ISceneNode> addSkyBoxSceneNode(boost::shared_ptr<video::ITexture> top, boost::shared_ptr<video::ITexture> bottom,
			boost::shared_ptr<video::ITexture> left, boost::shared_ptr<video::ITexture> right, boost::shared_ptr<video::ITexture> front,
			boost::shared_ptr<video::ITexture> back, boost::shared_ptr<ISceneNode> parent = 0, s32 id=-1);

		//! Adds a skydome scene node. A skydome is a large (half-) sphere with a
		//! panoramic texture on it and is drawn around the camera position.
		virtual boost::shared_ptr<ISceneNode> addSkyDomeSceneNode(boost::shared_ptr<video::ITexture> texture,
			u32 horiRes=16, u32 vertRes=8,
			f32 texturePercentage=0.9, f32 spherePercentage=2.0,f32 radius = 1000.f,
			boost::shared_ptr<ISceneNode> parent=0, s32 id=-1);

		//! Adds a text scene node, which is able to display
		//! 2d text at a position in three dimensional space
		virtual boost::shared_ptr<ITextSceneNode> addTextSceneNode(boost::shared_ptr<gui::IGUIFont> font, const wchar_t* text,
			video::SColor color=video::SColor(100,255,255,255),
			boost::shared_ptr<ISceneNode> parent = 0,	const core::vector3df& position = core::vector3df(0,0,0),
			s32 id=-1);

		//! Adds a text scene node, which uses billboards
		virtual boost::shared_ptr<IBillboardTextSceneNode> addBillboardTextSceneNode(boost::shared_ptr<gui::IGUIFont> font, const wchar_t* text,
			boost::shared_ptr<ISceneNode> parent = 0,
			const core::dimension2d<f32>& size = core::dimension2d<f32>(10.0f, 10.0f),
			const core::vector3df& position = core::vector3df(0,0,0), s32 id=-1,
			video::SColor colorTop = 0xFFFFFFFF, video::SColor colorBottom = 0xFFFFFFFF);

		//! Adds a scene node, which can render a quake3 shader
		virtual boost::shared_ptr<IMeshSceneNode> addQuake3SceneNode(const IMeshBuffer* meshBuffer, const boost::shared_ptr<quake3::IShader>  shader,
												boost::shared_ptr<ISceneNode> parent=0, s32 id=-1
												);


		//! Adds a Hill Plane mesh to the mesh pool. The mesh is
		//! generated on the fly and looks like a plane with some hills
		//! on it. You can specify how many hills should be on the plane
		//! and how high they should be. Also you must specify a name
		//! for the mesh because the mesh is added to the mesh pool and
		//! can be retrieved back using ISceneManager::getMesh with the
		//! name as parameter.
		virtual boost::shared_ptr<IAnimatedMesh> addHillPlaneMesh(const io::path& name,
			const core::dimension2d<f32>& tileSize, const core::dimension2d<u32>& tileCount,
			video::SMaterial* material = 0,	f32 hillHeight = 0.0f,
			const core::dimension2d<f32>& countHills = core::dimension2d<f32>(1.0f, 1.0f),
			const core::dimension2d<f32>& textureRepeatCount = core::dimension2d<f32>(1.0f, 1.0f));

		//! Adds a terrain mesh to the mesh pool.
		virtual boost::shared_ptr<IAnimatedMesh> addTerrainMesh(const io::path& meshname,	boost::shared_ptr<video::IImage> texture, boost::shared_ptr<video::IImage> heightmap,
			const core::dimension2d<f32>& stretchSize = core::dimension2d<f32>(10.0f,10.0f),
			f32 maxHeight=200.0f,
			const core::dimension2d<u32>& defaultVertexBlockSize = core::dimension2d<u32>(64,64));

		//! Add a arrow mesh to the mesh pool
		virtual boost::shared_ptr<IAnimatedMesh> addArrowMesh(const io::path& name,
				video::SColor vtxColor0, video::SColor vtxColor1,
				u32 tesselationCylinder, u32 tesselationCone,
				f32 height, f32 cylinderHeight, f32 width0,
				f32 width1);

		//! Adds a static sphere mesh to the mesh pool.
		boost::shared_ptr<IAnimatedMesh> addSphereMesh(const io::path& name,
				f32 radius=5.f, u32 polyCountX=16, u32 polyCountY=16);

		//! Adds a static volume light mesh to the mesh pool.
		boost::shared_ptr<IAnimatedMesh> addVolumeLightMesh(const io::path& name,
			const u32 SubdivideU = 32, const u32 SubdivideV = 32,
			const video::SColor FootColor = video::SColor(51, 0, 230, 180),
			const video::SColor TailColor = video::SColor(0, 0, 0, 0));

		//! Adds a particle system scene node.
		virtual boost::shared_ptr<IParticleSystemSceneNode> addParticleSystemSceneNode(
			bool withDefaultEmitter=true, boost::shared_ptr<ISceneNode> parent=0, s32 id=-1,
			const core::vector3df& position = core::vector3df(0,0,0),
			const core::vector3df& rotation = core::vector3df(0,0,0),
			const core::vector3df& scale = core::vector3df(1.0f, 1.0f, 1.0f));

		//! Adds a terrain scene node to the scene graph.
		virtual boost::shared_ptr<ITerrainSceneNode> addTerrainSceneNode(
			const io::path& heightMapFileName,
			boost::shared_ptr<ISceneNode> parent=0, s32 id=-1,
			const core::vector3df& position = core::vector3df(0.0f,0.0f,0.0f),
			const core::vector3df& rotation = core::vector3df(0.0f,0.0f,0.0f),
			const core::vector3df& scale = core::vector3df(1.0f,1.0f,1.0f),
			video::SColor vertexColor = video::SColor(255,255,255,255),
			s32 maxLOD=4, E_TERRAIN_PATCH_SIZE patchSize=ETPS_17,s32 smoothFactor=0,
			bool addAlsoIfHeightmapEmpty = false);

		//! Adds a terrain scene node to the scene graph.
		virtual boost::shared_ptr<ITerrainSceneNode> addTerrainSceneNode(
			boost::shared_ptr<io::IReadFile> heightMap,
			boost::shared_ptr<ISceneNode> parent=0, s32 id=-1,
			const core::vector3df& position = core::vector3df(0.0f,0.0f,0.0f),
			const core::vector3df& rotation = core::vector3df(0.0f,0.0f,0.0f),
			const core::vector3df& scale = core::vector3df(1.0f,1.0f,1.0f),
			video::SColor vertexColor = video::SColor(255,255,255,255),
			s32 maxLOD=4, E_TERRAIN_PATCH_SIZE patchSize=ETPS_17,s32 smoothFactor=0,
			bool addAlsoIfHeightmapEmpty=false);

		//! Adds a dummy transformation scene node to the scene graph.
		virtual boost::shared_ptr<IDummyTransformationSceneNode> addDummyTransformationSceneNode(
			boost::shared_ptr<ISceneNode> parent=0, s32 id=-1);

		//! Adds an empty scene node.
		virtual boost::shared_ptr<ISceneNode> addEmptySceneNode(boost::shared_ptr<ISceneNode> parent, s32 id=-1);

		//! Returns the root scene node. This is the scene node wich is parent
		//! of all scene nodes. The root scene node is a special scene node which
		//! only exists to manage all scene nodes. It is not rendered and cannot
		//! be removed from the scene.
		//! \return Pointer to the root scene node.
		virtual boost::shared_ptr<ISceneNode> getRootSceneNode();

		//! Returns the current active camera.
		//! \return The active camera is returned. Note that this can be NULL, if there
		//! was no camera created yet.
		virtual boost::shared_ptr<ICameraSceneNode> getActiveCamera() const;

		//! Sets the active camera. The previous active camera will be deactivated.
		//! \param camera: The new camera which should be active.
		virtual void setActiveCamera(boost::shared_ptr<ICameraSceneNode> camera);

		//! creates a rotation animator, which rotates the attached scene node around itself.
		//! \param rotationPerSecond: Specifies the speed of the animation
		//! \return The animator. Attach it to a scene node with ISceneNode::addAnimator()
		//! and the animator will animate it.
		virtual boost::shared_ptr<ISceneNodeAnimator> createRotationAnimator(const core::vector3df& rotationPerSecond);

		//! creates a fly circle animator
		/** Lets the attached scene node fly around a center.
		\param center Center relative to node origin
		 \param speed: The orbital speed, in radians per millisecond.
		 \param direction: Specifies the upvector used for alignment of the mesh.
		 \param startPosition: The position on the circle where the animator will
			begin. Value is in multiples  of a circle, i.e. 0.5 is half way around.
		 \return The animator. Attach it to a scene node with ISceneNode::addAnimator()
		 */
		virtual boost::shared_ptr<ISceneNodeAnimator> createFlyCircleAnimator(
				const core::vector3df& center=core::vector3df(0.f, 0.f, 0.f),
				f32 radius=100.f, f32 speed=0.001f,
				const core::vector3df& direction=core::vector3df(0.f, 1.f, 0.f),
				f32 startPosition = 0.f,
				f32 radiusEllipsoid = 0.f);

		//! Creates a fly straight animator, which lets the attached scene node
		//! fly or move along a line between two points.
		virtual boost::shared_ptr<ISceneNodeAnimator> createFlyStraightAnimator(const core::vector3df& startPoint,
			const core::vector3df& endPoint, u32 timeForWay, bool loop=false,bool pingpong = false);

		//! Creates a texture animator, which switches the textures of the target scene
		//! node based on a list of textures.
		virtual boost::shared_ptr<ISceneNodeAnimator> createTextureAnimator(const core::array<boost::shared_ptr<video::ITexture>>& textures,
			s32 timePerFrame, bool loop);

		//! Creates a scene node animator, which deletes the scene node after
		//! some time automaticly.
		virtual boost::shared_ptr<ISceneNodeAnimator> createDeleteAnimator(u32 timeMS);


		//! Creates a special scene node animator for doing automatic collision detection
		//! and response.
		virtual boost::shared_ptr<ISceneNodeAnimatorCollisionResponse> createCollisionResponseAnimator(
			boost::shared_ptr<ITriangleSelector> world, boost::shared_ptr<ISceneNode> sceneNode,
			const core::vector3df& ellipsoidRadius = core::vector3df(30,60,30),
			const core::vector3df& gravityPerSecond = core::vector3df(0,-1.0f,0),
			const core::vector3df& ellipsoidTranslation = core::vector3df(0,0,0),
			f32 slidingValue = 0.0005f);

		//! Creates a follow spline animator.
		virtual boost::shared_ptr<ISceneNodeAnimator> createFollowSplineAnimator(s32 startTime,
			const core::array< core::vector3df >& points,
			f32 speed, f32 tightness, bool loop, bool pingpong);


		//! Creates a simple ITriangleSelector, based on a mesh.
		virtual boost::shared_ptr<ITriangleSelector> createTriangleSelector(boost::shared_ptr<IMesh> mesh, boost::shared_ptr<ISceneNode> node);

		//! Creates a simple ITriangleSelector, based on an animated mesh scene node.
		//! Details of the mesh associated with the node will be extracted internally.
		//! Call ITriangleSelector::update() to have the triangle selector updated based
		//! on the current frame of the animated mesh scene node.
		//! \param: The animated mesh scene node from which to build the selector
		virtual boost::shared_ptr<ITriangleSelector> createTriangleSelector(boost::shared_ptr<scene::IAnimatedMeshSceneNode> node);

		//! Creates a simple ITriangleSelector, based on a mesh.
		virtual boost::shared_ptr<ITriangleSelector> createOctreeTriangleSelector(boost::shared_ptr<IMesh> mesh,
			boost::shared_ptr<ISceneNode> node, s32 minimalPolysPerNode);

		//! Creates a simple dynamic ITriangleSelector, based on a axis aligned bounding box.
		virtual boost::shared_ptr<ITriangleSelector> createTriangleSelectorFromBoundingBox(
			boost::shared_ptr<ISceneNode> node);

		//! Creates a meta triangle selector.
		virtual boost::shared_ptr<IMetaTriangleSelector> createMetaTriangleSelector();

		//! Creates a triangle selector which can select triangles from a terrain scene node
		//! \param: Pointer to the created terrain scene node
		//! \param: Level of detail, 0 for highest detail.
		virtual boost::shared_ptr<ITriangleSelector> createTerrainTriangleSelector(
			boost::shared_ptr<ITerrainSceneNode> node, s32 LOD=0);

		//! Adds an external mesh loader.
		virtual void addExternalMeshLoader(boost::shared_ptr<IMeshLoader> externalLoader);

		//! Returns the number of mesh loaders supported by Irrlicht at this time
		virtual u32 getMeshLoaderCount() const;

		//! Retrieve the given mesh loader
		virtual boost::shared_ptr<IMeshLoader> getMeshLoader(u32 index) const;

		//! Adds an external scene loader.
		virtual void addExternalSceneLoader(boost::shared_ptr<ISceneLoader> externalLoader);

		//! Returns the number of scene loaders supported by Irrlicht at this time
		virtual u32 getSceneLoaderCount() const;

		//! Retrieve the given scene loader
		virtual boost::shared_ptr<ISceneLoader> getSceneLoader(u32 index) const;

		//! Returns a pointer to the scene collision manager.
		virtual boost::shared_ptr<ISceneCollisionManager> getSceneCollisionManager();

		//! Returns a pointer to the mesh manipulator.
		virtual boost::shared_ptr<IMeshManipulator> getMeshManipulator();

		//! Sets the color of stencil buffers shadows drawn by the scene manager.
		virtual void setShadowColor(video::SColor color);

		//! Returns the current color of shadows.
		virtual video::SColor getShadowColor() const;

		//! Adds a scene node to the deletion queue.
		virtual void addToDeletionQueue(boost::shared_ptr<ISceneNode> node);

		//! Returns the first scene node with the specified id.
		virtual boost::shared_ptr<ISceneNode> getSceneNodeFromId(s32 id, boost::shared_ptr<ISceneNode> start=0);

		//! Returns the first scene node with the specified name.
		virtual boost::shared_ptr<ISceneNode> getSceneNodeFromName(const c8* name, boost::shared_ptr<ISceneNode> start=0);

		//! Returns the first scene node with the specified type.
		virtual boost::shared_ptr<ISceneNode> getSceneNodeFromType(scene::ESCENE_NODE_TYPE type, boost::shared_ptr<ISceneNode> start=0);

		//! returns scene nodes by type.
		virtual void getSceneNodesFromType(ESCENE_NODE_TYPE type, core::array<boost::shared_ptr<scene::ISceneNode>>& outNodes, boost::shared_ptr<ISceneNode> start=0);

		//! Posts an input event to the environment. Usually you do not have to
		//! use this method, it is used by the internal engine.
		virtual bool postEventFromUser(const SEvent& event);

		//! Clears the whole scene. All scene nodes are removed.
		virtual void clear();

		//! Removes all children of this scene node
		virtual void removeAll();

		//! Returns interface to the parameters set in this scene.
		virtual io::IAttributes* getParameters();

		//! Returns current render pass.
		virtual E_SCENE_NODE_RENDER_PASS getSceneNodeRenderPass() const;

		//! Creates a new scene manager.
		virtual boost::shared_ptr<scene::ISceneManager> createNewSceneManager(bool cloneContent);

		//! Returns type of the scene node
		virtual ESCENE_NODE_TYPE getType() const { return ESNT_SCENE_MANAGER; }

		//! Returns the default scene node factory which can create all built in scene nodes
		virtual boost::shared_ptr<ISceneNodeFactory> getDefaultSceneNodeFactory();

		//! Adds a scene node factory to the scene manager.
		/** Use this to extend the scene manager with new scene node types which it should be
		able to create automaticly, for example when loading data from xml files. */
		virtual void registerSceneNodeFactory(boost::shared_ptr<ISceneNodeFactory> factoryToAdd);

		//! Returns amount of registered scene node factories.
		virtual u32 getRegisteredSceneNodeFactoryCount() const;

		//! Returns a scene node factory by index
		virtual boost::shared_ptr<ISceneNodeFactory> getSceneNodeFactory(u32 index);

		//! Returns a typename from a scene node type or null if not found
		virtual const c8* getSceneNodeTypeName(ESCENE_NODE_TYPE type);

		//! Returns a typename from a scene node animator type or null if not found
		virtual const c8* getAnimatorTypeName(ESCENE_NODE_ANIMATOR_TYPE type);

		//! Adds a scene node to the scene by name
		virtual boost::shared_ptr<ISceneNode> addSceneNode(const char* sceneNodeTypeName, boost::shared_ptr<ISceneNode> parent=0);

		//! creates a scene node animator based on its type name
		virtual boost::shared_ptr<ISceneNodeAnimator> createSceneNodeAnimator(const char* typeName, boost::shared_ptr<ISceneNode> target=0);

		//! Returns the default scene node animator factory which can create all built-in scene node animators
		virtual ISceneNodeAnimatorFactory* getDefaultSceneNodeAnimatorFactory();

		//! Adds a scene node animator factory to the scene manager.
		virtual void registerSceneNodeAnimatorFactory(ISceneNodeAnimatorFactory* factoryToAdd);

		//! Returns amount of registered scene node animator factories.
		virtual u32 getRegisteredSceneNodeAnimatorFactoryCount() const;

		//! Returns a scene node animator factory by index
		virtual ISceneNodeAnimatorFactory* getSceneNodeAnimatorFactory(u32 index);

		//! Saves the current scene into a file.
		virtual bool saveScene(const io::path& filename, ISceneUserDataSerializer* userDataSerializer=0, boost::shared_ptr<ISceneNode> node=0);

		//! Saves the current scene into a file.
		virtual bool saveScene(boost::shared_ptr<io::IWriteFile> file, ISceneUserDataSerializer* userDataSerializer=0, boost::shared_ptr<ISceneNode> node=0);

		//! Saves the current scene into a file.
		virtual bool saveScene(io::IXMLWriter* writer, const io::path& currentPath, ISceneUserDataSerializer* userDataSerializer=0, boost::shared_ptr<ISceneNode> node=0);

		//! Loads a scene. Note that the current scene is not cleared before.
		virtual bool loadScene(const io::path& filename, ISceneUserDataSerializer* userDataSerializer=0, boost::shared_ptr<ISceneNode> rootNode=0);

		//! Loads a scene. Note that the current scene is not cleared before.
		virtual bool loadScene(boost::shared_ptr<io::IReadFile> file, ISceneUserDataSerializer* userDataSerializer=0, boost::shared_ptr<ISceneNode> rootNode=0);

		//! Writes attributes of the scene node.
		virtual void serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options=0);

		//! Reads attributes of the scene node.
		virtual void deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options=0);

		//! Returns a mesh writer implementation if available
		virtual boost::shared_ptr<IMeshWriter> createMeshWriter(EMESH_WRITER_TYPE type);

		//! Get a skinned mesh, which is not available as header-only code
		virtual boost::shared_ptr<ISkinnedMesh> createSkinnedMesh();

		//! Sets ambient color of the scene
		virtual void setAmbientLight(const video::SColorf &ambientColor);

		//! Returns ambient color of the scene
		virtual const video::SColorf& getAmbientLight() const;

		//! Register a custom callbacks manager which gets callbacks during scene rendering.
		virtual void setLightManager(boost::shared_ptr<ILightManager> lightManager);

		//! Get current render time.
		virtual E_SCENE_NODE_RENDER_PASS getCurrentRendertime() const { return CurrentRendertime; }

		//! Set current render time.
		virtual void setCurrentRendertime(E_SCENE_NODE_RENDER_PASS currentRendertime) { CurrentRendertime = currentRendertime; }

		//! Get an instance of a geometry creator.
		virtual boost::shared_ptr<const IGeometryCreator> getGeometryCreator(void) const { return GeometryCreator; }

		//! returns if node is culled
		virtual bool isCulled(const boost::shared_ptr<ISceneNode> node);

	private:

		//! clears the deletion list
		void clearDeletionList();

		//! writes a scene node
		void writeSceneNode(io::IXMLWriter* writer, boost::shared_ptr<ISceneNode> node, ISceneUserDataSerializer* userDataSerializer, const fschar_t* currentPath=0, bool init=false);

		struct DefaultNodeEntry
		{
			DefaultNodeEntry(boost::shared_ptr<ISceneNode> n) :
				Node(n), TextureValue(0)
			{
				if (n->getMaterialCount())
					TextureValue = n->getMaterial(0).getTexture(0).get();
			}

			bool operator < (const DefaultNodeEntry& other) const
			{
				return (TextureValue < other.TextureValue);
			}

			boost::shared_ptr<ISceneNode> Node;
			private:
			void* TextureValue;
		};

		//! sort on distance (center) to camera
		struct TransparentNodeEntry
		{
			TransparentNodeEntry(boost::shared_ptr<ISceneNode> n, const core::vector3df& camera)
				: Node(n)
			{
				Distance = Node->getAbsoluteTransformation().getTranslation().getDistanceFromSQ(camera);
			}

			bool operator < (const TransparentNodeEntry& other) const
			{
				return Distance > other.Distance;
			}

			boost::shared_ptr<ISceneNode> Node;
			private:
				f64 Distance;
		};

		//! sort on distance (sphere) to camera
		struct DistanceNodeEntry
		{
			DistanceNodeEntry(boost::shared_ptr<ISceneNode> n, const core::vector3df& cameraPos)
				: Node(n)
			{
				setNodeAndDistanceFromPosition(n, cameraPos);
			}

			bool operator < (const DistanceNodeEntry& other) const
			{
				return Distance < other.Distance;
			}

			void setNodeAndDistanceFromPosition(boost::shared_ptr<ISceneNode> n, const core::vector3df & fromPosition)
			{
				Node = n;
				Distance = Node->getAbsoluteTransformation().getTranslation().getDistanceFromSQ(fromPosition);
				Distance -= Node->getBoundingBox().getExtent().getLengthSQ() * 0.5;
			}

			boost::shared_ptr<ISceneNode> Node;
			private:
			f64 Distance;
		};

		//! file system
		boost::shared_ptr<io::IFileSystem> FileSystem;

		//! GUI Enviroment ( Debug Purpose )
		boost::shared_ptr<gui::IGUIEnvironment> GUIEnvironment;

		//! cursor control
		boost::shared_ptr<gui::ICursorControl> CursorControl;

		//! collision manager
		boost::shared_ptr<ISceneCollisionManager> CollisionManager;

		//! render pass lists
		core::array<boost::shared_ptr<ISceneNode>> CameraList;
		core::array<boost::shared_ptr<ISceneNode>> LightList;
		core::array<boost::shared_ptr<ISceneNode>> ShadowNodeList;
		core::array<boost::shared_ptr<ISceneNode>> SkyBoxList;
		core::array<DefaultNodeEntry> SolidNodeList;
		core::array<TransparentNodeEntry> TransparentNodeList;
		core::array<TransparentNodeEntry> TransparentEffectNodeList;

		core::array<boost::shared_ptr<IMeshLoader>> MeshLoaderList;
		core::array<boost::shared_ptr<ISceneLoader>> SceneLoaderList;
		core::array<boost::shared_ptr<ISceneNode>> DeletionList;
		core::array<boost::shared_ptr<ISceneNodeFactory>> SceneNodeFactoryList;
		core::array<ISceneNodeAnimatorFactory*> SceneNodeAnimatorFactoryList;

		//! current active camera
		boost::shared_ptr<ICameraSceneNode> ActiveCamera;
		core::vector3df camWorldPos; // Position of camera for transparent nodes.

		video::SColor ShadowColor;
		video::SColorf AmbientLight;

		//! String parameters
		// NODE: Attributes are slow and should only be used for debug-info and not in release
		io::CAttributes Parameters;

		//! Mesh cache
		boost::shared_ptr<IMeshCache> MeshCache;

		E_SCENE_NODE_RENDER_PASS CurrentRendertime;

		//! An optional callbacks manager to allow the user app finer control
		//! over the scene lighting and rendering.
		boost::shared_ptr<ILightManager> LightManager;

		//! constants for reading and writing XML.
		//! Not made static due to portability problems.
		const core::stringw IRR_XML_FORMAT_SCENE;
		const core::stringw IRR_XML_FORMAT_NODE;
		const core::stringw IRR_XML_FORMAT_NODE_ATTR_TYPE;

		boost::shared_ptr<IGeometryCreator> GeometryCreator;
	};

} // end namespace video
} // end namespace scene

#endif

