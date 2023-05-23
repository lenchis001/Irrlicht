#ifndef WATERCOLOR_IPLUGIN
#define WATERCOLOR_IPLUGIN

#include "string"

#include "boost/config.hpp"

#include "wx/wx.h"

#include "PluginType.h"

class BOOST_SYMBOL_VISIBLE IPlugin {
public:
	virtual std::wstring getName() = 0;

	virtual PluginType getType() = 0;

	virtual void onTopMenuItemClicked(const std::wstring& openedProjectPath, HWND parent) = 0;

	virtual std::wstring onWelcomeWindowButtonClicked(HWND parent) = 0;

	virtual ~IPlugin() {};
};

#endif