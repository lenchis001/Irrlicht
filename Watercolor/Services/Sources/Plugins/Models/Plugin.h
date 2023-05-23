#pragma once

#include "string"

namespace Watercolor::Services::Plugin {
	class Plugin {
	public:
		std::wstring getName();

	private:
		std::wstring _name;
	};
}