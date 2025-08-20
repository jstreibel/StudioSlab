//
// Created by joao on 8/14/25.
//

#ifndef STUDIOSLAB_WINDOWMANAGER_H
#define STUDIOSLAB_WINDOWMANAGER_H

#include "Graphics/Modules/ImGui/ImGuiContext.h"
#include "Graphics/Window/WindowManager.h"

class StudioWindowManager final : public Slab::Graphics::FWindowManager {

public:
    explicit StudioWindowManager();

    void AddSlabWindow(const Slab::TPointer<Slab::Graphics::FSlabWindow>&, bool hidden) override;

    bool NotifyRender(const Slab::Graphics::FPlatformWindow&) override;

    bool NotifySystemWindowReshape(int w, int h) override;

private:
    using FSlabWindowPtr = Slab::TPointer<Slab::Graphics::FSlabWindow>;
    using FSlabWindowVec = Slab::Vector<FSlabWindowPtr>;
    FSlabWindowVec SlabWindows;
    int WidthSysWin = 200, HeightSysWin = 200;

    Slab::TPointer<Slab::Graphics::FImGuiContext> ImGuiContext;
    int SidePaneWidth;

    Slab::TPointer<class FSimulationManager> SimulationManager;
};



#endif //STUDIOSLAB_WINDOWMANAGER_H
