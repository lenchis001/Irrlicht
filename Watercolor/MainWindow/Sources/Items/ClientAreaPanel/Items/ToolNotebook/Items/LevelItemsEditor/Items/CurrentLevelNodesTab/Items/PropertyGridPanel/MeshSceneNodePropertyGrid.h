#pragma once

#include "string"
#include "vector"

#include "TexturedSceneNodePropertyGrid.h"

using namespace std;

namespace Watercolor {
class MeshSceneNodePropertyGrid : public TexturedSceneNodePropertyGrid {
    void addCollisionPhysicsProperty();

    void _changePhysicsTypePropertyHandler(wxPGProperty* eventProperty);

public:
    MeshSceneNodePropertyGrid(wxWindow* parent, std::wstring nodeName);
    ~MeshSceneNodePropertyGrid();
};
}