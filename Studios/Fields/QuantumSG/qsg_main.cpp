//
// Created by joao on 26/03/25.
//

#include <Core/Backend/Modules/TaskManager/TaskManager.h>
#include <Graphics/DataViewers/MainViewer.h>
#include <Graphics/DataViewers/Viewers/FourierViewer.h>
#include <Graphics/Window/SlabWindowManager.h>
#include <Math/Function/RtoR/Model/FunctionsCollection/AbsFunction.h>
#include <Math/Numerics/NumericTask.h>
#include <Models/KleinGordon/R2toR/Graphics/R2toROutputOpenGLGeneric.h>

#include "Application.h"
#include "CrashPad.h"
#include "StudioSlab.h"
#include "Models/KleinGordon/RtoR/Graphics/Panels/RtoRRealtimePanel.h"
#include "Models/Stochastic-Path-Integral/SPI-Recipe.h"

class SPIApp final : public Slab::Application {
    using Recipe = Slab::Models::StochasticPathIntegrals::SPIRecipe;
    Slab::Pointer<Recipe> recipe;
    Slab::Pointer<Slab::Math::NumericTask> numeric_task;

public:
    SPIApp(const int argc, const char *argv[])
    : Application("SPI-App", argc, argv)
    , recipe(Slab::New<Recipe>())
    , numeric_task(Slab::New<Slab::Math::NumericTask>(recipe, false))
    {    }

    ~SPIApp() override = default;

protected:
    Slab::Pointer<Slab::Graphics::MainViewer> main_viewer;
    void OnStart() override {
        GET task_manager = Slab::GetModule<Slab::Core::TaskManagerModule>("TaskManager");

        numeric_task->init();
        // auto monitor = Slab::New<Slab::Models::KGR2toR::OutputOpenGL>(1000);

        // auto dV = Slab::New<Slab::Math::RtoR::AbsFunction>();
        // auto config = Slab::New<Slab::Models::KGNumericConfig>();
        // Slab::Models::KGRtoR::KGEnergy hamiltonian(dV);
        // Slab::Graphics::GUIWindow guiWindow;
        // auto monitor = Slab::New<Slab::Models::KGRtoR::RealtimePanel>(config, hamiltonian, guiWindow);

        // main_viewer = Slab::New<Slab::Graphics::MainViewer>();
        // // Slab::Graphics::GetGraphicsBackend()->GetMainSystemWindow()->addAndOwnEventListener(main_viewer);
        // auto sys_win = Slab::Graphics::GetGraphicsBackend()->GetMainSystemWindow();
        // auto manager = Slab::New<Slab::Graphics::SlabWindowManager>();
        // sys_win->addAndOwnEventListener(manager);
        // manager->addSlabWindow(main_viewer);

        // Slab::Graphics::GetGraphicsBackend()->GetMainSystemWindow()->addEventListener(main_viewer);

        //lab::Graphics::GetGraphicsBackend()->GetMainSystemWindow()->addAndOwnEventListener(monitor);

        // auto job = task_manager.addTask(numeric_task);
    };
};

int main(const int argc, const char **argv) {
    SPIApp app(argc, argv);

    Slab::SafetyNet::Jump(app);

    return 0;
}