#pragma once

#include "IPluginService.h"

#include "map"

namespace Watercolor::Services::Plugins {
	class PluginService: public IPluginService, public boost::enable_shared_from_this<IPluginService> {
	public:
		PluginService();

		void loadPlugins() override;

		std::vector<boost::shared_ptr<IPlugin>>& getByType(PluginType type) override;
	private:
		std::vector<std::wstring> _collectPluginFiles();

		std::map<PluginType, std::vector<boost::shared_ptr<IPlugin>>> _plugins;
	};
}