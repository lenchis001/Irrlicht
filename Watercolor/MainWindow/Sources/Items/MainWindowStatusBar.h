#pragma once

#include "wx/wx.h"

#include "boost/any.hpp"
#include "boost/shared_ptr.hpp"

#include "TCoreFacade.h"

#include "Items/Graphic/GizmoGraphicContext.h"

namespace Watercolor {
class MainWindowStatusBar : public wxStatusBar {
public:
    MainWindowStatusBar(wxWindow* parent, boost::shared_ptr<IFunctionsProcessingManager> functionsProcessingManager);
    ~MainWindowStatusBar();

private:
    wxSizer* _sizer;

    boost::shared_ptr<wxGauge> _loadingStatus;

    Common::GraphicContext* _graphicContext;

    boost::shared_ptr<IFunctionsProcessingManager> _functionsProcessingManager;

    void onStartProcess(boost::any);
    void onFinishProcess(boost::any);
};
}