#include "GizmoProxySceneNode.h"

namespace Watercolor::Common {
void GizmoProxySceneNode::updateAbsolutePosition()
{
    if (!Parent.expired()) {
        irr::core::matrix4 newAbsoluteTransformation;
        newAbsoluteTransformation.setTranslation(Parent.lock()->getAbsoluteTransformation().getTranslation());

        AbsoluteTransformation = newAbsoluteTransformation;
    } else
        ISceneNode::updateAbsolutePosition();
}

void GizmoProxySceneNode::render()
{
    for (auto child : this->Children) {
        child->render();
    }
}

const irr::core::aabbox3d<irr::f32>& GizmoProxySceneNode::getBoundingBox() const
{
    return _boundingBox;
}

GizmoProxySceneNode::GizmoProxySceneNode(boost::shared_ptr<ISceneNode> parent)
    : ISceneNode(parent, parent->getSceneManager())
{
}

GizmoProxySceneNode::~GizmoProxySceneNode()
{
}
}