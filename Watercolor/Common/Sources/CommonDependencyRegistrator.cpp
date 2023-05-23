#include "CommonFacade.h"

#include "TCoreFacade.h"

#include "ProjectDataManager/ProjectDataManager.h"

using namespace Watercolor::Common;

void registerCommonDependencies(boost::shared_ptr<IDiContainer> container)
{
	container->registerSingleton<IProjectDataManager>(boost::make_shared<ProjectDataManager>());
}
