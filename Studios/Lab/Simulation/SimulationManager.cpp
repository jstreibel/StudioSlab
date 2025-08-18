//
// Created by joao on 8/15/25.
//

#include "SimulationManager.h"

#include <utility>

#include "ParameterGUIRenderer.h"
#include "Graphics/Modules/GUIModule/GUIContext.h"
#include "Graphics/Modules/ImGui/ImGuiContext.h"

#include "../../Fields/RtoR-Modes/Sim/Recipes/NumericalRecipe_PlaneWaves.h"
#include "Core/SlabCore.h"
#include "Core/Backend/Modules/TaskManager/TaskManager.h"
#include "Math/Numerics/NumericTask.h"
#include "Models/MolecularDynamics/Recipe.h"

#define MAX_INTERFACE_DEPTH 10

FSimulationManager::FSimulationManager(Slab::TPointer<Slab::Graphics::FImGuiContext> ImGuiContext)
: ImGuiContext(std::move(ImGuiContext))
{

}

void FSimulationManager::ExposeInterface(const Slab::TPointer<Slab::Core::FInterface>& Interface, int Level)
{
    if (Level > MAX_INTERFACE_DEPTH)
    {
        throw Exception("Maximum Interface depth exceeded. Cyclic Interface referencing?");
    }

    if (Level == 0 || ImGui::CollapsingHeader(Interface->GetName().c_str()))
    {
        IN GeneralDescription = Interface->GetGeneralDescription();
        if (!GeneralDescription.empty())
        {
            ImGui::TextDisabled("%s", Interface->GetGeneralDescription().c_str());
            ImGui::NewLine();
        }

        for (const auto Parameters = Interface->GetParameters();
            const auto &Parameter : Parameters) ParameterGUIRenderer::RenderParameter(Parameter);

        ImGui::NewLine();

        const auto SubInterfaces = Interface->GetSubInterfaces();
        for (const auto &SubInterface : SubInterfaces)
            ExposeInterface(SubInterface, Level + 1);
    }
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
                BeginRecipe(Recipe);
            }
        }
    };
    ImGuiContext->AddMainMenuItem(Item);

    if (Recipe != nullptr)
    {
        const auto Interface = Recipe->GetInterface();

        bool bOpen = true;
        if (ImGui::Begin(Interface->GetName().c_str(), &bOpen)) ExposeInterface(Interface);

        if (ImGui::Button("Run"))
        {
            const auto TaskManager = Slab::Core::GetModule<Slab::Core::MTaskManager>("TaskManager");
            const auto Task = Slab::New<Slab::Math::NumericTask>(Recipe, false);
            TaskManager->AddTask(Task);
            Recipe = nullptr;
        }

        ImGui::End();

        if (!bOpen) Recipe = nullptr;


    }

    return FPlatformWindowEventListener::NotifyRender(platform_window);
}

void FSimulationManager::BeginRecipe(Slab::TPointer<Slab::Math::Base::FNumericalRecipe> Recipe)
{
    this->Recipe = std::move(Recipe);
}
