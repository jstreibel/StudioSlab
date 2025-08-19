//
// Created by joao on 8/15/25.
//

#ifndef SIMULATIONMANAGER_H
#define SIMULATIONMANAGER_H
#include <utility>

#include "Graphics/Backend/Events/SystemWindowEventListener.h"
#include "Graphics/Modules/ImGui/ImGuiContext.h"
#include "Math/Numerics/NumericalRecipe.h"
#include "Math/Numerics/OutputManager.h"


class FSimulationManager final : public Slab::Graphics::FPlatformWindowEventListener {
public:
    FSimulationManager() = delete;
    explicit FSimulationManager(Slab::TPointer<Slab::Graphics::FImGuiContext>);

    bool NotifyRender(const Slab::Graphics::FPlatformWindow&) override;

    static void ExposeInterface(const Slab::TPointer<Slab::Core::FInterface>&, int Level=0);

private:
    struct FMaterial
    {
        using FBuilderFunc = std::function<Slab::TPointer<Slab::Math::FOutputManager>(Slab::TPointer<Slab::Math::Base::FNumericalRecipe>)>;
        FMaterial() = default;
        explicit FMaterial(Slab::TPointer<Slab::Math::Base::FNumericalRecipe> Recipe, FBuilderFunc OutputManagerBuilder) : Recipe(std::move(Recipe)), OutputManagerBuilder(std::move(OutputManagerBuilder)) {}

        [[nodiscard]] Slab::TPointer<Slab::Math::FOutputManager>
        BuildOutputManager() const
        {
            assert(Recipe != nullptr);
            if (OutputManagerBuilder != nullptr) return OutputManagerBuilder(Recipe);
            return nullptr;
        }

        [[nodiscard]] Slab::TPointer<Slab::Math::Base::FNumericalRecipe>
        GetRecipe() const
        {
            return Recipe;
        }

        void
        Clear()
        {
            Recipe = nullptr; OutputManagerBuilder = nullptr;
        }

    private:
        Slab::TPointer<Slab::Math::Base::FNumericalRecipe> Recipe = nullptr;
        FBuilderFunc OutputManagerBuilder = nullptr;

    } Material;

    void BeginRecipe(FMaterial);


    Slab::TPointer<Slab::Graphics::FImGuiContext> ImGuiContext;

};



#endif //SIMULATIONMANAGER_H
