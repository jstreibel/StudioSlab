//
// Created by joao on 8/15/25.
//

#include "SimulationManager.h"

#include <utility>
#include <sys/wait.h>

#include "../ParameterGUIRenderer.h"
#include "../../Fields/RtoR-Modes/Sim/Recipes/NumericalRecipe_Ak2.h"
#include "Graphics/Modules/GUIModule/GUIContext.h"
#include "Graphics/Modules/ImGui/ImGuiContext.h"

#include "../../Fields/RtoR-Modes/Sim/Recipes/NumericalRecipe_PlaneWaves.h"

#include "../../Fields/RtoR-Modes/Sim/Recipes/NumericalRecipe_wkA.h"
#include "../../Fields/RtoR-Modes/Sim/Recipes/Signal_Ak2_Recipe.h"

#include "../../Fields/RtoR-PureSG/InteractingFormations/input-general-oscillons.h"
#include "../../Fields/RtoR-PureSG/InteractingFormations/input-sym-oscillons.h"
#include "../../Fields/RtoR-PureSG/SingleFormations/input-perturbed.h"
#include "../../Fields/RtoR-PureSG/SingleFormations/input-shockwave.h"
#include "../../Fields/RtoR-PureSG/SingleFormations/InputSingleOscillon.h"

#include "../../Fields/R2toR/LeadingDelta/LeadingDelta.h"

#include "Core/SlabCore.h"
#include "Core/Backend/Modules/TaskManager/TaskManager.h"
#include "Graphics/SlabGraphics.h"
#include "Math/Data/DataManager.h"
#include "Math/Numerics/NumericTask.h"
#include "Math/Numerics/ODE/Output/Sockets/DummyOutput.h"
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
    AddSimulationMenu();
    AddBlueprintMenu(platform_window);

    if (const auto Recipe = Material.GetRecipe(); Recipe != nullptr)
    {
        const auto Interface = Recipe->GetInterface();

        bool bOpen = true;
        if (ImGui::Begin(Interface->GetName().c_str(), &bOpen)) ExposeInterface(Interface);

        if (ImGui::Button("Run"))
        {
            const auto TaskManager = Slab::Core::GetModule<Slab::Core::MTaskManager>("TaskManager");
            const auto Task = Slab::New<Slab::Math::FNumericTask>(Material.GetRecipe(), false);
            Task->SetOutputManager(Material.BuildOutputManager());
            TaskManager->AddTask(Task);
            Material.Clear();
        }

        ImGui::End();

        if (!bOpen) Material.Clear();
    }

    return FPlatformWindowEventListener::NotifyRender(platform_window);
}

void FSimulationManager::CreateBlueprint(const Slab::Graphics::FPlatformWindow& PlatformWindow)
{
    using namespace Lab::Blueprints;

    Blueprint = Slab::New<FBlueprint>();

    BlueprintRenderer = Slab::New<FBlueprintRenderer>(Blueprint, ImGuiContext);
    AddResponder(BlueprintRenderer);

    Blueprint->SetupDemo();
    const auto Recipe = Slab::New<Modes::FNumericalRecipe_PlaneWaves>();

    FBlueprintNode* Node;
    Node = Blueprint->SpawnNodeFromInterface(*Recipe->GetInterface());

    Editor::NavigateToContent();

    Blueprint->BuildNodes();
}

void FSimulationManager::AddBlueprintMenu(const Slab::Graphics::FPlatformWindow& PlatformWindow)
{
    const auto ItemLocation = Slab::Graphics::MainMenuLocation{"Simulations"};
    const auto Item = Slab::Graphics::MainMenuItem{ItemLocation,
        { {"Custom Simulation"} },
        [this, &PlatformWindow](const Slab::Str &ItemString)
        {
            if (ItemString == "Custom Simulation")
            {
                CreateBlueprint(PlatformWindow);

                return;
            }

            Slab::Core::Log::Error() << "Internal error: unidentified item " << ItemString << Slab::Core::Log::Flush;
        }
    };

    ImGuiContext->AddMainMenuItem(Item);
}

