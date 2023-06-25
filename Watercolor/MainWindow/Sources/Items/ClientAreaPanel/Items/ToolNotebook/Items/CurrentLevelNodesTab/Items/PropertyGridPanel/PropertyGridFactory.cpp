#include "PropertyGridFactory.h"

#include "TUtils.h"

#include "CameraSceneNodePropertyGrid.h"
#include "LightSceneNodePropertyGrid.h"
#include "MeshSceneNodePropertyGrid.h"
#include "TexturedSceneNodePropertyGrid.h"

namespace Watercolor {
boost::shared_ptr<SceneNodePropertyGrid> PropertyGridFactory::getGrid(
    wxWindow* parent, boost::shared_ptr<irr::scene::ISceneNode> node)
{
    boost::shared_ptr<SceneNodePropertyGrid> grid(nullptr);

    if (node) {
        std::wstring nodeName = TUtils::toWstring(node->getName());

        if (node) {
            auto nodeType = node->getType();

            switch (nodeType) {
            case irr::scene::ESNT_MESH:
            case irr::scene::ESNT_OCTREE:
            case irr::scene::ESNT_ANIMATED_MESH:
                grid = boost::make_shared<MeshSceneNodePropertyGrid>(parent, nodeName);
                break;
            case irr::scene::ESNT_TERRAIN:
            case irr::scene::ESNT_WATER_SURFACE:
            case irr::scene::ESNT_SKY_BOX:
            case irr::scene::ESNT_SKY_DOME:
            case irr::scene::ESNT_MD3_SCENE_NODE:
            case irr::scene::ESNT_CUBE:
            case irr::scene::ESNT_SPHERE:
            case irr::scene::ESNT_TEXT:
                grid = boost::make_shared<TexturedSceneNodePropertyGrid>(parent, nodeName);
                break;
            case irr::scene::ESNT_CAMERA:
            case irr::scene::ESNT_CAMERA_MAYA:
            case irr::scene::ESNT_CAMERA_FPS:
                grid = boost::make_shared<CameraSceneNodePropertyGrid>(parent, nodeName);
                break;
            case irr::scene::ESNT_SCENE_MANAGER:
            case irr::scene::ESNT_SHADOW_VOLUME:
            case irr::scene::ESNT_EMPTY:
            case irr::scene::ESNT_DUMMY_TRANSFORMATION:
            case irr::scene::ESNT_BILLBOARD:
            case irr::scene::ESNT_PARTICLE_SYSTEM:
            case irr::scene::ESNT_Q3SHADER_SCENE_NODE:
            case irr::scene::ESNT_VOLUME_LIGHT:
            case irr::scene::ESNT_UNKNOWN:
            case irr::scene::ESNT_ANY:
                grid = boost::make_shared<SceneNodePropertyGrid>(parent, nodeName);
                break;
            case irr::scene::ESNT_LIGHT:
                grid = boost::make_shared<LightSceneNodePropertyGrid>(parent, nodeName);
                break;
            default:
                break;
            }
        }
    }

    return grid;
}
}