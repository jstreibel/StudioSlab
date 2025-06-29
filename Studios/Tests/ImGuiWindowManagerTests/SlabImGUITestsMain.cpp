//
// Created by joao on 6/29/25.
//

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

        auto& ImGuiModule = Slab::GetModule<Slab::Graphics::ImGuiModule>("GUI:ImGui");
        auto GUIContext = ImGuiModule.CreateContext(&*MainSystemWindow);
        auto ImGuiContext =
            Slab::DynamicPointerCast
            <Slab::Graphics::SlabImGuiContext>
            (GUIContext);

        // Slab::Pointer<Slab::Graphics::FWindowManager> WM = Slab::New<Slab::Graphics::SlabWindowManager>();
        const Slab::Pointer<Slab::Graphics::FWindowManager> WM = Slab::New<Slab::Graphics::FImGuiWindowManager>(&*MainSystemWindow, ImGuiContext);
        MainSystemWindow->AddAndOwnEventListener(WM);

        WM->AddSlabWindow(Slab::New<Tests::BezierTests>());
    }
};

int main(int argc, const char** argv)
{
    Slab::Run<FImGuiTest>(argc, argv);

    return 0;
}