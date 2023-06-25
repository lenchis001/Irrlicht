#pragma once

#include "wx/propgrid/propgrid.h"
#include "wx/wx.h"

#include "boost/smart_ptr.hpp"

#include "irrlicht.h"

#include "SceneNodePropertyGrid.h"

namespace Watercolor {
class PropertyGridFactory {
public:
    static boost::shared_ptr<SceneNodePropertyGrid> getGrid(
        wxWindow* parent, boost::shared_ptr<irr::scene::ISceneNode> node);
};
}