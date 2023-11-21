#pragma once

#include "string"

#include "wx/wx.h"

#include "boost/smart_ptr.hpp"
#include "boost/thread.hpp"

#include "Services/FunctionsProcessing/IFunctionsProcessingManager.h"
#include "Utils/EventHandler.h"

#include "irrlicht.h"

namespace GUIEditor {
class CGUIEditWorkspace;

typedef boost::function<void(boost::shared_ptr<irr::gui::IGUIElement>)> AddGuiElementEventHandler;
typedef boost::function<void(bool)> BoolEventCallback;
typedef boost::function<void()> VoidEventCallback;

#define EMPTY_CALLBACK []() {}
#define EMPTY_CALLBACK_WITH_PARAM [](auto _) {}

class GuiEditorGraphicContext: public wxPanel {
public:
    GuiEditorGraphicContext(wxWindow* parent, boost::shared_ptr<IFunctionsProcessingManager> functionsProcessingManager);
    ~GuiEditorGraphicContext();

    void addButton(AddGuiElementEventHandler callback = EMPTY_CALLBACK_WITH_PARAM);
    void addLabel(AddGuiElementEventHandler callback = EMPTY_CALLBACK_WITH_PARAM);
    void addTextbox(AddGuiElementEventHandler callback = EMPTY_CALLBACK_WITH_PARAM);
    void addWindow(AddGuiElementEventHandler callback = EMPTY_CALLBACK_WITH_PARAM);
    void addCheckBox(AddGuiElementEventHandler callback = EMPTY_CALLBACK_WITH_PARAM);
    void addImage(AddGuiElementEventHandler callback = EMPTY_CALLBACK_WITH_PARAM);
    void addListBox(AddGuiElementEventHandler callback = EMPTY_CALLBACK_WITH_PARAM);

    std::wstring getSelectedElementName() const;
    void setSelectedElementName(const std::wstring& newName, const BoolEventCallback& callback = EMPTY_CALLBACK_WITH_PARAM);
    irr::core::recti getSelectedElementPosition() const;
    void setSelectedElementPosition(const irr::core::position2di& newPosition, const BoolEventCallback& callback = EMPTY_CALLBACK_WITH_PARAM);
    void setSelectedElementPosition(const irr::core::recti& newPosition, const BoolEventCallback& callback = EMPTY_CALLBACK_WITH_PARAM);
    std::wstring getSelectedElementText() const;
    void setSelectedElementText(const std::wstring& newText, const BoolEventCallback& callback = EMPTY_CALLBACK_WITH_PARAM);
    void setSelectedElementAlignment(irr::gui::EGUI_ALIGNMENT left, irr::gui::EGUI_ALIGNMENT right, irr::gui::EGUI_ALIGNMENT top, irr::gui::EGUI_ALIGNMENT bottom);

    // Only fo image elements
    std::wstring getSelectImageElementPath() const;
    void setSelectedImageElementPath(const std::wstring& newPath, const VoidEventCallback& callback = EMPTY_CALLBACK);
    // ----------------------

    std::vector<std::wstring> getSelectedListBoxOptions() const;
    void setSelectedListBoxOptions(const std::vector<std::wstring>& options);

    void makeNew(VoidEventCallback callback = EMPTY_CALLBACK);
    void loadSubView(const std::wstring& path, VoidEventCallback callback = EMPTY_CALLBACK);
    void saveSubView(BoolEventCallback callback = EMPTY_CALLBACK_WITH_PARAM);
    void saveSubViewAs(const std::wstring& path, BoolEventCallback callback = EMPTY_CALLBACK_WITH_PARAM);
    bool removeSubView();

    bool isViewPathSet();

    boost::shared_ptr<CGUIEditWorkspace> getGuiEditWorkspace();

    Utils::EventHandler<boost::shared_ptr<irr::gui::IGUIElement>> onGuiElementSelected, onPositionChanged;
    Utils::EventHandler<void*> onClear;
    Utils::EventHandler<irr::SEvent> onIrrEvent;

private:
    void _clearWorkspace();

    void _shutdownContext();
    void _prepareContext(void *windowId);
    inline void _reDraw();

    void _onMouseEvent(wxMouseEvent& eventData);
    void _onKeyEvent(wxKeyEvent& eventData);
    void _onSizeChanged(wxSizeEvent& eventData);

    boost::shared_ptr<boost::thread> _drawThread;
    boost::shared_ptr<boost::mutex> _renderingMutex;
    boost::shared_ptr<IFunctionsProcessingManager> _functionsProcessingManager;

    boost::shared_ptr<irr::gui::IGUIEnvironment> _guiEnvironment;
    boost::shared_ptr<CGUIEditWorkspace> _editWorkspace;

    boost::shared_ptr<irr::IrrlichtDevice> _device;
    boost::shared_ptr<irr::video::IVideoDriver> _driver;

    std::wstring _currentViewPath;

    wxDECLARE_EVENT_TABLE();
};

inline void GuiEditorGraphicContext::_reDraw()
{
    if (_device->run()) {
        _driver->beginScene(true, true, irr::video::SColor(255, 255, 255, 255));

        _guiEnvironment->drawAll();

        _driver->endScene();
    }
}
}
