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

class FApp : public Slab::Core::FAppBase {
public:
    FApp(int argc, const char **argv) : FAppBase(argc, argv, false) {
        // interface->addParameters({&snapshotDBFolders, &criticalParameter});
        Slab::Core::RegisterCLInterface(Interface);

        Slab::Core::FBackendManager::Startup("GLFW");

        Slab::Graphics::FPlotThemeManager::GetInstance();

        Slab::Core::ParseCLArgs(argc, argv);
    }

    int run() override {
        auto gui_backend = Slab::Graphics::GetGraphicsBackend();

        auto osc_plot = Slab::New<Studios::FOscillonPlotting>();

        auto wm = Slab::New<Slab::Graphics::FSlabWindowManager>();
        wm->AddSlabWindow(osc_plot, false);
        gui_backend->GetMainSystemWindow()->AddEventListener(wm);

        gui_backend->Run();

        return 0;
    }
};



int main(int argc, const char **argv) {
    auto run =
            [](int argc, const char **argv)
            {
                FApp app(argc, argv);
                return app.run();
            };

    return Slab::SafetyNet::jump(run, argc, argv);
}
