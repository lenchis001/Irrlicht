#pragma once

#include "string"
#include "vector"

#include "boost/property_tree/ptree.hpp"
#include "boost/shared_ptr.hpp"

#include "irrlicht.h"

#include "TUtils.h"
#include "CommonFacade.h"
#include "TCoreFacade.h"

namespace Watercolor::Common {
class SceneNodeGraphicContextMixin : private PatchableMixin {
public:
    SceneNodeGraphicContextMixin(ThreadType _threadType, boost::shared_ptr<IFunctionsProcessingManager> functionsProcessingManager);

    Utils::EventHandler<boost::shared_ptr<irr::scene::ISceneNode>> onNodeAdded;
    Utils::EventHandler<void*> onBeforeSaveLevel;
    Utils::EventHandler<bool> onAfterSaveLevel;
    Utils::EventHandler<boost::shared_ptr<irr::scene::ISceneNode>> onBeforeNodeRemove;
    Utils::EventHandler<void*> onAfterNodeRemove;
    Utils::EventHandler<void*> onLevelLoaded;
    Utils::EventHandler<void*> onInitLevelLoad;
    Utils::EventHandler<void*> onClearComplete;
    Utils::EventHandler<void*> onItemsTreeChanged;
    Utils::EventHandler<void*> onNodePropertyChanged;

    void setSceneManager(boost::shared_ptr<irr::scene::ISceneManager> sceneManager);

    boost::property_tree::wptree getObjectsTree() const;

    void loadLevel(std::wstring& path);
    void saveLevel();

    bool isNodeExists(std::wstring& name);

    void changeParent(const std::wstring& sourceName, const std::wstring& newParentName);

    void setPosition(const std::wstring& itemName, const irr::core::vector3df& position);
    const irr::core::vector3df& getPosition(const std::wstring& itemName);
    void setRotation(const std::wstring& itemName, const irr::core::vector3df& rotation);
    const irr::core::vector3df& getRotation(const std::wstring& itemName);
    void setScale(const std::wstring& itemName, const irr::core::vector3df& scale);
    const irr::core::vector3df& getScale(const std::wstring& itemName);

    //TLMM::PhysicsTypes getPhysicsType(const std::wstring& itemName) const;
    //void setPhysicsType(const std::wstring& itemName, TLMM::PhysicsTypes physicsType);

    void setName(const std::wstring& currentName, const std::wstring& newName);

    void setMaterialTexture(const std::wstring& nodeName, int materialNumber, int textureNumber, const std::wstring& path);
    unsigned int getMaterialsCount(const std::wstring& nodeName);
    std::vector<std::wstring> getTextures(const std::wstring& nodeName, const int materialNumber);

    bool isMaterialWaterframed(const std::wstring& nodeName, int materialNumber);
    void setMaterialWaterframed(const std::wstring& nodeName, int materialNumber, bool value);

    bool isMaterialLighting(const std::wstring& nodeName, int materialNumber);
    void setMaterialLighting(const std::wstring& nodeName, int materialNumber, bool value);

    irr::video::E_MATERIAL_TYPE getMaterialType(const std::wstring& nodeName, int materialNumber);
    void setMaterialType(const std::wstring& nodeName, int materialNumber, irr::video::E_MATERIAL_TYPE value);

    const irr::core::vector3df getCameraTarget(std::wstring& nodeName);
    void setCameraTarget(std::wstring& nodeName, irr::core::vector3df& target);

    float getLightRadius(std::wstring& nodeName);
    void setLightRadius(std::wstring& nodeName, float value);

    irr::video::E_LIGHT_TYPE getLightType(const std::wstring& nodeName);
    void setLightType(const std::wstring& nodeName, irr::video::E_LIGHT_TYPE value);

    void addArrow();
    void addEmpty();
    void addSkySphere(const std::wstring& pathToFile);
    void removeNode(const std::wstring& itemName);
    void addMesh(const std::wstring& pathToFile);
    void addOstreeMesh(const std::wstring& pathToFile);
    void addAnimatedMesh(const std::wstring& pathToFile);
    void addWaterSurface(const std::wstring& pathToFile);
    void addTerrainFromHeightmap(const std::wstring& pathToFile);
    void addSkyCube();
    void addLightSource();
    void addCamera();

    void shutdown();

protected:
    const std::wstring _internalCameraName = L"Internal camera";

    boost::shared_ptr<irr::scene::ISceneManager> _sceneManager;
    ThreadType _threadType;
    boost::shared_ptr<IFunctionsProcessingManager> _functionsProcessingManager;

    boost::shared_ptr<irr::scene::ISceneNode> _getChildByName(boost::shared_ptr<irr::scene::ISceneNode> parent, const std::wstring& name);

private:
    boost::property_tree::wptree _createMap(boost::shared_ptr<irr::scene::ISceneNode> node) const;

    boost::shared_ptr<irr::scene::ISceneNode> _getNodeByName(const std::wstring& name);

    std::wstring _getFreeObjectName(const std::wstring& namePattern);

    std::wstring _activeScenePath;
};
}
