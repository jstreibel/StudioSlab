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
    Slab::Pointer<Slab::Models::StochasticPathIntegrals::SPIRecipe> recipe;
    Slab::Pointer<Slab::Math::NumericTask> numeric_task;

public:
    SPIApp(const int argc, const char *argv[])
    : Application("SPI-App", argc, argv)
    , recipe(Slab::New<Slab::Models::StochasticPathIntegrals::SPIRecipe>())
    , numeric_task(Slab::New<Slab::Math::NumericTask>(recipe, false))
    {    }

    ~SPIApp() override = default;

protected:
    Slab::Pointer<Slab::Graphics::MainViewer> main_viewer;
    void OnStart() override {
        GET task_manager = Slab::GetModule<Slab::Core::MTaskManager>("TaskManager");

        numeric_task->init();

        auto job = task_manager.AddTask(numeric_task);
    };
};

int main(const int argc, const char **argv) {
    SPIApp app(argc, argv);

    Slab::SafetyNet::Jump(app);

    return 0;
}