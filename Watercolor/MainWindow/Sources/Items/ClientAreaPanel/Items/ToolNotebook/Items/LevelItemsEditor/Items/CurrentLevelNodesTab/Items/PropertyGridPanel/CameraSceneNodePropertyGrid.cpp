#include "CameraSceneNodePropertyGrid.h"

#include "TUtils.h"

#include "Common/LanguageDependents.h"

#define TARGET_PROPERTY_NAME L"1targetPropertyName"

namespace Watercolor {
CameraSceneNodePropertyGrid::CameraSceneNodePropertyGrid(wxWindow* parent, std::wstring nodeName)
    : SceneNodePropertyGrid(parent, nodeName)
{
    _addTargetProperty();
}

void CameraSceneNodePropertyGrid::_addTargetProperty()
{
    auto nodePosition = _graphicContext->getCameraTarget(_nodeName);
    wxPGPropArg position = this->Append(new wxStringProperty(
        TARGET_PROPERTY_TITLE, TARGET_PROPERTY_NAME, wxT("<composed>")));
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
        TARGET_PROPERTY_NAME,
        boost::bind(&CameraSceneNodePropertyGrid::_changePositionPropertyHandler, this, boost::placeholders::_1));
}

void CameraSceneNodePropertyGrid::_changePositionPropertyHandler(wxPGProperty* _)
{
    auto data = this->GetProperty(TARGET_PROPERTY_NAME);
    auto value = data->GetValueAsString().ToStdWstring();

    auto vectorData = _parse3DVector(value);
    _graphicContext->setCameraTarget(_nodeName, vectorData);
}
}