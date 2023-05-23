#pragma once

#include "irrlicht.h"

namespace Watercolor::Common {
class GizmoProxySceneNode : public irr::scene::ISceneNode {
public:
    void updateAbsolutePosition() override;
    void render() override;
    const irr::core::aabbox3d<irr::f32>& getBoundingBox() const override;

    GizmoProxySceneNode(boost::shared_ptr<irr::scene::ISceneNode> parent);
    virtual ~GizmoProxySceneNode();

private:
    irr::core::aabbox3d<irr::f32> _boundingBox;
};
}