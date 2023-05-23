#pragma once

#include "boost/smart_ptr.hpp"

#include "IControlIdService.h"

namespace Watercolor::Services::Controls {
	class ControlIdService : public IControlIdService, public boost::enable_shared_from_this<IControlIdService> {
	public:
		int getFree() override;

		ControlIdService();
	private:
		int _currentFreeId;
	};
}