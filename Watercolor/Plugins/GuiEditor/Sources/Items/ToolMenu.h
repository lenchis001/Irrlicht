#pragma once

#include <wx/wx.h>

#include "boost/any.hpp"

#include "Services/FunctionsProcessing/IFunctionsProcessingManager.h"

#include "Items/ClientAreaPanel/Items/GuiEditorGraphicContext/GuiEditorGraphicContext.h"

namespace GUIEditor {
class ToolMenu : public wxToolBar {
public:
    ToolMenu(
        wxWindow* parent, 
        boost::shared_ptr<IFunctionsProcessingManager> functionsProcessingManager,
        std::wstring projectDirectoryPath);
    ~ToolMenu();

private:
    GuiEditorGraphicContext* _graphicContext;

    boost::shared_ptr<IFunctionsProcessingManager> _functionsProcessingManager;
    std::wstring _projectDirectoryPath;

    std::wstring _choiceView();

    void _addView(wxCommandEvent& eventData);
    void _removeView(wxCommandEvent& eventData);
    void _openView(wxCommandEvent& eventData);
    void _saveView(wxCommandEvent& eventData);

    wxDECLARE_EVENT_TABLE();
};
}
