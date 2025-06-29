//
// Created by joao on 24/05/24.
//

#include "RtoRScenePanel.h"
#include "Graphics/Plot3D/Actors/TestActor.h"
#include "Graphics/Plot3D/Actors/R2toRFunctionActor.h"

namespace Slab::Models::KGRtoR {
    RtoRScenePanel::RtoRScenePanel(const Pointer<KGNumericConfig> &params,
                                   FGUIWindow &guiWindow,
                                   KGEnergy &hamiltonian)
    : FRtoRPanel(params, guiWindow, hamiltonian, "Scene", "3D view of some data")
    , scene(Slab::New<Scene3DWindow>())
    {
        AddWindow(scene);

        // auto testy = Slab::New<Slab::Graphics::TestActor>();
        // testy->setLabel("Test Artie");
        // scene->addActor(testy);
    }

    void RtoRScenePanel::SetSimulationHistory(Pointer<const R2toR::FNumericFunction>
 simulationHistory,
                                              const R2toRFunctionArtist_ptr &simHistoryArtist) {
        /*
        auto functionActor = Slab::New<Slab::Graphics::R2toRFunctionActor>(simulationHistory);

        functionActor->setLabel("ϕ(t,x)");
        scene->addActor(functionActor);
         */

        FRtoRPanel::SetSimulationHistory(simulationHistory, simHistoryArtist);
    }

    void
    RtoRScenePanel::SetSpaceFourierHistory(Pointer<const R2toR::FNumericFunction>
 sftHistory, const FDFTDataHistory &history,
                                           const R2toRFunctionArtist_ptr &sftHistoryGraph) {
        auto functionActor = Slab::New<Slab::Graphics::R2toRFunctionActor>(sftHistory);

        functionActor->setLabel("ℱₓ(t,k)");
        scene->addActor(functionActor);

        FRtoRPanel::SetSpaceFourierHistory(sftHistory, history, sftHistoryGraph);
    }

} // Slab::Models::KGRtoR