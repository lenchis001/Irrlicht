#pragma once

#include "map"

#include "SceneNodePropertyGrid.h"

namespace Watercolor {
class TexturedSceneNodePropertyGrid : public SceneNodePropertyGrid {
public:
    TexturedSceneNodePropertyGrid(wxWindow* parent, std::wstring nodeName);

private:
    void _initMaterialTypesNames();
    wxPGChoices _getMaterialNamesChoices();
    std::wstring _getMaterialNameByType(irr::video::E_MATERIAL_TYPE type);

    void _addMaterialsProperty();

    void _changeMaterialTypePropertyHandler(wxPGProperty* eventProperty);
    void _changeLightingTypePropertyHandler(wxPGProperty* eventProperty);
    void _changeWireframeTypePropertyHandler(wxPGProperty* eventProperty);
    void _changeTexturePropertyHandler(wxPGProperty* eventProperty);

    std::map<std::wstring, irr::video::E_MATERIAL_TYPE> _materialTypesNames;
};
}