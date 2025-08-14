//
// Created by joao on 8/14/25.
//

#include "StudioWindowManager.h"

#include "imgui.h"
#include "StudioConfig.h"

StudioWindowManager::StudioWindowManager(): SidePaneWidth(StudioConfig::SidePaneWidth)
{
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
    fix MenuHeight = Slab::Graphics::WindowStyle::menu_height;
    ImGui::SetNextWindowPos(ImVec2(0, MenuHeight));
    ImGui::SetNextWindowSize(ImVec2(StudioConfig::SidePaneWidth, HeightSysWin-MenuHeight), ImGuiCond_Appearing);
    ImGui::Begin(StudioConfig::SidePaneId, nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
    ImGui::Text("Side pane");

    if (fix WindowWidth = ImGui::GetWindowWidth(); SidePaneWidth != WindowWidth)
    {
        SidePaneWidth = WindowWidth;
        NotifySystemWindowReshape(WidthSysWin, HeightSysWin);
    }

    ImGui::End();

    return FWindowManager::NotifyRender(PlatformWindow);
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

