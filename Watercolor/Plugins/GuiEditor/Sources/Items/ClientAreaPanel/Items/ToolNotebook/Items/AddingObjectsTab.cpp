#include "AddingObjectsTab.h"

#include "string"
#include "map"

#include "Utils/ElementIDs.h"
#include "Items/RichButtonGroupBox/RichButtonGroupBox.h"

namespace GUIEditor {
AddingObjectsTab::AddingObjectsTab(wxWindow* parent, boost::shared_ptr<GuiEditorGraphicContext> graphicContext)
    : wxScrolled(parent)
    , _graphicContext(graphicContext)
{
    SetScrollRate(10, 10);

    this->SetBackgroundColour(wxColour(0xFF, 0xFF, 0xFF));
    _sizer = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(_sizer);

    std::map<wxWindowID, std::wstring> addGeneralButtons;
    addGeneralButtons.insert({ Utils::ElementsIDs::ADD_WINDOW , L"Window"});
    addGeneralButtons.insert({ Utils::ElementsIDs::ADD_BUTTON, L"Button" });
    addGeneralButtons.insert({ Utils::ElementsIDs::ADD_IMAGE , L"Image"});
    auto generalAddingPanel = boost::make_shared<RichButtonGroupBox>(
        this, addGeneralButtons, L"General");
    _controls.push_back(generalAddingPanel);
    _sizer->Add(generalAddingPanel.get(), 0, wxALL | wxEXPAND, 5);

    std::map<wxWindowID, std::wstring> addTextContainerButtons;
    addTextContainerButtons.insert({ Utils::ElementsIDs::ADD_LABLE, L"Label" });
    addTextContainerButtons.insert({ Utils::ElementsIDs::ADD_TEXTBOX, L"Textbox" });
    auto textContainerAddingPanel = boost::make_shared<RichButtonGroupBox>(
        this, addTextContainerButtons, L"Text containers");
    _controls.push_back(textContainerAddingPanel);
    _sizer->Add(textContainerAddingPanel.get(), 0, wxALL | wxEXPAND, 5);

    std::map<wxWindowID, std::wstring> addSelectableContainerButtons;
    addSelectableContainerButtons.insert({ Utils::ElementsIDs::ADD_CHECKBOX, L"CheckBox" });
    addSelectableContainerButtons.insert({ Utils::ElementsIDs::ADD_LISTBOX, L"ListBox" });
    auto selectableContainerAddingPanel = boost::make_shared<RichButtonGroupBox>(
        this, addSelectableContainerButtons, L"Selectable");
    _controls.push_back(selectableContainerAddingPanel);
    _sizer->Add(selectableContainerAddingPanel.get(), 0, wxALL | wxEXPAND, 5);
}

void AddingObjectsTab::_onAddButton(wxCommandEvent& event)
{
    _graphicContext->addButton(EMPTY_CALLBACK_WITH_PARAM);
}

void AddingObjectsTab::_onAddImage(wxCommandEvent& event)
{
    _graphicContext->addImage(EMPTY_CALLBACK_WITH_PARAM);
}

void AddingObjectsTab::_onAddWindow(wxCommandEvent& event)
{
    _graphicContext->addWindow(EMPTY_CALLBACK_WITH_PARAM);
}

void AddingObjectsTab::_onAddLabel(wxCommandEvent& event)
{
    _graphicContext->addLabel(EMPTY_CALLBACK_WITH_PARAM);
}

void AddingObjectsTab::_onAddTextbox(wxCommandEvent& event)
{
    _graphicContext->addTextbox(EMPTY_CALLBACK_WITH_PARAM);
}

void AddingObjectsTab::_onAddCheckBox(wxCommandEvent& event)
{
    _graphicContext->addCheckBox(EMPTY_CALLBACK_WITH_PARAM);
}

void AddingObjectsTab::_onAddListBox(wxCommandEvent& event)
{
    _graphicContext->addListBox(EMPTY_CALLBACK_WITH_PARAM);
}

wxBEGIN_EVENT_TABLE(AddingObjectsTab, wxScrolled)
    EVT_BUTTON(Utils::ElementsIDs::ADD_WINDOW, AddingObjectsTab::_onAddWindow)
        EVT_BUTTON(Utils::ElementsIDs::ADD_BUTTON, AddingObjectsTab::_onAddButton)
            EVT_BUTTON(Utils::ElementsIDs::ADD_IMAGE, AddingObjectsTab::_onAddImage)
                EVT_BUTTON(Utils::ElementsIDs::ADD_LABLE, AddingObjectsTab::_onAddLabel)
                    EVT_BUTTON(Utils::ElementsIDs::ADD_TEXTBOX, AddingObjectsTab::_onAddTextbox)
                        EVT_BUTTON(Utils::ElementsIDs::ADD_CHECKBOX, AddingObjectsTab::_onAddCheckBox)
                            EVT_BUTTON(Utils::ElementsIDs::ADD_LISTBOX, AddingObjectsTab::_onAddListBox)
                                wxEND_EVENT_TABLE()
}