#ifndef STUDIOSLAB_LAB_V2_WINDOW_MANAGER_H
#define STUDIOSLAB_LAB_V2_WINDOW_MANAGER_H

#include "Graphics/Modules/ImGui/ImGuiContext.h"
#include "Graphics/Window/WindowManager.h"

#include "Math/Data/V2/LiveDataHubV2.h"

class FLabV2WindowManager final : public Slab::Graphics::FWindowManager {
public:
    explicit FLabV2WindowManager();

    void AddSlabWindow(const Slab::TPointer<Slab::Graphics::FSlabWindow> &) override;
    void AddSlabWindow(const Slab::TPointer<Slab::Graphics::FSlabWindow> &, bool hidden) override;

    bool NotifyRender(const Slab::Graphics::FPlatformWindow &) override;
    bool NotifySystemWindowReshape(int w, int h) override;

    [[nodiscard]] auto GetImGuiContext() const -> Slab::TPointer<Slab::Graphics::FImGuiContext>;
    [[nodiscard]] auto GetLiveDataHub() const -> Slab::TPointer<Slab::Math::LiveData::V2::FLiveDataHubV2>;

private:
    using FSlabWindowPtr = Slab::TPointer<Slab::Graphics::FSlabWindow>;
    using FSlabWindowVec = Slab::Vector<FSlabWindowPtr>;

    FSlabWindowVec SlabWindows;

    int WidthSysWin = 200;
    int HeightSysWin = 200;
    int SidePaneWidth = 0;

    Slab::TPointer<Slab::Graphics::FImGuiContext> ImGuiContext;
    Slab::TPointer<Slab::Math::LiveData::V2::FLiveDataHubV2> LiveDataHub;
};

#endif // STUDIOSLAB_LAB_V2_WINDOW_MANAGER_H
