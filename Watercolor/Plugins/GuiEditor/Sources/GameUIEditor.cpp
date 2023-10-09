#include "GameUIEditor.h"

#include <wx/stc/stc.h>

#include "Items/ClientAreaPanel/ClientAreaPanel.h"
#include "Items/ToolMenu.h"

#include "Services/UIThreadFunctionsRunner/UIThreadFunctionsRunner.h"

namespace GUIEditor
{
    GameUIEditor::GameUIEditor(
        wxWindow *parent,
        boost::shared_ptr<IFunctionsProcessingManager> functionsProcessingManager,
        std::wstring projectDirectoryPath)
        : wxDialog(parent, wxID_ANY, "GUI Editor", wxDefaultPosition,
                   wxSize(1200, 650), wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
    {
        _sizer = new wxBoxSizer(wxVERTICAL);
        this->SetSizer(_sizer);

        auto toolMenu = boost::make_shared<ToolMenu>(this, functionsProcessingManager, projectDirectoryPath);
        _sizer->Add(toolMenu.get(), 0, wxTOP, 5);
        _components.push_back(toolMenu);

        auto clientAreaPanel = boost::make_shared<ClientAreaPanel>(this, functionsProcessingManager);
        _sizer->Add(clientAreaPanel.get(), 1, wxALL | wxEXPAND, 5);
        _components.push_back(clientAreaPanel);

        _sizer->Layout();

        _commandsRunner = boost::make_shared<Services::UIThreadFunctionsRunner>(functionsProcessingManager);
        // todo: change this approach
        _commandsRunner->Start(300);
    }

    GameUIEditor::~GameUIEditor() {}
}