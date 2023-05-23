#pragma once

#include "irrlicht.h"

#include "Gizmo/ManipulationType.h"

#include "Utils/EventHandler.h"
#include "GraphicContext.h"

namespace Watercolor::Common {
class GizmoGraphicContext : public GraphicContext {
public:
    Utils::EventHandler<ManipulationType> onGizmoManipulationTypeChanged;
    Utils::EventHandler<ManipulationType> onManipulationStarted;
    Utils::EventHandler<ManipulationType> onManipulationFinished;
    Utils::EventHandler<boost::shared_ptr<irr::scene::ISceneNode>> onNodeSelected;

    void swichManipilationType(ManipulationType type);
    void selectNode(std::wstring& name);

    GizmoGraphicContext(
        wxWindow* parent,
        ThreadType threadType,
        boost::shared_ptr<IFunctionsProcessingManager> functionsProcessingManager,
        wxSize contextSize = wxSize(200, 150),
        wxPoint position = wxDefaultPosition, wxWindowID id = wxID_ANY);
    ~GizmoGraphicContext();

protected:
    void _selectNode(irr::core::vector2di& cursorPosition);

    void _startGizmoProcessing(irr::core::vector2di& cursorPosition);
    bool _processGizmo(irr::core::vector2di& cursorPosition);
    void _finishGizmoProcessing();
    bool _isGizmoProcessig() const;

private:
    wxDECLARE_EVENT_TABLE();

    void _onNodeWasSelected(boost::shared_ptr<irr::scene::ISceneNode> node);
    void _onBeforeSave(boost::any value);
    void _onBeforeNodeRemove(boost::shared_ptr<irr::scene::ISceneNode> node);
    void _onSwichManipilationType(boost::any data);
    void _onInitLevelLoading(void* _);

    void _onLeftMouseButtonDoubleClick(wxMouseEvent& eventData);
    void _onLeftMouseButtonDown(wxMouseEvent& eventData);
    void _onLeftMouseButtonUp(wxMouseEvent& eventData);
    void _onMouseMove(wxMouseEvent& eventData);

    void _onShowEventHandler(wxShowEvent& _);

    boost::shared_ptr<irr::scene::ISceneNode> _filterSelection(boost::shared_ptr<irr::scene::ISceneNode> node) const;

    void _removeGizmo();
    void _createGizmo(boost::shared_ptr<irr::scene::ISceneNode> node);

    void _moveGizmo(irr::core::vector2di& cursorPosition);
    void _rotateGizmo(irr::core::vector2di& cursorPosition);
    void _scaleGizmo(irr::core::vector2di& cursorPosition);

    irr::core::vector3df _toRelativePosition(irr::core::vector3df& absolutePosition, boost::shared_ptr<irr::scene::ISceneNode> sceneNode);

    boost::shared_ptr<irr::scene::ISceneNode> _pickNode(const irr::core::vector2di& screenCoodinate);

    boost::shared_ptr<irr::scene::ISceneNode> _selectedNode, _selectedGizmo, _gizmoProxy;

    boost::shared_ptr<irr::scene::IMeshSceneNode> _gizmoX, _gizmoY, _gizmoZ;
    ManipulationType _manipulationType;
    bool _isGizmoProcessing;
};
}