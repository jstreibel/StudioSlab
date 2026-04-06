#include "HostedSurfaceV2.h"

#include <utility>

namespace Slab::Graphics::Windowing::V2 {

    FHostedSurfaceV2::FHostedSurfaceV2(FHostedSurfaceDescriptorV2 descriptor,
                                       FHostedSurfaceVisibilityHookV2 visibilityHook,
                                       FHostedSurfacePrepareToFocusHookV2 prepareToFocusHook)
    : Descriptor(std::move(descriptor))
    , VisibilityHook(std::move(visibilityHook))
    , PrepareToFocusHook(std::move(prepareToFocusHook)) {
    }

    auto FHostedSurfaceV2::GetSurfaceId() const -> const Str & {
        return Descriptor.SurfaceId;
    }

    auto FHostedSurfaceV2::GetTitle() const -> const Str & {
        return Descriptor.Title;
    }

    auto FHostedSurfaceV2::SetTitle(Str title) -> void {
        Descriptor.Title = std::move(title);
    }

    auto FHostedSurfaceV2::GetPreferredWorkspaceId() const -> const Str & {
        return Descriptor.PreferredWorkspaceId;
    }

    auto FHostedSurfaceV2::SetPreferredWorkspaceId(Str preferredWorkspaceId) -> void {
        Descriptor.PreferredWorkspaceId = std::move(preferredWorkspaceId);
    }

    auto FHostedSurfaceV2::IsVisibleRequested() const -> bool {
        if (!VisibilityHook) return true;
        return VisibilityHook();
    }

    auto FHostedSurfaceV2::PrepareToFocus() const -> void {
        if (PrepareToFocusHook) PrepareToFocusHook();
    }

    auto FHostedSurfaceV2::MatchesBackingWindow(const Slab::TPointer<Slab::Graphics::FSlabWindow> &window) const -> bool {
        return GetBackingWindow() == window;
    }

    FSlabHostedSurfaceV2::FSlabHostedSurfaceV2(FHostedSurfaceDescriptorV2 descriptor,
                                               Slab::TPointer<Slab::Graphics::FSlabWindow> slabWindow,
                                               FHostedSurfaceVisibilityHookV2 visibilityHook,
                                               FHostedSurfacePrepareToFocusHookV2 prepareToFocusHook)
    : FHostedSurfaceV2(std::move(descriptor), std::move(visibilityHook), std::move(prepareToFocusHook))
    , BackingWindow(std::move(slabWindow)) {
    }

    auto FSlabHostedSurfaceV2::GetBackingWindow() const -> Slab::TPointer<Slab::Graphics::FSlabWindow> {
        return BackingWindow;
    }

    auto FSlabHostedSurfaceV2::WantsClose() const -> bool {
        return BackingWindow == nullptr || BackingWindow->WantsClose();
    }

} // namespace Slab::Graphics::Windowing::V2
