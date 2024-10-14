//
// Created by joao on 7/6/24.
//

#include "App/AppBase.h"
#include "App/CrashPad.h"
#include "Core/Backend/BackendManager.h"
#include "Core/Controller/Interface/InterfaceManager.h"

#include "Graphics/Graph/PlotThemeManager.h"

#include "OscillonPlotting.h"

class App : public Slab::Core::AppBase {
public:
    App(int argc, const char **argv) : AppBase(argc, argv, false) {
        // interface->addParameters({&snapshotDBFolders, &criticalParameter});
        Slab::Core::InterfaceManager::getInstance().registerInterface(interface);

        Slab::Core::BackendManager::Startup("GLFW");

        Slab::Graphics::PlotThemeManager::GetInstance();

        this->parseCLArgs();
    }

    int run() override {
        auto &gui_backend = Slab::Core::BackendManager::GetGUIBackend();

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