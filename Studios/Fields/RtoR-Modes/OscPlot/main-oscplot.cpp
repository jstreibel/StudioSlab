//
// Created by joao on 7/6/24.
//

#include "AppBase.h"
#include "CrashPad.h"
#include "Core/Backend/BackendManager.h"

#include "Graphics/Plot2D/PlotThemeManager.h"

#include "OscillonPlotting.h"
#include "Graphics/Window/SlabWindowManager.h"

#include "Core/SlabCore.h"

class App : public Slab::Core::AppBase {
public:
    App(int argc, const char **argv) : AppBase(argc, argv, false) {
        // interface->addParameters({&snapshotDBFolders, &criticalParameter});
        Slab::Core::RegisterCLInterface(interface);

        Slab::Core::BackendManager::Startup("GLFW");

        Slab::Graphics::PlotThemeManager::GetInstance();

        Slab::Core::ParseCLArgs(argc, argv);
    }

    int run() override {
        auto &gui_backend = Slab::Graphics::GetGraphicsBackend();

        auto osc_plot = Slab::New<Studios::OscillonPlotting>();

        auto wm = Slab::New<Slab::Graphics::SlabWindowManager>();
        wm->addSlabWindow(osc_plot);
        gui_backend.addEventListener(wm);

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