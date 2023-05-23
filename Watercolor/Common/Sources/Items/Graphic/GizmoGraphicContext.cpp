#include "GizmoGraphicContext.h"

#include "boost/preprocessor/stringize.hpp"

#include "Gizmo/GizmoProxySceneNode/GizmoProxySceneNode.h"
#include "ThreadTypes/ThreadTypes.h"

#include "TCoreFacade.h"
#include "TUtils.h"

const std::wstring ON_NODE_GIZMO_MANIPULATION_TYPE_WAS_CHANGED_HANDLER_NAME = L"GizmoGraphicContext_11";
const std::wstring ON_INIT_COMPLETE_HANDLER_NAME = L"GizmoGraphicContext_12";
const std::wstring ON_BEFORE_SAVE_HANDLER_NAME = L"GizmoGraphicContext_13";
const std::wstring ON_BEFORE_NODE_REMOVE_HANDLER_NAME = L"GizmoGraphicContext_14";

namespace Watercolor::Common {
void GizmoGraphicContext::_onNodeWasSelected(boost::shared_ptr<irr::scene::ISceneNode> node)
{
    // clear debug data from previous selected node
    if (_selectedNode) {
        auto previousSelectedNode = _selectedNode;
        _functionsProcessingManager->addFuctionToQueue(_threadType, [=]() {
            previousSelectedNode->setDebugDataVisible(
                irr::scene::E_DEBUG_SCENE_TYPE::EDS_OFF);
            _removeGizmo();
        });
    }

    if (node) {
        _selectedNode = node;
        // if selected node exist in current scene
        _functionsProcessingManager->addFuctionToQueue(_threadType, [=]() {
            node->setDebugDataVisible(
                irr::scene::E_DEBUG_SCENE_TYPE::EDS_BBOX);
        });
    }

    onGizmoManipulationTypeChanged.callHandlers(ManipulationType::SELECT);
}

void GizmoGraphicContext::_onBeforeSave(boost::any value)
{
    _removeGizmo();
    onGizmoManipulationTypeChanged.callHandlers(ManipulationType::SELECT);
}

void GizmoGraphicContext::_onBeforeNodeRemove(boost::shared_ptr<irr::scene::ISceneNode> node)
{
    if (_selectedNode) {
        std::wstring selectedNodeName = TUtils::toWstring(_selectedNode->getName());
        if (selectedNodeName == node->getName()) {
            _functionsProcessingManager->addFuctionToQueue(_threadType, [=]() {
                _removeGizmo();
            });
            _selectedNode = nullptr;
        }
    }
}

void GizmoGraphicContext::_onSwichManipilationType(boost::any data)
{
    ManipulationType type = boost::any_cast<ManipulationType>(data);
    _manipulationType = type;

    _functionsProcessingManager->addFuctionToQueue(_threadType, [=]() {
        if (_selectedNode) {
            _removeGizmo();
            if (_manipulationType != ManipulationType::SELECT) {
                _createGizmo(_selectedNode);
            }
        }
    });
}

void GizmoGraphicContext::_onInitLevelLoading(void* _)
{
    _isGizmoProcessing = false;
    _gizmoProxy = nullptr;
    _selectedNode = nullptr;
}

boost::shared_ptr<irr::scene::ISceneNode> GizmoGraphicContext::_filterSelection(boost::shared_ptr<irr::scene::ISceneNode> node) const
{
    boost::shared_ptr<irr::scene::ISceneNode> result = nullptr;

    if (node) {
        result = node;
        if (TUtils::toWstring(node->getName()) == _internalCameraName) {
            result = nullptr;
        } else if (node == _gizmoX || node == _gizmoY || node == _gizmoZ || node == _gizmoProxy) {
            result = nullptr;
        }
    }

    return result;
}

void GizmoGraphicContext::_removeGizmo()
{
    if (_gizmoProxy) {
        _gizmoProxy->removeAll();
        _gizmoProxy->remove();

        _selectedGizmo = nullptr;
        _isGizmoProcessing = false;
    }
}

void GizmoGraphicContext::_createGizmo(boost::shared_ptr<irr::scene::ISceneNode> node)
{
    _gizmoProxy = boost::make_shared<GizmoProxySceneNode>(node);
    _gizmoProxy->setWeakThis(_gizmoProxy);

    boost::shared_ptr<irr::scene::IAnimatedMesh> arrowMeshRed = _sceneManager->addArrowMesh("x_axisArrow", irr::video::SColor(255, 255, 0, 0), irr::video::SColor(255, 255, 0, 0));
    _gizmoX = _sceneManager->addMeshSceneNode(arrowMeshRed, _gizmoProxy);
    if (_gizmoX) {
        _gizmoX->setMaterialFlag(irr::video::EMF_LIGHTING, false);
        _gizmoX->setRotation(irr::core::vector3df(0, 0, -90));
        _gizmoX->setScale(irr::core::vector3df(10, 30, 10));
        _gizmoX->setVisible(true);
        _gizmoX->setIsDebugObject(true);
    }

    boost::shared_ptr<irr::scene::IAnimatedMesh> arrowMeshGreen = _sceneManager->addArrowMesh("y_axisArrow", irr::video::SColor(255, 0, 255, 0), irr::video::SColor(255, 0, 255, 0));
    _gizmoY = _sceneManager->addMeshSceneNode(arrowMeshGreen, _gizmoProxy);
    if (_gizmoY) {
        _gizmoY->setMaterialFlag(irr::video::EMF_LIGHTING, false);
        _gizmoY->setScale(irr::core::vector3df(10, 30, 10));
        _gizmoY->setVisible(true);
        _gizmoY->setIsDebugObject(true);
    }

    boost::shared_ptr<irr::scene::IAnimatedMesh> arrowMeshBlue = _sceneManager->addArrowMesh("z_axisArrow", irr::video::SColor(255, 0, 0, 255), irr::video::SColor(255, 0, 0, 255));
    _gizmoZ = _sceneManager->addMeshSceneNode(arrowMeshBlue, _gizmoProxy);
    if (_gizmoZ) {
        _gizmoZ->setMaterialFlag(irr::video::EMF_LIGHTING, false);
        _gizmoZ->setRotation(irr::core::vector3df(90, 0, 0));
        _gizmoZ->setScale(irr::core::vector3df(10, 30, 10));
        _gizmoZ->setVisible(true);
        _gizmoZ->setIsDebugObject(true);
    }
}

void GizmoGraphicContext::_moveGizmo(irr::core::vector2di& cursorPosition)
{
    irr::core::vector3df gizmoAbsolutePosition = _selectedGizmo->getAbsolutePosition();
    const irr::core::plane3df planeXZ(gizmoAbsolutePosition, irr::core::vector3df(0.f, 1.f, 0.f));
    const irr::core::plane3df planeXY(gizmoAbsolutePosition, irr::core::vector3df(1.f, 0.f, 0.f));

    irr::core::line3df ray = _sceneManager->getSceneCollisionManager()->getRayFromScreenCoordinates(cursorPosition);
    irr::core::vector3df intersectWithPlane;

    auto relativePosition = _selectedNode->getPosition();
    if (planeXZ.getIntersectionWithLine(ray.start, ray.getVector(), intersectWithPlane)) {
        if (_selectedGizmo == _gizmoX) {
            relativePosition.X = intersectWithPlane.X;
        }

        if (_selectedGizmo == _gizmoZ) {
            relativePosition.Z = intersectWithPlane.Z;
        }
    }
    if (planeXY.getIntersectionWithLine(ray.start, ray.getVector(), intersectWithPlane)) {
        if (_selectedGizmo == _gizmoY) {
            relativePosition.Y = intersectWithPlane.Y;
        }
    }
    _selectedNode->setPosition(relativePosition);

    auto absolutePosition = _selectedNode->getAbsolutePosition();
    _gizmoProxy->setPosition(_toRelativePosition(absolutePosition, _gizmoProxy));
}

void GizmoGraphicContext::_rotateGizmo(irr::core::vector2di& cursorPosition)
{
    irr::core::vector3df gizmoAbsolutePosition = _selectedGizmo->getAbsolutePosition();
    const irr::core::plane3df planeXZ(gizmoAbsolutePosition, irr::core::vector3df(0.f, 1.f, 0.f));
    const irr::core::plane3df planeXY(gizmoAbsolutePosition, irr::core::vector3df(1.f, 0.f, 0.f));

    irr::core::line3df ray = _sceneManager->getSceneCollisionManager()->getRayFromScreenCoordinates(cursorPosition);
    irr::core::vector3df intersectWithPlane;

    auto relativeRotation = _selectedNode->getRotation();
    if (planeXZ.getIntersectionWithLine(ray.start, ray.getVector(), intersectWithPlane)) {
        if (_selectedGizmo == _gizmoX) {
            relativeRotation.X = intersectWithPlane.Z;
        }

        if (_selectedGizmo == _gizmoZ) {
            relativeRotation.Z = intersectWithPlane.X;
        }
    }
    if (planeXY.getIntersectionWithLine(ray.start, ray.getVector(), intersectWithPlane)) {
        if (_selectedGizmo == _gizmoY) {
            relativeRotation.Y = intersectWithPlane.Z;
        }
    }
    _selectedNode->setRotation(relativeRotation);
}

void GizmoGraphicContext::_scaleGizmo(irr::core::vector2di& cursorPosition)
{
    irr::core::vector3df gizmoAbsolutePosition = _selectedGizmo->getAbsolutePosition();
    const irr::core::plane3df planeXZ(gizmoAbsolutePosition, irr::core::vector3df(0.f, 1.f, 0.f));
    const irr::core::plane3df planeXY(gizmoAbsolutePosition, irr::core::vector3df(1.f, 0.f, 0.f));

    irr::core::line3df ray = _sceneManager->getSceneCollisionManager()->getRayFromScreenCoordinates(cursorPosition);
    irr::core::vector3df intersectWithPlane;

    auto relativeScale = _selectedNode->getScale();
    if (planeXZ.getIntersectionWithLine(ray.start, ray.getVector(), intersectWithPlane)) {
        if (_selectedGizmo == _gizmoX) {
            relativeScale.X = intersectWithPlane.X;
        }

        if (_selectedGizmo == _gizmoZ) {
            relativeScale.Z = intersectWithPlane.Z;
        }
    }
    if (planeXY.getIntersectionWithLine(ray.start, ray.getVector(), intersectWithPlane)) {
        if (_selectedGizmo == _gizmoY) {
            relativeScale.Y = intersectWithPlane.Y;
        }
    }
    _selectedNode->setScale(relativeScale);

    auto absolutePosition = _selectedNode->getAbsolutePosition();
    _gizmoProxy->setPosition(_toRelativePosition(absolutePosition, _gizmoProxy));
}

GizmoGraphicContext::GizmoGraphicContext(
    wxWindow* parent, 
    ThreadType threadType, 
    boost::shared_ptr<IFunctionsProcessingManager> functionsProcessingManager,
    wxSize contextSize,
    wxPoint position, 
    wxWindowID id)
    : GraphicContext(parent, threadType, functionsProcessingManager, contextSize, position, id)
{
    onGizmoManipulationTypeChanged.addEventHandler(ON_NODE_GIZMO_MANIPULATION_TYPE_WAS_CHANGED_HANDLER_NAME, boost::bind(&GizmoGraphicContext::_onSwichManipilationType, this, boost::placeholders::_1));
    onInitLevelLoad.addEventHandler(ON_INIT_COMPLETE_HANDLER_NAME, boost::bind(&GizmoGraphicContext::_onInitLevelLoading, this, boost::placeholders::_1));
    onBeforeSaveLevel.addEventHandler(ON_BEFORE_SAVE_HANDLER_NAME, boost::bind(&GizmoGraphicContext::_onBeforeSave, this, boost::placeholders::_1));
    onBeforeNodeRemove.addEventHandler(ON_BEFORE_NODE_REMOVE_HANDLER_NAME, boost::bind(&GizmoGraphicContext::_onBeforeNodeRemove, this, boost::placeholders::_1));
    _isGizmoProcessing = false;
    _gizmoProxy = nullptr;
    _selectedNode = nullptr;
}

GizmoGraphicContext::~GizmoGraphicContext()
{
    onGizmoManipulationTypeChanged.removeEventHandler(ON_NODE_GIZMO_MANIPULATION_TYPE_WAS_CHANGED_HANDLER_NAME);
    onInitLevelLoad.removeEventHandler(ON_INIT_COMPLETE_HANDLER_NAME);
    onBeforeSaveLevel.removeEventHandler(ON_BEFORE_SAVE_HANDLER_NAME);
    onBeforeNodeRemove.removeEventHandler(ON_BEFORE_NODE_REMOVE_HANDLER_NAME);
}

void GizmoGraphicContext::_selectNode(irr::core::vector2di& cursorPosition)
{
    _functionsProcessingManager->addFuctionToQueue(_threadType, [&, cursorPosition]() {
        auto node = _pickNode(cursorPosition);
        node = _filterSelection(node);

        _functionsProcessingManager->addFuctionToQueue(ThreadTypes::UI_THREAD, [&, cursorPosition, node]() {
            _onNodeWasSelected(node);
            onNodeSelected.callHandlers(node);
        });
    });
}

void GizmoGraphicContext::swichManipilationType(ManipulationType type)
{
    onGizmoManipulationTypeChanged.callHandlers(type);
}

void GizmoGraphicContext::_startGizmoProcessing(irr::core::vector2di& cursorPosition)
{
    _selectedGizmo = _pickNode(cursorPosition);

    if (_selectedGizmo && (_selectedGizmo == _gizmoX || _selectedGizmo == _gizmoY || _selectedGizmo == _gizmoZ)) {
        onManipulationStarted.callHandlers(_manipulationType);
        _isGizmoProcessing = true;
    }
}

void GizmoGraphicContext::_finishGizmoProcessing()
{
    if (_isGizmoProcessig()) {
        onManipulationFinished.callHandlers(_manipulationType);
        _isGizmoProcessing = false;
    }
}

bool GizmoGraphicContext::_processGizmo(irr::core::vector2di& cursorPosition)
{
    bool isProcessed = false;

    if (_isGizmoProcessig()) {
        switch (_manipulationType) {
        case ManipulationType::SELECT:
            break;
        case ManipulationType::MOVE:
            _moveGizmo(cursorPosition);
            isProcessed = true;
            break;
        case ManipulationType::ROTATE:
            _rotateGizmo(cursorPosition);
            isProcessed = true;
            break;
        case ManipulationType::SCALE:
            _scaleGizmo(cursorPosition);
            isProcessed = true;
            break;
        default:
            break;
        }
    }

    return isProcessed;
}

bool GizmoGraphicContext::_isGizmoProcessig() const
{
    return _isGizmoProcessing;
}

void GizmoGraphicContext::selectNode(std::wstring& name)
{
    auto selectedItem = _getChildByName(this->_sceneManager->getRootSceneNode(), name);

    _onNodeWasSelected(selectedItem);
    onNodeSelected.callHandlers(selectedItem);
}

void GizmoGraphicContext::_onLeftMouseButtonDown(wxMouseEvent& eventData)
{
    irr::SEvent irrEventData;

    irrEventData.EventType = irr::EEVENT_TYPE::EET_MOUSE_INPUT_EVENT;
    irrEventData.MouseInput.Event = irr::EMOUSE_INPUT_EVENT::EMIE_LMOUSE_PRESSED_DOWN;
    irrEventData.MouseInput.X = eventData.GetX();
    irrEventData.MouseInput.Y = eventData.GetY();
    _functionsProcessingManager->addFuctionToQueue(
        _threadType, [=]() { _device->postEventFromUser(irrEventData); });

    this->SetFocus();

    if (_manipulationType != ManipulationType::SELECT) {
        auto cursorPosition = irr::core::vector2di(eventData.GetX(), eventData.GetY());
        this->_startGizmoProcessing(cursorPosition);
    }
    onIrrEvent.callHandlers(irrEventData);
}

void GizmoGraphicContext::_onLeftMouseButtonDoubleClick(wxMouseEvent& eventData)
{
    auto cursorPosition = irr::core::vector2di(eventData.GetX(), eventData.GetY());
    this->_selectNode(cursorPosition);
}

void GizmoGraphicContext::_onLeftMouseButtonUp(wxMouseEvent& eventData)
{
    irr::SEvent irrEventData;

    irrEventData.EventType = irr::EEVENT_TYPE::EET_MOUSE_INPUT_EVENT;
    irrEventData.MouseInput.Event = irr::EMOUSE_INPUT_EVENT::EMIE_LMOUSE_LEFT_UP;
    irrEventData.MouseInput.X = eventData.GetX();
    irrEventData.MouseInput.Y = eventData.GetY();
    _functionsProcessingManager->addFuctionToQueue(
        _threadType, [=]() { _device->postEventFromUser(irrEventData); });

    this->SetFocus();

    this->_finishGizmoProcessing();
    onIrrEvent.callHandlers(irrEventData);
}

void GizmoGraphicContext::_onMouseMove(wxMouseEvent& eventData)
{
    auto cursorPosition = irr::core::vector2di(eventData.GetX(), eventData.GetY());
    bool isProcessed = this->_processGizmo(cursorPosition);

    if (!isProcessed) {
        irr::SEvent irrEventData;

        irrEventData.EventType = irr::EEVENT_TYPE::EET_MOUSE_INPUT_EVENT;
        irrEventData.MouseInput.Event = irr::EMOUSE_INPUT_EVENT::EMIE_MOUSE_MOVED;
        irrEventData.MouseInput.X = eventData.GetX();
        irrEventData.MouseInput.Y = eventData.GetY();

        if (eventData.LeftIsDown()) {
            irrEventData.MouseInput.ButtonStates |= irr::E_MOUSE_BUTTON_STATE_MASK::EMBSM_LEFT;
        }
        _functionsProcessingManager->addFuctionToQueue(
            _threadType, [=]() { _device->postEventFromUser(irrEventData); });
        onIrrEvent.callHandlers(irrEventData);
    }
}

void GizmoGraphicContext::_onShowEventHandler(wxShowEvent& _)
{
    onGizmoManipulationTypeChanged.callHandlers(ManipulationType::SELECT);
}


irr::core::vector3df GizmoGraphicContext::_toRelativePosition(irr::core::vector3df& absolutePosition, boost::shared_ptr<irr::scene::ISceneNode> sceneNode)
{
    auto nodeAbsolutePosition = sceneNode->getAbsolutePosition();
    auto result = absolutePosition - nodeAbsolutePosition;
    return result;
}

boost::shared_ptr<irr::scene::ISceneNode> GizmoGraphicContext::_pickNode(const irr::core::vector2di& screenCoodinate)
{
    auto node = _sceneManager->getSceneCollisionManager()->getSceneNodeFromScreenCoordinatesBB(screenCoodinate);
    return node;
}

#pragma warning(disable : 4407)
wxBEGIN_EVENT_TABLE(GizmoGraphicContext, GraphicContext)
    EVT_LEFT_UP(GizmoGraphicContext::_onLeftMouseButtonUp)
        EVT_LEFT_DOWN(GizmoGraphicContext::_onLeftMouseButtonDown)
            EVT_LEFT_DCLICK(GizmoGraphicContext::_onLeftMouseButtonDoubleClick)
                EVT_MOTION(GizmoGraphicContext::_onMouseMove)
                    EVT_SHOW(GizmoGraphicContext::_onShowEventHandler)
                        wxEND_EVENT_TABLE()
#pragma warning(default : 4407)
}