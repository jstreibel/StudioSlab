#include "VisualHost.h"

#include "Core/SlabCore.h"
#include "Graphics/Plot2D/PlotThemeManager.h"
#include "Graphics/SlabGraphics.h"
#include "StudioSlab.h"

#include <chrono>

namespace Slab::Studios::Common {

    namespace {

        class FAutoCloseOnRenderBudgetListener final : public Graphics::FPlatformWindowEventListener {
            std::optional<UInt> MaxFrames;
            std::optional<double> MaxSeconds;
            UInt FrameCount = 0;
            std::optional<std::chrono::steady_clock::time_point> StartTime = std::nullopt;

        public:
            explicit FAutoCloseOnRenderBudgetListener(const FVisualHostAutoCloseConfig &cfg)
            : MaxFrames(cfg.MaxFrames)
            , MaxSeconds(cfg.MaxSeconds) {
            }

            auto NotifyRender(const Graphics::FPlatformWindow &platformWindow) -> bool override {
                const auto now = std::chrono::steady_clock::now();
                if (!StartTime.has_value()) StartTime = now;

                ++FrameCount;

                bool shouldClose = false;
                if (MaxFrames.has_value() && FrameCount >= *MaxFrames) shouldClose = true;
                if (MaxSeconds.has_value()) {
                    const auto elapsed = std::chrono::duration<double>(now - *StartTime).count();
                    if (elapsed >= *MaxSeconds) shouldClose = true;
                }

                if (shouldClose) {
                    const_cast<Graphics::FPlatformWindow&>(platformWindow).SignalClose();
                }

                return false;
            }
        };

        auto ValidateHost(const FVisualHostContext &host) -> void {
            if (host.Backend == nullptr) throw Exception("Visual host backend is null.");
            if (host.PlatformWindow == nullptr) throw Exception("Visual host platform window is null.");
            if (host.WindowManager == nullptr) throw Exception("Visual host window manager is null.");
        }

    } // namespace

    auto CreateGLFWVisualHost(const Str &windowTitle) -> FVisualHostContext {
        Slab::Startup();
        Core::StartBackend("GLFW");
        Core::LoadModule("ModernOpenGL");
        Graphics::FPlotThemeManager::GetInstance();

        FVisualHostContext host;
        host.Backend = Graphics::GetGraphicsBackend();
        if (host.Backend == nullptr) throw Exception("Could not get graphics backend.");

        host.PlatformWindow = host.Backend->GetMainSystemWindow();
        if (host.PlatformWindow == nullptr) throw Exception("Could not get main system window.");

        host.PlatformWindow->SetupGUIContext();
        host.PlatformWindow->SetSystemWindowTitle(windowTitle);

        host.WindowManager = New<Graphics::FSlabWindowManager>();
        host.PlatformWindow->AddAndOwnEventListener(host.WindowManager);

        return host;
    }

    auto AddOwnedEventListener(const FVisualHostContext &host,
                               const TPointer<Graphics::FPlatformWindowEventListener> &listener) -> void {
        ValidateHost(host);
        if (listener == nullptr) throw Exception("Visual host listener is null.");
        host.PlatformWindow->AddAndOwnEventListener(listener);
    }

    auto AddRootSlabWindow(const FVisualHostContext &host,
                           const TPointer<Graphics::FSlabWindow> &rootWindow,
                           const bool hidden) -> void {
        ValidateHost(host);
        if (rootWindow == nullptr) throw Exception("Visual host root slab window is null.");
        host.WindowManager->AddSlabWindow(rootWindow, hidden);
    }

    auto AttachAutoCloseOnRenderBudget(const FVisualHostContext &host,
                                       const FVisualHostAutoCloseConfig &cfg) -> void {
        if (!cfg.MaxFrames.has_value() && !cfg.MaxSeconds.has_value()) return;
        AddOwnedEventListener(host, New<FAutoCloseOnRenderBudgetListener>(cfg));
    }

    auto RunVisualHost(const FVisualHostContext &host) -> void {
        ValidateHost(host);
        host.Backend->Run();
    }

} // namespace Slab::Studios::Common
