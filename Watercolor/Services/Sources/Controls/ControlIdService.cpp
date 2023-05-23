#include "ControlIdService.h"

#include "CommonFacade.h"

using namespace Watercolor::Services::Controls;

int ControlIdService::getFree()
{
    return _currentFreeId++;
}

ControlIdService::ControlIdService() {
    _currentFreeId = Common::ElementsIDs::FIRST_FREE;
}
