#ifndef STUDIOSLAB_HOSTED_SURFACE_V2_H
#define STUDIOSLAB_HOSTED_SURFACE_V2_H

#include "Graphics/Window/SlabWindow.h"

#include <functional>

namespace Slab::Graphics::Windowing::V2 {

    struct FHostedSurfaceDescriptorV2 {
        Str SurfaceId;
        Str Title;
        Str PreferredWorkspaceId;
    };

    using FHostedSurfaceVisibilityHookV2 = std::function<bool()>;
    using FHostedSurfacePrepareToFocusHookV2 = std::function<void()>;

    class FHostedSurfaceV2 {
    public:
        explicit FHostedSurfaceV2(FHostedSurfaceDescriptorV2 descriptor,
                                  FHostedSurfaceVisibilityHookV2 visibilityHook = {},
                                  FHostedSurfacePrepareToFocusHookV2 prepareToFocusHook = {});
        virtual ~FHostedSurfaceV2() = default;

        [[nodiscard]] auto GetSurfaceId() const -> const Str &;
        [[nodiscard]] auto GetTitle() const -> const Str &;
        auto SetTitle(Str title) -> void;

        [[nodiscard]] auto GetPreferredWorkspaceId() const -> const Str &;
        auto SetPreferredWorkspaceId(Str preferredWorkspaceId) -> void;

        [[nodiscard]] auto IsVisibleRequested() const -> bool;
        auto PrepareToFocus() const -> void;
        [[nodiscard]] auto MatchesBackingWindow(const Slab::TPointer<Slab::Graphics::FSlabWindow> &window) const -> bool;

        [[nodiscard]] virtual auto GetBackingWindow() const -> Slab::TPointer<Slab::Graphics::FSlabWindow> = 0;
        [[nodiscard]] virtual auto WantsClose() const -> bool = 0;

    private:
        FHostedSurfaceDescriptorV2 Descriptor;
        FHostedSurfaceVisibilityHookV2 VisibilityHook;
        FHostedSurfacePrepareToFocusHookV2 PrepareToFocusHook;
    };

    class FSlabHostedSurfaceV2 final : public FHostedSurfaceV2 {
    public:
        FSlabHostedSurfaceV2(FHostedSurfaceDescriptorV2 descriptor,
                             Slab::TPointer<Slab::Graphics::FSlabWindow> slabWindow,
                             FHostedSurfaceVisibilityHookV2 visibilityHook = {},
                             FHostedSurfacePrepareToFocusHookV2 prepareToFocusHook = {});

        [[nodiscard]] auto GetBackingWindow() const -> Slab::TPointer<Slab::Graphics::FSlabWindow> override;
        [[nodiscard]] auto WantsClose() const -> bool override;

    private:
        Slab::TPointer<Slab::Graphics::FSlabWindow> BackingWindow;
    };

    DefinePointers(FHostedSurfaceV2)
    DefinePointers(FSlabHostedSurfaceV2)

} // namespace Slab::Graphics::Windowing::V2

#endif // STUDIOSLAB_HOSTED_SURFACE_V2_H
