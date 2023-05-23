#include "UIThreadFunctionsRunner.h"

#include "ThreadTypes/ThreadTypes.h"

namespace Watercolor::Common {
void UIThreadFunctionsRunner::Notify()
{
    _functionsProcessingManager->processFunctions(ThreadTypes::UI_THREAD);
    wxTimer::Notify();
}
UIThreadFunctionsRunner::UIThreadFunctionsRunner(boost::shared_ptr<IFunctionsProcessingManager> functionsProcessingManager):
    _functionsProcessingManager(functionsProcessingManager)
{
}
}