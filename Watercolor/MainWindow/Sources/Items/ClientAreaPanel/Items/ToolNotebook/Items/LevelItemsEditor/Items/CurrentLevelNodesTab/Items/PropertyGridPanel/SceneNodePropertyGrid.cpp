#include "SceneNodePropertyGrid.h"
#include "TCoreFacade.h"
#include "TUtils.h"

#include "Common/CommonData.h"
#include "Common/LanguageDependents.h"

#define POSITION_PROPERTY_NAME L"positionPropertyName"
#define ROTATION_PROPERTY_NAME L"rotationPropertyName"
#define SCALE_PROPERTY_NAME L"scalePropertyName"
#define NAME_PROPERTY_NAME L"namePropertyName"

using namespace std;

namespace Watercolor {
void SceneNodePropertyGrid::_addPositionProperty()
{
    auto nodePosition = _graphicContext->getPosition(_nodeName);
    wxPGPropArg position = this->Append(new wxStringProperty(
        POSITION_PROPERTY_TITLE, POSITION_PROPERTY_NAME, wxT("<composed>")));
    this->AppendIn(
        position,
        new wxFloatProperty(L"X", *wxPGProperty::sm_wxPG_LABEL, nodePosition.X));
    this->AppendIn(
        position,
        new wxFloatProperty(L"Y", *wxPGProperty::sm_wxPG_LABEL, nodePosition.Y));
    this->AppendIn(
        position,
        new wxFloatProperty(L"Z", *wxPGProperty::sm_wxPG_LABEL, nodePosition.Z));

    this->addPropertyChangeHandler(
        POSITION_PROPERTY_NAME,
        boost::bind(&SceneNodePropertyGrid::_changePositionPropertyHandler, this, boost::placeholders::_1));
}

void SceneNodePropertyGrid::_addRotationProperty()
{
    auto nodeRotation = _graphicContext->getRotation(_nodeName);
    wxPGPropArg rotation = this->Append(new wxStringProperty(
        ROTATION_PROPERTY_TITLE, ROTATION_PROPERTY_NAME, wxT("<composed>")));
    this->AppendIn(
        rotation,
        new wxFloatProperty(L"X", *wxPGProperty::sm_wxPG_LABEL, nodeRotation.X));
    this->AppendIn(
        rotation,
        new wxFloatProperty(L"Y", *wxPGProperty::sm_wxPG_LABEL, nodeRotation.Y));
    this->AppendIn(
        rotation,
        new wxFloatProperty(L"Z", *wxPGProperty::sm_wxPG_LABEL, nodeRotation.Z));

    this->addPropertyChangeHandler(
        ROTATION_PROPERTY_NAME,
        boost::bind(&SceneNodePropertyGrid::_changeRotationPropertyHandler, this, boost::placeholders::_1));
}

void SceneNodePropertyGrid::_addScaleProperty()
{
    auto nodeScale = _graphicContext->getScale(_nodeName);
    wxPGPropArg scale = this->Append(
        new wxStringProperty(SCALE_PROPERTY_TITLE, SCALE_PROPERTY_NAME, wxT("<composed>")));
    this->AppendIn(scale, new wxFloatProperty(L"X", *wxPGProperty::sm_wxPG_LABEL, nodeScale.X));
    this->AppendIn(scale, new wxFloatProperty(L"Y", *wxPGProperty::sm_wxPG_LABEL, nodeScale.Y));
    this->AppendIn(scale, new wxFloatProperty(L"Z", *wxPGProperty::sm_wxPG_LABEL, nodeScale.Z));

    this->addPropertyChangeHandler(
        SCALE_PROPERTY_NAME,
        boost::bind(&SceneNodePropertyGrid::_changeScalePropertyHandler, this, boost::placeholders::_1));
}

void SceneNodePropertyGrid::_addNameProperty()
{
    this->Append(new wxStringProperty(L"Name", NAME_PROPERTY_NAME, _nodeName));

    this->addPropertyChangeHandler(
        NAME_PROPERTY_NAME, boost::bind(&SceneNodePropertyGrid::_changeNamePropertyHandler, this, boost::placeholders::_1));
}

void SceneNodePropertyGrid::_changePositionPropertyHandler(
    wxPGProperty* _)
{
    auto data = this->GetProperty(POSITION_PROPERTY_NAME);
    auto value = data->GetValueAsString().ToStdWstring();

    auto vectorData = _parse3DVector(value);
    _graphicContext->setPosition(_nodeName, vectorData);
}

void SceneNodePropertyGrid::_changeRotationPropertyHandler(
    wxPGProperty* _)
{
    auto data = this->GetProperty(ROTATION_PROPERTY_NAME);
    auto value = data->GetValueAsString().ToStdWstring();

    auto vectorData = _parse3DVector(value);
    _graphicContext->setRotation(_nodeName, vectorData);
}

void SceneNodePropertyGrid::_changeScalePropertyHandler(
    wxPGProperty* _)
{
    auto data = this->GetProperty(SCALE_PROPERTY_NAME);
    auto value = data->GetValueAsString().ToStdWstring();

    auto vectorData = _parse3DVector(value);
    _graphicContext->setScale(_nodeName, vectorData);
}

void SceneNodePropertyGrid::_changeNamePropertyHandler(
    wxPGProperty* eventProperty)
{
    auto newName = eventProperty->GetValueAsString().ToStdWstring();

    _graphicContext->setName(_nodeName, newName);
    _nodeName = newName;
}

void SceneNodePropertyGrid::_onPropertyWasSet(wxPropertyGridEvent& eventData)
{
    wstring propertyName = eventData.GetPropertyName().ToStdWstring();

    // TODO: check it!
    for (auto handler : _changeHandlers) {
        if (propertyName.find(handler.first) != -1) {
            auto property = eventData.GetProperty();
            handler.second(property);
            break;
        }
    }
}

void SceneNodePropertyGrid::_onGizmoManipulationFinished(Common::ManipulationType manipulationType)
{
    switch (manipulationType) {
    case Common::ManipulationType::SELECT:
        break;
    case Common::ManipulationType::MOVE: {
        const auto& position = _graphicContext->getPosition(_nodeName);
        this->GetProperty(POSITION_PROPERTY_NAME + std::wstring(L".X"))->SetValue(position.X);
        this->GetProperty(POSITION_PROPERTY_NAME + std::wstring(L".Y"))->SetValue(position.Y);
        this->GetProperty(POSITION_PROPERTY_NAME + std::wstring(L".Z"))->SetValue(position.Z);
        break;
    }
    case Common::ManipulationType::ROTATE: {
        const auto& rotation = _graphicContext->getRotation(_nodeName);
        this->GetProperty(ROTATION_PROPERTY_NAME + std::wstring(L".X"))->SetValue(rotation.X);
        this->GetProperty(ROTATION_PROPERTY_NAME + std::wstring(L".Y"))->SetValue(rotation.Y);
        this->GetProperty(ROTATION_PROPERTY_NAME + std::wstring(L".Z"))->SetValue(rotation.Z);
        break;
    }
    case Common::ManipulationType::SCALE: {
        const auto& scale = _graphicContext->getScale(_nodeName);
        this->GetProperty(SCALE_PROPERTY_NAME + std::wstring(L".X"))->SetValue(scale.X);
        this->GetProperty(SCALE_PROPERTY_NAME + std::wstring(L".Y"))->SetValue(scale.Y);
        this->GetProperty(SCALE_PROPERTY_NAME + std::wstring(L".Z"))->SetValue(scale.Z);
        break;
    }
    default:
        break;
    }
}

void SceneNodePropertyGrid::addPropertyChangeHandler(
    wstring propertyName, PropertyChangeHandler handler)
{
    this->_changeHandlers.insert({ propertyName, handler });
}

void SceneNodePropertyGrid::removePropertyChangeHandler(wstring propertyName)
{
    this->_changeHandlers.erase(propertyName);
}

SceneNodePropertyGrid::SceneNodePropertyGrid(wxWindow* parent,
    std::wstring nodeName)
    : wxPropertyGrid(parent, Common::ElementsIDs::SELECT_OBJECT_PROPERTY_GRID, wxDefaultPosition, wxSize(9999, 300))
{
    _nodeName = nodeName;

    this->_graphicContext = (Common::GizmoGraphicContext*)(wxTheApp->GetTopWindow()->FindWindowById(
        Common::ElementsIDs::MAIN_CANVAS));

    // Name
    this->_addNameProperty();

    // Position
    this->_addPositionProperty();

    // Rotation
    this->_addRotationProperty();

    // Scale
    this->_addScaleProperty();

    _graphicContext->onManipulationFinished.addEventHandler(POSITION_PROPERTY_NAME,
        boost::bind(&SceneNodePropertyGrid::_onGizmoManipulationFinished, this, boost::placeholders::_1));
}

SceneNodePropertyGrid::~SceneNodePropertyGrid()
{
    _graphicContext->onManipulationFinished.removeEventHandler(POSITION_PROPERTY_NAME);
}

wstring SceneNodePropertyGrid::getObjectName() const
{
    return this->GetProperty(NAME_PROPERTY_NAME)
        ->GetValueAsString()
        .ToStdWstring();
}

irr::core::vector3df SceneNodePropertyGrid::_parse3DVector(const std::wstring& textVector)
{
    std::vector<float> vectorParts = TUtils::parseNumbers<float>(textVector);

    irr::core::vector3df vector3D(vectorParts[0], vectorParts[1], vectorParts[2]);

    return vector3D;
}

wxBEGIN_EVENT_TABLE(SceneNodePropertyGrid, wxPropertyGrid)
    EVT_PG_CHANGED(Common::ElementsIDs::SELECT_OBJECT_PROPERTY_GRID,
        SceneNodePropertyGrid::_onPropertyWasSet) wxEND_EVENT_TABLE()
}