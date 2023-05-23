#include "PluginService.h"
#include "boost/dll.hpp"

#include "TUtils.h"

#define PLUGINS_FOLDER L"./Plugins/V1"

#ifdef _WINDOWS
#define PLUGINS_EXTENSION L".+\\.dll$"
#else
#define PLUGINS_EXTENSION L".+\\.so$"
#endif

using namespace std;

namespace Watercolor::Services::Plugins {
    PluginService::PluginService()
    {
        _plugins.insert(std::pair(PluginType::WELCOME_WINDOW, std::vector<boost::shared_ptr<IPlugin>>()));
        _plugins.insert(std::pair(PluginType::TOP_MENU, std::vector<boost::shared_ptr<IPlugin>>()));
    }

    void PluginService::loadPlugins()
    {
        auto pluginFiles = _collectPluginFiles();

        for(auto pluginFile: pluginFiles) {
            auto plugin = boost::dll::import_symbol<IPlugin>(
                pluginFile,
                "plugin"
                );

            _plugins[plugin->getType()].push_back(plugin);
        };
    }

    std::vector<boost::shared_ptr<IPlugin>>& PluginService::getByType(PluginType type)
    {
        return _plugins[type];
    }

    std::vector<std::wstring> PluginService::_collectPluginFiles()
    {
        auto pluginsFolder = std::wstring(PLUGINS_FOLDER);
        auto pluginsExtension = std::wstring(PLUGINS_EXTENSION);

        return TUtils::Filesystem::collectFilesByTemplate(pluginsFolder, pluginsExtension);
    }


}