#include "ServicesFacade.h"

#include "Plugins/PluginService.h"
#include "Controls/ControlIdService.h"

using namespace Watercolor::Services;

void registerServicesDependencies(boost::shared_ptr<IDiContainer> container)
{
	container->registerSingleton<Plugins::IPluginService>(boost::make_shared<Plugins::PluginService>());
	container->registerSingleton<Controls::IControlIdService>(boost::make_shared<Controls::ControlIdService>());
}
