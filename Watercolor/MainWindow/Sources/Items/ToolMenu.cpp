#include <boost/process.hpp>
#include "ToolMenu.h"

#include "Utils/Utils.h"

#include "Common/CommonData.h"
#include "Common/LanguageDependents.h"

#define ON_NODE_GIZMO_MANIPULATION_TYPE_WAS_CHANGED_HANDLER_NAME L"on gizmo manipulation type changed (tool menu)"

namespace Watercolor {
void ToolMenu::_openVisualCode(wxCommandEvent& eventData)
{
    /*std::wstring editorPath = boost::process::search_path(L"code").wstring();
    if (!editorPath.empty()) {
        std::wstring projectDirectory = _projectDataManager->getPathToScriptsDirectory();
        wxExecute("\"" + editorPath + "\" \"" + projectDirectory + "\"", wxEXEC_ASYNC | wxEXEC_HIDE_CONSOLE);
    } else {
        wxMessageBox("Visual Code isn't installed or added to PATH. You can download it on https://code.visualstudio.com/");
    }*/
}

void ToolMenu::_playLauncher(wxCommandEvent& eventData)
{
    if (!_projectDataManager->isProjectOpened()) {
        wxMessageBox(L"You should open/create a project before.");
    } else {
        std::wstring wProjectDirectory = _projectDataManager->getProjectDirectory();
        std::wstring wencodedProjectDirectory = TUtils::toBase64(wProjectDirectory);
        std::string encodedProjectDirectory = TUtils::toString(wencodedProjectDirectory);

        std::string command = "TLauncher.exe --projectDirectory \"" + encodedProjectDirectory + "\"";

        wxExecute(command.c_str());
    }
}

void ToolMenu::_selectGizmo(wxCommandEvent& eventData)
{
    _graphicContext->swichManipilationType(Common::ManipulationType::SELECT);
}

void ToolMenu::_moveGizmo(wxCommandEvent& eventData)
{
    _graphicContext->swichManipilationType(Common::ManipulationType::MOVE);
}

void ToolMenu::_rotateGizmo(wxCommandEvent& eventData)
{
    _graphicContext->swichManipilationType(Common::ManipulationType::ROTATE);
}

void ToolMenu::_scaleGizmo(wxCommandEvent& eventData)
{
    _graphicContext->swichManipilationType(Common::ManipulationType::SCALE);
}

void ToolMenu::onGizmoManipulationTypeChanged(boost::any value)
{
    Common::ManipulationType type = boost::any_cast<Common::ManipulationType>(value);

    this->ToggleTool(Common::ElementsIDs::GIZMO_SELECT_BUTTON, false);
    this->ToggleTool(Common::ElementsIDs::GIZMO_MOVE_BUTTON, false);
    this->ToggleTool(Common::ElementsIDs::GIZMO_ROTATE_BUTTON, false);
    this->ToggleTool(Common::ElementsIDs::GIZMO_SCALE_BUTTON, false);

    switch (type) {
    case Common::ManipulationType::SELECT:
        this->ToggleTool(Common::ElementsIDs::GIZMO_SELECT_BUTTON, true);
        break;
    case Common::ManipulationType::MOVE:
        this->ToggleTool(Common::ElementsIDs::GIZMO_MOVE_BUTTON, true);
        break;
    case Common::ManipulationType::ROTATE:
        this->ToggleTool(Common::ElementsIDs::GIZMO_ROTATE_BUTTON, true);
        break;
    case Common::ManipulationType::SCALE:
        this->ToggleTool(Common::ElementsIDs::GIZMO_SCALE_BUTTON, true);
        break;
    default:
        break;
    }
}

ToolMenu::ToolMenu(
    wxWindow* parent, 
    boost::shared_ptr<IFunctionsProcessingManager> functionsProcessingManager,
    boost::shared_ptr<Common::IProjectDataManager> projectDataManager)
    : wxToolBar(parent, wxID_ANY, wxDefaultPosition), _projectDataManager(projectDataManager)
{
    this->SetBackgroundColour(wxColour(0xF5, 0xF5, 0xF5));
    this->AddSeparator();
    this->AddSeparator();

    this->AddTool(Common::ElementsIDs::GIZMO_SELECT_BUTTON, MAIN_WINDOW_TOOLS_MENU_SELECT,
            wxBitmap("Resources/GUI/Select.png", wxBITMAP_TYPE_PNG),
            wxNullBitmap, wxITEM_NORMAL, MAIN_WINDOW_TOOLS_MENU_SELECT)
        ->SetToggle(true);
    this->AddSeparator();
    this->AddTool(Common::ElementsIDs::GIZMO_MOVE_BUTTON, MAIN_WINDOW_TOOLS_MENU_MOVE,
            wxBitmap("Resources/GUI/Move.png", wxBITMAP_TYPE_PNG),
            wxNullBitmap, wxITEM_NORMAL, MAIN_WINDOW_TOOLS_MENU_MOVE)
        ->SetToggle(true);
    this->AddSeparator();
    this->AddTool(Common::ElementsIDs::GIZMO_ROTATE_BUTTON, MAIN_WINDOW_TOOLS_MENU_ROTATE,
            wxBitmap("Resources/GUI/Rotate.png", wxBITMAP_TYPE_PNG),
            wxNullBitmap, wxITEM_NORMAL, MAIN_WINDOW_TOOLS_MENU_ROTATE)
        ->SetToggle(true);

    this->AddSeparator();
    this->AddTool(Common::ElementsIDs::GIZMO_SCALE_BUTTON, MAIN_WINDOW_TOOLS_MENU_ROTATE,
            wxBitmap("Resources/GUI/Scale.png", wxBITMAP_TYPE_PNG),
            wxNullBitmap, wxITEM_NORMAL, MAIN_WINDOW_TOOLS_MENU_ROTATE)
        ->SetToggle(true);
    this->AddSeparator();
    this->AddTool(Common::ElementsIDs::OPEN_VISUAL_CODE, MAIN_WINDOW_TOOLS_MENU_OPEN_VC,
        wxBitmap("Resources/GUI/Code.png", wxBITMAP_TYPE_PNG),
        wxNullBitmap, wxITEM_NORMAL, MAIN_WINDOW_TOOLS_MENU_OPEN_VC);
    this->AddSeparator();
    this->AddTool(Common::ElementsIDs::PLAY_LAUNCHER, MAIN_WINDOW_TOOLS_MENU_PLAY,
        wxBitmap("Resources/GUI/Play.png", wxBITMAP_TYPE_PNG),
        wxNullBitmap, wxITEM_NORMAL, MAIN_WINDOW_TOOLS_MENU_PLAY);
    this->Realize();
    this->Layout();

    functionsProcessingManager->addFuctionToQueue(ThreadTypes::RENDER_THREAD, [&]() {
        _graphicContext = (Common::GizmoGraphicContext*)(wxTheApp->GetTopWindow()->FindWindowById(
            Common::ElementsIDs::MAIN_CANVAS));

        _graphicContext->onGizmoManipulationTypeChanged.addEventHandler(ON_NODE_GIZMO_MANIPULATION_TYPE_WAS_CHANGED_HANDLER_NAME,
            boost::bind(&ToolMenu::onGizmoManipulationTypeChanged, this, boost::placeholders::_1));
    });
}

ToolMenu::~ToolMenu()
{
}

wxBEGIN_EVENT_TABLE(ToolMenu, wxToolBar)
    EVT_TOOL(Common::ElementsIDs::OPEN_VISUAL_CODE, ToolMenu::_openVisualCode)
        EVT_TOOL(Common::ElementsIDs::PLAY_LAUNCHER, ToolMenu::_playLauncher)
            EVT_TOOL(Common::ElementsIDs::GIZMO_SELECT_BUTTON, ToolMenu::_selectGizmo)
                EVT_TOOL(Common::ElementsIDs::GIZMO_MOVE_BUTTON, ToolMenu::_moveGizmo)
                    EVT_TOOL(Common::ElementsIDs::GIZMO_ROTATE_BUTTON, ToolMenu::_rotateGizmo)
                        EVT_TOOL(Common::ElementsIDs::GIZMO_SCALE_BUTTON, ToolMenu::_scaleGizmo)
                            wxEND_EVENT_TABLE()
}