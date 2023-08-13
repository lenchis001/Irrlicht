#include "GuiElementPropertyGrid.h"

#include "Utils/ElementIDs.h"
#include "Utils/LanguageDependents.h"
#include "Utils/Convert.h"

#define NAME_PROPERTY_NAME L"namePropertyName"
#define TEXT_PROPERTY_NAME L"textPropertyName"
#define POSITION_PROPERTY_NAME L"positionPropertyName"

namespace GUIEditor {
GuiElementPropertyGrid::GuiElementPropertyGrid(wxWindow* parent, std::wstring elementName, boost::shared_ptr<GuiEditorGraphicContext> graphicContext)
    : wxPropertyGrid(parent, Utils::ElementsIDs::SELECT_GUI_ELEMENT_PROPEERTY_GRID, wxDefaultPosition, wxSize(100, 100))
{
    _graphicContext = graphicContext.get();
    _elementName = elementName;

    _addNameProperty();
    _addPositionProperty();
    _addTextProperty();

    _graphicContext->onPositionChanged.addEventHandler(POSITION_PROPERTY_NAME,
        boost::bind(&GuiElementPropertyGrid::_onChangePositionPropertyOutside, this, boost::placeholders::_1));
}

GuiElementPropertyGrid::~GuiElementPropertyGrid()
{
    _graphicContext->onPositionChanged.removeEventHandler(POSITION_PROPERTY_NAME);
}

void GuiElementPropertyGrid::addPropertyChangeHandler(
    std::wstring propertyName, PropertyChangeHandler handler)
{
    this->_changeHandlers.insert({ propertyName, handler });
}

void GuiElementPropertyGrid::removePropertyChangeHandler(std::wstring propertyName)
{
    this->_changeHandlers.erase(propertyName);
}

void GuiElementPropertyGrid::_onPropertyWasSet(wxPropertyGridEvent& eventData)
{
    std::wstring propertyName = eventData.GetPropertyName().ToStdWstring();

    // TODO: check it!
    for (auto handler : _changeHandlers) {
        if (propertyName.find(handler.first) != -1) {
            auto property = eventData.GetProperty();
            handler.second(property);
            break;
        }
    }
}

void GuiElementPropertyGrid::_addNameProperty()
{
    this->Append(new wxStringProperty(NAME_PROPERTY_TITLE, NAME_PROPERTY_NAME, _elementName));

    this->addPropertyChangeHandler(
        NAME_PROPERTY_NAME, boost::bind(&GuiElementPropertyGrid::_changeNamePropertyHandler, this, boost::placeholders::_1));
}

void GuiElementPropertyGrid::_changeNamePropertyHandler(wxPGProperty* eventProperty)
{
    auto newName = eventProperty->GetValueAsString().ToStdWstring();

    _graphicContext->setSelectedElementName(newName, EMPTY_CALLBACK_WITH_PARAM);
    _elementName = newName;
}

void GuiElementPropertyGrid::_changeTextPropertyHandler(wxPGProperty* eventProperty)
{
    auto newText = eventProperty->GetValueAsString().ToStdWstring();

    _graphicContext->setSelectedElementText(newText, EMPTY_CALLBACK_WITH_PARAM);
}

void GuiElementPropertyGrid::_addPositionProperty()
{
    auto nodePosition = _graphicContext->getSelectedElementPosition();
    wxPGPropArg position = this->Append(new wxStringProperty(
        POSITION_PROPERTY_TITLE, POSITION_PROPERTY_NAME, wxT("<composed>")));
    this->AppendIn(
        position,
        new wxIntProperty(L"Left", *wxPGProperty::sm_wxPG_LABEL, nodePosition.UpperLeftCorner.X));
    this->AppendIn(
        position,
        new wxIntProperty(L"Top", *wxPGProperty::sm_wxPG_LABEL, nodePosition.UpperLeftCorner.Y));
    this->AppendIn(
        position,
        new wxIntProperty(L"Right", *wxPGProperty::sm_wxPG_LABEL, nodePosition.LowerRightCorner.X));
    this->AppendIn(
        position,
        new wxIntProperty(L"Bottom", *wxPGProperty::sm_wxPG_LABEL, nodePosition.LowerRightCorner.Y));

    this->addPropertyChangeHandler(
        POSITION_PROPERTY_NAME,
        boost::bind(&GuiElementPropertyGrid::_changePositionPropertyHandler, this, boost::placeholders::_1));
}

void GuiElementPropertyGrid::_addTextProperty()
{
    auto text = _graphicContext->getSelectedElementText();
    this->Append(new wxStringProperty(TEXT_PROPERTY_TITLE, TEXT_PROPERTY_NAME, text));

    this->addPropertyChangeHandler(
        TEXT_PROPERTY_NAME, boost::bind(&GuiElementPropertyGrid::_changeTextPropertyHandler, this, boost::placeholders::_1));
}

void GuiElementPropertyGrid::_onChangePositionPropertyOutside(boost::shared_ptr<irr::gui::IGUIElement> _)
{
    auto position = _graphicContext->getSelectedElementPosition();

    this->GetProperty(POSITION_PROPERTY_NAME + std::wstring(L".Left"))->SetValue(position.UpperLeftCorner.X);
    this->GetProperty(POSITION_PROPERTY_NAME + std::wstring(L".Top"))->SetValue(position.UpperLeftCorner.Y);
    this->GetProperty(POSITION_PROPERTY_NAME + std::wstring(L".Right"))->SetValue(position.LowerRightCorner.X);
    this->GetProperty(POSITION_PROPERTY_NAME + std::wstring(L".Bottom"))->SetValue(position.LowerRightCorner.Y);
}

irr::core::rect<irr::s32> GuiElementPropertyGrid::_parseRect(const std::wstring& textRect)
{
    std::vector<int> rectParts = Utils::parseNumbers<int>(textRect);

    // left, top, right, bottom
    irr::core::rect<irr::s32> parsedRect(rectParts[0], rectParts[1], rectParts[2], rectParts[3]);

    return parsedRect;
}

void GuiElementPropertyGrid::_changePositionPropertyHandler(wxPGProperty* eventProperty)
{
    auto data = this->GetProperty(POSITION_PROPERTY_NAME);
    auto value = data->GetValueAsString().ToStdWstring();

    auto result = _parseRect(value);
    _graphicContext->setSelectedElementPosition(result, EMPTY_CALLBACK_WITH_PARAM);
}

wxBEGIN_EVENT_TABLE(GuiElementPropertyGrid, wxPropertyGrid)
    EVT_PG_CHANGED(Utils::ElementsIDs::SELECT_GUI_ELEMENT_PROPEERTY_GRID,
        GuiElementPropertyGrid::_onPropertyWasSet)
        wxEND_EVENT_TABLE()
}