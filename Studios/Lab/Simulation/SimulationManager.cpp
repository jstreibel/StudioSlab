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
#include "Math/Data/DataManager.h"
#include "Math/Numerics/NumericTask.h"
#include "Models/KleinGordon/RtoR/LinearStepping/Output/SimHistory.h"

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
                const auto Recipe = Slab::New<Modes::FNumericalRecipe_PlaneWaves>();
                auto Param = Slab::DynamicPointerCast<Slab::Core::IntegerParameter>(
                    Recipe->GetDeviceConfig().GetInterface()->GetParameter(Slab::Math::PDeviceLongName));
                Param->SetValue(0);

                auto BuilderFunc = [](Slab::TPointer<Slab::Math::Base::FNumericalRecipe> BaseRecipe)
                {
                    const auto KGRecipe = Slab::DynamicPointerCast<Slab::Models::KGRecipe>(std::move(BaseRecipe));
                    const auto NumericConfig = Slab::DynamicPointerCast<Slab::Models::FKGNumericConfig>(KGRecipe->GetNumericConfig());

                    fix NOut = KGRecipe->GetOutputOptions().GetOutputResolution();
                    fix L = NumericConfig->GetL();
                    fix t = NumericConfig->Get_t();
                    fix MOut = t/L * static_cast<Slab::DevFloat>(NOut);
                    fix MaxSteps = NumericConfig->getn();
                    fix xMin = NumericConfig->Get_xMin();

                    auto SimHistory = Slab::New<Slab::Models::KGRtoR::SimHistory>(
                        MaxSteps, t, NOut, MOut, xMin, L, KGRecipe->GetInterface()->GetName() + " (History)", true);

                    auto Data = SimHistory->GetData();
                    Slab::Math::DataKeeper::AddData(Data);

                    auto OutputManager = Slab::New<Slab::Math::FOutputManager>(MaxSteps);
                    OutputManager->AddOutputChannel(SimHistory);

                    return OutputManager;
                };
                BeginRecipe(FMaterial{Recipe, BuilderFunc});
            }
        }
    };
    ImGuiContext->AddMainMenuItem(Item);

    if (const auto Recipe = Material.GetRecipe(); Recipe != nullptr)
    {
        const auto Interface = Recipe->GetInterface();

        bool bOpen = true;
        if (ImGui::Begin(Interface->GetName().c_str(), &bOpen)) ExposeInterface(Interface);

        if (ImGui::Button("Run"))
        {
            const auto TaskManager = Slab::Core::GetModule<Slab::Core::MTaskManager>("TaskManager");
            const auto Task = Slab::New<Slab::Math::NumericTask>(Material.GetRecipe());
            Task->SetOutputManager(Material.BuildOutputManager());
            TaskManager->AddTask(Task);
            Material.Clear();
        }

        ImGui::End();

        if (!bOpen) Material.Clear();
    }

    return FPlatformWindowEventListener::NotifyRender(platform_window);
}

void FSimulationManager::BeginRecipe(FMaterial Material)
{
    this->Material = std::move(Material);
}
