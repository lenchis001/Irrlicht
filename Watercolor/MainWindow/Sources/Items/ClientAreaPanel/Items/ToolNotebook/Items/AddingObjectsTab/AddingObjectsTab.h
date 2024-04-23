#pragma once

#include "wx/wx.h"
#include <wx/print.h>

#include "vector"

#include "boost/shared_ptr.hpp"

#include "Items/Graphic/GizmoGraphicContext.h"

namespace Watercolor {
class AddingObjectsTab : public wxScrolled<wxPanel> {
public:
    AddingObjectsTab(wxWindow* parent, boost::shared_ptr<IFunctionsProcessingManager> functionsProcessingManager);
    ~AddingObjectsTab();

private:
    void _addArrow(wxCommandEvent& eventData);
    void _addEmpty(wxCommandEvent& eventData);
    void _addSkySphere(wxCommandEvent& eventData);
    void _addSkyCube(wxCommandEvent& eventData);
    void _addMesh(wxCommandEvent& eventData);
    void _addOstreeMesh(wxCommandEvent& eventData);
    void _addAnimatedMesh(wxCommandEvent& eventData);
    void _addTerrainFromHeightmap(wxCommandEvent& eventData);
    void _addWaterSurface(wxCommandEvent& eventData);
    void _addLightSource(wxCommandEvent& eventData);
    void _addCamera(wxCommandEvent& eventData);

    void _addSection(std::wstring name, std::map<int, std::wstring> controls);

    std::vector<boost::shared_ptr<wxWindow>> _controls;
    wxSizer* _sizer;
    Common::GraphicContext* _graphicContext;
    boost::shared_ptr<IFunctionsProcessingManager> _functionsProcessingManager;

    wxDECLARE_EVENT_TABLE();
};
}