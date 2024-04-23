#include "AddingObjectsTab.h"
#include "TCoreFacade.h"

#include "boost/make_shared.hpp"

#include "Items/RichButtonGroupBox/RichButtonGroupBox.h"

#include "Common/CommonData.h"
#include "Items/Graphic/GraphicContext.h"
#include "ThreadTypes/ThreadTypes.h"

namespace Watercolor {
	void AddingObjectsTab::_addArrow(wxCommandEvent& eventData)
	{
		_graphicContext->addArrow();
	}

	void AddingObjectsTab::_addEmpty(wxCommandEvent& eventData)
	{
		_graphicContext->addEmpty();
	}

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

	void AddingObjectsTab::_addSection(std::wstring name, std::map<wxWindowID, std::wstring> controls)
	{
		auto meshAddingPanel = boost::make_shared<Common::RichButtonGroupBox>(
			this, controls, name);
		_controls.push_back(meshAddingPanel);
		_sizer->Add(meshAddingPanel.get(), 0, wxALL | wxEXPAND, 5);
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

		_addSection(L"Mesh adding", {
			{ Common::ElementsIDs::ADD_MESH, L"Add mesh" },
			{ Common::ElementsIDs::ADD_OSTREE_MESH, L"Add ostree mesh" },
			{ Common::ElementsIDs::ADD_ANIMATED_MESH, L"Add animated mesh"}
			});

		_addSection(L"Light adding", {
			{ Common::ElementsIDs::ADD_LIGHT_SOURCE, L"Add light source" }
			});

		_addSection(L"Sky adding", {
			{ Common::ElementsIDs::ADD_SKY_SPHERE, L"Add sky sphere" },
			{ Common::ElementsIDs::ADD_SKY_CUBE, L"Add sky cube" }
			});

		_addSection(L"Terrain adding", {
			{ Common::ElementsIDs::ADD_WATER_SURFACE, L"Add water surface" },
			{ Common::ElementsIDs::ADD_TERRAIN_FROM_HEIGHTMAP, L"Add from heightmap" }
			});

		_addSection(L"Other adding", {
			{ Common::ElementsIDs::ADD_CAMERA, L"Add camera" },
			{ Common::ElementsIDs::ADD_EMPTY, L"Add empty" },
			{ Common::ElementsIDs::ADD_ARROW, L"Add arrow" },
			});
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
		EVT_BUTTON(
			Common::ElementsIDs::ADD_EMPTY,
			AddingObjectsTab::_addEmpty)
		EVT_BUTTON(
			Common::ElementsIDs::ADD_ARROW,
			AddingObjectsTab::_addArrow)
		wxEND_EVENT_TABLE()
}