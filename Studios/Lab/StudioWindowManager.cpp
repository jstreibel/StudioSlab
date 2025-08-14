//
// Created by joao on 8/14/25.
//

#include "StudioWindowManager.h"

#include "imgui.h"
#include "StudioConfig.h"

void StudioWindowManager::AddSlabWindow(const Slab::TPointer<Slab::Graphics::FSlabWindow>& Window, bool hidden)
{
    if (SlabWindow != nullptr) RemoveResponder(SlabWindow);

    fix MenuHeight = Slab::Graphics::WindowStyle::menu_height;
    SlabWindow = Window;
    SlabWindow->NotifyReshape(WidthSysWin, HeightSysWin-MenuHeight);
    SlabWindow->Set_x(0);
    SlabWindow->Set_y(MenuHeight);

    AddResponder(Window);
}

bool StudioWindowManager::NotifyRender(const Slab::Graphics::FPlatformWindow& PlatformWindow)
{
    fix MenuHeight = Slab::Graphics::WindowStyle::menu_height;
    ImGui::SetNextWindowPos(ImVec2(0, MenuHeight));
    ImGui::SetNextWindowSize(ImVec2(StudioConfig::SidePaneWidth, HeightSysWin-MenuHeight));
    ImGui::Begin(StudioConfig::SidePaneId, nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
    ImGui::Text("Side pane");
    ImGui::End();
    return FWindowManager::NotifyRender(PlatformWindow);
}


bool StudioWindowManager::NotifySystemWindowReshape(int w, int h)
{
    WidthSysWin = h;
    HeightSysWin = w;

    return FWindowManager::NotifySystemWindowReshape(w, h);
}

