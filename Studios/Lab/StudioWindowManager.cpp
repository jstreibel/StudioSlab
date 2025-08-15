//
// Created by joao on 8/14/25.
//

#include "StudioWindowManager.h"

#include "imgui.h"
#include "StudioConfig.h"
#include "StudioSlab.h"
#include "Graphics/SlabGraphics.h"
#include "Graphics/Modules/ImGui/ImGuiModule.h"

StudioWindowManager::StudioWindowManager(): SidePaneWidth(StudioConfig::SidePaneWidth)
{
    auto &ImGuiModule = Slab::GetModule<Slab::Graphics::FImGuiModule>("ImGui");
    fix PlatformWindow = Slab::Graphics::GetGraphicsBackend()->GetMainSystemWindow();

    ImGuiContext = Slab::DynamicPointerCast<Slab::Graphics::FImGuiContext>(ImGuiModule.CreateContext(PlatformWindow));
    ImGuiContext->SetManualRender(true);

    this->AddResponder(ImGuiContext);
}

void StudioWindowManager::AddSlabWindow(const Slab::TPointer<Slab::Graphics::FSlabWindow>& Window, bool hidden)
{
    if (SlabWindow != nullptr) RemoveResponder(SlabWindow);

    SlabWindow = Window;

    AddResponder(Window);

    NotifySystemWindowReshape(WidthSysWin, HeightSysWin);
}

bool StudioWindowManager::NotifyRender(const Slab::Graphics::FPlatformWindow& PlatformWindow)
{
    ImGuiContext->NewFrame();

    fix MenuHeight = Slab::Graphics::WindowStyle::menu_height;
    ImGui::SetNextWindowPos(ImVec2(0, static_cast<float>(MenuHeight)));
    ImGui::SetNextWindowSize(ImVec2(StudioConfig::SidePaneWidth, static_cast<float>(HeightSysWin - MenuHeight)), ImGuiCond_Appearing);
    ImGui::Begin(StudioConfig::SidePaneId, nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
    ImGui::Text("Side pane");

    if (fix WindowWidth = static_cast<int>(ImGui::GetWindowWidth());
        SidePaneWidth != WindowWidth)
    {
        SidePaneWidth = WindowWidth;
        NotifySystemWindowReshape(WidthSysWin, HeightSysWin);
    }

    ImGui::End();

    FWindowManager::NotifyRender(PlatformWindow);

    ImGuiContext->Render();

    return true;
}


bool StudioWindowManager::NotifySystemWindowReshape(int w, int h)
{
    WidthSysWin = h;
    HeightSysWin = w;

    if (SlabWindow != nullptr)
    {
        fix MenuHeight = Slab::Graphics::WindowStyle::menu_height;
        SlabWindow->NotifyReshape(WidthSysWin-SidePaneWidth, HeightSysWin-MenuHeight);
        SlabWindow->Set_x(SidePaneWidth);
        SlabWindow->Set_y(MenuHeight);
    }

    return FWindowManager::NotifySystemWindowReshape(w, h);
}

