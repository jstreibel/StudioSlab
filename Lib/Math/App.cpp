//
// Created by joao on 7/30/23.
//

#include "App.h"

#include <utility>

#include "Math/Numerics/Program/Integrator.h"
#include "Core/Backend/Backend.h"
#include "Core/Backend/BackendManager.h"


namespace Slab::Math {

    App::App(int argc, const char **argv, VoidBuilder::Ptr simBuilder)
            : AppBase(argc, argv), builder(std::move(simBuilder)) {
        parseCLArgs();
    }

    auto App::run() -> int {
        auto program = new NumericalIntegration(*builder.get());
        auto &backend = Core::BackendManager::GetBackend();

        backend.run(program);

        return 0;
    }

}