#include "ListBoxGuiElementPropertyGrid.h"

#include <wx/propgrid/advprops.h>

#include "Utils/LanguageDependents.h"
#include "Utils/Convert.h"

#define OPTIONS_PROPERTY_NAME L"OPTIONS_PROPERTY_NAME"

namespace GUIEditor {
ListBoxGuiElementPropertyGrid::ListBoxGuiElementPropertyGrid(wxWindow* parent, std::wstring elementName, boost::shared_ptr<GuiEditorGraphicContext> graphicContext)
    : GuiElementPropertyGrid(parent, elementName, graphicContext)
{
    _addOptionsProperty();
}

void ListBoxGuiElementPropertyGrid::_addOptionsProperty()
{
    auto options = _graphicContext->getSelectedListBoxOptions();

    this->Append(new wxArrayStringProperty(OPTIONS_PROPERTY_TITLE, OPTIONS_PROPERTY_NAME, Utils::toWxArrayString(options)));

    this->addPropertyChangeHandler(
        OPTIONS_PROPERTY_NAME, boost::bind(&ListBoxGuiElementPropertyGrid::_changeOptionsPropertyHandler, this, boost::placeholders::_1));
}

void ListBoxGuiElementPropertyGrid::_changeOptionsPropertyHandler(wxPGProperty* eventProperty)
{
    auto newWxOptions = eventProperty->GetValue().GetArrayString();

    std::vector<std::wstring> newOptions(newWxOptions.Count());

    for (auto i = 0; i < newWxOptions.Count(); i++) {
        newOptions[i] = newWxOptions[i];
    }

    _graphicContext->setSelectedListBoxOptions(newOptions);
}
}