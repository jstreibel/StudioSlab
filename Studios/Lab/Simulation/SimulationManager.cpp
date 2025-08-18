//
// Created by joao on 8/15/25.
//

#include "SimulationManager.h"

#include <utility>

#include "ParameterGUIRenderer.h"
#include "Graphics/Modules/GUIModule/GUIContext.h"
#include "Graphics/Modules/ImGui/ImGuiContext.h"

#include "../../Fields/RtoR-Modes/Sim/Recipes/NumericalRecipe_PlaneWaves.h"
#include "Models/MolecularDynamics/Recipe.h"

FSimulationManager::FSimulationManager(Slab::TPointer<Slab::Graphics::FImGuiContext> ImGuiContext)
: ImGuiContext(std::move(ImGuiContext))
{

}

void FSimulationManager::ExposeInterface(const Slab::TPointer<Slab::Core::FInterface>& Interface, int Level)
{
    ImGui::Indent(Level * 15);
    if (const auto Flags = Level == 0 ? 0 : ImGuiTreeNodeFlags_DefaultOpen; ImGui::CollapsingHeader(Interface->GetName().c_str(), Flags))
    {
        ImGui::TextDisabled("%s", Interface->GetGeneralDescription().c_str());
        ImGui::NewLine();
        const auto Parameters = Interface->GetParameters();
        for (const auto &Parameter : Parameters)
        {
            ParameterGUIRenderer::RenderParameter(Parameter);
        }

        ImGui::NewLine();

        const auto SubInterfaces = Interface->GetSubInterfaces();
        for (const auto &SubInterface : SubInterfaces)
            ExposeInterface(SubInterface, Level + 1);
    }
    ImGui::Unindent(Level * 15);
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

                ExposeInterface(Interface);
            }
        }
        ImGui::End();
    }

    return FPlatformWindowEventListener::NotifyRender(platform_window);
}

void FSimulationManager::AddRecipe(Slab::TPointer<Slab::Math::Base::FNumericalRecipe> Recipe)
{
    Recipes.push_back(std::move(Recipe));
}
