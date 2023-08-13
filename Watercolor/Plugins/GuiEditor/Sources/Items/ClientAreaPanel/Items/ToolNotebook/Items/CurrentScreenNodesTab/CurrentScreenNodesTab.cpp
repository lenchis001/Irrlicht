#include "CurrentScreenNodesTab.h"

#include "boost/make_shared.hpp"

#include "Utils/ElementIDs.h"
#include "Items/PropertyGridPanel/PropertyGridFactory.h"

#include "Items/ClientAreaPanel/Items/GuiEditorGraphicContext/Core/CGUIEditWorkspace.h"
#include "Items/ClientAreaPanel/Items/GuiEditorGraphicContext/GuiEditorGraphicContext.h"

#define ON_GUI_ELEMENT_SELECTED_HANDLER_NAME L"CurrentScreenNodesTab::_onGuiElementSelected"
#define ON_CLEAR_HANDLER_NAME L"CurrentScreenNodesTab::_onClear"

namespace GUIEditor {
CurrentScreenNodesTab::CurrentScreenNodesTab(wxWindow* parent, boost::shared_ptr<GuiEditorGraphicContext> graphicContext, boost::shared_ptr<IFunctionsProcessingManager> functionsProcessingManager)
    : wxScrolled<wxPanel>(parent, wxID_ANY)
    , _functionsProcessingManager(functionsProcessingManager)
{
    _graphicContext = graphicContext;

    SetScrollRate(10, 10);

    this->SetBackgroundColour(wxColour(0xFF, 0xFF, 0xFF));

    _sizer = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(_sizer);

    _useGridCheckBox = boost::make_shared<wxCheckBox>(this, Utils::ElementsIDs::USE_GRID_CHECKBOX, "Use grid");
    _sizer->Add(_useGridCheckBox.get(), 0, wxALL, 10);

    _sizer->Layout();

    _graphicContext->onGuiElementSelected.addEventHandler(ON_GUI_ELEMENT_SELECTED_HANDLER_NAME, boost::bind(&CurrentScreenNodesTab::_onGuiElementSelected, this, boost::placeholders::_1));
    _graphicContext->onClear.addEventHandler(ON_CLEAR_HANDLER_NAME, boost::bind(&CurrentScreenNodesTab::_onClear, this, boost::placeholders::_1));
}

CurrentScreenNodesTab::~CurrentScreenNodesTab()
{
    _graphicContext->onGuiElementSelected.removeEventHandler(ON_GUI_ELEMENT_SELECTED_HANDLER_NAME);
    _graphicContext->onGuiElementSelected.removeEventHandler(ON_CLEAR_HANDLER_NAME);
}

void CurrentScreenNodesTab::_onUseGridChange(wxCommandEvent& eventData)
{
    _graphicContext->getGuiEditWorkspace()->setUseGrid(eventData.IsChecked());
}

void CurrentScreenNodesTab::_onGuiElementSelected(boost::shared_ptr<irr::gui::IGUIElement> element)
{
    _functionsProcessingManager->addFuctionToQueue(ThreadTypes::GUI_THREAD, [&, element] {
        if (element) {
            _selectedElementPropertyGrid = PropertyGridFactory::getGrid(this, element, _graphicContext);
            _sizer->Add(_selectedElementPropertyGrid.get(), 1, wxALL | wxEXPAND, 10);
            _sizer->Layout();
        } else {
            _selectedElementPropertyGrid = nullptr;
        }
    });
}

void CurrentScreenNodesTab::_onClear(void* _)
{
    _useGridCheckBox->SetValue(_graphicContext->getGuiEditWorkspace()->getUseGrid());
}

wxBEGIN_EVENT_TABLE(CurrentScreenNodesTab, wxPanel)
    EVT_CHECKBOX(Utils::ElementsIDs::USE_GRID_CHECKBOX, CurrentScreenNodesTab::_onUseGridChange)
        wxEND_EVENT_TABLE()
}