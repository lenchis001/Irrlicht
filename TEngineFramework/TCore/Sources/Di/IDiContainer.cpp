#include"IDiContainer.h"
#include "DiContainer.h"

boost::shared_ptr<IDiContainer> IDiContainer::createContainer()
{
	return boost::make_shared<DiContainer>();
}
