//
// Created by joao on 8/15/25.
//

#include "SimulationManager.h"

#include <utility>

#include "Graphics/Modules/GUIModule/GUIContext.h"
#include "Graphics/Modules/ImGui/ImGuiContext.h"

#include "../../Fields/RtoR-Modes/Sim/Recipes/NumericalRecipe_PlaneWaves.h"
#include "Models/MolecularDynamics/Recipe.h"

FSimulationManager::FSimulationManager(Slab::TPointer<Slab::Graphics::FImGuiContext> ImGuiContext)
: ImGuiContext(std::move(ImGuiContext))
{

}

bool FSimulationManager::NotifyRender(const Slab::Graphics::FPlatformWindow& platform_window)
{
    const auto ItemLocation = Slab::Graphics::MainMenuLocation{"Simulation", "Recipes", "𝕄²↦ℝ"};
    const auto Item = Slab::Graphics::MainMenuItem{ItemLocation,
        {
            {"Signum-Gordon Plane Waves"}
        },
        [this](const Slab::Str &ItemString)
        {
            if (ItemString == "Signum-Gordon Plane Waves")
            {
                auto Recipe = Slab::New<Modes::FNumericalRecipe_PlaneWaves>();
                AddRecipe(Recipe);
            }
        }
    };
    ImGuiContext->AddMainMenuItem(Item);

    if (!Recipes.empty())
    {
        if (ImGui::Begin("Recipes"))
        {
            for (const auto &Recipe : Recipes)
            {
                const auto Interface = Recipe->GetInterface();
                ImGui::Text("%s", Interface->GetName().c_str());
                ImGui::Text("%s", Interface->GetGeneralDescription().c_str());

                const auto Parameters = Interface->GetParameters();
                for (const auto &Parameter : Parameters)
                {
                    auto ParamName = Parameter->getCommandLineArgumentName(true);
                    auto ParamDescr = Parameter->getDescription();

                    ImGui::Text("%s", ParamName.c_str());
                    ImGui::SameLine();
                    ImGui::TextDisabled("%s", ParamDescr.c_str());

                }

                ImGui::Separator();
            }
        }
        ImGui::End();
    }

    return FPlatformWindowEventListener::NotifyRender(platform_window);
}

void FSimulationManager::AddRecipe(Slab::TPointer<Slab::Math::Base::NumericalRecipe> Recipe)
{
    Recipes.push_back(std::move(Recipe));
}
