#pragma once

#include "boost/smart_ptr.hpp"

#include "TCoreFacade.h"

#include "Plugins/IPluginService.h"
#include "Controls/IControlIdService.h"

void registerServicesDependencies(boost::shared_ptr<IDiContainer> container);