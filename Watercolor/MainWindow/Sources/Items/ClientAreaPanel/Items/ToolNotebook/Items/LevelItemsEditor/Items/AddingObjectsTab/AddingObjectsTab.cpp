#include "AddingObjectsTab.h"
#include "TCoreFacade.h"

#include "boost/make_shared.hpp"

#include "Items/RichButtonGroupBox/RichButtonGroupBox.h"

#include "Common/CommonData.h"
#include "Items/Graphic/GraphicContext.h"
#include "ThreadTypes/ThreadTypes.h"

namespace Watercolor {
void AddingObjectsTab::_addSkySphere(wxCommandEvent& eventData)
{
    wxFileDialog fileDialog(this, wxFileSelectorPromptStr, wxEmptyString,
        wxEmptyString,
        "JPEG file (*.jpg)|*.jpg|PNG file (*.png)|*.png",
        wxFD_OPEN | wxFD_FILE_MUST_EXIST);
    fileDialog.ShowModal();

    std::wstring selectedPath = fileDialog.GetPath().ToStdWstring();

    if (!selectedPath.empty()) {
        _graphicContext->addSkySphere(selectedPath);
    }
}

void AddingObjectsTab::_addSkyCube(wxCommandEvent& eventData)
{
    _graphicContext->addSkyCube();
}

void AddingObjectsTab::_addMesh(wxCommandEvent& eventData)
{
    wxFileDialog fileDialog(this, wxFileSelectorPromptStr, wxEmptyString,
        wxEmptyString,
        "3DS Max model (*.3ds)|*.3ds|Wavefront (*.obj)|*.obj|MD3 (*.md3)|*.md3",
        wxFD_OPEN | wxFD_FILE_MUST_EXIST);
    fileDialog.ShowModal();

    std::wstring selectedPath = fileDialog.GetPath().ToStdWstring();

    if (!selectedPath.empty()) {
        _graphicContext->addMesh(selectedPath);
    }
}

void AddingObjectsTab::_addOstreeMesh(wxCommandEvent& eventData)
{
    wxFileDialog fileDialog(this, wxFileSelectorPromptStr, wxEmptyString,
        wxEmptyString,
        "3DS Max model (*.3ds)|*.3ds|Wavefront (*.obj)|*.obj",
        wxFD_OPEN | wxFD_FILE_MUST_EXIST);
    fileDialog.ShowModal();

    std::wstring selectedPath = fileDialog.GetPath().ToStdWstring();

    if (!selectedPath.empty()) {
        _graphicContext->addOstreeMesh(selectedPath);
    }
}

void AddingObjectsTab::_addAnimatedMesh(wxCommandEvent& eventData)
{
    wxFileDialog fileDialog(this, wxFileSelectorPromptStr, wxEmptyString,
        wxEmptyString,
        "MD2 (*.md2)|*.md2|MD3 (*.md3)|*.md3",
        wxFD_OPEN | wxFD_FILE_MUST_EXIST);
    fileDialog.ShowModal();

    std::wstring selectedPath = fileDialog.GetPath().ToStdWstring();

    if (!selectedPath.empty()) {
        _graphicContext->addAnimatedMesh(selectedPath);
    }
}

void AddingObjectsTab::_addTerrainFromHeightmap(wxCommandEvent& eventData)
{
    wxFileDialog fileDialog(this, wxFileSelectorPromptStr, wxEmptyString,
        wxEmptyString,
        "JPEG file (*.jpg)|*.jpg|PNG file (*.png)|*.png",
        wxFD_OPEN | wxFD_FILE_MUST_EXIST);
    fileDialog.ShowModal();

    std::wstring selectedPath = fileDialog.GetPath().ToStdWstring();
    if (!selectedPath.empty()) {
        _graphicContext->addTerrainFromHeightmap(selectedPath);
    }
}

void AddingObjectsTab::_addWaterSurface(wxCommandEvent& eventData)
{
    wxFileDialog fileDialog(this, wxFileSelectorPromptStr, wxEmptyString,
        wxEmptyString,
        "3DS Max model (*.3ds)|*.3ds|Wavefront (*.obj)|*.obj",
        wxFD_OPEN | wxFD_FILE_MUST_EXIST);
    fileDialog.ShowModal();

    std::wstring selectedPath = fileDialog.GetPath().ToStdWstring();

    if (!selectedPath.empty()) {
        _graphicContext->addWaterSurface(selectedPath);
    }
}

void AddingObjectsTab::_addLightSource(wxCommandEvent& eventData)
{
    _graphicContext->addLightSource();
}

void AddingObjectsTab::_addCamera(wxCommandEvent& eventData)
{
    _graphicContext->addCamera();
}

AddingObjectsTab::AddingObjectsTab(wxWindow* parent, boost::shared_ptr<IFunctionsProcessingManager> functionsProcessingManager)
    : wxScrolled<wxPanel>(parent)
    , _functionsProcessingManager(functionsProcessingManager)
{
    SetScrollRate(10, 10);

    this->SetBackgroundColour(wxColour(0xFF, 0xFF, 0xFF));
    _sizer = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(_sizer);

    _functionsProcessingManager->addFuctionToQueue(
        ThreadTypes::RENDER_THREAD, [=]() {
            this->_graphicContext = (Common::GraphicContext*)(wxTheApp->GetTopWindow()->FindWindowById(
                Common::ElementsIDs::MAIN_CANVAS));
        });

    std::map<wxWindowID, std::wstring> addMeshButtons;
    addMeshButtons.insert({ Common::ElementsIDs::ADD_MESH, L"Add mesh" });
    addMeshButtons.insert({ Common::ElementsIDs::ADD_OSTREE_MESH, L"Add ostree mesh" });
    addMeshButtons.insert({ Common::ElementsIDs::ADD_ANIMATED_MESH, L"Add animated mesh" });
    auto meshAddingPanel = boost::make_shared<Common::RichButtonGroupBox>(
        this, addMeshButtons, L"Mesh adding");
    _controls.push_back(meshAddingPanel);
    _sizer->Add(meshAddingPanel.get(), 0, wxALL | wxEXPAND, 5);

    std::map<wxWindowID, std::wstring> addLightButtons;
    addLightButtons.insert(
        { Common::ElementsIDs::ADD_LIGHT_SOURCE, L"Add light source" });
    auto lightAddingPanel = boost::make_shared<Common::RichButtonGroupBox>(
        this, addLightButtons, L"Light adding");
    _controls.push_back(lightAddingPanel);
    _sizer->Add(lightAddingPanel.get(), 0, wxALL | wxEXPAND, 5);

    std::map<wxWindowID, std::wstring> addSkyButtons;
    addSkyButtons.insert({ Common::ElementsIDs::ADD_SKY_SPHERE, L"Add sky sphere" });
    addSkyButtons.insert({ Common::ElementsIDs::ADD_SKY_CUBE, L"Add sky cube" });
    auto skyAddingPanel = boost::make_shared<Common::RichButtonGroupBox>(
        this, addSkyButtons, L"Sky adding");
    _controls.push_back(skyAddingPanel);
    _sizer->Add(skyAddingPanel.get(), 0, wxALL | wxEXPAND, 5);

    std::map<wxWindowID, std::wstring> addWaterButtons;
    addWaterButtons.insert(
        { Common::ElementsIDs::ADD_WATER_SURFACE, L"Add water surface" });
    addWaterButtons.insert(
        { Common::ElementsIDs::ADD_TERRAIN_FROM_HEIGHTMAP, L"Add from heightmap"});
    auto waterAddingPanel = boost::make_shared<Common::RichButtonGroupBox>(
        this, addWaterButtons, L"Terrain adding");
    _controls.push_back(waterAddingPanel);
    _sizer->Add(waterAddingPanel.get(), 0, wxALL | wxEXPAND, 5);

    std::map<wxWindowID, std::wstring> addOtherButtons;
    addOtherButtons.insert({ Common::ElementsIDs::ADD_CAMERA, L"Add camera" });
    auto otherAddingPanel = boost::make_shared<Common::RichButtonGroupBox>(
        this, addOtherButtons, L"Other adding");
    _controls.push_back(otherAddingPanel);
    _sizer->Add(otherAddingPanel.get(), 0, wxALL | wxEXPAND, 5);
}

AddingObjectsTab::~AddingObjectsTab() {}

wxBEGIN_EVENT_TABLE(AddingObjectsTab, wxPanel)
    EVT_BUTTON(Common::ElementsIDs::ADD_MESH, AddingObjectsTab::_addMesh)
        EVT_BUTTON(Common::ElementsIDs::ADD_OSTREE_MESH,
            AddingObjectsTab::_addOstreeMesh)
            EVT_BUTTON(Common::ElementsIDs::ADD_ANIMATED_MESH,
                AddingObjectsTab::_addAnimatedMesh)
                EVT_BUTTON(Common::ElementsIDs::ADD_WATER_SURFACE,
                    AddingObjectsTab::_addWaterSurface)
                    EVT_BUTTON(Common::ElementsIDs::ADD_SKY_SPHERE,
                        AddingObjectsTab::_addSkySphere)
                        EVT_BUTTON(Common::ElementsIDs::ADD_SKY_CUBE,
                            AddingObjectsTab::_addSkyCube)
                            EVT_BUTTON(Common::ElementsIDs::ADD_LIGHT_SOURCE,
                                AddingObjectsTab::_addLightSource)
                                EVT_BUTTON(
                                    Common::ElementsIDs::ADD_TERRAIN_FROM_HEIGHTMAP,
                                    AddingObjectsTab::_addTerrainFromHeightmap)
                                    EVT_BUTTON(
                                        Common::ElementsIDs::ADD_CAMERA,
                                        AddingObjectsTab::_addCamera)
                                        wxEND_EVENT_TABLE()
}