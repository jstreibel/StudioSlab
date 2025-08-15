//
// Created by joao on 8/15/25.
//

#include "SimulationManager.h"

#include <utility>

#include "Graphics/Modules/GUIModule/GUIContext.h"
#include "Graphics/Modules/ImGui/ImGuiContext.h"

FSimulationManager::FSimulationManager(Slab::TPointer<Slab::Graphics::FImGuiContext> ImGuiContext)
: ImGuiContext(std::move(ImGuiContext))
{

}

bool FSimulationManager::NotifyRender(const Slab::Graphics::FPlatformWindow& platform_window)
{
    const auto ItemLocation = Slab::Graphics::MainMenuLocation{"Simulation", "New"};
    const auto Item = Slab::Graphics::MainMenuItem{ItemLocation, {{"𝕄²↦ℝ", "Lorentz-invariant scalar field"}},
    [](const Slab::Str &ItemString)
    {
        if (ItemString == "𝕄²↦ℝ")
        {

        }
    }};
    ImGuiContext->AddMainMenuItem(Item);

    return FPlatformWindowEventListener::NotifyRender(platform_window);
}
