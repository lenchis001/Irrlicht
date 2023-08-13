#pragma once

#include "wx/wx.h"

#include "boost/smart_ptr.hpp"

#include "Services/FunctionsProcessing/IFunctionsProcessingManager.h"

namespace GUIEditor::Services
{
    class UIThreadFunctionsRunner : public wxTimer
    {
        boost::shared_ptr<IFunctionsProcessingManager> _functionsProcessingManager;

        void Notify() override;

    public:
        UIThreadFunctionsRunner(boost::shared_ptr<IFunctionsProcessingManager> functionsProcessingManager);
    };
}
