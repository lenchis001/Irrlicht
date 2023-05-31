#include "LightSceneNodePropertyGrid.h"

#include "numeric"

#define LIGHT_RADIUS_PROPERTY_NAME L"LIGHT_RADIUS_PROPERTY_NAME"
#define LIGHT_TYPE_PROPERTY_NAME L"LIGHT_TYPE_PROPERTY_NAME"

namespace Watercolor {
LightSceneNodePropertyGrid::LightSceneNodePropertyGrid(wxWindow* parent, std::wstring nodeName)
    : SceneNodePropertyGrid(parent, nodeName)
{
    _addRadiusProperty();

    _initLightTypesNames();
    _addLightTypeProperty();
}

void LightSceneNodePropertyGrid::_addRadiusProperty()
{
    auto radius = _graphicContext->getLightRadius(_nodeName);
    this->Append(new wxFloatProperty(L"Name", LIGHT_RADIUS_PROPERTY_NAME, radius));

    this->addPropertyChangeHandler(
        LIGHT_RADIUS_PROPERTY_NAME, boost::bind(&LightSceneNodePropertyGrid::_changeRadiusPropertyHandler, this, boost::placeholders::_1));
}

void LightSceneNodePropertyGrid::_addLightTypeProperty()
{
    auto choices = _getLightTypeNamesChoices();
    auto currentLightTypeName = _getLightTypeNameByType(_graphicContext->getLightType(_nodeName));
    int currentLightTypeIndex = choices.Index(currentLightTypeName);

    this->Append(new wxEnumProperty("Light type", LIGHT_TYPE_PROPERTY_NAME, choices, currentLightTypeIndex));

    this->addPropertyChangeHandler(
        LIGHT_TYPE_PROPERTY_NAME, boost::bind(&LightSceneNodePropertyGrid::_changeLightTypePropertyHandler, this, boost::placeholders::_1));
}

void LightSceneNodePropertyGrid::_changeRadiusPropertyHandler(wxPGProperty* eventProperty)
{
    auto newRadius = eventProperty->GetValue().GetInteger();
    _graphicContext->setLightRadius(_nodeName, newRadius);
}

void LightSceneNodePropertyGrid::_changeLightTypePropertyHandler(wxPGProperty* eventProperty)
{
    auto newType = eventProperty->GetValue().GetInteger();

    _graphicContext->setLightType(_nodeName, static_cast<irr::video::E_LIGHT_TYPE>(newType));
}

void LightSceneNodePropertyGrid::_initLightTypesNames()
{
    _lightTypesNames.insert(
        { L"Point", irr::video::E_LIGHT_TYPE::ELT_POINT });
    _lightTypesNames.insert(
        { L"Spot", irr::video::E_LIGHT_TYPE::ELT_SPOT });
    _lightTypesNames.insert(
        { L"Directional", irr::video::E_LIGHT_TYPE::ELT_DIRECTIONAL });
}

wxPGChoices LightSceneNodePropertyGrid::_getLightTypeNamesChoices()
{
    wxPGChoices choices;

    for (const auto& materialType : _lightTypesNames) {
        choices.Add(materialType.first);
    }

    return choices;
}

std::wstring LightSceneNodePropertyGrid::_getLightTypeNameByType(irr::video::E_LIGHT_TYPE lightType) const
{
    std::wstring name;

    auto destinationType = std::find_if(
        _lightTypesNames.begin(), _lightTypesNames.end(),
        [&](auto currentType) { return currentType.second == lightType; });

    name = destinationType->first;

    return name;
}
}