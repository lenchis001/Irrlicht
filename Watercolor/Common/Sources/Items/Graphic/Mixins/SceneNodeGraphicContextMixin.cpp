#include "SceneNodeGraphicContextMixin.h"

namespace Watercolor::Common {
SceneNodeGraphicContextMixin::SceneNodeGraphicContextMixin(
    ThreadType threadType, 
    boost::shared_ptr<IFunctionsProcessingManager> functionsProcessingManager):
    _threadType(threadType)
    , _functionsProcessingManager(functionsProcessingManager)
{
}

void SceneNodeGraphicContextMixin::setSceneManager(boost::shared_ptr<irr::scene::ISceneManager> sceneManager)
{
    _sceneManager = sceneManager;
}

boost::property_tree::wptree SceneNodeGraphicContextMixin::getObjectsTree() const
{
    auto globalMap = _createMap(_sceneManager->getRootSceneNode());
    return globalMap;
}

void SceneNodeGraphicContextMixin::loadLevel(std::wstring& path)
{
    _functionsProcessingManager->addFuctionToQueue(ThreadTypes::RENDER_THREAD, [&, path]() {
        _sceneManager->clear();
        onClearComplete.callHandlers();
        onInitLevelLoad.callHandlers();

        patchWorkingDirectory(path);

        auto subScene = _sceneManager->loadScene(path.c_str());

        unpatchWorkingDirectory();

        if (subScene) {
            _activeScenePath = path;
            _sceneManager->setActiveCamera(_sceneManager->addCameraSceneNodeEditor());

            onLevelLoaded.callHandlers();
        }
    });
}

void SceneNodeGraphicContextMixin::saveLevel()
{
    onBeforeSaveLevel.callHandlers();

    _functionsProcessingManager->addFuctionToQueue(ThreadTypes::RENDER_THREAD, [&]() {
        patchWorkingDirectory(_activeScenePath);
        auto isSuccessful = _sceneManager->saveScene(_activeScenePath.c_str());
        unpatchWorkingDirectory();

        onAfterSaveLevel.callHandlers(isSuccessful);
    });
}

bool SceneNodeGraphicContextMixin::isNodeExists(std::wstring& name)
{
    return _getNodeByName(name) != nullptr;
}

void SceneNodeGraphicContextMixin::changeParent(const std::wstring& sourceName, const std::wstring& newParentName)
{
    _functionsProcessingManager->addFuctionToQueue(ThreadTypes::RENDER_THREAD, [&, sourceName, newParentName]() {
        auto source = _getNodeByName(sourceName);
        auto newParent = _getNodeByName(newParentName);

        if (source && newParent && !_getChildByName(source, newParentName)) {
            source->setParent(newParent);
            onNodePropertyChanged.callHandlers();
        }
    });
}

void SceneNodeGraphicContextMixin::setPosition(const std::wstring& itemName, const irr::core::vector3df& position)
{
    _functionsProcessingManager->addFuctionToQueue(ThreadTypes::RENDER_THREAD, [&, itemName, position]() {
        _getNodeByName(itemName)->setPosition(position);
        onNodePropertyChanged.callHandlers();
    });
}

const irr::core::vector3df& SceneNodeGraphicContextMixin::getPosition(const std::wstring& itemName)
{
    return _getNodeByName(itemName)->getPosition();
}

void SceneNodeGraphicContextMixin::setRotation(const std::wstring& itemName, const irr::core::vector3df& rotation)
{
    _functionsProcessingManager->addFuctionToQueue(ThreadTypes::RENDER_THREAD, [&, itemName, rotation]() {
        _getNodeByName(itemName)->setRotation(rotation);
        onNodePropertyChanged.callHandlers();
    });
}

const irr::core::vector3df& SceneNodeGraphicContextMixin::getRotation(const std::wstring& itemName)
{
    return _getNodeByName(itemName)->getRotation();
}

void SceneNodeGraphicContextMixin::setScale(const std::wstring& itemName, const irr::core::vector3df& scale)
{
    _functionsProcessingManager->addFuctionToQueue(ThreadTypes::RENDER_THREAD, [&, itemName, scale]() {
        _getNodeByName(itemName)->setScale(scale);
        onNodePropertyChanged.callHandlers();
    });
}

const irr::core::vector3df& SceneNodeGraphicContextMixin::getScale(const std::wstring& itemName)
{
    return _getNodeByName(itemName)->getScale();
}

//TLMM::PhysicsTypes SceneNodeGraphicContextMixin::getPhysicsType(const std::wstring& itemName) const
//{
//    return _activeSubScene.lock()->getPhysicsType(itemName);
//}

//void SceneNodeGraphicContextMixin::setPhysicsType(const std::wstring& itemName, TLMM::PhysicsTypes physicsType)
//{
//    _functionsProcessingManager->addFuctionToQueue(ThreadTypes::RENDER_THREAD, [&, itemName, physicsType]() {
//        _activeSubScene.lock()->setPhysicsType(itemName, physicsType);
//    });
//}

void SceneNodeGraphicContextMixin::setName(const std::wstring& currentName, const std::wstring& newName)
{
    _functionsProcessingManager->addFuctionToQueue(ThreadTypes::RENDER_THREAD, [&, currentName, newName]() {
        _getNodeByName(currentName)->setName(newName.c_str());
        onNodePropertyChanged.callHandlers();
    });
}

void SceneNodeGraphicContextMixin::setMaterialTexture(const std::wstring& nodeName, int materialNumber, int textureNumber, const std::wstring& path)
{
    auto node = _getNodeByName(nodeName);
    auto& material = node->getMaterial(materialNumber);

    auto newTexture = node->getSceneManager()->getVideoDriver()->getTexture(path.c_str());

    material.setTexture(textureNumber, newTexture);
}

unsigned int SceneNodeGraphicContextMixin::getMaterialsCount(const std::wstring& nodeName)
{
    return _getNodeByName(nodeName)->getMaterialCount();
}

std::vector<std::wstring> SceneNodeGraphicContextMixin::getTextures(const std::wstring& nodeName, const int materialNumber)
{
    std::vector<std::wstring> result;

    auto material = _getNodeByName(nodeName)->getMaterial(materialNumber);

    for (int i = 0; i < irr::video::MATERIAL_MAX_TEXTURES; i++) {
        auto texture = material.getTexture(i);

        std::wstring path = texture ? TUtils::toWstring(((irr::core::stringw)texture->getName()).c_str()) : L"";
        result.push_back(path);
    }

    return result;
}

bool SceneNodeGraphicContextMixin::isMaterialWaterframed(const std::wstring& nodeName, int materialNumber)
{
    auto node = _getNodeByName(nodeName);
    auto& material = node->getMaterial(materialNumber);

    return material.Wireframe;
}

void SceneNodeGraphicContextMixin::setMaterialWaterframed(const std::wstring& nodeName, int materialNumber, bool value)
{
    _getNodeByName(nodeName)->getMaterial(materialNumber).Wireframe = value;
}

bool SceneNodeGraphicContextMixin::isMaterialLighting(const std::wstring& nodeName, int materialNumber)
{
    auto node = _getNodeByName(nodeName);
    auto& material = node->getMaterial(materialNumber);

    return material.Lighting;
}

void SceneNodeGraphicContextMixin::setMaterialLighting(const std::wstring& nodeName, int materialNumber, bool value)
{
    _functionsProcessingManager->addFuctionToQueue(ThreadTypes::RENDER_THREAD, [&, nodeName, materialNumber, value]() {
        auto node = _getNodeByName(nodeName);
        auto& material = node->getMaterial(materialNumber);

        material.Lighting = value;
    });
}

irr::video::E_MATERIAL_TYPE SceneNodeGraphicContextMixin::getMaterialType(const std::wstring& nodeName, int materialNumber)
{
    auto node = _getNodeByName(nodeName);
    auto& material = node->getMaterial(materialNumber);

    return material.MaterialType;
}

void SceneNodeGraphicContextMixin::setMaterialType(const std::wstring& nodeName, int materialNumber, irr::video::E_MATERIAL_TYPE value)
{
    _functionsProcessingManager->addFuctionToQueue(ThreadTypes::RENDER_THREAD, [&, nodeName, materialNumber, value]() {
        auto node = _getNodeByName(nodeName);
        auto& material = node->getMaterial(materialNumber);

        material.MaterialType = value;
    });
}

const irr::core::vector3df SceneNodeGraphicContextMixin::getCameraTarget(std::wstring& nodeName)
{
    auto sceneItem = _getNodeByName(nodeName);

    auto cameraItem = boost::dynamic_pointer_cast<irr::scene::ICameraSceneNode>(sceneItem);

    if (cameraItem) {
        return cameraItem->getTarget();
    }
    else {
        throw std::runtime_error("Node isn't a camera.");
    }
}

void SceneNodeGraphicContextMixin::setCameraTarget(std::wstring& nodeName, irr::core::vector3df& target)
{
    _functionsProcessingManager->addFuctionToQueue(ThreadTypes::RENDER_THREAD, [&]() {
        auto sceneItem = _getNodeByName(nodeName);

        auto cameraItem = boost::dynamic_pointer_cast<irr::scene::ICameraSceneNode>(sceneItem);

        if (cameraItem) {
            cameraItem->setTarget(target);
        }
    });
}

float SceneNodeGraphicContextMixin::getLightRadius(std::wstring& nodeName)
{
    auto node = boost::dynamic_pointer_cast<irr::scene::ILightSceneNode>(_getNodeByName(nodeName));
    return node->getRadius();
}

void SceneNodeGraphicContextMixin::setLightRadius(std::wstring& nodeName, float value)
{
    _functionsProcessingManager->addFuctionToQueue(ThreadTypes::RENDER_THREAD, [&]() {
        auto node = boost::dynamic_pointer_cast<irr::scene::ILightSceneNode>(_getNodeByName(nodeName));
        node->setRadius(value);
    });
}

irr::video::E_LIGHT_TYPE SceneNodeGraphicContextMixin::getLightType(const std::wstring& nodeName)
{
    auto node = boost::dynamic_pointer_cast<irr::scene::ILightSceneNode>(_getNodeByName(nodeName));
    return node->getLightType();
}

void SceneNodeGraphicContextMixin::setLightType(const std::wstring& nodeName, irr::video::E_LIGHT_TYPE value)
{
    _functionsProcessingManager->addFuctionToQueue(ThreadTypes::RENDER_THREAD, [&]() {
        auto node = boost::dynamic_pointer_cast<irr::scene::ILightSceneNode>(_getNodeByName(nodeName));
        node->setLightType(value);
    });
}

void SceneNodeGraphicContextMixin::addSkySphere(const std::wstring& pathToFile)
{
    _functionsProcessingManager->addFuctionToQueue(ThreadTypes::RENDER_THREAD, [&, pathToFile]() {
            auto texture = this->_sceneManager->getVideoDriver()->getTexture(TUtils::toString(pathToFile).c_str());
            auto newNode = _sceneManager->addSkyDomeSceneNode(texture);
            auto newNodeName = _getFreeObjectName(L"SkySphere");
            newNode->setName(TUtils::toString(newNodeName).c_str());

            onNodeAdded.callHandlers(newNode);
        });
}

void SceneNodeGraphicContextMixin::removeNode(const std::wstring& itemName)
{
    auto existingNode = _getNodeByName(itemName);

    if (!existingNode) {
        throw L"Item with name \"" + itemName + L"\" isn\\'t exist!";
    }

    onBeforeNodeRemove.callHandlers(existingNode);

    _functionsProcessingManager->addFuctionToQueue(ThreadTypes::RENDER_THREAD, [&]() {
        bool isDone = false;

            auto destinationNode = _getChildByName(_sceneManager->getRootSceneNode(), itemName);
            if (destinationNode) {
                destinationNode->removeAll();
                destinationNode->remove();
                isDone = true;
            }

            if (isDone)
                onAfterNodeRemove.callHandlers();
        });
}

void SceneNodeGraphicContextMixin::addMesh(const std::wstring& pathToFile)
{
    _functionsProcessingManager->addFuctionToQueue(ThreadTypes::RENDER_THREAD, [&, pathToFile]() {
        auto mesh = _sceneManager->getMesh(TUtils::toString(pathToFile).c_str());
        boost::shared_ptr<irr::scene::IMeshSceneNode> newNode = nullptr;
        if (mesh) {
            newNode = _sceneManager->addMeshSceneNode(mesh);
            auto newNodeName = _getFreeObjectName(L"Mesh");
            newNode->setName(TUtils::toString(newNodeName).c_str());
        }

        onNodeAdded.callHandlers(newNode);
    });
}

void SceneNodeGraphicContextMixin::addOstreeMesh(const std::wstring& pathToFile)
{
    _functionsProcessingManager->addFuctionToQueue(ThreadTypes::RENDER_THREAD, [&, pathToFile]() {
        auto mesh = _sceneManager->getMesh(TUtils::toString(pathToFile).c_str());
        auto newNode = _sceneManager->addOctreeSceneNode(mesh);
        auto newNodeName = _getFreeObjectName(L"OstreeMesh");
        newNode->setName(TUtils::toString(newNodeName).c_str());

        onNodeAdded.callHandlers(newNode);
    });
}

void SceneNodeGraphicContextMixin::addAnimatedMesh(const std::wstring& pathToFile)
{
    _functionsProcessingManager->addFuctionToQueue(ThreadTypes::RENDER_THREAD, [&, pathToFile]() {
        auto mesh = _sceneManager->getMesh(TUtils::toString(pathToFile).c_str());
        auto sceneNode = _sceneManager->addAnimatedMeshSceneNode(mesh);
        sceneNode->setName(L"AnimatedMesh");

        onNodeAdded.callHandlers(sceneNode);
    });
}

void SceneNodeGraphicContextMixin::addWaterSurface(const std::wstring& pathToFile)
{
    _functionsProcessingManager->addFuctionToQueue(ThreadTypes::RENDER_THREAD, [&, pathToFile]() {
        auto mesh = _sceneManager->getMesh(TUtils::toString(pathToFile).c_str());
        auto newNode = _sceneManager->addWaterSurfaceSceneNode(mesh);
        auto newNodeName = _getFreeObjectName(L"Water");
        newNode->setName(TUtils::toString(newNodeName).c_str());

        onNodeAdded.callHandlers(newNode);
    });
}

void SceneNodeGraphicContextMixin::addTerrainFromHeightmap(const std::wstring& pathToFile)
{
    _functionsProcessingManager->addFuctionToQueue(ThreadTypes::RENDER_THREAD, [&, pathToFile]() {
        auto newNode = _sceneManager->addTerrainSceneNode(TUtils::toString(pathToFile).c_str());
        auto newNodeName = _getFreeObjectName(L"Terrain");
        newNode->setName(TUtils::toString(newNodeName).c_str());

        onNodeAdded.callHandlers(newNode);
    });
}

void SceneNodeGraphicContextMixin::addSkyCube()
{
    _functionsProcessingManager->addFuctionToQueue(ThreadTypes::RENDER_THREAD, [&]() {
        auto newNode = _sceneManager->addSkyBoxSceneNode(nullptr, nullptr, nullptr, nullptr, nullptr, nullptr);
        auto newNodeName = _getFreeObjectName(L"SkyBox");
        newNode->setName(TUtils::toString(newNodeName).c_str());

        onNodeAdded.callHandlers(newNode);
    });
}

void SceneNodeGraphicContextMixin::addLightSource()
{
    _functionsProcessingManager->addFuctionToQueue(ThreadTypes::RENDER_THREAD, [&]() {
        auto sceneNode = _sceneManager->addLightSceneNode();
        onNodeAdded.callHandlers(sceneNode);
    });
}

void SceneNodeGraphicContextMixin::addCamera()
{
    _functionsProcessingManager->addFuctionToQueue(ThreadTypes::RENDER_THREAD, [&]() {
        auto newNode = _sceneManager->addCameraSceneNode(_sceneManager->getRootSceneNode(), {0, 0, 0}, {0, 0, 100}, -1, false);
        auto newNodeName = _getFreeObjectName(L"Camera");
        newNode->setName(TUtils::toString(newNodeName).c_str());

        onNodeAdded.callHandlers(newNode);
        });
}

void SceneNodeGraphicContextMixin::shutdown()
{
    _sceneManager->clear();
    _sceneManager = nullptr;
}

boost::property_tree::wptree SceneNodeGraphicContextMixin::_createMap(
    boost::shared_ptr<irr::scene::ISceneNode> node) const
{
    boost::property_tree::wptree localTree;

    for (auto child : node->getChildren()) {
        if (!child->isDebugObject()) {
            auto nodeName = TUtils::toWstring((const char*)child->getName());
            if (nodeName != _internalCameraName) {
                auto subMap = _createMap(child);
                localTree.push_back({ nodeName, subMap });
            }
        }
    }

    return localTree;
}

boost::shared_ptr<irr::scene::ISceneNode> SceneNodeGraphicContextMixin::_getChildByName(
    boost::shared_ptr<irr::scene::ISceneNode> parent, 
    const std::wstring& name)
{
    if (TUtils::toWstring(parent->getName()) == name) {
        return parent;
    }

    for (auto currentChild : parent->getChildren()) {
        auto result = _getChildByName(currentChild, name);
        if (result) {
            return result;
        }
    };

    return nullptr;
}

boost::shared_ptr<irr::scene::ISceneNode> SceneNodeGraphicContextMixin::_getNodeByName(const std::wstring& name)
{
    return _getChildByName(_sceneManager->getRootSceneNode(), name);
}

std::wstring SceneNodeGraphicContextMixin::_getFreeObjectName(const std::wstring& namePattern)
{
    std::wstring name;

    boost::shared_ptr<irr::scene::ISceneNode> rootSceneNode = _sceneManager->getRootSceneNode();

    for (int i = 0;
        _getChildByName(rootSceneNode, name = (namePattern + L" " + TUtils::toWstring(i)));
        i++);

    return name;
}
}