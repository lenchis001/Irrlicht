#pragma once

#include <wx/wx.h>

#include "boost/enable_shared_from_this.hpp"
#include "boost/property_tree/ptree.hpp"
#include "boost/smart_ptr.hpp"
#include "boost/thread.hpp"

#include "CommonFacade.h"

#include "Mixins/SceneNodeGraphicContextMixin.h"

namespace Watercolor::Common {
class GraphicContext : public wxPanel, public Common::SceneNodeGraphicContextMixin, public boost::enable_shared_from_this<GraphicContext> {
public:
    Utils::EventHandler<irr::SEvent> onIrrEvent;

    void pauseRendering();
    void resumeRendering();

    GraphicContext(wxWindow* parent, ThreadType threadType,
        boost::shared_ptr<IFunctionsProcessingManager> functionsProcessingManager,
        wxSize contextSize = wxSize(200, 150),
        wxPoint position = wxDefaultPosition, wxWindowID id = wxID_ANY);
    ~GraphicContext();

protected:
    boost::shared_ptr<irr::IrrlichtDevice> _device = nullptr;
    boost::shared_ptr<irr::video::IVideoDriver> _driver;

    virtual void _shutdownContext();

private:
    wxDECLARE_EVENT_TABLE();

    irr::EKEY_CODE _wxKeyCodeToIrrKeyCore(wxKeyCode code) const;

    void _prepareContext();
    inline void _reDraw();

    void _onKeyDown(wxKeyEvent& eventData);
    void _onKeyUp(wxKeyEvent& eventData);
    void _onSizeChanged(wxSizeEvent& eventData);

    boost::shared_ptr<boost::thread> _drawThread;
    boost::shared_ptr<boost::mutex> _renderingMutex;
};

inline void GraphicContext::_reDraw()
{
    if (_device->run()) {
        _driver->beginScene(true, true, irr::video::SColor(255, 255, 255, 255));

        _sceneManager->drawAll();
        _device->getGUIEnvironment()->drawAll();

        _driver->endScene();
    }
}
}
