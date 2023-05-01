#include "TamFacade.h"

#include "ISoundService.h"
#include "SoundService.h"

using namespace TAMM;

void registerTamDependencies(boost::shared_ptr<IDiContainer> container)
{
	container->registerSingleton<ISoundService>(boost::make_shared<SoundService>());
}
