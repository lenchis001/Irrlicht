#pragma once

#include "wx/propgrid/propgrid.h"
#include "wx/wx.h"

#include "irrlicht.h"

#include "boost/shared_ptr.hpp"

#include "SceneNodePropertyGrid.h"

namespace Watercolor {
class PropertyGridFactory {
public:
    static boost::shared_ptr<SceneNodePropertyGrid> getGrid(
        wxWindow* parent, boost::shared_ptr<irr::scene::ISceneNode> node);
};
}