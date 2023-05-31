#pragma once

#include "map"
#include "vector"

#include "wx/propgrid/propgrid.h"
#include <wx/wx.h>

#include "boost/function.hpp"

#include "irrlicht.h"

#include "Items/Graphic/GizmoGraphicContext.h"

typedef boost::function<void(wxPGProperty*)> PropertyChangeHandler;

namespace Watercolor {
class SceneNodePropertyGrid : public wxPropertyGrid {
public:
    SceneNodePropertyGrid(wxWindow* parent, std::wstring nodeName);
    ~SceneNodePropertyGrid();

    std::wstring getObjectName() const;

protected:
    std::wstring _nodeName;

    Common::GizmoGraphicContext* _graphicContext;

    void addPropertyChangeHandler(std::wstring propertyName,
        PropertyChangeHandler handler);
    void removePropertyChangeHandler(std::wstring propertyName);
    irr::core::vector3df _parse3DVector(const std::wstring& textVector);

private:
    std::map<std::wstring, PropertyChangeHandler> _changeHandlers;

    void _addPositionProperty();
    void _addRotationProperty();
    void _addScaleProperty();
    void _addNameProperty();

    void _changePositionPropertyHandler(wxPGProperty* _);
    void _changeRotationPropertyHandler(wxPGProperty* _);
    void _changeScalePropertyHandler(wxPGProperty* _);
    void _changeNamePropertyHandler(wxPGProperty* eventProperty);

    void _onPropertyWasSet(wxPropertyGridEvent& eventData);

    void _onGizmoManipulationFinished(Common::ManipulationType manipulationType);

    wxDECLARE_EVENT_TABLE();
};
}