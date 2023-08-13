#include <boost/process.hpp>
#include "ToolMenu.h"

#include "Utils/ElementIDs.h"
#include "Utils/LanguageDependents.h"

#include "Utils/Convert.h"

#define ON_NODE_GIZMO_MANIPULATION_TYPE_WAS_CHANGED_HANDLER_NAME L"on gizmo manipulation type changed (tool menu)"

namespace GUIEditor {
std::wstring ToolMenu::_choiceView()
{
    auto views = Utils::collectFilesByTemplate(_projectDirectoryPath, L".*\\.xml");

    wxArrayString choices;
    for (auto& view : views) {
        choices.Add(view);
    }

    wxSingleChoiceDialog viewChoiceDialog(this, L"Choice a view to open:", L"View choicing", choices);
    viewChoiceDialog.ShowModal();

    std::wstring result;
    result = viewChoiceDialog.GetStringSelection();

    return result;
}

void ToolMenu::_addView(wxCommandEvent& eventData)
{
    _graphicContext->makeNew();
}

void ToolMenu::_removeView(wxCommandEvent& eventData)
{
    if (_graphicContext->isViewPathSet()) {
        if (!_graphicContext->removeSubView()) {
            wxMessageBox("An error happened!");
        }
    } else {
        wxMessageBox("View should opened/created before removing.");
    }
}

void ToolMenu::_openView(wxCommandEvent& eventData)
{
    auto choicedView = _choiceView();
    if (!choicedView.empty()) {
        _graphicContext->loadSubView(choicedView, EMPTY_CALLBACK);
    }
}

void ToolMenu::_saveView(wxCommandEvent& eventData)
{
    if (_graphicContext->isViewPathSet()) {
        _graphicContext->saveSubView([](bool result) {
            if (!result) {
                wxMessageBox("An error happened!");
            }
        });
    } else {
        wxTextEntryDialog viewNameEntryDialog(this, L"Name of new view:");

        if (viewNameEntryDialog.ShowModal() == wxID_OK) {
            std::wstring viewName = viewNameEntryDialog.GetValue().ToStdWstring();
            if (!viewName.empty()) {
                //todo: need to save in "Views"
                auto viewsDirectory = _projectDirectoryPath;
                auto path = viewsDirectory + L"/" + viewName + L".xml";
                _graphicContext->saveSubViewAs(path, [](bool result) {
                    if (!result) {
                        wxMessageBox("An error happened!");
                    }
                });
            } else {
                wxMessageBox(L"View name can't be empty.");
            }
        }
    }
}

ToolMenu::ToolMenu(
    wxWindow* parent, 
    boost::shared_ptr<IFunctionsProcessingManager> functionsProcessingManager,
    std::wstring projectDirectoryPath)
    : wxToolBar(parent, wxID_ANY, wxDefaultPosition)
    , _functionsProcessingManager(functionsProcessingManager)
    , _projectDirectoryPath(projectDirectoryPath)
{
    this->SetBackgroundColour(wxColour(0xF5, 0xF5, 0xF5));
    this->AddSeparator();

    this->AddTool(Utils::ElementsIDs::GUI_EDITOR_ADD_VIEW, GUI_EDITOR_WINDOW_TOOLS_MENU_ADD,
        wxBitmap("Resources/GUI/Add.bmp", wxBITMAP_TYPE_BMP),
        wxNullBitmap, wxITEM_NORMAL, GUI_EDITOR_WINDOW_TOOLS_MENU_ADD);

    this->AddTool(Utils::ElementsIDs::GUI_EDITOR_SAVE_VIEW, GUI_EDITOR_WINDOW_TOOLS_MENU_SAVE,
        wxBitmap("Resources/GUI/Save.bmp", wxBITMAP_TYPE_BMP),
        wxNullBitmap, wxITEM_NORMAL, GUI_EDITOR_WINDOW_TOOLS_MENU_SAVE);

    this->AddTool(Utils::ElementsIDs::GUI_EDITOR_REMOVE_VIEW, GUI_EDITOR_WINDOW_TOOLS_MENU_REMOVE,
        wxBitmap("Resources/GUI/Remove.bmp", wxBITMAP_TYPE_BMP),
        wxNullBitmap, wxITEM_NORMAL, GUI_EDITOR_WINDOW_TOOLS_MENU_REMOVE);

    this->AddTool(Utils::ElementsIDs::GUI_EDITOR_OPEN_VIEW, GUI_EDITOR_WINDOW_TOOLS_MENU_OPEN,
        wxBitmap("Resources/GUI/Open.bmp", wxBITMAP_TYPE_BMP),
        wxNullBitmap, wxITEM_NORMAL, GUI_EDITOR_WINDOW_TOOLS_MENU_OPEN);

    this->Realize();
    this->Layout();

    _functionsProcessingManager->addFuctionToQueue(ThreadTypes::GUI_THREAD, [&]() {
        _graphicContext = (GuiEditorGraphicContext*)(wxTheApp->GetTopWindow()->FindWindowById(
            Utils::ElementsIDs::GUI_EDITOR_CANVAS));
    });
}

ToolMenu::~ToolMenu()
{
}

wxBEGIN_EVENT_TABLE(ToolMenu, wxToolBar)
    EVT_TOOL(Utils::ElementsIDs::GUI_EDITOR_ADD_VIEW, ToolMenu::_addView)
        EVT_TOOL(Utils::ElementsIDs::GUI_EDITOR_REMOVE_VIEW, ToolMenu::_removeView)
            EVT_TOOL(Utils::ElementsIDs::GUI_EDITOR_OPEN_VIEW, ToolMenu::_openView)
                EVT_TOOL(Utils::ElementsIDs::GUI_EDITOR_SAVE_VIEW, ToolMenu::_saveView)
                    wxEND_EVENT_TABLE()
}