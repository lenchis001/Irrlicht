#pragma once

#include "SceneNodePropertyGrid.h"

namespace Watercolor {
class CameraSceneNodePropertyGrid : public SceneNodePropertyGrid {
public:
    CameraSceneNodePropertyGrid(wxWindow* parent, std::wstring nodeName);

private:
    void _addTargetProperty();

    void _changePositionPropertyHandler(wxPGProperty* _);
};
}