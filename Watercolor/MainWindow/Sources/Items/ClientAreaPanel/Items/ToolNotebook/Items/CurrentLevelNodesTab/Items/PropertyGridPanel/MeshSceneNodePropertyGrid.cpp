#include "MeshSceneNodePropertyGrid.h"
#include "TCoreFacade.h"

#define PHYSICS_PROPERTY_NAME L"physicsPropertyName"

#include "ThreadTypes/ThreadTypes.h"

namespace Watercolor {
MeshSceneNodePropertyGrid::MeshSceneNodePropertyGrid(
    wxWindow* parent, std::wstring nodeName)
    : TexturedSceneNodePropertyGrid(parent, nodeName)
{
    addCollisionPhysicsProperty();

    this->addPropertyChangeHandler(PHYSICS_PROPERTY_NAME, boost::bind(&MeshSceneNodePropertyGrid::_changePhysicsTypePropertyHandler, this, boost::placeholders::_1));
}

MeshSceneNodePropertyGrid::~MeshSceneNodePropertyGrid() {}

void MeshSceneNodePropertyGrid::addCollisionPhysicsProperty()
{
   /* wxPGChoices soc;
    soc.Add(L"Without physics", (int)TLMM::PhysicsTypes::WITHOUT_PHYSICS);
    soc.Add(L"Collisionable", (int)TLMM::PhysicsTypes::COLLISIONABLE);
    soc.Add(L"Standart", (int)TLMM::PhysicsTypes::STANDART);

    auto physicsType = _graphicContext->getPhysicsType(_nodeName);
    auto physicsTypesProperty = new wxEnumProperty(L"Physics type", PHYSICS_PROPERTY_NAME, soc, static_cast<int>(physicsType));

    this->Append(physicsTypesProperty);*/
}

void MeshSceneNodePropertyGrid::_changePhysicsTypePropertyHandler(
    wxPGProperty* eventProperty)
{
   /* auto selectedPhysicsTypeName = eventProperty->GetValue().GetInteger();
    _graphicContext->setPhysicsType(_nodeName, static_cast<TLMM::PhysicsTypes>(selectedPhysicsTypeName));*/
}
}