#ifndef STUDIOSLAB_PLOT2D_WINDOW_HOST_V2_H
#define STUDIOSLAB_PLOT2D_WINDOW_HOST_V2_H

#include "Graphics/Window/SlabWindow.h"
#include "Graphics/Plot2D/V2/Backends/OpenGLRenderBackendV2.h"
#include "Graphics/Plot2D/V2/Plot2DWindowV2.h"
#include "Graphics/Plot2D/V2/PlotReflectionSchemaV2.h"

namespace Slab::Graphics::Plot2D::V2 {

    class FPlot2DWindowHostV2 final : public Slab::Graphics::FSlabWindow {
        Str PlotWindowId;
        FOpenGLRenderBackendV2 RenderBackend;
        double AnimatedXMin = -1.0;
        double AnimatedXMax = 1.0;
        double AnimatedYMin = -1.0;
        double AnimatedYMax = 1.0;
        Slab::Graphics::RectR TargetRegion = {-1.0, 1.0, -1.0, 1.0};
        bool bAnimationStateInitialized = false;
        bool bLockUnitAspectRatio = false;
        double AnimationTimeSeconds = 0.28;
        Slab::Graphics::FPoint2D LeftMouseDownViewportPosition{};
        Slab::Graphics::FPoint2D RightMouseDownViewportPosition{};
        bool bLeftDragCommitted = false;
        bool bRightDragCommitted = false;
        bool bLeftPressStartedOnInteractiveTarget = false;
        bool bPendingFitToArtists = false;
        bool bShowInterface = false;
        bool bPendingContextPopup = false;

        [[nodiscard]] static auto DistanceSquared(const Slab::Graphics::FPoint2D &lhs,
                                                  const Slab::Graphics::FPoint2D &rhs) -> Slab::DevFloat;
        [[nodiscard]] auto MakeOverlayId(const char *suffix) const -> Str;

        [[nodiscard]] auto BuildPointerEvent(const FPlot2DWindowV2 &window,
                                             EPlotPointerEventKindV2 kind,
                                             const Slab::Graphics::FPoint2D &viewportPosition,
                                             Slab::Graphics::EMouseButton button = Slab::Graphics::MouseButton_LEFT,
                                             Slab::Graphics::EKeyState buttonState = Slab::Graphics::Release,
                                             Slab::Graphics::EModKeys modKeys = {},
                                             double wheelDx = 0.0,
                                             double wheelDy = 0.0) const -> FPlotPointerEventV2;
        [[nodiscard]] auto FindWindow() const -> FPlot2DWindowV2 *;
        [[nodiscard]] auto IsRegionAnimating() const -> bool;
        auto SyncAnimationStateFromWindow(const FPlot2DWindowV2 &window) -> void;
        [[nodiscard]] auto BuildAnimatedRegion() const -> Slab::Graphics::RectR;
        auto SetRegionImmediate(FPlot2DWindowV2 &window, const Slab::Graphics::RectR &region) -> void;
        auto AnimateRegionTo(const Slab::Graphics::RectR &region) -> void;
        auto EnforceUnitAspectRatio(Slab::Graphics::RectR &region) const -> void;
        auto TryBuildFittedRegion(FPlot2DWindowV2 &window,
                                  Slab::Graphics::RectR &fittedRegionOut,
                                  Slab::DevFloat paddingFraction = 0.08) const -> bool;
        auto AnimateToFittedRegion(FPlot2DWindowV2 &window,
                                   Slab::DevFloat paddingFraction = 0.08) -> bool;
        auto ZoomAroundCenter(FPlot2DWindowV2 &window, Slab::DevFloat scale) -> void;
        auto ToggleUnitAspectRatioLock(FPlot2DWindowV2 &window) -> void;
        [[nodiscard]] auto BuildHudLayout(const Slab::Graphics::RectI &viewport) const -> FPlotHudLayoutV2;
        auto SaveSnapshot() const -> void;

        [[nodiscard]] static auto IsOverlayEditableParameter(
            const FPlotReflectionParameterBindingV2 &parameter) -> bool;
        auto DrawReflectionParameterControl(FPlotReflectionParameterBindingV2 &parameter) -> void;
        auto DrawOverlay(FPlot2DWindowV2 &window) -> void;

    public:
        FPlot2DWindowHostV2(Str plotWindowId, Str title);

        auto RequestFitToArtists() -> void;

        auto NotifyMouseButton(Slab::Graphics::EMouseButton button,
                               Slab::Graphics::EKeyState state,
                               Slab::Graphics::EModKeys keys) -> bool override;
        auto NotifyKeyboard(Slab::Graphics::EKeyMap key,
                            Slab::Graphics::EKeyState state,
                            Slab::Graphics::EModKeys modKeys) -> bool override;
        auto NotifyMouseMotion(int x, int y, int dx, int dy) -> bool override;
        auto NotifyMouseWheel(double dx, double dy) -> bool override;
        auto RegisterDeferredDrawCalls(const Slab::Graphics::FPlatformWindow &platformWindow) -> void override;
        auto ImmediateDraw(const Slab::Graphics::FPlatformWindow &platformWindow) -> void override;

        [[nodiscard]] auto GetPlotWindowId() const -> const Str &;
    };

    DefinePointers(FPlot2DWindowHostV2)

} // namespace Slab::Graphics::Plot2D::V2

#endif // STUDIOSLAB_PLOT2D_WINDOW_HOST_V2_H
