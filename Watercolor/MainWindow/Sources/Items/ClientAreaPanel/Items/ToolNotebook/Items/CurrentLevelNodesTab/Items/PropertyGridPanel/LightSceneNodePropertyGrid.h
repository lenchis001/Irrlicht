#pragma once

#include "map"
#include "string"

#include "SceneNodePropertyGrid.h"

namespace Watercolor {
class LightSceneNodePropertyGrid : public SceneNodePropertyGrid {
public:
    LightSceneNodePropertyGrid(wxWindow* parent, std::wstring nodeName);

private:
    void _addRadiusProperty();
    void _addLightTypeProperty();

    void _changeRadiusPropertyHandler(wxPGProperty* eventProperty);
    void _changeLightTypePropertyHandler(wxPGProperty* eventProperty);

    void _initLightTypesNames();
    wxPGChoices _getLightTypeNamesChoices();

    std::wstring _getLightTypeNameByType(irr::video::E_LIGHT_TYPE lightType) const;

    std::map<std::wstring, irr::video::E_LIGHT_TYPE> _lightTypesNames;
};
}