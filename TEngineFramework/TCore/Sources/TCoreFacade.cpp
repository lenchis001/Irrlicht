#include "TCoreFacade.h"

#include"FunctionsProcessing/FunctionsProcessingManager.h"

void registerTCoreDependencies(boost::shared_ptr<IDiContainer> container) {
	container->registerSingleton<IFunctionsProcessingManager>(boost::make_shared<FunctionsProcessingManager>());
}