#pragma once

#include"vector"

#include "boost/smart_ptr.hpp"

#include "Api/IPlugin.h"

namespace Watercolor::Services::Plugins {
	class IPluginService {
	public:
		virtual void loadPlugins() = 0;
		virtual std::vector<boost::shared_ptr<IPlugin>>& getByType(PluginType type) = 0;
	};
}