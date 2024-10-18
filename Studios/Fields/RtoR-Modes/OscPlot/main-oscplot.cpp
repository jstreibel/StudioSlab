//
// Created by joao on 7/6/24.
//

#include "AppBase.h"
#include "CrashPad.h"
#include "Core/Backend/BackendManager.h"
#include "Core/Controller/CommandLine/CLInterfaceManager.h"

#include "Graphics/Plot2D/PlotThemeManager.h"

#include "OscillonPlotting.h"

class App : public Slab::Core::AppBase {
public:
    App(int argc, const char **argv) : AppBase(argc, argv, false) {
        // interface->addParameters({&snapshotDBFolders, &criticalParameter});
        Slab::Core::CLInterfaceManager::getInstance().registerInterface(interface);

        Slab::Core::BackendManager::Startup("GLFW");

        Slab::Graphics::PlotThemeManager::GetInstance();

        this->parseCLArgs();
    }

    int run() override {
        auto &gui_backend = Slab::Graphics::GetGraphicsBackend();

        auto osc_plot = Slab::New<Studios::OscillonPlotting>();

        gui_backend.addEventListener(osc_plot);

        gui_backend.run();

        return 0;
    }
};



int main(int argc, const char **argv) {
    auto run =
            [](int argc, const char **argv)
            {
                App app(argc, argv);
                return app.run();
            };

    return Slab::Core::SafetyNet::jump(run, argc, argv);
}