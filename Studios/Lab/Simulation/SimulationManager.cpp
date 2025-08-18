//
// Created by joao on 8/15/25.
//

#include "SimulationManager.h"

#include <utility>

#include "crude_json.h"
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

Slab::TPointer<Slab::Math::FOutputManager> MakeGenericRtoROutputManager()
{
    auto OutputManager = Slab::New<Slab::Math::FOutputManager>();

    return OutputManager;
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
                const auto Recipe = Slab::New<Modes::FNumericalRecipe_PlaneWaves>();

                BeginRecipe({Recipe, nullptr});
            }
        }
    };
    ImGuiContext->AddMainMenuItem(Item);

    if (Material.Recipe != nullptr)
    {
        const auto Interface = Material.Recipe->GetInterface();

        bool bOpen = true;
        if (ImGui::Begin(Interface->GetName().c_str(), &bOpen)) ExposeInterface(Interface);

        if (ImGui::Button("Run"))
        {
            const auto TaskManager = Slab::Core::GetModule<Slab::Core::MTaskManager>("TaskManager");
            const auto Task = Slab::New<Slab::Math::NumericTask>(Material.Recipe);
            if (Material.OutputManager != nullptr) Task->SetOutputManager(Material.OutputManager);
            TaskManager->AddTask(Task);
            Material.OutputManager = nullptr;
            Material.Recipe = nullptr;
        }

        ImGui::End();

        if (!bOpen)
        {
            Material.Recipe = nullptr;
            Material.OutputManager = nullptr;
        }
    }

    return FPlatformWindowEventListener::NotifyRender(platform_window);
}

void FSimulationManager::BeginRecipe(FMaterial Material)
{
    this->Material = std::move(Material);
}
