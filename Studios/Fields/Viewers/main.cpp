//
// Created by joao on 7/3/24.
//

#include "Graphics/OpenGL/OpenGL.h"

#include "Core/App/AppBase.h"
#include "Core/App/CrashPad.h"

#include "Core/Controller/Parameter/CommonParameters.h"
#include "Core/Controller/Interface/InterfaceManager.h"

#include "Graphics/Window/WindowStyles.h"

#include "Math/App.h"

#include "Core/Tools/Log.h"

#include "Core/Backend/GLFW/GLFWBackend.h"
#include "Core/Backend/BackendManager.h"

#include "HistoryFileLoader.h"
#include "FourierViewer.h"
#include "Graphics/Graph/PlotThemeManager.h"

using namespace Slab;

class App : public Math::AppBase {
    Core::StringParameter      filename = Core::StringParameter("", "filename,f", ".oscb file.");

public:
    App(int argc, const char **argv)
            : AppBase(argc, argv, false)
    {
        interface->addParameters({&filename});
        Core::InterfaceManager::getInstance().registerInterface(interface);

        Core::BackendManager::Startup(Core::GLFW);

        Slab::Graphics::PlotThemeManager::GetInstance();

        this->parseCLArgs();
    }

    auto run() -> int override {
        auto function = Modes::HistoryFileLoader::Load(*filename);

        auto &guiBackend = Core::BackendManager::GetGUIBackend();

        auto viewer = Slab::New<Studios::Fields::OscViewer>();
        viewer->setFunction(function);

        guiBackend.addEventListener(viewer);

        guiBackend.run();

        return 0;
    }
};

int run(int argc, const char** argv){
    App app(argc, argv);

    return app.run();
}

int main(int argc, const char* argv[]) {
    return Slab::Core::SafetyNet::jump(
            [](int argc, const char **argv)
            {
                App app(argc, argv);
                return app.run();
            }, argc, argv);
}