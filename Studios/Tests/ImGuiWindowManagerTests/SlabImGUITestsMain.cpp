//
// Created by joao on 6/29/25.
//

#include <SFML/Graphics/Rect.hpp>

#include "Application.h"
#include "StudioSlab.h"
#include "../BezierTests.h"
#include "Core/Tools/Log.h"
#include "Graphics/ImGui/ImGuiWindowManager.h"
#include "Graphics/Window/SlabWindowManager.h"

class FImGuiTest final : public Slab::FApplication
{
public:
    FImGuiTest(int argc, const char* argv[])
        : FApplication("Slab ImGui WindowManager Tests", argc, argv)
    {

    }

protected:
    void OnStart() override
    {
        const auto Platform = GetPlatform();
        auto MainSystemWindow = Platform->GetMainSystemWindow();

        // auto& ImGuiModule = Slab::GetModule<Slab::Graphics::ImGuiModule>("GUI:ImGui");
        // auto GUIContext = ImGuiModule.CreateContext(&*MainSystemWindow);
        const auto GUIContext = MainSystemWindow->GetGUIContext();
        auto ImGuiContext =
            Slab::DynamicPointerCast
            <Slab::Graphics::FImGuiContext>
            (GUIContext);


        Slab::TPointer<Slab::Graphics::FWindowManager> WindowManager;
        if constexpr (false)
            WindowManager = Slab::New<Slab::Graphics::SlabWindowManager>();
        else
            WindowManager = Slab::New<Slab::Graphics::FImGuiWindowManager>(ImGuiContext);

        MainSystemWindow->AddAndOwnEventListener(WindowManager);

        const auto Window = Slab::New<Tests::FBezierTests>(ImGuiContext);
        // Slab::Graphics::FSlabWindow::Config config;
        // config.parent_syswin = &*MainSystemWindow;
        // config.title = "Slab ImGui Test Window";
        // config.win_rect = Slab::Graphics::RectI(100, 700, 100, 600);
        // const auto Window = Slab::New<Slab::Graphics::FSlabWindow>(config);

        WindowManager->AddSlabWindow(Window, false);
    }
};

int main(int argc, const char** argv)
{
    Slab::Run<FImGuiTest>(argc, argv);

    return 0;
}