#pragma once

namespace Watercolor::Services::Controls {
	class IControlIdService {
	public:
		virtual int getFree() = 0;
	};
}