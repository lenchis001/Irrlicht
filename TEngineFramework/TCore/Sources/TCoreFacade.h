#pragma once

#include "FunctionsProcessing/IFunctionsProcessingManager.h"
#include "Mixins/SingletonMixin.h"
#include "Di/IDiContainer.h"

void registerTCoreDependencies(boost::shared_ptr<IDiContainer> container);