#pragma once

#include "vector"

#include <wx/wx.h>

#include "boost/shared_ptr.hpp"

#include "Services/FunctionsProcessing/IFunctionsProcessingManager.h"

#include "Services/UIThreadFunctionsRunner/UIThreadFunctionsRunner.h"


namespace GUIEditor {
class GameUIEditor : public wxDialog {
public:
    GameUIEditor(
        wxWindow* parent, 
        boost::shared_ptr<IFunctionsProcessingManager> functionsProcessingManager, 
        std::wstring projectDirectoryPath);
    ~GameUIEditor();

private:
    wxSizer* _sizer;

    std::vector<boost::shared_ptr<wxWindow>> _components;
    boost::shared_ptr<Services::UIThreadFunctionsRunner> _commandsRunner;
};
}
