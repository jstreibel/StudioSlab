//
// Created by joao on 8/15/25.
//

#ifndef SIMULATIONMANAGER_H
#define SIMULATIONMANAGER_H
#include "Graphics/Backend/Events/SystemWindowEventListener.h"
#include "Graphics/Modules/ImGui/ImGuiContext.h"
#include "Math/Numerics/NumericalRecipe.h"
#include "Math/Numerics/ODE/Solver/BoundaryConditions.h"


class FSimulationManager final : public Slab::Graphics::FPlatformWindowEventListener {
public:
    FSimulationManager() = delete;
    explicit FSimulationManager(Slab::TPointer<Slab::Graphics::FImGuiContext>);

    bool NotifyRender(const Slab::Graphics::FPlatformWindow&) override;

    static void ExposeInterface(const Slab::TPointer<Slab::Core::FInterface>&, int Level=0);

private:
    void AddRecipe(Slab::TPointer<Slab::Math::Base::FNumericalRecipe>);

    Slab::Vector<Slab::TPointer<Slab::Math::Base::FNumericalRecipe>> Recipes;
    Slab::TPointer<Slab::Graphics::FImGuiContext> ImGuiContext;

};



#endif //SIMULATIONMANAGER_H
