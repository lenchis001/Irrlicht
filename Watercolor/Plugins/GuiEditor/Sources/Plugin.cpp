#include "Plugin.h"

#include "GameUIEditor.h"
#include "Services/FunctionsProcessing/FunctionsProcessingManager.h"

#define CURRENR_MODULE_NAME L"GUIEditor.dll"

using namespace GUIEditor;

std::wstring Plugin::getName()
{
    return L"GUI editor";
}

PluginType Plugin::getType()
{
    return PluginType::TOP_MENU;
}

void Plugin::onTopMenuItemClicked(const std::wstring& openedProjectPath, 
    #ifdef _WINDOWS_
	HWND
	#else
	int
	#endif
    parent
) {
    // That's okay, don't worry. wxWidgets library
    // will clean the memory on it's own
    new wxApp();

    wxImage::AddHandler(new wxJPEGHandler);
    wxWindow parentWindow;

    #ifdef _WINDOWS_
    auto handle = GetModuleHandle(CURRENR_MODULE_NAME);

    assert(handle != nullptr);
    wxEntryStart(handle);

    parentWindow.SetHWND((WXHWND)parent);
    parentWindow.AdoptAttributesFromHWND();
    #else
    #endif
    wxTopLevelWindows.Append(&parentWindow);

    boost::shared_ptr<IFunctionsProcessingManager> functionsProcessingManager = boost::make_shared<FunctionsProcessingManager>();

    // This too
    GameUIEditor* dialog = new GameUIEditor(&parentWindow, functionsProcessingManager, openedProjectPath);
    dialog->ShowModal();

    wxTopLevelWindows.DeleteObject(&parentWindow);
    #ifdef _WINDOWS_
    parentWindow.SetHWND((WXHWND)NULL);
    #endif

    wxEntryCleanup();
}

std::wstring Plugin::onWelcomeWindowButtonClicked(
    #ifdef _WINDOWS_
	HWND
	#else
	int
	#endif
    parent
) { return L""; }

Plugin::~Plugin()
{

}