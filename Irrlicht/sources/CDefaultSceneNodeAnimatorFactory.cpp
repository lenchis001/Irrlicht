// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "CDefaultSceneNodeAnimatorFactory.h"
#include "CSceneNodeAnimatorCameraFPS.h"
#include "CSceneNodeAnimatorCameraMaya.h"
#include "CSceneNodeAnimatorCameraEditor.h"
#include "CSceneNodeAnimatorCameraThirdPerson.h"
#include "ICursorControl.h"
#include "ISceneNodeAnimatorCollisionResponse.h"
#include "ISceneManager.h"

namespace irr
{
namespace scene
{

//! Names for scene node types
const c8* const SceneNodeAnimatorTypeNames[] =
{
	"flyCircle",
	"flyStraight",
	"followSpline",
	"rotation",
	"texture",
	"deletion",
	"collisionResponse",
	"cameraFPS",
	"cameraMaya",
	0
};


CDefaultSceneNodeAnimatorFactory::CDefaultSceneNodeAnimatorFactory(boost::shared_ptr<scene::ISceneManager> mgr, boost::shared_ptr<gui::ICursorControl> crs)
: Manager(mgr), CursorControl(crs)
{
	#ifdef _DEBUG
	setDebugName("CDefaultSceneNodeAnimatorFactory");
	#endif
}


CDefaultSceneNodeAnimatorFactory::~CDefaultSceneNodeAnimatorFactory()
{
}


//! creates a scene node animator based on its type id
boost::shared_ptr<ISceneNodeAnimator> CDefaultSceneNodeAnimatorFactory::createSceneNodeAnimator(ESCENE_NODE_ANIMATOR_TYPE type, boost::shared_ptr<ISceneNode> target)
{
	boost::shared_ptr<scene::ISceneNodeAnimator> anim = 0;

	switch(type)
	{
	case ESNAT_FLY_CIRCLE:
		anim = Manager->createFlyCircleAnimator(core::vector3df(0,0,0), 10);
		break;
	case ESNAT_FLY_STRAIGHT:
		anim = Manager->createFlyStraightAnimator(core::vector3df(0,0,0), core::vector3df(100,100,100), 10000, true );
		break;
	case ESNAT_FOLLOW_SPLINE:
		{
			core::array<core::vector3df> points;
			points.push_back(core::vector3df(0,0,0));
			points.push_back(core::vector3df(10,5,10));
			anim = Manager->createFollowSplineAnimator(0, points);
		}
		break;
	case ESNAT_ROTATION:
		anim = Manager->createRotationAnimator(core::vector3df(0.3f,0,0));
		break;
	case ESNAT_TEXTURE:
		{
			core::array<boost::shared_ptr<video::ITexture>> textures;
			anim = Manager->createTextureAnimator(textures, 250);
		}
		break;
	case ESNAT_DELETION:
		anim = Manager->createDeleteAnimator(5000);
		break;
	case ESNAT_COLLISION_RESPONSE:
		anim = Manager->createCollisionResponseAnimator(0, target);
		break;
	case ESNAT_CAMERA_FPS:
		anim = boost::make_shared<CSceneNodeAnimatorCameraFPS>(CursorControl);
		break;
	case ESNAT_CAMERA_EDITOR:
		anim = boost::make_shared<CSceneNodeAnimatorCameraEditor>();
		break;
	case ESNAT_CAMERA_THIRD_PERSON:
		anim = boost::make_shared<CSceneNodeAnimatorCameraThirdPerson>(CursorControl);
		break;
	case ESNAT_CAMERA_MAYA:
		anim = boost::make_shared<CSceneNodeAnimatorCameraMaya>(CursorControl);
		break;
	default:
		break;
	}

	if (anim && target)
		target->addAnimator(anim);

	return anim;
}


//! creates a scene node animator based on its type name
boost::shared_ptr<ISceneNodeAnimator> CDefaultSceneNodeAnimatorFactory::createSceneNodeAnimator(const c8* typeName, boost::shared_ptr<ISceneNode> target)
{
	return createSceneNodeAnimator( getTypeFromName(typeName), target );
}


//! returns amount of scene node animator types this factory is able to create
u32 CDefaultSceneNodeAnimatorFactory::getCreatableSceneNodeAnimatorTypeCount() const
{
	return ESNAT_COUNT;
}


//! returns type of a createable scene node animator type
ESCENE_NODE_ANIMATOR_TYPE CDefaultSceneNodeAnimatorFactory::getCreateableSceneNodeAnimatorType(u32 idx) const
{
	if (idx<ESNAT_COUNT)
		return (ESCENE_NODE_ANIMATOR_TYPE)idx;
	else
		return ESNAT_UNKNOWN;
}


//! returns type name of a createable scene node animator type 
const c8* CDefaultSceneNodeAnimatorFactory::getCreateableSceneNodeAnimatorTypeName(u32 idx) const
{
	if (idx<ESNAT_COUNT)
		return SceneNodeAnimatorTypeNames[idx];
	else
		return 0;
}

//! returns type name of a createable scene node animator type 
const c8* CDefaultSceneNodeAnimatorFactory::getCreateableSceneNodeAnimatorTypeName(ESCENE_NODE_ANIMATOR_TYPE type) const
{
	// for this factory: index == type

	if (type<ESNAT_COUNT)
		return SceneNodeAnimatorTypeNames[type];
	else
		return 0;
}

ESCENE_NODE_ANIMATOR_TYPE CDefaultSceneNodeAnimatorFactory::getTypeFromName(const c8* name) const
{
	for ( u32 i=0; SceneNodeAnimatorTypeNames[i]; ++i)
		if (!strcmp(name, SceneNodeAnimatorTypeNames[i]) )
			return (ESCENE_NODE_ANIMATOR_TYPE)i;

	return ESNAT_UNKNOWN;
}


} // end namespace scene
} // end namespace irr

