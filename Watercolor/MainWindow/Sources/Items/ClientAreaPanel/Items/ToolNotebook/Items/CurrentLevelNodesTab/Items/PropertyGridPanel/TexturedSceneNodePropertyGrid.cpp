#include "TexturedSceneNodePropertyGrid.h"
#include "TCoreFacade.h"
#include "TUtils.h"

#include "algorithm"

#include <wx/propgrid/advprops.h>

#define MATERIAL_PROPRTY_NAME L"Material "
#define TEXTURE_PROPRTY_NAME L" Texture "
#define WIREFRAME_PROPRTY_NAME L"Waterframe "
#define LIGHTING_PROPERTY_NAME L"Lighting "
#define MATERIAL_TYPE_PROPRTY_NAME L"Material type "

using namespace std;

namespace Watercolor {
void TexturedSceneNodePropertyGrid::_initMaterialTypesNames()
{
    _materialTypesNames.insert(
        { L"Detail map", irr::video::E_MATERIAL_TYPE::EMT_DETAIL_MAP });
    _materialTypesNames.insert(
        { L"Force 32 bit", irr::video::E_MATERIAL_TYPE::EMT_FORCE_32BIT });
    _materialTypesNames.insert(
        { L"Lightmap", irr::video::E_MATERIAL_TYPE::EMT_LIGHTMAP });
    _materialTypesNames.insert(
        { L"Lightmap add", irr::video::E_MATERIAL_TYPE::EMT_LIGHTMAP_ADD });
    _materialTypesNames.insert(
        { L"Lightmap lighting",
            irr::video::E_MATERIAL_TYPE::EMT_LIGHTMAP_LIGHTING });
    _materialTypesNames.insert(
        { L"Lightmap lighting M2",
            irr::video::E_MATERIAL_TYPE::EMT_LIGHTMAP_LIGHTING_M2 });
    _materialTypesNames.insert(
        { L"Lightmap lighting M4",
            irr::video::E_MATERIAL_TYPE::EMT_LIGHTMAP_LIGHTING_M4 });
    _materialTypesNames.insert(
        { L"Lightmap M2", irr::video::E_MATERIAL_TYPE::EMT_LIGHTMAP_M2 });
    _materialTypesNames.insert(
        { L"Lightmap M4", irr::video::E_MATERIAL_TYPE::EMT_LIGHTMAP_M4 });
    _materialTypesNames.insert(
        { L"Normal map solid", irr::video::E_MATERIAL_TYPE::EMT_NORMAL_MAP_SOLID });
    _materialTypesNames.insert(
        { L"Normal map transparent add color",
            irr::video::E_MATERIAL_TYPE::EMT_NORMAL_MAP_TRANSPARENT_ADD_COLOR });
    _materialTypesNames.insert(
        { L"Normal map vertex alpha",
            irr::video::E_MATERIAL_TYPE::EMT_NORMAL_MAP_TRANSPARENT_VERTEX_ALPHA });
    _materialTypesNames.insert(
        { L"Onetexture blend", irr::video::E_MATERIAL_TYPE::EMT_ONETEXTURE_BLEND });
    _materialTypesNames.insert(
        { L"Parralax map solid",
            irr::video::E_MATERIAL_TYPE::EMT_PARALLAX_MAP_SOLID });
    _materialTypesNames.insert(
        { L"Parallax map transparent add color",
            irr::video::E_MATERIAL_TYPE::EMT_PARALLAX_MAP_TRANSPARENT_ADD_COLOR });
    _materialTypesNames.insert(
        { L"Parallax map transparent vertex alpha",
            irr::video::E_MATERIAL_TYPE::EMT_PARALLAX_MAP_TRANSPARENT_VERTEX_ALPHA });
    _materialTypesNames.insert(
        { L"Reflection 2 layer",
            irr::video::E_MATERIAL_TYPE::EMT_REFLECTION_2_LAYER });
    _materialTypesNames.insert(
        { L"Solid", irr::video::E_MATERIAL_TYPE::EMT_SOLID });
    _materialTypesNames.insert(
        { L"Solid 2 layer", irr::video::E_MATERIAL_TYPE::EMT_SOLID_2_LAYER });
    _materialTypesNames.insert(
        { L"Sphere map", irr::video::E_MATERIAL_TYPE::EMT_SPHERE_MAP });
    _materialTypesNames.insert(
        { L"Transparent add color",
            irr::video::E_MATERIAL_TYPE::EMT_TRANSPARENT_ADD_COLOR });
    _materialTypesNames.insert(
        { L"Transparent alpha chanel",
            irr::video::E_MATERIAL_TYPE::EMT_TRANSPARENT_ALPHA_CHANNEL });
    _materialTypesNames.insert(
        { L"Transparent alpha chanel REF",
            irr::video::E_MATERIAL_TYPE::EMT_TRANSPARENT_ALPHA_CHANNEL_REF });
    _materialTypesNames.insert(
        { L"Transparent reflection 2 LAYER",
            irr::video::E_MATERIAL_TYPE::EMT_TRANSPARENT_REFLECTION_2_LAYER });
    _materialTypesNames.insert(
        { L"Transparent vertex alpha",
            irr::video::E_MATERIAL_TYPE::EMT_TRANSPARENT_VERTEX_ALPHA });
}

wxPGChoices TexturedSceneNodePropertyGrid::_getMaterialNamesChoices()
{
    wxPGChoices choices;

    for (const auto& materialType : _materialTypesNames) {
        choices.Add(materialType.first);
    }

    return choices;
}

wstring TexturedSceneNodePropertyGrid::_getMaterialNameByType(
    irr::video::E_MATERIAL_TYPE type)
{
    wstring name;

    auto destinationType = std::find_if(
        _materialTypesNames.begin(), _materialTypesNames.end(),
        [&](auto currentType) { return currentType.second == type; });

    name = destinationType->first;

    return name;
}

void TexturedSceneNodePropertyGrid::_addMaterialsProperty()
{
    auto materialsCount = _graphicContext->getMaterialsCount(_nodeName);

    for (unsigned short materialNumber = 0; materialNumber < materialsCount;
         materialNumber++) {
        wxPGPropArg textures = this->Append(new wxStringProperty(
            MATERIAL_PROPRTY_NAME + std::to_wstring(materialNumber + 1),
            MATERIAL_PROPRTY_NAME + std::to_wstring(materialNumber),
            wxT("<composed>")));

        auto texturePathes = _graphicContext->getTextures(_nodeName, materialNumber);

        for (int textureNumber = 0; textureNumber < texturePathes.size(); textureNumber++) {
            auto textureFilePicker = new wxImageFileProperty(
                TEXTURE_PROPRTY_NAME + std::to_wstring(textureNumber),
                *wxPGProperty::sm_wxPG_LABEL,
                texturePathes[textureNumber]);
            this->AppendIn(textures, textureFilePicker);
        }

        this->AppendIn(textures,
            new wxBoolProperty(L"Wireframe", WIREFRAME_PROPRTY_NAME,
                _graphicContext->isMaterialWaterframed(_nodeName, materialNumber)));
        this->AppendIn(textures,
            new wxBoolProperty("Lighting", LIGHTING_PROPERTY_NAME,
                _graphicContext->isMaterialLighting(_nodeName, materialNumber)));

        auto choices = _getMaterialNamesChoices();
        wstring currentMaterialTypeName = _getMaterialNameByType(_graphicContext->getMaterialType(_nodeName, materialNumber));
        int currentMaterialTypeIndex = choices.Index(currentMaterialTypeName);
        this->AppendIn(textures, new wxEnumProperty("Material type", MATERIAL_TYPE_PROPRTY_NAME, choices, currentMaterialTypeIndex));

        this->addPropertyChangeHandler(
            MATERIAL_TYPE_PROPRTY_NAME,
            boost::bind(&TexturedSceneNodePropertyGrid::_changeMaterialTypePropertyHandler, this, boost::placeholders::_1));
        this->addPropertyChangeHandler(
            LIGHTING_PROPERTY_NAME,
            boost::bind(&TexturedSceneNodePropertyGrid::_changeLightingTypePropertyHandler, this, boost::placeholders::_1));
        this->addPropertyChangeHandler(
            WIREFRAME_PROPRTY_NAME,
            boost::bind(&TexturedSceneNodePropertyGrid::_changeWireframeTypePropertyHandler, this, boost::placeholders::_1));
        this->addPropertyChangeHandler(
            TEXTURE_PROPRTY_NAME,
            boost::bind(&TexturedSceneNodePropertyGrid::_changeTexturePropertyHandler, this, boost::placeholders::_1));
    }
}

void TexturedSceneNodePropertyGrid::_changeMaterialTypePropertyHandler(
    wxPGProperty* eventProperty)
{
    auto propertyName = eventProperty->GetName().ToStdWstring();
    auto materialNumber = TUtils::parseNumbers<int>(propertyName)[0];
    auto selectedType = eventProperty->GetValueAsString().ToStdWstring();

    _graphicContext->setMaterialType(_nodeName, materialNumber, _materialTypesNames.at(selectedType));
}

void TexturedSceneNodePropertyGrid::_changeLightingTypePropertyHandler(
    wxPGProperty* eventProperty)
{
    auto propertyName = eventProperty->GetName().ToStdWstring();
    auto materialNumber = TUtils::parseNumbers<int>(propertyName)[0];
    auto selectedType = eventProperty->GetValue().GetBool();

    _graphicContext->setMaterialLighting(_nodeName, materialNumber, selectedType);
}

void TexturedSceneNodePropertyGrid::_changeWireframeTypePropertyHandler(
    wxPGProperty* eventProperty)
{
    auto propertyName = eventProperty->GetName().ToStdWstring();
    auto materialNumber = TUtils::parseNumbers<int>(propertyName)[0];
    auto selectedType = eventProperty->GetValue().GetBool();

    _graphicContext->setMaterialWaterframed(_nodeName, materialNumber, selectedType);
}

void TexturedSceneNodePropertyGrid::_changeTexturePropertyHandler(
    wxPGProperty* eventProperty)
{
    auto propertyName = eventProperty->GetName().ToStdWstring();
    auto materialNumber = TUtils::parseNumbers<int>(propertyName)[0];
    auto textureNumber = TUtils::parseNumbers<int>(propertyName)[1];
    auto selectedPath = eventProperty->GetValueAsString().ToStdWstring();

    _graphicContext->setMaterialTexture(_nodeName, materialNumber, textureNumber, selectedPath);
}

TexturedSceneNodePropertyGrid::TexturedSceneNodePropertyGrid(
    wxWindow* parent, std::wstring nodeName)
    : SceneNodePropertyGrid(parent, nodeName)
{
    _initMaterialTypesNames();

    _addMaterialsProperty();
}
}