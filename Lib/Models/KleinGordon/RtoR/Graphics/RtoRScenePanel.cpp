//
// Created by joao on 24/05/24.
//

#include "RtoRScenePanel.h"
#include "Graphics/Graph3D/Actors/TestActor.h"
#include "Graphics/Graph3D/Actors/R2toRFunctionActor.h"

namespace Slab::Models::KGRtoR {
    RtoRScenePanel::RtoRScenePanel(const NumericConfig &params,
                                   GUIWindow &guiWindow,
                                   KGEnergy &hamiltonian)
    : RtoRPanel(params, guiWindow, hamiltonian, "Scene", "3D view of some data")
    , scene(Slab::New<Scene3DWindow>())
    {
        addWindow(scene);

        auto testy = Slab::New<Slab::Graphics::TestActor>();
        testy->setLabel("Test Artie");
        scene->addActor(testy);
    }

    void RtoRScenePanel::setSimulationHistory(R2toR::NumericFunction_constptr simulationHistory,
                                              const R2toRFunctionArtist_ptr &simHistoryArtist) {
        auto functionActor = Slab::New<Slab::Graphics::R2toRFunctionActor>(simulationHistory);

        functionActor->setLabel("ϕ(t,x)");
        scene->addActor(functionActor);

        RtoRPanel::setSimulationHistory(simulationHistory, simHistoryArtist);
    }

} // Slab::Models::KGRtoR