void FSimulationManager::AddSimulationMenu()
{
    {
        static auto RtoROutputManagerBuilderFunc = [](Slab::TPointer<Slab::Math::Base::FNumericalRecipe> BaseRecipe)
        {
            const auto KGRecipe = Slab::DynamicPointerCast<Slab::Models::KGRecipe>(std::move(BaseRecipe));
            const auto NumericConfig = Slab::DynamicPointerCast<Slab::Models::FKGNumericConfig>(KGRecipe->GetNumericConfig());

            fix NOut = KGRecipe->GetOutputOptions().GetOutputResolution();
            fix L = NumericConfig->GetL();
            fix t = NumericConfig->Get_t();
            fix MOut = t/L * static_cast<Slab::DevFloat>(NOut);
            fix MaxSteps = NumericConfig->Get_n();
            fix xMin = NumericConfig->Get_xMin();

            const auto SimHistory = Slab::New<Slab::Models::KGRtoR::SimHistory>(
                MaxSteps, t, NOut, MOut, xMin, L, KGRecipe->GetInterface()->GetName(), true);

            auto Data = SimHistory->GetData();
            Slab::Math::FDataManager::AddData(Data);

            auto OutputManager = Slab::New<Slab::Math::FOutputManager>(MaxSteps);
            OutputManager->AddOutputChannel(SimHistory);

            return OutputManager;
        };

        const auto ItemLocation = Slab::Graphics::MainMenuLocation{"Simulations", "ℝ↦ℝ"};
        const auto Item = Slab::Graphics::MainMenuItem{ItemLocation,
            {
                    {"Plane Waves", "Analytic Signum-Gordon"},
                    {"Monochromatic sine wave##1", "{ω,k,A} parameters"},
                    {"Monochromatic sine wave##2", "Q=Ak² invariant parameter"},
                    {"Monochromatic signal", "Q=Ak² invariant parameter"},
                    {Slab::Graphics::MainMenuSeparator},
                    {"Symmetric Oscillon Scattering"},
                    {"General Oscillon Scattering"},
                    {Slab::Graphics::MainMenuSeparator},
                    {"Perturbed Simple Oscillon"},
                    {"Shockwave"},
                    {"Single Oscillon"},
                },
                [this](const Slab::Str &ItemString)
                {
                    Slab::TPointer<Slab::Models::KGRecipe> Recipe;

                    if      (ItemString == "Plane Waves")                 Recipe = Slab::New<Modes::FNumericalRecipe_PlaneWaves>();
                    else if (ItemString == "Monochromatic sine wave##1")  Recipe = Slab::New<Modes::NumericalRecipe_wkA>();
                    else if (ItemString == "Monochromatic sine wave##2")  Recipe = Slab::New<Modes::NumericalRecipe_Ak2>();
                    else if (ItemString == "Monochromatic signal")        Recipe = Slab::New<Modes::Signal_Ak2_Recipe>();
                    else if (ItemString == "Symmetric Oscillon Scattering") Recipe = Slab::New<Studios::PureSG::InputSymmetricOscillon>();
                    else if (ItemString == "Perturbed Simple Oscillon")   Recipe = Slab::New<Studios::PureSG::InputPerturbations>();
                    else if (ItemString == "General Oscillon Scattering") Recipe = Slab::New<Studios::PureSG::InputGeneralOscillons>();
                    else if (ItemString == "Shockwave")                   Recipe = Slab::New<Studios::PureSG::InputShockwave>();
                    else if (ItemString == "Single Oscillon")             Recipe = Slab::New<Studios::PureSG::InputSingleOscillon>();

                    if (Recipe == nullptr)
                    {
                        Slab::Core::Log::Error() << "Internal error: unidentified item " << ItemString << Slab::Core::Log::Flush;
                        return;
                    }

                    const auto DeviceParam = Slab::DynamicPointerCast<Slab::Core::IntegerParameter>(
                            Recipe->GetDeviceConfig().GetInterface()->GetParameter(Slab::Math::PDeviceParamLongName));
                    DeviceParam->SetValue(0);

                    BeginRecipe(FMaterial{Recipe, RtoROutputManagerBuilderFunc});
                }
        };

        ImGuiContext->AddMainMenuItem(Item);
    }


    {
        static auto R2toROutputManagerBuilderFunc = [](Slab::TPointer<Slab::Math::Base::FNumericalRecipe> BaseRecipe)
        {
            const auto KGRecipe = Slab::DynamicPointerCast<Slab::Models::KGRecipe>(std::move(BaseRecipe));
            const auto NumericConfig = Slab::DynamicPointerCast<Slab::Models::FKGNumericConfig>(KGRecipe->GetNumericConfig());

            fix MaxSteps = NumericConfig->Get_n();

            fix DummyOutput = Slab::New<Slab::Math::FDummyOutput>();

            // auto Data = SimHistory->GetData();
            // Slab::Math::FDataManager::AddData(Data);

            auto OutputManager = Slab::New<Slab::Math::FOutputManager>(MaxSteps);
            OutputManager->AddOutputChannel(DummyOutput);

            return OutputManager;
        };

        const auto ItemLocation = Slab::Graphics::MainMenuLocation{"Simulations", "ℝ²↦ℝ"};
        const auto Item = Slab::Graphics::MainMenuItem{ItemLocation,
            {
                    {"Shockwave", ""},
                },
                [this](const Slab::Str &ItemString)
                {
                    Slab::TPointer<Slab::Models::KGRecipe> Recipe;

                    if (ItemString == "Shockwave") Recipe = Slab::New<Studios::Fields::R2toRLeadingDelta::Builder>();

                    if (Recipe == nullptr)
                    {
                        Slab::Core::Log::Error() << "Internal error: unidentified item " << ItemString << Slab::Core::Log::Flush;
                        return;
                    }

                    const auto DeviceParam = Slab::DynamicPointerCast<Slab::Core::IntegerParameter>(
                            Recipe->GetDeviceConfig().GetInterface()->GetParameter(Slab::Math::PDeviceParamLongName));
                    DeviceParam->SetValue(1);

                    Recipe->NotifyInterfaceSetupIsFinished();
                    Recipe->NotifyAllInterfacesSetupIsFinished();

                    BeginRecipe(FMaterial{Recipe, R2toROutputManagerBuilderFunc});
                }
        };

        ImGuiContext->AddMainMenuItem(Item);
    }


}

void FSimulationManager::BeginRecipe(FMaterial RecipeMaterial)
{
    this->Material = std::move(RecipeMaterial);
}
