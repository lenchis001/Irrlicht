#include "ImageGuiElementPropertyGrid.h"

#include <wx/propgrid/advprops.h>

#include "Utils/LanguageDependents.h"

#define PATH_PROPERTY_NAME L"pathPropertyName"
#define SCALE_PROPERTY_NAME L"scalePropertyName"

namespace GUIEditor {
ImageGuiElementPropertyGrid::ImageGuiElementPropertyGrid(wxWindow* parent, std::wstring elementName, boost::shared_ptr<GuiEditorGraphicContext> graphicContext)
    : GuiElementPropertyGrid(parent, elementName, graphicContext)
{
    _addPathProperty();
    _addScaleProperty();
}

ImageGuiElementPropertyGrid::~ImageGuiElementPropertyGrid()
{
}

void ImageGuiElementPropertyGrid::_addPathProperty()
{
    auto path = _graphicContext->getSelectImageElementPath();
    this->Append(new wxImageFileProperty(PATH_PROPERTY_TITLE, PATH_PROPERTY_NAME, path));

    this->addPropertyChangeHandler(
        PATH_PROPERTY_NAME, boost::bind(&ImageGuiElementPropertyGrid::_changePathPropertyHandler, this, boost::placeholders::_1));
}

void ImageGuiElementPropertyGrid::_addScaleProperty()
{
    auto value = _graphicContext->isSelectedImageScaled();
    this->Append(new wxBoolProperty(SCALE_PROPERTY_TITLE, SCALE_PROPERTY_NAME, value));

    this->addPropertyChangeHandler(
        SCALE_PROPERTY_NAME, boost::bind(&ImageGuiElementPropertyGrid::_changeScalePropertyHandler, this, boost::placeholders::_1));
}

void ImageGuiElementPropertyGrid::_changePathPropertyHandler(wxPGProperty* eventProperty)
{
    auto newPath = eventProperty->GetValue().GetString().ToStdWstring();

    _graphicContext->setSelectedImageElementPath(newPath);
}

void ImageGuiElementPropertyGrid::_changeScalePropertyHandler(wxPGProperty* eventProperty)
{
    auto newValue = eventProperty->GetValue().GetBool();

    _graphicContext->setSelectedImageScale(newValue, EMPTY_CALLBACK);
}
}