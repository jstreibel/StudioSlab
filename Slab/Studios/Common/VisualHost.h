#ifndef STUDIOSLAB_STUDIOS_VISUAL_HOST_H
#define STUDIOSLAB_STUDIOS_VISUAL_HOST_H

#include "Graphics/Backend/Events/SystemWindowEventListener.h"
#include "Graphics/Backend/GraphicBackend.h"
#include "Graphics/Backend/PlatformWindow.h"
#include "Graphics/Window/SlabWindow.h"
#include "Graphics/Window/SlabWindowManager.h"

#include <optional>

namespace Slab::Studios::Common {

    struct FVisualHostAutoCloseConfig {
        std::optional<UInt> MaxFrames = std::nullopt;
        std::optional<double> MaxSeconds = std::nullopt;
    };

    struct FVisualHostContext {
        TPointer<Graphics::FGraphicBackend> Backend = nullptr;
        TPointer<Graphics::FPlatformWindow> PlatformWindow = nullptr;
        TPointer<Graphics::FSlabWindowManager> WindowManager = nullptr;
    };

    // Phase 1: startup + backend + GUI context initialization for a standard GLFW visual host.
    auto CreateGLFWVisualHost(const Str &windowTitle) -> FVisualHostContext;

    // Phase 2: attach app listeners and slab windows to the initialized host.
    auto AddOwnedEventListener(const FVisualHostContext &host,
                               const TPointer<Graphics::FPlatformWindowEventListener> &listener) -> void;
    auto AddRootSlabWindow(const FVisualHostContext &host,
                           const TPointer<Graphics::FSlabWindow> &rootWindow,
                           bool hidden = false) -> void;
    auto AttachAutoCloseOnRenderBudget(const FVisualHostContext &host,
                                       const FVisualHostAutoCloseConfig &cfg) -> void;

    // Phase 3: run the backend/event/render loop.
    auto RunVisualHost(const FVisualHostContext &host) -> void;

} // namespace Slab::Studios::Common

#endif // STUDIOSLAB_STUDIOS_VISUAL_HOST_H
