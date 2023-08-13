#include "UIThreadFunctionsRunner.h"

namespace GUIEditor::Services {
void UIThreadFunctionsRunner::Notify()
{
    _functionsProcessingManager->processFunctions(ThreadTypes::GUI_THREAD);
    wxTimer::Notify();
}

UIThreadFunctionsRunner::UIThreadFunctionsRunner(boost::shared_ptr<IFunctionsProcessingManager> functionsProcessingManager):
    _functionsProcessingManager(functionsProcessingManager)
{
}
}