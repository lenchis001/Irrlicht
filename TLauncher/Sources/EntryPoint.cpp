#include "iostream"

#include "irrlicht.h"

#include "TEngineGameLogicApi/IGame.h"

#include "boost/dll.hpp"
#include "boost/filesystem.hpp"

#include "TCoreFacade.h"
#include "TNCFacade.h"
#include "TAMFacade.h"

#include "Utils/Filesystem.h"

#include "StartupHandler.h"

#if defined (_IRR_WINDOWS_)
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#endif

using namespace TLauncher;

void registerBaseDependencies(boost::shared_ptr<IDiContainer> container) {
    registerTCoreDependencies(container);
    registerTamDependencies(container);
}

void registerIrrlichtDependencies(boost::shared_ptr<irr::IrrlichtDevice> device, boost::shared_ptr<IDiContainer> container) {
    container->registerSingleton(device->getSceneManager());
}

boost::shared_ptr<IGame> loadGame() {
    #if defined (_IRR_WINDOWS_)
        std::wstring gameLibraryPath = L"./GameLogic.dll";
    #else
        std::wstring gameLibraryPath = L"./GameLogic.so";
    #endif

    if (!boost::filesystem::exists(gameLibraryPath)) {
        return nullptr;
    }

    return boost::dll::import_symbol<IGame>(
        gameLibraryPath,
        "game"
        );
}

int main(int argc, char* argv[])
{
    int returnCode = 0;

    auto container = IDiContainer::createContainer();
    registerBaseDependencies(container);

    auto startupHandler = StartupHandler();
    if (startupHandler.parseCommandLine(argc, argv)) {
        boost::shared_ptr<irr::IrrlichtDevice> device = irr::createDevice(irr::video::EDT_OPENGL, irr::core::dimension2du(1270, 600), 32, startupHandler.isFullscreen(), true, true);
        device->setResizable(true);

        if (device) {
            registerIrrlichtDependencies(device, container);

            auto game = loadGame();
            if(!game) {
                std::cout<< "Game library file was not found." << std::endl;
                return 3;
            }

            game->startGame(device);

            auto functionsProcessingManager = container->resolve<IFunctionsProcessingManager>();

            boost::shared_ptr<irr::video::IVideoDriver> videoDriver = device->getVideoDriver();
            boost::shared_ptr<irr::scene::ISceneManager> sceneManager = device->getSceneManager();
            boost::shared_ptr<irr::gui::IGUIEnvironment> guiEnvironment= device->getGUIEnvironment();
            while (device->run() && videoDriver) {
                functionsProcessingManager->processFunctions(ThreadTypes::RENDER_THREAD);
                if (device->isWindowActive()) {
                    videoDriver->beginScene();

                    sceneManager->drawAll();
                    guiEnvironment->drawAll();
#ifdef _DEBUG
                    device->setWindowCaption(std::to_wstring(videoDriver->getFPS()).c_str());
#endif // _DEBUG
                    videoDriver->endScene();
                } else {
                    device->yield();
                }
            }

            functionsProcessingManager->processAll(ThreadTypes::RENDER_THREAD);

            game->finishGame();

            functionsProcessingManager->processAll(ThreadTypes::RENDER_THREAD);

            functionsProcessingManager->processAll(ThreadTypes::RENDER_THREAD);

            device->setEventReceiver(nullptr);
            device->closeDevice();
        } else {
            returnCode = 2;
        }
    } else {
        returnCode = 1;
    }

    return returnCode;
}
