#include "LabV2WindowManager.h"

#include "LabV2Panels.h"
#include "SimulationManagerV2.h"
#include "StudioConfigV2.h"

#include "Core/Backend/Modules/TaskManager/TaskManager.h"
#include "3rdParty/imgui-node-editor/crude_json.h"
#include "imgui.h"
#ifdef IMGUI_HAS_DOCK
#include "imgui_internal.h"
#endif

#include "Core/SlabCore.h"
#include "Core/Reflection/V2/ReflectionCodecsV2.h"
#include "Core/Reflection/V2/GraphSubstrateV2.h"
#include "Core/Reflection/V2/ReflectionSemanticsOverlayV1.h"
#include "Graphics/Modules/Animator/Animator.h"
#include "Graphics/Modules/ImGui/ImGuiModule.h"
#include "Graphics/SlabGraphics.h"
#include "Graphics/Plot2D/Plot2DWindow.h"
#include "Graphics/Plot2D/Plotter.h"
#include "Graphics/Plot2D/PlotThemeManager.h"
#include "Graphics/Plot2D/V2/Artists/AxisArtistV2.h"
#include "Graphics/Plot2D/V2/Artists/PointSetArtistV2.h"
#include "Graphics/Plot2D/V2/Artists/RtoRFunctionArtistV2.h"
#include "Graphics/Plot2D/V2/Artists/BackgroundArtistV2.h"
#include "Graphics/Plot2D/V2/Backends/OpenGLRenderBackendV2.h"
#include "Graphics/Plot2D/V2/Plot2DWindowV2.h"
#include "Graphics/Plot2D/V2/PlotReflectionSchemaV2.h"
#include "Graphics/Window/WindowStyles.h"
#include "Math/Data/V2/SessionLiveViewV2.h"
#include "Math/Function/RtoR/Model/FunctionsCollection/Trigonometric.h"

#include <algorithm>
#include <cctype>
#include <cfloat>
#include <cmath>
#include <cstring>
#include <filesystem>
#include <limits>
#include <sstream>
#include <utility>

namespace {

    constexpr bool CEnableBlueprintGraphTuningUi = false;

    auto AddExitMenuEntry(const Slab::Graphics::FPlatformWindow &platformWindow,
                          Slab::Graphics::FImGuiContext &imguiContext) -> void {
        const auto itemLocation = Slab::Graphics::MainMenuLocation{"System"};
        const auto entry = Slab::Graphics::MainMenuLeafEntry{"Exit", "Alt+F4"};
        const auto action = [&platformWindow](const Slab::Str &item) {
            if (item == "Exit") {
                const_cast<Slab::Graphics::FPlatformWindow *>(&platformWindow)->SignalClose();
            }
        };

        imguiContext.AddMainMenuItem(Slab::Graphics::MainMenuItem{itemLocation, {entry}, action});
    }

    constexpr auto WindowTitleLab = "Lab";
    constexpr auto WindowTitleTasks = "Tasks";
    constexpr auto WindowTitleLiveData = "Live Data";
    constexpr auto WindowTitleLiveControl = "Live Control";
    constexpr auto WindowTitleLiveInteraction = "Live Interaction";
    constexpr auto WindowTitleViews = "Views";
    constexpr auto WindowTitleSimulationLauncher = "Simulation Launcher";
    constexpr auto WindowTitleSchemesInspector = "Interface Inspector";
    constexpr auto WindowTitleBlueprintGraph = "Blueprint Graph";
    constexpr auto WindowTitleGraphPlayground = "Graph Playground";
    constexpr auto WindowTitlePlotInspector = "Plot Inspector";
    constexpr auto PopupBlueprintGraphContext = "SchemesBlueprintGraphContext";
    constexpr auto DockspaceHostName = "##LabDockspaceHost";
    constexpr auto DockspaceNameSimulations = "##LabDockspace-Simulations";
    constexpr auto DockspaceNameSchemes = "##LabDockspace-Schemes";
    constexpr auto DockspaceNameGraphPlayground = "##LabDockspace-GraphPlayground";
    constexpr auto DockspaceNamePlots = "##LabDockspace-Plots";
    constexpr auto WorkspaceTabSimulations = "Simulations";
    constexpr auto WorkspaceTabSchemes = "Schemes";
    constexpr auto WorkspaceTabGraphPlayground = "Graph Playground";
    constexpr auto WorkspaceTabPlots = "Plots";
    constexpr auto PlotWindowInterfaceIdPrefix = "v2.plot.window.";

    class FPlot2DWindowHostV2 final : public Slab::Graphics::FSlabWindow {
        using FPlot2DWindowV2 = Slab::Graphics::Plot2D::V2::FPlot2DWindowV2;

        Slab::Str PlotWindowId;
        Slab::Graphics::Plot2D::V2::FOpenGLRenderBackendV2 RenderBackend;
        double AnimatedXMin = -1.0;
        double AnimatedXMax = 1.0;
        double AnimatedYMin = -1.0;
        double AnimatedYMax = 1.0;
        Slab::Graphics::RectR TargetRegion = {-1.0, 1.0, -1.0, 1.0};
        bool bAnimationStateInitialized = false;
        bool bLockUnitAspectRatio = false;
        double AnimationTimeSeconds = 0.28;

        [[nodiscard]] auto FindWindow() const -> FPlot2DWindowV2 * {
            const auto windows = FPlot2DWindowV2::GetLiveWindows();
            const auto it = std::find_if(windows.begin(), windows.end(), [&](const auto *window) {
                return window != nullptr && window->GetWindowId() == PlotWindowId;
            });
            if (it == windows.end()) return nullptr;
            return *it;
        }

        [[nodiscard]] auto IsRegionAnimating() const -> bool {
            return Slab::Graphics::FAnimator::Contains(AnimatedXMin) ||
                   Slab::Graphics::FAnimator::Contains(AnimatedXMax) ||
                   Slab::Graphics::FAnimator::Contains(AnimatedYMin) ||
                   Slab::Graphics::FAnimator::Contains(AnimatedYMax);
        }

        auto SyncAnimationStateFromWindow(const FPlot2DWindowV2 &window) -> void {
            const auto region = window.GetRegion();
            if (!bAnimationStateInitialized || !IsRegionAnimating()) {
                AnimatedXMin = region.xMin;
                AnimatedXMax = region.xMax;
                AnimatedYMin = region.yMin;
                AnimatedYMax = region.yMax;
                TargetRegion = region;
                bAnimationStateInitialized = true;
            }
        }

        auto BuildAnimatedRegion() const -> Slab::Graphics::RectR {
            return {
                static_cast<Slab::DevFloat>(AnimatedXMin),
                static_cast<Slab::DevFloat>(AnimatedXMax),
                static_cast<Slab::DevFloat>(AnimatedYMin),
                static_cast<Slab::DevFloat>(AnimatedYMax)
            };
        }

        auto SetRegionImmediate(FPlot2DWindowV2 &window, const Slab::Graphics::RectR &region) -> void {
            AnimatedXMin = region.xMin;
            AnimatedXMax = region.xMax;
            AnimatedYMin = region.yMin;
            AnimatedYMax = region.yMax;
            TargetRegion = region;
            bAnimationStateInitialized = true;
            window.SetRegion(region);
        }

        auto AnimateRegionTo(const Slab::Graphics::RectR &region) -> void {
            TargetRegion = region;
            Slab::Graphics::FAnimator::Set(AnimatedXMin, region.xMin, AnimationTimeSeconds);
            Slab::Graphics::FAnimator::Set(AnimatedXMax, region.xMax, AnimationTimeSeconds);
            Slab::Graphics::FAnimator::Set(AnimatedYMin, region.yMin, AnimationTimeSeconds);
            Slab::Graphics::FAnimator::Set(AnimatedYMax, region.yMax, AnimationTimeSeconds);
            bAnimationStateInitialized = true;
        }

    public:
        FPlot2DWindowHostV2(Slab::Str plotWindowId, Slab::Str title)
        : FSlabWindow(Slab::Graphics::FSlabWindowConfig(std::move(title)))
        , PlotWindowId(std::move(plotWindowId)) {
            Slab::Core::LoadModule("RealTimeAnimation");
        }

        auto NotifyMouseButton(const Slab::Graphics::EMouseButton button,
                               const Slab::Graphics::EKeyState state,
                               const Slab::Graphics::EModKeys keys) -> bool override {
            return FSlabWindow::NotifyMouseButton(button, state, keys);
        }

        auto NotifyMouseMotion(const int x, const int y, const int dx, const int dy) -> bool override {
            if (FSlabWindow::NotifyMouseMotion(x, y, dx, dy)) return true;

            auto *window = FindWindow();
            if (window == nullptr) return false;

            SyncAnimationStateFromWindow(*window);

            const auto mouseState = GetMouseState();
            if (mouseState == nullptr) return false;

            auto region = BuildAnimatedRegion();
            bool bHandled = false;

            if (mouseState->IsLeftPressed()) {
                const auto viewportWidth = static_cast<Slab::DevFloat>(std::max(1, GetWidth()));
                const auto viewportHeight = static_cast<Slab::DevFloat>(std::max(1, GetHeight()));

                const auto dxClamped = -static_cast<Slab::DevFloat>(mouseState->dx) / viewportWidth;
                const auto dyClamped = static_cast<Slab::DevFloat>(mouseState->dy) / viewportHeight;

                const auto dxGraph = region.GetWidth() * dxClamped;
                const auto dyGraph = region.GetHeight() * dyClamped;

                region = {
                    region.xMin + dxGraph,
                    region.xMax + dxGraph,
                    region.yMin + dyGraph,
                    region.yMax + dyGraph
                };

                window->SetAutoFitRanges(false);
                SetRegionImmediate(*window, region);
                bHandled = true;
            }

            if (mouseState->IsRightPressed()) {
                if (bLockUnitAspectRatio) {
                    constexpr Slab::DevFloat factor = 0.01;
                    const auto d = static_cast<Slab::DevFloat>(1.0 - factor * static_cast<Slab::DevFloat>(dx - dy));
                    const auto x0 = region.xCenter();
                    const auto hw = static_cast<Slab::DevFloat>(0.5 * region.GetWidth() * d);
                    region = {x0 - hw, x0 + hw, region.yMin, region.yMax};
                } else {
                    constexpr Slab::DevFloat factor = 0.01;
                    const auto dw = static_cast<Slab::DevFloat>(1.0 - factor * static_cast<Slab::DevFloat>(dx));
                    const auto dh = static_cast<Slab::DevFloat>(1.0 + factor * static_cast<Slab::DevFloat>(dy));

                    const auto x0 = region.xCenter();
                    const auto y0 = region.yCenter();
                    const auto hw = static_cast<Slab::DevFloat>(0.5 * region.GetWidth() * dw);
                    const auto hh = static_cast<Slab::DevFloat>(0.5 * region.GetHeight() * dh);
                    region = {x0 - hw, x0 + hw, y0 - hh, y0 + hh};
                }

                window->SetAutoFitRanges(false);
                SetRegionImmediate(*window, region);
                bHandled = true;
            }

            return bHandled;
        }

        auto NotifyMouseWheel(const double dx, const double dy) -> bool override {
            if (FSlabWindow::NotifyMouseWheel(dx, dy)) return true;
            if (std::abs(dy) < 1.0e-12) return false;

            auto *window = FindWindow();
            if (window == nullptr) return false;

            SyncAnimationStateFromWindow(*window);

            constexpr Slab::DevFloat factor = 1.2;
            const auto d = std::pow(factor, -dy);

            auto targetRegion = TargetRegion;
            if (!IsRegionAnimating()) {
                targetRegion = BuildAnimatedRegion();
            }

            const auto x0 = targetRegion.xCenter();
            const auto hw = static_cast<Slab::DevFloat>(0.5 * targetRegion.GetWidth() * d);
            targetRegion.xMin = x0 - hw;
            targetRegion.xMax = x0 + hw;

            const auto y0 = targetRegion.yCenter();
            const auto hh = static_cast<Slab::DevFloat>(0.5 * targetRegion.GetHeight() * d);
            targetRegion.yMin = y0 - hh;
            targetRegion.yMax = y0 + hh;

            if (bLockUnitAspectRatio) {
                const auto viewport = GetViewport();
                const auto viewportWidth = static_cast<Slab::DevFloat>(std::max(1, viewport.GetWidth()));
                const auto viewportHeight = static_cast<Slab::DevFloat>(std::max(1, viewport.GetHeight()));
                const auto viewportAspect = viewportWidth / viewportHeight;
                if (viewportAspect > 0.0) {
                    const auto halfWidth = static_cast<Slab::DevFloat>(0.5 * targetRegion.GetWidth());
                    const auto halfHeight =
                        static_cast<Slab::DevFloat>(std::max(halfWidth / viewportAspect, static_cast<Slab::DevFloat>(1e-8)));
                    targetRegion.yMin = y0 - halfHeight;
                    targetRegion.yMax = y0 + halfHeight;
                }
            }

            window->SetAutoFitRanges(false);
            AnimateRegionTo(targetRegion);

            return true;
        }

        auto ImmediateDraw(const Slab::Graphics::FPlatformWindow &platformWindow) -> void override {
            FSlabWindow::ImmediateDraw(platformWindow);

            auto *window = FindWindow();
            if (window == nullptr) {
                Close();
                return;
            }

            const auto width = std::max(1, GetWidth());
            const auto height = std::max(1, GetHeight());
            window->SetViewport({0, width, 0, height});

            if (!window->GetTitle().empty()) {
                GetConfig().Title = window->GetTitle();
            }

            SyncAnimationStateFromWindow(*window);
            window->SetRegion(BuildAnimatedRegion());

            (void) window->Render(RenderBackend);
        }

        [[nodiscard]] auto GetPlotWindowId() const -> const Slab::Str & {
            return PlotWindowId;
        }
    };

    auto BuildOperationSummary(const Slab::Core::Reflection::V2::FOperationResultV2 &result) -> Slab::Str {
        using namespace Slab::Core::Reflection::V2;

        Slab::StringStream ss;
        ss << "[" << ToString(result.Status) << "]";
        if (!result.ErrorCode.empty()) ss << " " << result.ErrorCode;
        if (!result.ErrorMessage.empty()) ss << " - " << result.ErrorMessage;
        if (result.IsOk()) {
            ss << " (" << result.OutputMap.size() << " outputs)";
        }
        return ss.str();
    }

    auto TruncateLabel(const Slab::Str &label, std::size_t maxChars = 56) -> Slab::Str {
        if (label.size() <= maxChars) return label;
        return label.substr(0, maxChars - 3) + "...";
    }

    auto ContainsCaseInsensitive(const Slab::Str &value, const Slab::Str &needle) -> bool {
        if (needle.empty()) return true;
        if (needle.size() > value.size()) return false;

        auto loweredValue = value;
        auto loweredNeedle = needle;
        std::transform(loweredValue.begin(), loweredValue.end(), loweredValue.begin(), [](const unsigned char c) {
            return static_cast<char>(std::tolower(c));
        });
        std::transform(loweredNeedle.begin(), loweredNeedle.end(), loweredNeedle.begin(), [](const unsigned char c) {
            return static_cast<char>(std::tolower(c));
        });
        return loweredValue.find(loweredNeedle) != Slab::Str::npos;
    }

    auto DrawNodeBadge(ImDrawList *drawList, const ImVec2 position, const Slab::Str &text, const ImU32 color) -> float {
        if (drawList == nullptr || text.empty()) return 0.0f;

        const auto compactText = TruncateLabel(text, 14);
        const auto textSize = ImGui::CalcTextSize(compactText.c_str());
        const auto size = ImVec2(textSize.x + 12.0f, textSize.y + 4.0f);
        drawList->AddRectFilled(position, ImVec2(position.x + size.x, position.y + size.y), color, 5.0f);
        drawList->AddText(ImVec2(position.x + 6.0f, position.y + 2.0f), IM_COL32(246, 248, 252, 255), compactText.c_str());
        return size.x + 6.0f;
    }

    auto MeasureBadgeWidth(const Slab::Str &text) -> float {
        if (text.empty()) return 0.0f;
        const auto compactText = TruncateLabel(text, 14);
        const auto textSize = ImGui::CalcTextSize(compactText.c_str());
        return textSize.x + 12.0f + 6.0f;
    }

    auto ToCompactMutabilityLabel(const Slab::Core::Reflection::V2::EParameterMutability value) -> Slab::Str {
        using namespace Slab::Core::Reflection::V2;
        switch (value) {
            case EParameterMutability::Const: return "Const";
            case EParameterMutability::RuntimeMutable: return "Runtime";
            case EParameterMutability::RestartRequired: return "Restart";
        }
        return "Unknown";
    }

    auto ToCompactExposureLabel(const Slab::Core::Reflection::V2::EParameterExposure value) -> Slab::Str {
        using namespace Slab::Core::Reflection::V2;
        switch (value) {
            case EParameterExposure::Hidden: return "Hidden";
            case EParameterExposure::ReadOnlyExposed: return "ReadOnly";
            case EParameterExposure::WritableExposed: return "Writable";
        }
        return "Unknown";
    }

    auto ToCompactRunStateLabel(const Slab::Core::Reflection::V2::ERunStatePolicy value) -> Slab::Str {
        using namespace Slab::Core::Reflection::V2;
        switch (value) {
            case ERunStatePolicy::Any: return "Any";
            case ERunStatePolicy::StoppedOnly: return "Stopped";
            case ERunStatePolicy::RunningOnly: return "Running";
        }
        return "Unknown";
    }

    auto ToCompactThreadLabel(const Slab::Core::Reflection::V2::EThreadAffinity value) -> Slab::Str {
        using namespace Slab::Core::Reflection::V2;
        switch (value) {
            case EThreadAffinity::Any: return "Any";
            case EThreadAffinity::UI: return "UI";
            case EThreadAffinity::Simulation: return "Sim";
            case EThreadAffinity::Worker: return "Worker";
        }
        return "Unknown";
    }

    auto ToCompactSideEffectLabel(const Slab::Core::Reflection::V2::ESideEffectClass value) -> Slab::Str {
        using namespace Slab::Core::Reflection::V2;
        switch (value) {
            case ESideEffectClass::None: return "None";
            case ESideEffectClass::LocalState: return "Local";
            case ESideEffectClass::TaskLifecycle: return "Task";
            case ESideEffectClass::IO: return "IO";
            case ESideEffectClass::External: return "External";
        }
        return "Unknown";
    }

    auto WithPolicyPrefix(const char prefix, const Slab::Str &value) -> Slab::Str {
        return Slab::Str(1, prefix) + ":" + value;
    }

    namespace json = crude_json;

    [[nodiscard]] auto JsonTryGetValue(const json::value &value, const char *key) -> const json::value * {
        const auto *object = value.get_ptr<json::object>();
        if (object == nullptr || key == nullptr) return nullptr;
        const auto it = object->find(key);
        if (it == object->end()) return nullptr;
        return &it->second;
    }

    [[nodiscard]] auto JsonReadString(const json::value &value, const char *key, const Slab::Str &fallback = {}) -> Slab::Str {
        const auto *entry = JsonTryGetValue(value, key);
        if (entry == nullptr) return fallback;
        const auto *str = entry->get_ptr<json::string>();
        if (str == nullptr) return fallback;
        return *str;
    }

    [[nodiscard]] auto JsonReadBool(const json::value &value, const char *key, const bool fallback = false) -> bool {
        const auto *entry = JsonTryGetValue(value, key);
        if (entry == nullptr) return fallback;
        const auto *boolean = entry->get_ptr<json::boolean>();
        if (boolean == nullptr) return fallback;
        return *boolean;
    }

    [[nodiscard]] auto JsonReadDouble(const json::value &value, const char *key, const double fallback = 0.0) -> double {
        const auto *entry = JsonTryGetValue(value, key);
        if (entry == nullptr) return fallback;
        const auto *number = entry->get_ptr<json::number>();
        if (number == nullptr) return fallback;
        return *number;
    }

    [[nodiscard]] auto JsonReadUSize(const json::value &value, const char *key, const std::size_t fallback = 0) -> std::size_t {
        const auto numeric = JsonReadDouble(value, key, static_cast<double>(fallback));
        if (numeric < 0.0) return fallback;
        return static_cast<std::size_t>(numeric);
    }

    [[nodiscard]] auto JsonReadVec2(const json::value &value, const char *key, const ImVec2 fallback = ImVec2(0.0f, 0.0f)) -> ImVec2 {
        const auto *entry = JsonTryGetValue(value, key);
        if (entry == nullptr) return fallback;

        const auto *arr = entry->get_ptr<json::array>();
        if (arr == nullptr || arr->size() < 2) return fallback;
        const auto *x = (*arr)[0].get_ptr<json::number>();
        const auto *y = (*arr)[1].get_ptr<json::number>();
        if (x == nullptr || y == nullptr) return fallback;
        return ImVec2(static_cast<float>(*x), static_cast<float>(*y));
    }

    auto JsonWriteVec2(const ImVec2 value) -> json::value {
        json::array arr;
        arr.emplace_back(static_cast<double>(value.x));
        arr.emplace_back(static_cast<double>(value.y));
        return json::value(std::move(arr));
    }

    [[nodiscard]] auto DistanceSquared(const ImVec2 a, const ImVec2 b) -> float {
        const auto dx = a.x - b.x;
        const auto dy = a.y - b.y;
        return dx * dx + dy * dy;
    }

    [[nodiscard]] auto CubicBezierPoint(const ImVec2 p0, const ImVec2 p1, const ImVec2 p2, const ImVec2 p3, const float t) -> ImVec2 {
        const auto u = 1.0f - t;
        const auto tt = t * t;
        const auto uu = u * u;
        const auto uuu = uu * u;
        const auto ttt = tt * t;
        return ImVec2(
            uuu * p0.x + 3.0f * uu * t * p1.x + 3.0f * u * tt * p2.x + ttt * p3.x,
            uuu * p0.y + 3.0f * uu * t * p1.y + 3.0f * u * tt * p2.y + ttt * p3.y);
    }

    [[nodiscard]] auto DistanceSquaredToBezier(const ImVec2 p0,
                                               const ImVec2 p1,
                                               const ImVec2 p2,
                                               const ImVec2 p3,
                                               const ImVec2 point) -> float {
        constexpr int Samples = 24;
        float minDistance = std::numeric_limits<float>::max();
        auto previous = p0;
        for (int i = 1; i <= Samples; ++i) {
            const auto t = static_cast<float>(i) / static_cast<float>(Samples);
            const auto current = CubicBezierPoint(p0, p1, p2, p3, t);

            const auto segment = ImVec2(current.x - previous.x, current.y - previous.y);
            const auto fromPrevious = ImVec2(point.x - previous.x, point.y - previous.y);
            const auto segmentLengthSq = segment.x * segment.x + segment.y * segment.y;
            float projectedT = 0.0f;
            if (segmentLengthSq > 1.0e-6f) {
                projectedT = std::clamp(
                    (fromPrevious.x * segment.x + fromPrevious.y * segment.y) / segmentLengthSq,
                    0.0f,
                    1.0f);
            }

            const auto closestPoint = ImVec2(
                previous.x + segment.x * projectedT,
                previous.y + segment.y * projectedT);
            minDistance = std::min(minDistance, DistanceSquared(point, closestPoint));
            previous = current;
        }

        return minDistance;
    }

} // namespace

FLabV2WindowManager::FLabV2WindowManager()
: SidePaneWidth(FStudioConfigV2::SidePaneWidth) {
    SchemesBlueprintDocument.Mode = Slab::Core::Reflection::V2::EGraphModeV2::SchemesBlueprint;
    SchemesBlueprintDocument.Canvas.PanX = 80.0f;
    SchemesBlueprintDocument.Canvas.PanY = 80.0f;
    SchemesBlueprintDocument.Canvas.bShowGrid = true;
    SchemesBlueprintDocument.Canvas.bShowMinimap = false;
    PlaygroundTemplateDocument.Mode = Slab::Core::Reflection::V2::EGraphModeV2::Template;
    PlaygroundTemplateDocument.Canvas.PanX = 90.0f;
    PlaygroundTemplateDocument.Canvas.PanY = 70.0f;
    PlaygroundTemplateDocument.Canvas.bShowGrid = true;
    PlaygroundTemplateDocument.Canvas.bShowMinimap = true;
    PlaygroundRoutingDocument.Mode = Slab::Core::Reflection::V2::EGraphModeV2::Routing;

    const auto imGuiModule = Slab::Core::GetModule<Slab::Graphics::FImGuiModule>("ImGui");
    const auto platformWindow = Slab::Graphics::GetGraphicsBackend()->GetMainSystemWindow();

    ImGuiContext = Slab::DynamicPointerCast<Slab::Graphics::FImGuiContext>(imGuiModule->CreateContext(platformWindow));
    ImGuiContext->SetManualRender(true);
    ImGuiContext->SetMainMenuPresentation(Slab::Graphics::FImGuiContext::EMainMenuPresentation::Hidden);

    LiveDataHub = Slab::New<Slab::Math::LiveData::V2::FLiveDataHubV2>();
    LiveControlHub = Slab::New<Slab::Math::LiveControl::V2::FLiveControlHubV2>();
    SimulationManager = Slab::New<FSimulationManagerV2>(
        ImGuiContext,
        LiveDataHub,
        LiveControlHub,
        [this](const Slab::TPointer<Slab::Graphics::FSlabWindow> &window) { QueueSlabWindow(window); },
        [this]() { RequestSimulationLauncherVisible(); });

    LegacyCatalogSourceId = "labv2.catalog.legacy";
    PlotCatalogSourceId = "labv2.catalog.plots";
    Slab::Core::Reflection::V2::InitSemanticLayerV1();
    auto &catalogRegistry = Slab::Core::Reflection::V2::FReflectionCatalogRegistryV2::Get();
    catalogRegistry.RegisterSource(Slab::Core::Reflection::V2::FReflectionCatalogSourceBindingV2{
        .SourceId = LegacyCatalogSourceId,
        .DisplayName = "Legacy Interfaces",
        .Description = "Legacy CLI/interface reflection adapter.",
        .Refresh = [this] { ReflectionAdapter.RefreshFromLegacyInterfaces(); },
        .GetCatalog = [this]() -> const Slab::Core::Reflection::V2::FReflectionCatalogV2 & {
            return ReflectionAdapter.GetCatalog();
        },
        .Invoke = [this](const Slab::Str &interfaceId,
                         const Slab::Str &operationId,
                         const Slab::Core::Reflection::V2::FValueMapV2 &inputs,
                         const Slab::Core::Reflection::V2::FInvocationContextV2 &context) {
            return ReflectionAdapter.Invoke(interfaceId, operationId, inputs, context);
        },
        .EncodeCurrentParameterValue = [this](const Slab::Str &interfaceId, const Slab::Str &parameterId) {
            return ReflectionAdapter.EncodeCurrentParameterValue(interfaceId, parameterId);
        }
    });
    catalogRegistry.RegisterSource(Slab::Core::Reflection::V2::FReflectionCatalogSourceBindingV2{
        .SourceId = PlotCatalogSourceId,
        .DisplayName = "Plot Reflection",
        .Description = "Plot2D V2 and legacy plot reflection catalog.",
        .Refresh = [this] { PlotReflectionAdapter.RefreshFromLiveWindows(); },
        .GetCatalog = [this]() -> const Slab::Core::Reflection::V2::FReflectionCatalogV2 & {
            return PlotReflectionAdapter.GetCatalog();
        },
        .Invoke = [this](const Slab::Str &interfaceId,
                         const Slab::Str &operationId,
                         const Slab::Core::Reflection::V2::FValueMapV2 &inputs,
                         const Slab::Core::Reflection::V2::FInvocationContextV2 &context) {
            return PlotReflectionAdapter.Invoke(interfaceId, operationId, inputs, context);
        },
        .EncodeCurrentParameterValue = [this](const Slab::Str &interfaceId, const Slab::Str &parameterId) {
            return PlotReflectionAdapter.EncodeCurrentParameterValue(interfaceId, parameterId);
        }
    });

    AddResponder(ImGuiContext);
    LoadWorkspacePanelVisibility(ActiveWorkspace);

    // No default monitor window; monitors are attached per simulation on demand.
}

FLabV2WindowManager::~FLabV2WindowManager() {
    auto &catalogRegistry = Slab::Core::Reflection::V2::FReflectionCatalogRegistryV2::Get();
    if (!LegacyCatalogSourceId.empty()) catalogRegistry.UnregisterSource(LegacyCatalogSourceId);
    if (!PlotCatalogSourceId.empty()) catalogRegistry.UnregisterSource(PlotCatalogSourceId);
}

void FLabV2WindowManager::AddSlabWindow(const Slab::TPointer<Slab::Graphics::FSlabWindow> &window) {
    AddSlabWindowInWorkspace(window, EWorkspaceTab::Simulations, false);
}

void FLabV2WindowManager::AddSlabWindow(const Slab::TPointer<Slab::Graphics::FSlabWindow> &window, bool hidden) {
    AddSlabWindowInWorkspace(window, EWorkspaceTab::Simulations, hidden);
}

auto FLabV2WindowManager::AddSlabWindowInWorkspace(
    const Slab::TPointer<Slab::Graphics::FSlabWindow> &window,
    const EWorkspaceTab workspace,
    const bool hidden) -> void {
    (void) hidden;
    if (window == nullptr) return;
    AddResponder(window);
    SlabWindows.emplace_back(window);
    SlabWindowWorkspaceByUniqueName[window->GetUniqueName()] = workspace;
    if (SelectedViewUniqueName.empty()) {
        SelectedViewUniqueName = window->GetUniqueName();
    }
    RequestViewRetile();
    NotifySystemWindowReshape(WidthSysWin, HeightSysWin);
}

auto FLabV2WindowManager::QueueSlabWindow(const Slab::TPointer<Slab::Graphics::FSlabWindow> &window) -> void {
    QueueSlabWindowInWorkspace(window, EWorkspaceTab::Simulations);
}

auto FLabV2WindowManager::QueueSlabWindowInWorkspace(
    const Slab::TPointer<Slab::Graphics::FSlabWindow> &window,
    const EWorkspaceTab workspace) -> void {
    if (window == nullptr) return;
    PendingSlabWindows.push_back(FPendingSlabWindow{window, workspace});
}

auto FLabV2WindowManager::FlushPendingSlabWindows() -> void {
    if (PendingSlabWindows.empty()) return;

    auto pending = std::move(PendingSlabWindows);
    PendingSlabWindows.clear();
    for (const auto &entry : pending) AddSlabWindowInWorkspace(entry.Window, entry.Workspace, false);
}

auto FLabV2WindowManager::PruneClosedSlabWindows() -> bool {
    bool bRemovedAny = false;

    auto it = SlabWindows.begin();
    while (it != SlabWindows.end()) {
        const auto &window = *it;
        const bool bRemove = window == nullptr || window->WantsClose();
        if (!bRemove) {
            ++it;
            continue;
        }

        if (window != nullptr) {
            RemoveResponder(window);
            SlabWindowWorkspaceByUniqueName.erase(window->GetUniqueName());
            for (auto hostIt = PlotWindowHostsByWindowId.begin(); hostIt != PlotWindowHostsByWindowId.end();) {
                if (hostIt->second == window) {
                    hostIt = PlotWindowHostsByWindowId.erase(hostIt);
                } else {
                    ++hostIt;
                }
            }
        }
        it = SlabWindows.erase(it);
        bRemovedAny = true;
    }

    if (bRemovedAny) {
        if (const auto selected = FindWindowByUniqueName(SelectedViewUniqueName); selected == nullptr) {
            SelectedViewUniqueName = SlabWindows.empty() ? Slab::Str{} : SlabWindows.front()->GetUniqueName();
        }
    }

    return bRemovedAny;
}

auto FLabV2WindowManager::FindWindowByUniqueName(const Slab::Str &uniqueName) const
    -> Slab::TPointer<Slab::Graphics::FSlabWindow> {
    if (uniqueName.empty()) return nullptr;

    const auto it = std::find_if(SlabWindows.begin(), SlabWindows.end(), [&](const FSlabWindowPtr &window) {
        return window != nullptr && window->GetUniqueName() == uniqueName;
    });

    if (it == SlabWindows.end()) return nullptr;
    return *it;
}

auto FLabV2WindowManager::GetWorkspaceForWindow(const FSlabWindowPtr &window) const -> EWorkspaceTab {
    if (window == nullptr) return EWorkspaceTab::Simulations;

    const auto it = SlabWindowWorkspaceByUniqueName.find(window->GetUniqueName());
    if (it == SlabWindowWorkspaceByUniqueName.end()) return EWorkspaceTab::Simulations;
    return it->second;
}

auto FLabV2WindowManager::GetWorkspaceWindows(const EWorkspaceTab workspace) const -> FSlabWindowVec {
    FSlabWindowVec windows;
    windows.reserve(SlabWindows.size());

    for (const auto &window : SlabWindows) {
        if (window == nullptr) continue;
        if (GetWorkspaceForWindow(window) != workspace) continue;
        windows.push_back(window);
    }

    return windows;
}

auto FLabV2WindowManager::FocusWindow(const Slab::TPointer<Slab::Graphics::FSlabWindow> &window) -> void {
    if (window == nullptr) return;

    for (const auto &entry : SlabWindows) {
        if (entry == nullptr) continue;
        if (entry == window) entry->NotifyBecameActive();
        else entry->NotifyBecameInactive();
    }

    RemoveResponder(window);
    AddResponder(window);

    const auto it = std::find(SlabWindows.begin(), SlabWindows.end(), window);
    if (it != SlabWindows.end()) {
        auto moved = *it;
        SlabWindows.erase(it);
        SlabWindows.emplace_back(std::move(moved));
    }

    SelectedViewUniqueName = window->GetUniqueName();
}

auto FLabV2WindowManager::RequestViewRetile(const int stabilizationFrames) -> void {
    bPendingViewRetile = true;
    RetileStabilizationFramesRemaining = std::max(
        RetileStabilizationFramesRemaining,
        std::max(1, stabilizationFrames));
}

auto FLabV2WindowManager::FindTopWindowAtPoint(const int x, const int y) const
    -> Slab::TPointer<Slab::Graphics::FSlabWindow> {
    if (!ShouldRenderSlabWindowsInWorkspace()) return nullptr;

    const auto point = Slab::Graphics::FPoint2D{
        static_cast<Slab::DevFloat>(x),
        static_cast<Slab::DevFloat>(y)};

    for (auto it = SlabWindows.rbegin(); it != SlabWindows.rend(); ++it) {
        const auto &window = *it;
        if (window == nullptr) continue;
        if (GetWorkspaceForWindow(window) != ActiveWorkspace) continue;
        if (window->WantsClose()) continue;
        if (window->GetWidth() <= 0 || window->GetHeight() <= 0) continue;
        if (window->IsPointWithin(point)) return window;
    }

    return nullptr;
}

auto FLabV2WindowManager::FindKeyboardTargetWindow() const -> Slab::TPointer<Slab::Graphics::FSlabWindow> {
    if (!ShouldRenderSlabWindowsInWorkspace()) return nullptr;

    if (const auto selected = FindWindowByUniqueName(SelectedViewUniqueName); selected != nullptr) {
        if (GetWorkspaceForWindow(selected) == ActiveWorkspace) {
            return selected;
        }
    }

    for (auto it = SlabWindows.rbegin(); it != SlabWindows.rend(); ++it) {
        if (*it == nullptr) continue;
        if (GetWorkspaceForWindow(*it) != ActiveWorkspace) continue;
        if (!(*it)->WantsClose()) return *it;
    }

    return nullptr;
}

auto FLabV2WindowManager::SyncMousePositionFromImGui() -> void {
    if (ImGuiContext == nullptr) return;

    ImGuiContext->Bind();
    const auto &io = ImGui::GetIO();
    if (io.MousePos.x <= -FLT_MAX || io.MousePos.y <= -FLT_MAX) return;

    bHasLastMousePosition = true;
    LastMouseX = static_cast<int>(io.MousePos.x);
    LastMouseY = static_cast<int>(io.MousePos.y);
}

auto FLabV2WindowManager::DrawViewManagerPanel() -> void {
    const auto simulationWindows = GetWorkspaceWindows(EWorkspaceTab::Simulations);

    ImGui::TextUnformatted("Views");
    if (simulationWindows.empty()) {
        ImGui::TextDisabled("No active view windows.");
        return;
    }

    auto selected = FindWindowByUniqueName(SelectedViewUniqueName);
    if (selected != nullptr && GetWorkspaceForWindow(selected) != EWorkspaceTab::Simulations) {
        selected = nullptr;
    }
    if (selected == nullptr) {
        selected = simulationWindows.front();
        SelectedViewUniqueName = selected->GetUniqueName();
    }

    if (selected != nullptr) {
        ImGui::SameLine();
        if (ImGui::Button("Focus Selected")) {
            FocusWindow(selected);
        }

        ImGui::SameLine();
        if (ImGui::Button("Close Selected")) {
            selected->Close();
        }
    }
    ImGui::Separator();

    constexpr auto tableFlags =
        ImGuiTableFlags_Borders |
        ImGuiTableFlags_RowBg |
        ImGuiTableFlags_ScrollX |
        ImGuiTableFlags_SizingFixedFit;

    if (ImGui::BeginTable("LabViewTable", 5, tableFlags)) {
        ImGui::TableSetupColumn("Title", ImGuiTableColumnFlags_WidthStretch, 1.0f);
        ImGui::TableSetupColumn("x");
        ImGui::TableSetupColumn("y");
        ImGui::TableSetupColumn("w");
        ImGui::TableSetupColumn("h");
        ImGui::TableHeadersRow();

        for (const auto &window : simulationWindows) {
            if (window == nullptr) continue;

            const auto uniqueName = window->GetUniqueName();
            const bool bSelected = SelectedViewUniqueName == uniqueName;

            ImGui::TableNextRow();

            ImGui::TableSetColumnIndex(0);
            ImGui::PushID(uniqueName.c_str());
            if (ImGui::Selectable("##view", bSelected, ImGuiSelectableFlags_SpanAllColumns)) {
                SelectedViewUniqueName = uniqueName;
            }
            ImGui::SameLine();
            ImGui::TextUnformatted(window->GetTitle().c_str());
            ImGui::PopID();

            ImGui::TableSetColumnIndex(1);
            ImGui::Text("%d", window->Get_x());
            ImGui::TableSetColumnIndex(2);
            ImGui::Text("%d", window->Get_y());
            ImGui::TableSetColumnIndex(3);
            ImGui::Text("%d", window->GetWidth());
            ImGui::TableSetColumnIndex(4);
            ImGui::Text("%d", window->GetHeight());
        }

        ImGui::EndTable();
    }

    if (selected != nullptr) {
        ImGui::SeparatorText("Selected View");
        ImGui::TextUnformatted(selected->GetTitle().c_str());
        ImGui::Text("Id: %s", selected->GetUniqueName().c_str());
        ImGui::Text("Active: %s", selected->IsActive() ? "yes" : "no");
        ImGui::Text("Rect: x=%d y=%d w=%d h=%d",
            selected->Get_x(),
            selected->Get_y(),
            selected->GetWidth(),
            selected->GetHeight());
    }
}

auto FLabV2WindowManager::BuildReflectionInvocationContext() const -> Slab::Core::Reflection::V2::FInvocationContextV2 {
    using namespace Slab::Core::Reflection::V2;

    FInvocationContextV2 context;
    context.CurrentThread = EThreadAffinity::UI;
    context.bRuntimeRunning = false;

    try {
        const auto taskManager = Slab::Core::GetModule<Slab::Core::FTaskManager>("TaskManager");
        context.bRuntimeRunning = taskManager != nullptr && taskManager->HasRunningTasks();
    } catch (...) {
        context.bRuntimeRunning = false;
    }

    return context;
}

auto FLabV2WindowManager::BuildSchemeParameterDraftKey(const Slab::Str &interfaceId, const Slab::Str &parameterId) const
    -> Slab::Str {
    return interfaceId + "::" + parameterId;
}

auto FLabV2WindowManager::BuildPlotParameterDraftKey(const Slab::Str &interfaceId, const Slab::Str &parameterId) const
    -> Slab::Str {
    return interfaceId + "::" + parameterId;
}

auto FLabV2WindowManager::SyncPlotWorkspaceWindows() -> void {
    using namespace Slab::Core::Reflection::V2;

    auto &catalogRegistry = Slab::Core::Reflection::V2::FReflectionCatalogRegistryV2::Get();
    (void) catalogRegistry.RefreshSource(PlotCatalogSourceId);
    const auto *catalogPtr = catalogRegistry.GetCatalog(PlotCatalogSourceId);
    if (catalogPtr == nullptr) return;
    const auto &catalog = *catalogPtr;
    Slab::Core::Reflection::V2::SyncReflectionSemanticsOverlayV1(catalog);

    std::map<Slab::Str, Slab::Str> discoveredV2Windows;
    for (const auto &interfaceSchema : catalog.Interfaces) {
        if (!interfaceSchema.InterfaceId.starts_with(PlotWindowInterfaceIdPrefix)) continue;

        const auto windowId = interfaceSchema.InterfaceId.substr(std::char_traits<char>::length(PlotWindowInterfaceIdPrefix));
        if (windowId.empty()) continue;

        discoveredV2Windows[windowId] =
            interfaceSchema.DisplayName.empty() ? windowId : interfaceSchema.DisplayName;
    }

    for (auto it = PlotWindowHostsByWindowId.begin(); it != PlotWindowHostsByWindowId.end();) {
        const auto &windowId = it->first;
        const auto &host = it->second;
        const bool bMissing = !discoveredV2Windows.contains(windowId);
        const bool bHostClosed = host == nullptr || host->WantsClose();
        if (!bMissing && !bHostClosed) {
            ++it;
            continue;
        }

        if (host != nullptr) {
            host->Close();
        }
        it = PlotWindowHostsByWindowId.erase(it);
    }

    for (const auto &[windowId, displayName] : discoveredV2Windows) {
        if (PlotWindowHostsByWindowId.contains(windowId)) continue;

        auto host = Slab::New<FPlot2DWindowHostV2>(windowId, displayName);
        PlotWindowHostsByWindowId[windowId] = host;
        QueueSlabWindowInWorkspace(host, EWorkspaceTab::Plots);
    }
}

auto FLabV2WindowManager::EnsureSchemeSelectionIsValid(const Slab::Core::Reflection::V2::FReflectionCatalogV2 &catalog) -> void {
    using namespace Slab::Core::Reflection::V2;

    if (catalog.Interfaces.empty()) {
        SelectedSchemeInterfaceId.clear();
        SelectedSchemeParameterId.clear();
        return;
    }

    const auto *interfaceSchema = FindInterfaceById(catalog, SelectedSchemeInterfaceId);
    if (interfaceSchema == nullptr) {
        SelectedSchemeInterfaceId = catalog.Interfaces.front().InterfaceId;
        interfaceSchema = &catalog.Interfaces.front();
        SelectedSchemeParameterId.clear();
        SelectedSchemeOperationId.clear();
    }

    if (interfaceSchema->Parameters.empty()) {
        SelectedSchemeParameterId.clear();
        SelectedSchemeOperationId.clear();
        return;
    }

    const auto *parameterSchema = FindParameterById(*interfaceSchema, SelectedSchemeParameterId);
    if (parameterSchema == nullptr) {
        SelectedSchemeParameterId = interfaceSchema->Parameters.front().ParameterId;
        parameterSchema = &interfaceSchema->Parameters.front();
    }

    const auto draftKey = BuildSchemeParameterDraftKey(interfaceSchema->InterfaceId, parameterSchema->ParameterId);
    if (!SchemeParameterDraftByKey.contains(draftKey)) {
        if (const auto current = Slab::Core::Reflection::V2::FReflectionCatalogRegistryV2::Get().EncodeCurrentParameterValue(
                interfaceSchema->InterfaceId,
                parameterSchema->ParameterId); current.has_value()) {
            SchemeParameterDraftByKey[draftKey] = current->Encoded;
        } else {
            SchemeParameterDraftByKey[draftKey] = "";
        }
    }
}

auto FLabV2WindowManager::EnsurePlotSelectionIsValid() -> void {
    using namespace Slab::Core::Reflection::V2;

    const auto &catalog = PlotReflectionAdapter.GetCatalog();
    if (catalog.Interfaces.empty()) {
        SelectedPlotInterfaceId.clear();
        SelectedPlotParameterId.clear();
        return;
    }

    const auto *interfaceSchema = FindInterfaceById(catalog, SelectedPlotInterfaceId);
    if (interfaceSchema == nullptr) {
        SelectedPlotInterfaceId = catalog.Interfaces.front().InterfaceId;
        interfaceSchema = &catalog.Interfaces.front();
        SelectedPlotParameterId.clear();
    }

    if (interfaceSchema->Parameters.empty()) {
        SelectedPlotParameterId.clear();
        return;
    }

    const auto *parameterSchema = FindParameterById(*interfaceSchema, SelectedPlotParameterId);
    if (parameterSchema == nullptr) {
        SelectedPlotParameterId = interfaceSchema->Parameters.front().ParameterId;
        parameterSchema = &interfaceSchema->Parameters.front();
    }

    const auto draftKey = BuildPlotParameterDraftKey(interfaceSchema->InterfaceId, parameterSchema->ParameterId);
    if (!PlotParameterDraftByKey.contains(draftKey)) {
        if (const auto current = PlotReflectionAdapter.EncodeCurrentParameterValue(
                interfaceSchema->InterfaceId,
                parameterSchema->ParameterId); current.has_value()) {
            PlotParameterDraftByKey[draftKey] = current->Encoded;
        } else {
            PlotParameterDraftByKey[draftKey] = "";
        }
    }
}

auto FLabV2WindowManager::ApplySchemeOperationResult(const Slab::Str &interfaceId,
                                                     const Slab::Str &operationId,
                                                     const Slab::Core::Reflection::V2::FOperationResultV2 &result) -> void {
    SchemesLastOperationSummary = BuildOperationSummary(result);
    SchemesLastOperationOutput = result.OutputMap;

    FSchemeOperationTraceEntry trace;
    trace.SequenceId = ++SchemesOperationTraceSequence;
    trace.InterfaceId = interfaceId;
    trace.OperationId = operationId;
    trace.Summary = SchemesLastOperationSummary;
    trace.bOk = result.IsOk();
    if (result.LatencyMs.has_value()) {
        std::ostringstream os;
        os.setf(std::ios::fixed, std::ios::floatfield);
        os.precision(2);
        os << result.LatencyMs.value() << " ms";
        trace.LatencyLabel = os.str();
    }

    SchemesOperationTrace.push_front(std::move(trace));
    constexpr std::size_t MaxTraceEntries = 24;
    while (SchemesOperationTrace.size() > MaxTraceEntries) {
        SchemesOperationTrace.pop_back();
    }
}

auto FLabV2WindowManager::ApplyPlotOperationResult(const Slab::Str &interfaceId,
                                                   const Slab::Str &operationId,
                                                   const Slab::Core::Reflection::V2::FOperationResultV2 &result) -> void {
    (void) interfaceId;
    (void) operationId;
    PlotsLastOperationSummary = BuildOperationSummary(result);
    PlotsLastOperationOutput = result.OutputMap;
}

auto FLabV2WindowManager::InvokeSelectedSchemeOperation(
    const Slab::Core::Reflection::V2::FInterfaceSchemaV2 &interfaceSchema,
    const Slab::Str &operationId,
    const Slab::Core::Reflection::V2::FInvocationContextV2 &context) -> void {
    using namespace Slab::Core::Reflection::V2;

    FValueMapV2 inputs;
    const auto *operationSchema = FindOperationById(interfaceSchema, operationId);
    const auto hasRequiredInput = [operationSchema](const Slab::Str &fieldId) -> bool {
        if (operationSchema == nullptr) return false;
        return std::any_of(operationSchema->Inputs.begin(), operationSchema->Inputs.end(), [&](const auto &field) {
            return field.bRequired && field.FieldId == fieldId;
        });
    };

    const bool bNeedsParameterSelection = hasRequiredInput("parameter_id");
    const bool bNeedsValueInput = hasRequiredInput("value");

    if (bNeedsParameterSelection) {
        if (SelectedSchemeParameterId.empty()) {
            const auto missingParameterResult = FOperationResultV2::Error(
                "reflection.parameter.required",
                "Select a parameter before invoking this operation.");
            ApplySchemeOperationResult(interfaceSchema.InterfaceId, operationId, missingParameterResult);
            return;
        }

        inputs["parameter_id"] = MakeStringValue(SelectedSchemeParameterId);
    }

    if (bNeedsValueInput) {
        if (SelectedSchemeParameterId.empty()) {
            const auto missingParameterResult = FOperationResultV2::Error(
                "reflection.parameter.required",
                "Select a parameter before setting an encoded value.");
            ApplySchemeOperationResult(interfaceSchema.InterfaceId, operationId, missingParameterResult);
            return;
        }

        const auto *parameterSchema = FindParameterById(interfaceSchema, SelectedSchemeParameterId);
        if (parameterSchema == nullptr) {
            const auto missingParameterResult = FOperationResultV2::Error(
                "reflection.parameter.not_found",
                "Selected parameter is no longer available.");
            ApplySchemeOperationResult(interfaceSchema.InterfaceId, operationId, missingParameterResult);
            return;
        }

        const auto draftKey = BuildSchemeParameterDraftKey(interfaceSchema.InterfaceId, parameterSchema->ParameterId);
        const auto draftIt = SchemeParameterDraftByKey.find(draftKey);
        const auto draft = draftIt != SchemeParameterDraftByKey.end() ? draftIt->second : Slab::Str{};
        inputs["value"] = FReflectionValueV2(parameterSchema->TypeId, draft);
    }

    const auto result = Slab::Core::Reflection::V2::FReflectionCatalogRegistryV2::Get().Invoke(
        interfaceSchema.InterfaceId,
        operationId,
        inputs,
        context);
    ApplySchemeOperationResult(interfaceSchema.InterfaceId, operationId, result);

    if (!result.IsOk()) return;

    if (const auto it = result.OutputMap.find("value"); it != result.OutputMap.end() &&
        !SelectedSchemeParameterId.empty()) {
        const auto draftKey = BuildSchemeParameterDraftKey(interfaceSchema.InterfaceId, SelectedSchemeParameterId);
        SchemeParameterDraftByKey[draftKey] = it->second.Encoded;
    }
}

auto FLabV2WindowManager::InvokeSelectedPlotOperation(
    const Slab::Core::Reflection::V2::FInterfaceSchemaV2 &interfaceSchema,
    const Slab::Str &operationId,
    const Slab::Core::Reflection::V2::FInvocationContextV2 &context) -> void {
    using namespace Slab::Core::Reflection::V2;

    FValueMapV2 inputs;
    const auto *operationSchema = FindOperationById(interfaceSchema, operationId);
    const auto hasRequiredInput = [operationSchema](const Slab::Str &fieldId) -> bool {
        if (operationSchema == nullptr) return false;
        return std::any_of(operationSchema->Inputs.begin(), operationSchema->Inputs.end(), [&](const auto &field) {
            return field.bRequired && field.FieldId == fieldId;
        });
    };

    const bool bNeedsParameterSelection = hasRequiredInput("parameter_id");
    const bool bNeedsValueInput = hasRequiredInput("value");

    if (bNeedsParameterSelection) {
        if (SelectedPlotParameterId.empty()) {
            const auto missingParameterResult = FOperationResultV2::Error(
                "reflection.parameter.required",
                "Select a parameter before invoking this operation.");
            ApplyPlotOperationResult(interfaceSchema.InterfaceId, operationId, missingParameterResult);
            return;
        }

        inputs["parameter_id"] = MakeStringValue(SelectedPlotParameterId);
    }

    if (bNeedsValueInput) {
        if (SelectedPlotParameterId.empty()) {
            const auto missingParameterResult = FOperationResultV2::Error(
                "reflection.parameter.required",
                "Select a parameter before setting an encoded value.");
            ApplyPlotOperationResult(interfaceSchema.InterfaceId, operationId, missingParameterResult);
            return;
        }

        const auto *parameterSchema = FindParameterById(interfaceSchema, SelectedPlotParameterId);
        if (parameterSchema == nullptr) {
            const auto missingParameterResult = FOperationResultV2::Error(
                "reflection.parameter.not_found",
                "Selected parameter is no longer available.");
            ApplyPlotOperationResult(interfaceSchema.InterfaceId, operationId, missingParameterResult);
            return;
        }

        const auto draftKey = BuildPlotParameterDraftKey(interfaceSchema.InterfaceId, parameterSchema->ParameterId);
        const auto draftIt = PlotParameterDraftByKey.find(draftKey);
        const auto draft = draftIt != PlotParameterDraftByKey.end() ? draftIt->second : Slab::Str{};
        inputs["value"] = FReflectionValueV2(parameterSchema->TypeId, draft);
    }

    const auto result = PlotReflectionAdapter.Invoke(interfaceSchema.InterfaceId, operationId, inputs, context);
    ApplyPlotOperationResult(interfaceSchema.InterfaceId, operationId, result);

    if (!result.IsOk()) return;

    if (const auto it = result.OutputMap.find("value"); it != result.OutputMap.end() &&
        !SelectedPlotParameterId.empty()) {
        const auto draftKey = BuildPlotParameterDraftKey(interfaceSchema.InterfaceId, SelectedPlotParameterId);
        PlotParameterDraftByKey[draftKey] = it->second.Encoded;
    }
}

auto FLabV2WindowManager::DrawSchemesInspectorPanel() -> void {
    using namespace Slab::Core::Reflection::V2;

    auto &catalogRegistry = Slab::Core::Reflection::V2::FReflectionCatalogRegistryV2::Get();
    catalogRegistry.RefreshAll();
    const auto catalog = catalogRegistry.BuildMergedCatalog();
    Slab::Core::Reflection::V2::SyncReflectionSemanticsOverlayV1(catalog);
    EnsureSchemeSelectionIsValid(catalog);
    const auto context = BuildReflectionInvocationContext();
    const auto catalogSources = catalogRegistry.ListSources();

    ImGui::Text("Catalog version: %s", catalog.CatalogVersion.c_str());
    ImGui::SameLine();
    ImGui::TextDisabled("| interfaces: %zu", catalog.Interfaces.size());
    ImGui::SameLine();
    ImGui::TextDisabled("| catalogs: %zu", catalogSources.size());
    ImGui::SameLine();
    ImGui::TextDisabled("| runtime: %s", context.bRuntimeRunning ? "running" : "stopped");

    if (!catalogSources.empty() && ImGui::TreeNodeEx("Catalog Sources", ImGuiTreeNodeFlags_DefaultOpen)) {
        for (const auto &source : catalogSources) {
            ImGui::BulletText(
                "%s (%s) - %zu interfaces",
                source.DisplayName.c_str(),
                source.SourceId.c_str(),
                source.InterfaceCount);
            if (!source.Description.empty()) {
                ImGui::TextDisabled("%s", source.Description.c_str());
            }
        }
        ImGui::TreePop();
    }

    if (!SchemesLastOperationSummary.empty()) {
        if (!SchemesLastOperationSummary.starts_with("[Ok]")) {
            ImGui::TextColored(ImVec4(0.9f, 0.3f, 0.3f, 1.0f), "%s", SchemesLastOperationSummary.c_str());
        } else {
            ImGui::TextColored(ImVec4(0.3f, 0.8f, 0.4f, 1.0f), "%s", SchemesLastOperationSummary.c_str());
        }
    }

    if (catalog.Interfaces.empty()) {
        ImGui::Separator();
        ImGui::TextDisabled("No interfaces available in reflection catalog.");
        return;
    }

    const auto drawStringEditor = [](const char *label, Slab::Str &value, const std::size_t capacity) {
        auto buffer = std::vector<char>(capacity, '\0');
        if (!value.empty()) {
            std::strncpy(buffer.data(), value.c_str(), capacity - 1);
            buffer[capacity - 1] = '\0';
        }
        if (ImGui::InputText(label, buffer.data(), capacity)) {
            value = buffer.data();
        }
    };

    if (ImGui::BeginTable("SchemesCatalogLayout", 2, ImGuiTableFlags_Resizable | ImGuiTableFlags_BordersInnerV)) {
        ImGui::TableSetupColumn("Interfaces", ImGuiTableColumnFlags_WidthFixed, 280.0f);
        ImGui::TableSetupColumn("Inspector", ImGuiTableColumnFlags_WidthStretch, 1.0f);
        ImGui::TableNextRow();

        ImGui::TableSetColumnIndex(0);
        if (ImGui::BeginChild("SchemesInterfaceList", ImVec2(0.0f, 0.0f), true)) {
            const auto drawInterfaceRow = [&](const FInterfaceSchemaV2 &interfaceSchema) {
                const bool bSelected = interfaceSchema.InterfaceId == SelectedSchemeInterfaceId;
                ImGui::PushID(interfaceSchema.InterfaceId.c_str());
                if (ImGui::Selectable(interfaceSchema.DisplayName.c_str(), bSelected)) {
                    SelectedSchemeInterfaceId = interfaceSchema.InterfaceId;
                    SelectedSchemeParameterId.clear();
                    EnsureSchemeSelectionIsValid(catalog);
                }

                ImGui::SameLine();
                ImGui::TextDisabled("%s", interfaceSchema.InterfaceId.c_str());
                if (!interfaceSchema.Description.empty()) {
                    ImGui::TextDisabled("%s", TruncateLabel(interfaceSchema.Description).c_str());
                }
                ImGui::PopID();
            };

            std::map<Slab::Str, Slab::Vector<const FInterfaceSchemaV2 *>> interfacesBySourceId;
            std::map<Slab::Str, Slab::Str> sourceDisplayNameById;
            Slab::Vector<Slab::Str> sourceOrder;
            sourceOrder.reserve(catalogSources.size());
            for (const auto &source : catalogSources) {
                sourceOrder.push_back(source.SourceId);
                sourceDisplayNameById[source.SourceId] = source.DisplayName;
            }

            for (const auto &interfaceSchema : catalog.Interfaces) {
                Slab::Str sourceId = "catalog.unknown";
                Slab::Str sourceDisplayName = "Unknown Catalog";
                if (const auto source = catalogRegistry.FindSourceForInterface(interfaceSchema.InterfaceId); source.has_value()) {
                    sourceId = source->SourceId;
                    sourceDisplayName = source->DisplayName;
                }

                if (!sourceDisplayNameById.contains(sourceId)) {
                    sourceDisplayNameById[sourceId] = sourceDisplayName;
                }
                interfacesBySourceId[sourceId].push_back(&interfaceSchema);
            }

            const auto drawCatalogGroup = [&](const Slab::Str &sourceId, const Slab::Str &sourceDisplayName) {
                const auto sourceIt = interfacesBySourceId.find(sourceId);
                if (sourceIt == interfacesBySourceId.end()) return;
                if (sourceIt->second.empty()) return;

                const auto headerLabel = sourceDisplayName + " (" + Slab::ToStr(sourceIt->second.size()) + ")###" + sourceId;
                if (!ImGui::CollapsingHeader(headerLabel.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) return;

                for (const auto *interfaceSchema : sourceIt->second) {
                    if (interfaceSchema == nullptr) continue;
                    drawInterfaceRow(*interfaceSchema);
                }
            };

            for (const auto &sourceId : sourceOrder) {
                const auto displayIt = sourceDisplayNameById.find(sourceId);
                const auto displayName = displayIt != sourceDisplayNameById.end() ? displayIt->second : sourceId;
                drawCatalogGroup(sourceId, displayName);
            }

            for (const auto &[sourceId, interfaces] : interfacesBySourceId) {
                (void) interfaces;
                if (std::find(sourceOrder.begin(), sourceOrder.end(), sourceId) != sourceOrder.end()) continue;
                const auto displayIt = sourceDisplayNameById.find(sourceId);
                const auto displayName = displayIt != sourceDisplayNameById.end() ? displayIt->second : sourceId;
                drawCatalogGroup(sourceId, displayName);
            }
        }
        ImGui::EndChild();

        ImGui::TableSetColumnIndex(1);
        if (ImGui::BeginChild("SchemesInterfaceInspector", ImVec2(0.0f, 0.0f), false)) {
            const auto *interfaceSchema = FindInterfaceById(catalog, SelectedSchemeInterfaceId);
            if (interfaceSchema == nullptr) {
                ImGui::TextDisabled("Select an interface.");
            } else {
                ImGui::TextUnformatted(interfaceSchema->DisplayName.c_str());
                ImGui::TextDisabled("%s", interfaceSchema->InterfaceId.c_str());
                if (const auto source = catalogRegistry.FindSourceForInterface(interfaceSchema->InterfaceId);
                    source.has_value()) {
                    ImGui::TextDisabled("Catalog: %s (%s)", source->DisplayName.c_str(), source->SourceId.c_str());
                }
                if (!interfaceSchema->Description.empty()) {
                    ImGui::TextWrapped("%s", interfaceSchema->Description.c_str());
                }

                ImGui::SeparatorText("Operations");
                for (const auto &operation : interfaceSchema->Operations) {
                    const auto isBuiltinRequiredInput = [](const Slab::Str &fieldId) -> bool {
                        return fieldId == "parameter_id" || fieldId == "value";
                    };
                    const bool bNeedsParameterSelection = std::any_of(
                        operation.Inputs.begin(),
                        operation.Inputs.end(),
                        [](const auto &input) {
                            return input.bRequired && input.FieldId == "parameter_id";
                        });
                    const bool bNeedsValueInput = std::any_of(
                        operation.Inputs.begin(),
                        operation.Inputs.end(),
                        [](const auto &input) {
                            return input.bRequired && input.FieldId == "value";
                        });
                    const bool bHasNonBuiltinRequiredInput = std::any_of(
                        operation.Inputs.begin(),
                        operation.Inputs.end(),
                        [&](const auto &input) {
                            return input.bRequired && !isBuiltinRequiredInput(input.FieldId);
                        });

                    bool bCanInvoke = !bHasNonBuiltinRequiredInput;
                    if (bNeedsParameterSelection || bNeedsValueInput) {
                        bCanInvoke = bCanInvoke && !SelectedSchemeParameterId.empty();
                    }

                    const bool bShowsBuiltinInputFields = !bHasNonBuiltinRequiredInput &&
                        std::any_of(operation.Inputs.begin(), operation.Inputs.end(), [&](const auto &input) {
                            return input.bRequired && isBuiltinRequiredInput(input.FieldId);
                        });

                    if (bShowsBuiltinInputFields && !SelectedSchemeParameterId.empty()) {
                        const auto draftKey = BuildSchemeParameterDraftKey(interfaceSchema->InterfaceId, SelectedSchemeParameterId);
                        if (!SchemeParameterDraftByKey.contains(draftKey)) {
                            if (const auto liveValue = catalogRegistry.EncodeCurrentParameterValue(
                                    interfaceSchema->InterfaceId,
                                    SelectedSchemeParameterId); liveValue.has_value()) {
                                SchemeParameterDraftByKey[draftKey] = liveValue->Encoded;
                            } else {
                                SchemeParameterDraftByKey[draftKey] = "";
                            }
                        }
                    }

                    ImGui::PushID(operation.OperationId.c_str());
                    ImGui::BeginDisabled(!bCanInvoke);
                    if (ImGui::Button(operation.DisplayName.c_str())) {
                        InvokeSelectedSchemeOperation(*interfaceSchema, operation.OperationId, context);
                    }
                    const bool bOperationButtonHovered = ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort);
                    ImGui::EndDisabled();
                    ImGui::SameLine();
                    ImGui::TextDisabled(
                        "%s | %s | %s",
                        ToString(operation.Kind),
                        ToString(operation.RunStatePolicy),
                        ToString(operation.ThreadAffinity));
                    const bool bOperationMetaHovered = ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort);
                    if (!operation.Description.empty() && (bOperationButtonHovered || bOperationMetaHovered)) {
                        ImGui::BeginTooltip();
                        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 42.0f);
                        ImGui::TextUnformatted(operation.Description.c_str());
                        ImGui::PopTextWrapPos();
                        ImGui::EndTooltip();
                    }

                    if (bHasNonBuiltinRequiredInput) {
                        ImGui::SameLine();
                        ImGui::TextDisabled("requires non-builtin input");
                    }

                    if (bShowsBuiltinInputFields) {
                        ImGui::Indent();
                        if (bNeedsParameterSelection) {
                            const char *preview = SelectedSchemeParameterId.empty()
                                ? "<select parameter_id>"
                                : SelectedSchemeParameterId.c_str();
                            ImGui::SetNextItemWidth(260.0f);
                            if (ImGui::BeginCombo("parameter_id", preview)) {
                                for (const auto &parameter : interfaceSchema->Parameters) {
                                    const bool bSelectedParameter = parameter.ParameterId == SelectedSchemeParameterId;
                                    if (ImGui::Selectable(parameter.ParameterId.c_str(), bSelectedParameter)) {
                                        SelectedSchemeParameterId = parameter.ParameterId;
                                        const auto draftKey = BuildSchemeParameterDraftKey(
                                            interfaceSchema->InterfaceId,
                                            SelectedSchemeParameterId);
                                        if (!SchemeParameterDraftByKey.contains(draftKey)) {
                                            if (const auto liveValue = catalogRegistry.EncodeCurrentParameterValue(
                                                    interfaceSchema->InterfaceId,
                                                    SelectedSchemeParameterId); liveValue.has_value()) {
                                                SchemeParameterDraftByKey[draftKey] = liveValue->Encoded;
                                            } else {
                                                SchemeParameterDraftByKey[draftKey] = "";
                                            }
                                        }
                                    }
                                    if (bSelectedParameter) ImGui::SetItemDefaultFocus();
                                }
                                ImGui::EndCombo();
                            }
                        }

                        if (bNeedsValueInput) {
                            if (SelectedSchemeParameterId.empty()) {
                                ImGui::TextDisabled("value: select parameter_id first");
                            } else {
                                const auto draftKey = BuildSchemeParameterDraftKey(
                                    interfaceSchema->InterfaceId,
                                    SelectedSchemeParameterId);
                                auto &draft = SchemeParameterDraftByKey[draftKey];
                                auto valueBuffer = std::vector<char>(512, '\0');
                                if (!draft.empty()) {
                                    std::strncpy(valueBuffer.data(), draft.c_str(), valueBuffer.size() - 1);
                                    valueBuffer[valueBuffer.size() - 1] = '\0';
                                }
                                ImGui::SetNextItemWidth(320.0f);
                                if (ImGui::InputText("value", valueBuffer.data(), valueBuffer.size())) {
                                    draft = valueBuffer.data();
                                }
                                ImGui::SameLine();
                                if (ImGui::SmallButton("Load Live")) {
                                    if (const auto liveValue = catalogRegistry.EncodeCurrentParameterValue(
                                            interfaceSchema->InterfaceId,
                                            SelectedSchemeParameterId); liveValue.has_value()) {
                                        draft = liveValue->Encoded;
                                    }
                                }
                            }
                        }
                        ImGui::Unindent();
                    }
                    ImGui::PopID();
                }

                ImGui::SeparatorText("Parameters");
                if (interfaceSchema->Parameters.empty()) {
                    ImGui::TextDisabled("No parameters in this interface.");
                } else {
                    constexpr auto tableFlags =
                        ImGuiTableFlags_Borders |
                        ImGuiTableFlags_RowBg |
                        ImGuiTableFlags_SizingFixedFit |
                        ImGuiTableFlags_ScrollY;

                    if (ImGui::BeginTable("SchemesParameterTable", 4, tableFlags, ImVec2(0.0f, 220.0f))) {
                        ImGui::TableSetupColumn("Parameter", ImGuiTableColumnFlags_WidthStretch, 1.2f);
                        ImGui::TableSetupColumn("Type");
                        ImGui::TableSetupColumn("Mutability");
                        ImGui::TableSetupColumn("Live Value", ImGuiTableColumnFlags_WidthStretch, 1.6f);
                        ImGui::TableHeadersRow();

                        for (const auto &parameterSchema : interfaceSchema->Parameters) {
                            ImGui::TableNextRow();
                            ImGui::TableSetColumnIndex(0);
                            const bool bSelected = parameterSchema.ParameterId == SelectedSchemeParameterId;
                            ImGui::PushID(parameterSchema.ParameterId.c_str());
                            if (ImGui::Selectable(parameterSchema.DisplayName.c_str(), bSelected)) {
                                SelectedSchemeParameterId = parameterSchema.ParameterId;
                                const auto draftKey = BuildSchemeParameterDraftKey(
                                    interfaceSchema->InterfaceId,
                                    SelectedSchemeParameterId);
                                if (!SchemeParameterDraftByKey.contains(draftKey)) {
                                    if (const auto liveValue = catalogRegistry.EncodeCurrentParameterValue(
                                            interfaceSchema->InterfaceId,
                                            SelectedSchemeParameterId); liveValue.has_value()) {
                                        SchemeParameterDraftByKey[draftKey] = liveValue->Encoded;
                                    } else {
                                        SchemeParameterDraftByKey[draftKey] = "";
                                    }
                                }
                            }
                            ImGui::TableSetColumnIndex(1);
                            ImGui::TextUnformatted(parameterSchema.TypeId.c_str());
                            ImGui::TableSetColumnIndex(2);
                            ImGui::TextUnformatted(ToString(parameterSchema.Mutability));
                            ImGui::TableSetColumnIndex(3);
                            if (const auto liveValue = catalogRegistry.EncodeCurrentParameterValue(
                                    interfaceSchema->InterfaceId,
                                    parameterSchema.ParameterId); liveValue.has_value()) {
                                ImGui::TextWrapped("%s", liveValue->Encoded.c_str());
                            } else {
                                ImGui::TextDisabled("<unavailable>");
                            }
                            ImGui::PopID();
                        }

                        ImGui::EndTable();
                    }

                    const auto *selectedParameterSchema = FindParameterById(*interfaceSchema, SelectedSchemeParameterId);
                    if (selectedParameterSchema != nullptr) {
                        ImGui::SeparatorText("Parameter Editor");
                        ImGui::TextUnformatted(selectedParameterSchema->DisplayName.c_str());
                        ImGui::TextDisabled(
                            "id=%s | type=%s | mutability=%s",
                            selectedParameterSchema->ParameterId.c_str(),
                            selectedParameterSchema->TypeId.c_str(),
                            ToString(selectedParameterSchema->Mutability));
                        if (!selectedParameterSchema->Description.empty()) {
                            ImGui::TextWrapped("%s", selectedParameterSchema->Description.c_str());
                        }

                        const auto draftKey = BuildSchemeParameterDraftKey(
                            interfaceSchema->InterfaceId,
                            selectedParameterSchema->ParameterId);
                        auto &draft = SchemeParameterDraftByKey[draftKey];
                        const auto resolveOperationId = [interfaceSchema](const Slab::Str &primary,
                                                                          const Slab::Str &alternate) -> Slab::Str {
                            if (FindOperationById(*interfaceSchema, primary) != nullptr) return primary;
                            if (FindOperationById(*interfaceSchema, alternate) != nullptr) return alternate;
                            return {};
                        };
                        const auto setOperationId = resolveOperationId(
                            COperationIdCommandSetParameter,
                            Slab::Graphics::Plot2D::V2::CPlotOperationIdCommandSetParameterV2);
                        const auto applyOperationId = resolveOperationId(
                            COperationIdCommandApplyPending,
                            Slab::Graphics::Plot2D::V2::CPlotOperationIdCommandApplyPendingV2);

                        if (selectedParameterSchema->TypeId == CTypeIdFunctionRtoR) {
                            FFunctionDescriptorV2 descriptor;
                            Slab::Str decodeError;
                            const bool bDecoded = DecodeFunctionDescriptorCLI(draft, descriptor, &decodeError);
                            if (!bDecoded) {
                                ImGui::TextColored(
                                    ImVec4(0.9f, 0.3f, 0.3f, 1.0f),
                                    "Descriptor decode issue: %s",
                                    decodeError.c_str());
                            }

                            drawStringEditor("Family", descriptor.Family, 192);
                            drawStringEditor("Expression", descriptor.Expression, 512);
                            ImGui::DragScalar(
                                "Domain Min",
                                ImGuiDataType_Double,
                                &descriptor.DomainMin,
                                0.01,
                                nullptr,
                                nullptr,
                                "%.6f");
                            ImGui::DragScalar(
                                "Domain Max",
                                ImGuiDataType_Double,
                                &descriptor.DomainMax,
                                0.01,
                                nullptr,
                                nullptr,
                                "%.6f");
                            draft = EncodeFunctionDescriptorCLI(descriptor);
                            ImGui::TextDisabled("Encoded: %s", TruncateLabel(draft, 110).c_str());
                        } else {
                            drawStringEditor("Encoded Value", draft, 512);
                        }

                        if (ImGui::Button("Load Live Value")) {
                            if (const auto liveValue = catalogRegistry.EncodeCurrentParameterValue(
                                    interfaceSchema->InterfaceId,
                                    selectedParameterSchema->ParameterId); liveValue.has_value()) {
                                draft = liveValue->Encoded;
                            }
                        }
                        ImGui::SameLine();
                        ImGui::BeginDisabled(setOperationId.empty());
                        if (ImGui::Button("Set Parameter")) {
                            SelectedSchemeParameterId = selectedParameterSchema->ParameterId;
                            InvokeSelectedSchemeOperation(*interfaceSchema, setOperationId, context);
                        }
                        ImGui::EndDisabled();

                        if (selectedParameterSchema->Mutability == EParameterMutability::RestartRequired &&
                            !applyOperationId.empty()) {
                            ImGui::SameLine();
                            if (ImGui::Button("Apply Pending")) {
                                InvokeSelectedSchemeOperation(*interfaceSchema, applyOperationId, context);
                            }
                        }
                    }
                }

                ImGui::SeparatorText("Last Outputs");
                if (SchemesLastOperationOutput.empty()) {
                    ImGui::TextDisabled("No operation output yet.");
                } else {
                    for (const auto &[key, value] : SchemesLastOperationOutput) {
                        ImGui::BulletText("%s (%s): %s", key.c_str(), value.TypeId.c_str(), value.Encoded.c_str());
                    }
                }
            }
        }
        ImGui::EndChild();

        ImGui::EndTable();
    }
}

auto FLabV2WindowManager::DrawPlotsInspectorPanel() -> void {
    using namespace Slab::Core::Reflection::V2;
    using namespace Slab::Graphics::Plot2D::V2;

    PlotReflectionAdapter.RefreshFromLiveWindows();
    EnsurePlotSelectionIsValid();

    const auto &catalog = PlotReflectionAdapter.GetCatalog();
    const auto context = BuildReflectionInvocationContext();

    ImGui::Text("Catalog version: %s", catalog.CatalogVersion.c_str());
    ImGui::SameLine();
    ImGui::TextDisabled("| interfaces: %zu", catalog.Interfaces.size());
    ImGui::SameLine();
    ImGui::TextDisabled("| runtime: %s", context.bRuntimeRunning ? "running" : "stopped");

    if (!PlotsLastOperationSummary.empty()) {
        if (!PlotsLastOperationSummary.starts_with("[Ok]")) {
            ImGui::TextColored(ImVec4(0.9f, 0.3f, 0.3f, 1.0f), "%s", PlotsLastOperationSummary.c_str());
        } else {
            ImGui::TextColored(ImVec4(0.3f, 0.8f, 0.4f, 1.0f), "%s", PlotsLastOperationSummary.c_str());
        }
    }

    if (catalog.Interfaces.empty()) {
        ImGui::Separator();
        ImGui::TextDisabled("No Plot2D windows/artists are currently registered.");
        ImGui::TextDisabled("Create Plot2D V2 windows or legacy Plot2D windows to populate this catalog.");
        return;
    }

    const auto drawStringEditor = [](const char *label, Slab::Str &value, const std::size_t capacity) {
        auto buffer = std::vector<char>(capacity, '\0');
        if (!value.empty()) {
            std::strncpy(buffer.data(), value.c_str(), capacity - 1);
            buffer[capacity - 1] = '\0';
        }
        if (ImGui::InputText(label, buffer.data(), capacity)) {
            value = buffer.data();
        }
    };

    if (ImGui::BeginTable("PlotsCatalogLayout", 2, ImGuiTableFlags_Resizable | ImGuiTableFlags_BordersInnerV)) {
        ImGui::TableSetupColumn("Interfaces", ImGuiTableColumnFlags_WidthFixed, 300.0f);
        ImGui::TableSetupColumn("Inspector", ImGuiTableColumnFlags_WidthStretch, 1.0f);
        ImGui::TableNextRow();

        ImGui::TableSetColumnIndex(0);
        if (ImGui::BeginChild("PlotsInterfaceList", ImVec2(0.0f, 0.0f), true)) {
            for (const auto &interfaceSchema : catalog.Interfaces) {
                const bool bSelected = interfaceSchema.InterfaceId == SelectedPlotInterfaceId;
                ImGui::PushID(interfaceSchema.InterfaceId.c_str());
                if (ImGui::Selectable(interfaceSchema.DisplayName.c_str(), bSelected)) {
                    SelectedPlotInterfaceId = interfaceSchema.InterfaceId;
                    SelectedPlotParameterId.clear();
                    EnsurePlotSelectionIsValid();
                }

                ImGui::SameLine();
                ImGui::TextDisabled("%s", interfaceSchema.InterfaceId.c_str());
                if (!interfaceSchema.Description.empty()) {
                    ImGui::TextDisabled("%s", TruncateLabel(interfaceSchema.Description).c_str());
                }
                ImGui::PopID();
            }
        }
        ImGui::EndChild();

        ImGui::TableSetColumnIndex(1);
        if (ImGui::BeginChild("PlotsInterfaceInspector", ImVec2(0.0f, 0.0f), false)) {
            const auto *interfaceSchema = FindInterfaceById(catalog, SelectedPlotInterfaceId);
            if (interfaceSchema == nullptr) {
                ImGui::TextDisabled("Select a plot interface.");
            } else {
                ImGui::TextUnformatted(interfaceSchema->DisplayName.c_str());
                ImGui::TextDisabled("%s", interfaceSchema->InterfaceId.c_str());
                if (!interfaceSchema->Description.empty()) {
                    ImGui::TextWrapped("%s", interfaceSchema->Description.c_str());
                }

                ImGui::SeparatorText("Operations");
                for (const auto &operation : interfaceSchema->Operations) {
                    const auto isBuiltinRequiredInput = [](const Slab::Str &fieldId) -> bool {
                        return fieldId == "parameter_id" || fieldId == "value";
                    };
                    const bool bNeedsParameterSelection = std::any_of(
                        operation.Inputs.begin(),
                        operation.Inputs.end(),
                        [](const auto &input) {
                            return input.bRequired && input.FieldId == "parameter_id";
                        });
                    const bool bNeedsValueInput = std::any_of(
                        operation.Inputs.begin(),
                        operation.Inputs.end(),
                        [](const auto &input) {
                            return input.bRequired && input.FieldId == "value";
                        });
                    const bool bHasNonBuiltinRequiredInput = std::any_of(
                        operation.Inputs.begin(),
                        operation.Inputs.end(),
                        [&](const auto &input) {
                            return input.bRequired && !isBuiltinRequiredInput(input.FieldId);
                        });

                    bool bCanInvoke = !bHasNonBuiltinRequiredInput;
                    if (bNeedsParameterSelection || bNeedsValueInput) {
                        bCanInvoke = bCanInvoke && !SelectedPlotParameterId.empty();
                    }

                    const bool bShowsBuiltinInputFields = !bHasNonBuiltinRequiredInput &&
                        std::any_of(operation.Inputs.begin(), operation.Inputs.end(), [&](const auto &input) {
                            return input.bRequired && isBuiltinRequiredInput(input.FieldId);
                        });

                    if (bShowsBuiltinInputFields && !SelectedPlotParameterId.empty()) {
                        const auto draftKey = BuildPlotParameterDraftKey(interfaceSchema->InterfaceId, SelectedPlotParameterId);
                        if (!PlotParameterDraftByKey.contains(draftKey)) {
                            if (const auto liveValue = PlotReflectionAdapter.EncodeCurrentParameterValue(
                                    interfaceSchema->InterfaceId,
                                    SelectedPlotParameterId); liveValue.has_value()) {
                                PlotParameterDraftByKey[draftKey] = liveValue->Encoded;
                            } else {
                                PlotParameterDraftByKey[draftKey] = "";
                            }
                        }
                    }

                    ImGui::PushID(operation.OperationId.c_str());
                    ImGui::BeginDisabled(!bCanInvoke);
                    if (ImGui::Button(operation.DisplayName.c_str())) {
                        InvokeSelectedPlotOperation(*interfaceSchema, operation.OperationId, context);
                    }
                    const bool bOperationButtonHovered = ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort);
                    ImGui::EndDisabled();
                    ImGui::SameLine();
                    ImGui::TextDisabled(
                        "%s | %s | %s",
                        ToString(operation.Kind),
                        ToString(operation.RunStatePolicy),
                        ToString(operation.ThreadAffinity));
                    const bool bOperationMetaHovered = ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort);
                    if (!operation.Description.empty() && (bOperationButtonHovered || bOperationMetaHovered)) {
                        ImGui::BeginTooltip();
                        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 42.0f);
                        ImGui::TextUnformatted(operation.Description.c_str());
                        ImGui::PopTextWrapPos();
                        ImGui::EndTooltip();
                    }

                    if (bHasNonBuiltinRequiredInput) {
                        ImGui::SameLine();
                        ImGui::TextDisabled("requires non-builtin input");
                    }

                    if (bShowsBuiltinInputFields) {
                        ImGui::Indent();
                        if (bNeedsParameterSelection) {
                            const char *preview = SelectedPlotParameterId.empty()
                                ? "<select parameter_id>"
                                : SelectedPlotParameterId.c_str();
                            ImGui::SetNextItemWidth(260.0f);
                            if (ImGui::BeginCombo("parameter_id", preview)) {
                                for (const auto &parameter : interfaceSchema->Parameters) {
                                    const bool bSelectedParameter = parameter.ParameterId == SelectedPlotParameterId;
                                    if (ImGui::Selectable(parameter.ParameterId.c_str(), bSelectedParameter)) {
                                        SelectedPlotParameterId = parameter.ParameterId;
                                        const auto draftKey = BuildPlotParameterDraftKey(
                                            interfaceSchema->InterfaceId,
                                            SelectedPlotParameterId);
                                        if (!PlotParameterDraftByKey.contains(draftKey)) {
                                            if (const auto liveValue = PlotReflectionAdapter.EncodeCurrentParameterValue(
                                                    interfaceSchema->InterfaceId,
                                                    SelectedPlotParameterId); liveValue.has_value()) {
                                                PlotParameterDraftByKey[draftKey] = liveValue->Encoded;
                                            } else {
                                                PlotParameterDraftByKey[draftKey] = "";
                                            }
                                        }
                                    }
                                    if (bSelectedParameter) ImGui::SetItemDefaultFocus();
                                }
                                ImGui::EndCombo();
                            }
                        }

                        if (bNeedsValueInput) {
                            if (SelectedPlotParameterId.empty()) {
                                ImGui::TextDisabled("value: select parameter_id first");
                            } else {
                                const auto draftKey = BuildPlotParameterDraftKey(
                                    interfaceSchema->InterfaceId,
                                    SelectedPlotParameterId);
                                auto &draft = PlotParameterDraftByKey[draftKey];
                                auto valueBuffer = std::vector<char>(512, '\0');
                                if (!draft.empty()) {
                                    std::strncpy(valueBuffer.data(), draft.c_str(), valueBuffer.size() - 1);
                                    valueBuffer[valueBuffer.size() - 1] = '\0';
                                }
                                ImGui::SetNextItemWidth(320.0f);
                                if (ImGui::InputText("value", valueBuffer.data(), valueBuffer.size())) {
                                    draft = valueBuffer.data();
                                }
                                ImGui::SameLine();
                                if (ImGui::SmallButton("Load Live")) {
                                    if (const auto liveValue = PlotReflectionAdapter.EncodeCurrentParameterValue(
                                            interfaceSchema->InterfaceId,
                                            SelectedPlotParameterId); liveValue.has_value()) {
                                        draft = liveValue->Encoded;
                                    }
                                }
                            }
                        }
                        ImGui::Unindent();
                    }
                    ImGui::PopID();
                }

                ImGui::SeparatorText("Parameters");
                if (interfaceSchema->Parameters.empty()) {
                    ImGui::TextDisabled("No parameters in this interface.");
                } else {
                    constexpr auto tableFlags =
                        ImGuiTableFlags_Borders |
                        ImGuiTableFlags_RowBg |
                        ImGuiTableFlags_SizingFixedFit |
                        ImGuiTableFlags_ScrollY;

                    if (ImGui::BeginTable("PlotsParameterTable", 4, tableFlags, ImVec2(0.0f, 220.0f))) {
                        ImGui::TableSetupColumn("Parameter", ImGuiTableColumnFlags_WidthStretch, 1.2f);
                        ImGui::TableSetupColumn("Type");
                        ImGui::TableSetupColumn("Mutability");
                        ImGui::TableSetupColumn("Live Value", ImGuiTableColumnFlags_WidthStretch, 1.6f);
                        ImGui::TableHeadersRow();

                        for (const auto &parameterSchema : interfaceSchema->Parameters) {
                            ImGui::TableNextRow();
                            ImGui::TableSetColumnIndex(0);
                            const bool bSelected = parameterSchema.ParameterId == SelectedPlotParameterId;
                            ImGui::PushID(parameterSchema.ParameterId.c_str());
                            if (ImGui::Selectable(parameterSchema.DisplayName.c_str(), bSelected)) {
                                SelectedPlotParameterId = parameterSchema.ParameterId;
                                const auto draftKey = BuildPlotParameterDraftKey(
                                    interfaceSchema->InterfaceId,
                                    SelectedPlotParameterId);
                                if (!PlotParameterDraftByKey.contains(draftKey)) {
                                    if (const auto liveValue = PlotReflectionAdapter.EncodeCurrentParameterValue(
                                            interfaceSchema->InterfaceId,
                                            SelectedPlotParameterId); liveValue.has_value()) {
                                        PlotParameterDraftByKey[draftKey] = liveValue->Encoded;
                                    } else {
                                        PlotParameterDraftByKey[draftKey] = "";
                                    }
                                }
                            }
                            ImGui::TableSetColumnIndex(1);
                            ImGui::TextUnformatted(parameterSchema.TypeId.c_str());
                            ImGui::TableSetColumnIndex(2);
                            ImGui::TextUnformatted(ToString(parameterSchema.Mutability));
                            ImGui::TableSetColumnIndex(3);
                            if (const auto liveValue = PlotReflectionAdapter.EncodeCurrentParameterValue(
                                    interfaceSchema->InterfaceId,
                                    parameterSchema.ParameterId); liveValue.has_value()) {
                                ImGui::TextWrapped("%s", liveValue->Encoded.c_str());
                            } else {
                                ImGui::TextDisabled("<unavailable>");
                            }
                            ImGui::PopID();
                        }

                        ImGui::EndTable();
                    }

                    const auto *selectedParameterSchema = FindParameterById(*interfaceSchema, SelectedPlotParameterId);
                    if (selectedParameterSchema != nullptr) {
                        ImGui::SeparatorText("Parameter Editor");
                        ImGui::TextUnformatted(selectedParameterSchema->DisplayName.c_str());
                        ImGui::TextDisabled(
                            "id=%s | type=%s | mutability=%s",
                            selectedParameterSchema->ParameterId.c_str(),
                            selectedParameterSchema->TypeId.c_str(),
                            ToString(selectedParameterSchema->Mutability));
                        if (!selectedParameterSchema->Description.empty()) {
                            ImGui::TextWrapped("%s", selectedParameterSchema->Description.c_str());
                        }

                        const auto draftKey = BuildPlotParameterDraftKey(
                            interfaceSchema->InterfaceId,
                            selectedParameterSchema->ParameterId);
                        auto &draft = PlotParameterDraftByKey[draftKey];
                        drawStringEditor("Encoded Value", draft, 512);

                        if (ImGui::Button("Load Live Value")) {
                            if (const auto liveValue = PlotReflectionAdapter.EncodeCurrentParameterValue(
                                    interfaceSchema->InterfaceId,
                                    selectedParameterSchema->ParameterId); liveValue.has_value()) {
                                draft = liveValue->Encoded;
                            }
                        }
                        ImGui::SameLine();
                        if (ImGui::Button("Set Parameter")) {
                            SelectedPlotParameterId = selectedParameterSchema->ParameterId;
                            InvokeSelectedPlotOperation(*interfaceSchema, CPlotOperationIdCommandSetParameterV2, context);
                        }

                        if (selectedParameterSchema->Mutability == EParameterMutability::RestartRequired) {
                            ImGui::SameLine();
                            if (ImGui::Button("Apply Pending")) {
                                InvokeSelectedPlotOperation(*interfaceSchema, CPlotOperationIdCommandApplyPendingV2, context);
                            }
                        }
                    }
                }

                ImGui::SeparatorText("Last Outputs");
                if (PlotsLastOperationOutput.empty()) {
                    ImGui::TextDisabled("No operation output yet.");
                } else {
                    for (const auto &[key, value] : PlotsLastOperationOutput) {
                        ImGui::BulletText("%s (%s): %s", key.c_str(), value.TypeId.c_str(), value.Encoded.c_str());
                    }
                }
            }
        }
        ImGui::EndChild();

        ImGui::EndTable();
    }
}

auto FLabV2WindowManager::DrawSchemesBlueprintGraphPanel() -> void {
    using namespace Slab::Core::Reflection::V2;
    using namespace Slab::Graphics::Plot2D::V2;

    if (!CEnableBlueprintGraphTuningUi) {
        bShowBlueprintTuningWindow = false;
        BlueprintNodeHeaderScale = 1.55f;
        BlueprintNodeHeaderMinHeight = 36.0f;
        BlueprintNodeBodyTopPadding = 8.0f;
        BlueprintNodeBodyBottomPadding = 10.0f;
        BlueprintNodeBadgeRowGap = 4.0f;
        BlueprintNodeActionReserve = 14.0f;
        BlueprintNodeBadgeTopOffset = 42.0f;
        BlueprintNodeActionGap = 10.0f;
        BlueprintLegendPadding = 14.0f;
        BlueprintLegendRowGap = 6.0f;
        BlueprintLegendItemGap = 8.0f;
        BlueprintLegendChipExtraHeight = 12.0f;
        BlueprintLegendMarkerInset = 4.0f;
    }

    auto &catalogRegistry = Slab::Core::Reflection::V2::FReflectionCatalogRegistryV2::Get();
    catalogRegistry.RefreshAll();
    const auto catalog = catalogRegistry.BuildMergedCatalog();
    Slab::Core::Reflection::V2::SyncReflectionSemanticsOverlayV1(catalog);
    EnsureSchemeSelectionIsValid(catalog);

    const auto context = BuildReflectionInvocationContext();
    if (catalog.Interfaces.empty()) {
        ImGui::TextDisabled("No interfaces available in reflection catalog.");
        return;
    }

    const auto *interfaceSchema = FindInterfaceById(catalog, SelectedSchemeInterfaceId);
    if (interfaceSchema == nullptr) {
        ImGui::TextDisabled("Select an interface in Interface Inspector.");
        return;
    }

    FReflectionCatalogV2 selectedInterfaceCatalog;
    selectedInterfaceCatalog.Interfaces.push_back(*interfaceSchema);
    const auto previousBlueprintCanvas = SchemesBlueprintDocument.Canvas;
    SchemesBlueprintDocument = MakeGraphDocumentFromReflectionCatalog(
        selectedInterfaceCatalog,
        EGraphModeV2::SchemesBlueprint);
    SchemesBlueprintDocument.Canvas = previousBlueprintCanvas;

    ImGui::Text("Graph interface: %s", interfaceSchema->DisplayName.c_str());
    ImGui::SameLine();
    ImGui::TextDisabled("(%s)", interfaceSchema->InterfaceId.c_str());
    ImGui::SameLine();
    ImGui::TextDisabled(
        "| substrate nodes=%zu edges=%zu members=%zu",
        SchemesBlueprintDocument.Nodes.size(),
        SchemesBlueprintDocument.Edges.size(),
        CountGraphMembers(SchemesBlueprintDocument));
    ImGui::SameLine();
    ImGui::Checkbox("Grid", &SchemesBlueprintDocument.Canvas.bShowGrid);
    ImGui::SameLine();
    ImGui::Checkbox("Legend", &bShowBlueprintLegend);
    if constexpr (CEnableBlueprintGraphTuningUi) {
        ImGui::SameLine();
        ImGui::Checkbox("Tune", &bShowBlueprintTuningWindow);
    }
    ImGui::SameLine();
    {
        auto filterBuffer = std::vector<char>(256, '\0');
        if (!BlueprintGraphFilterText.empty()) {
            std::strncpy(filterBuffer.data(), BlueprintGraphFilterText.c_str(), filterBuffer.size() - 1);
            filterBuffer[filterBuffer.size() - 1] = '\0';
        }
        ImGui::SetNextItemWidth(260.0f);
        if (ImGui::InputTextWithHint("##BlueprintGraphFilter", "Search id/type/mutability...", filterBuffer.data(), filterBuffer.size())) {
            BlueprintGraphFilterText = filterBuffer.data();
        }
    }
    ImGui::SameLine();
    ImGui::Checkbox("Parameters", &bBlueprintGraphShowParameters);
    ImGui::SameLine();
    ImGui::Checkbox("Queries", &bBlueprintGraphShowQueries);
    ImGui::SameLine();
    ImGui::Checkbox("Commands", &bBlueprintGraphShowCommands);
    ImGui::SameLine();
    const char *mutabilityFilterLabel = "Mutability: Any";
    if (BlueprintGraphMutabilityFilter == 1) mutabilityFilterLabel = "Mutability: Const";
    if (BlueprintGraphMutabilityFilter == 2) mutabilityFilterLabel = "Mutability: Runtime";
    if (BlueprintGraphMutabilityFilter == 3) mutabilityFilterLabel = "Mutability: Restart";
    if (ImGui::BeginCombo("##BlueprintMutabilityFilter", mutabilityFilterLabel)) {
        if (ImGui::Selectable("Any", BlueprintGraphMutabilityFilter == 0)) BlueprintGraphMutabilityFilter = 0;
        if (ImGui::Selectable("Const", BlueprintGraphMutabilityFilter == 1)) BlueprintGraphMutabilityFilter = 1;
        if (ImGui::Selectable("RuntimeMutable", BlueprintGraphMutabilityFilter == 2)) BlueprintGraphMutabilityFilter = 2;
        if (ImGui::Selectable("RestartRequired", BlueprintGraphMutabilityFilter == 3)) BlueprintGraphMutabilityFilter = 3;
        ImGui::EndCombo();
    }
    ImGui::SameLine();
    if (ImGui::Button("Auto Layout")) {
        const auto keyPrefix = interfaceSchema->InterfaceId + "::";
        for (auto it = BlueprintNodePositionById.begin(); it != BlueprintNodePositionById.end(); ) {
            if (it->first == interfaceSchema->InterfaceId || it->first.rfind(keyPrefix, 0) == 0) {
                it = BlueprintNodePositionById.erase(it);
            } else {
                ++it;
            }
        }
    }
    ImGui::SameLine();
    if (ImGui::Button("Reset View")) {
        SchemesBlueprintDocument.Canvas.PanX = 80.0f;
        SchemesBlueprintDocument.Canvas.PanY = 80.0f;
        bBlueprintGraphShowParameters = true;
        bBlueprintGraphShowQueries = true;
        bBlueprintGraphShowCommands = true;
        BlueprintGraphMutabilityFilter = 0;
    }

    ImGui::Separator();

    constexpr float SplitterHeight = 6.0f;
    constexpr float GraphTraceMinHeight = 80.0f;
    constexpr float GraphTraceMaxHeight = 340.0f;
    // Keep a small epsilon to avoid 1-2px overflow/scrollbar jitter from fractional layout math.
    constexpr float CanvasLayoutEpsilon = 6.0f;
    BlueprintGraphTraceHeight = std::clamp(BlueprintGraphTraceHeight, GraphTraceMinHeight, GraphTraceMaxHeight);
    const float canvasBottomReserve = BlueprintGraphTraceHeight + 44.0f + SplitterHeight + CanvasLayoutEpsilon;

    const auto canvasPos = ImGui::GetCursorScreenPos();
    auto canvasSize = ImGui::GetContentRegionAvail();
    if (canvasSize.x < 64.0f) canvasSize.x = 64.0f;
    canvasSize.y = std::max(180.0f, canvasSize.y - canvasBottomReserve);
    const auto canvasEnd = ImVec2(canvasPos.x + canvasSize.x, canvasPos.y + canvasSize.y);

    ImGui::InvisibleButton(
        "SchemesBlueprintCanvas",
        canvasSize,
        ImGuiButtonFlags_MouseButtonRight);
    ImGui::SetItemAllowOverlap();

    const bool bCanvasHovered = ImGui::IsItemHovered();
    if (bCanvasHovered && ImGui::IsMouseDragging(ImGuiMouseButton_Right, 0.0f)) {
        const auto &io = ImGui::GetIO();
        SchemesBlueprintDocument.Canvas.PanX += io.MouseDelta.x;
        SchemesBlueprintDocument.Canvas.PanY += io.MouseDelta.y;
    }

    auto *drawList = ImGui::GetWindowDrawList();
    drawList->AddRectFilled(canvasPos, canvasEnd, IM_COL32(20, 24, 30, 255), 6.0f);
    drawList->AddRect(canvasPos, canvasEnd, IM_COL32(72, 82, 92, 255), 6.0f, 0, 1.0f);

    drawList->PushClipRect(canvasPos, canvasEnd, true);

    if (SchemesBlueprintDocument.Canvas.bShowGrid) {
        constexpr float GridStep = 64.0f;
        const float xOffset = std::fmod(SchemesBlueprintDocument.Canvas.PanX, GridStep);
        const float yOffset = std::fmod(SchemesBlueprintDocument.Canvas.PanY, GridStep);

        for (float x = canvasPos.x + xOffset; x < canvasEnd.x; x += GridStep) {
            drawList->AddLine(ImVec2(x, canvasPos.y), ImVec2(x, canvasEnd.y), IM_COL32(36, 42, 52, 255), 1.0f);
        }
        for (float y = canvasPos.y + yOffset; y < canvasEnd.y; y += GridStep) {
            drawList->AddLine(ImVec2(canvasPos.x, y), ImVec2(canvasEnd.x, y), IM_COL32(36, 42, 52, 255), 1.0f);
        }
    }

    Slab::Str hoveredLegendLabel;
    Slab::Str hoveredLegendDetail;
    if (bShowBlueprintLegend) {
        struct FLegendEntry {
            const char *Id;
            ImVec4 Color;
            const char *Label;
            const char *Tooltip;
        };

        const std::array<FLegendEntry, 7> entries = {{
            {"legend-interface", ImVec4(112.0f / 255.0f, 94.0f / 255.0f, 154.0f / 255.0f, 1.0f), "Interface", "Root interface descriptor node."},
            {"legend-parameter", ImVec4(92.0f / 255.0f, 106.0f / 255.0f, 132.0f / 255.0f, 1.0f), "Parameter", "Reflected parameter schema + live/draft policy badges."},
            {"legend-query", ImVec4(78.0f / 255.0f, 132.0f / 255.0f, 120.0f / 255.0f, 1.0f), "Query", "Read/query operation endpoint."},
            {"legend-command", ImVec4(146.0f / 255.0f, 124.0f / 255.0f, 72.0f / 255.0f, 1.0f), "Command", "Mutating command operation endpoint."},
            {"legend-edge-param", ImVec4(104.0f / 255.0f, 168.0f / 255.0f, 224.0f / 255.0f, 1.0f), "Param Link", "Parameter-to-interface relation."},
            {"legend-edge-op", ImVec4(116.0f / 255.0f, 214.0f / 255.0f, 174.0f / 255.0f, 1.0f), "Op Link", "Interface-to-operation relation."},
            {"legend-prefix", ImVec4(168.0f / 255.0f, 174.0f / 255.0f, 188.0f / 255.0f, 1.0f), "Prefixes", "M=Mutability, E=Exposure, S=State, R=Run State, T=Thread, X=Side Effect, I=Required Inputs, N=Node Kind."}
        }};

        const float legendPadding = BlueprintLegendPadding;
        const float legendTextHeight = ImGui::GetTextLineHeight();
        const float legendChipHeight = legendTextHeight + BlueprintLegendChipExtraHeight;
        const float legendMarkerSize = legendChipHeight - BlueprintLegendMarkerInset;
        const float legendRowGap = BlueprintLegendRowGap;
        const float legendItemGap = BlueprintLegendItemGap;
        const float legendContentWidth = (canvasEnd.x - 6.0f) - (canvasPos.x + 6.0f) - 2.0f * legendPadding;

        int legendRowCount = 1;
        float currentRowWidth = 0.0f;
        for (const auto &entry : entries) {
            const float labelW = ImGui::CalcTextSize(entry.Label).x;
            const float itemW = legendMarkerSize + 6.0f + labelW + 14.0f;
            if (currentRowWidth > 0.0f && currentRowWidth + itemW > legendContentWidth) {
                ++legendRowCount;
                currentRowWidth = 0.0f;
            }
            currentRowWidth += itemW + legendItemGap;
        }

        const float legendHeight = legendPadding * 2.0f +
            static_cast<float>(legendRowCount) * legendChipHeight +
            static_cast<float>(std::max(0, legendRowCount - 1)) * legendRowGap;
        const ImVec2 legendMin = ImVec2(canvasPos.x + 6.0f, canvasEnd.y - legendHeight - 6.0f);
        const ImVec2 legendMax = ImVec2(canvasEnd.x - 6.0f, canvasEnd.y - 6.0f);
        drawList->AddRectFilled(legendMin, legendMax, IM_COL32(12, 16, 22, 228), 4.0f);
        drawList->AddRect(legendMin, legendMax, IM_COL32(108, 120, 138, 195), 4.0f, 0, 1.0f);

        const auto legendMousePos = ImGui::GetIO().MousePos;
        drawList->PushClipRect(legendMin, legendMax, true);
        float legendX = legendMin.x + legendPadding;
        float legendY = legendMin.y + legendPadding;
        for (const auto &entry : entries) {
            const float labelW = ImGui::CalcTextSize(entry.Label).x;
            const float itemW = legendMarkerSize + 6.0f + labelW + 14.0f;
            if (legendX > legendMin.x + legendPadding && legendX + itemW > legendMax.x - legendPadding) {
                legendX = legendMin.x + legendPadding;
                legendY += legendChipHeight + legendRowGap;
            }

            const ImVec2 chipMin = ImVec2(legendX, legendY);
            const ImVec2 chipMax = ImVec2(legendX + itemW, legendY + legendChipHeight);
            drawList->AddRectFilled(chipMin, chipMax, IM_COL32(22, 28, 40, 168), 4.0f);

            const ImVec2 markerMin = ImVec2(legendX + 4.0f, legendY + 2.0f);
            const ImVec2 markerMax = ImVec2(markerMin.x + legendMarkerSize, markerMin.y + legendMarkerSize);
            drawList->AddRectFilled(markerMin, markerMax, ImGui::GetColorU32(entry.Color), 2.0f);
            drawList->AddRect(markerMin, markerMax, IM_COL32(205, 212, 225, 200), 2.0f, 0, 1.0f);
            drawList->AddText(ImVec2(markerMax.x + 6.0f, legendY + 3.0f), IM_COL32(226, 234, 244, 255), entry.Label);

            if (legendMousePos.x >= chipMin.x && legendMousePos.y >= chipMin.y &&
                legendMousePos.x <= chipMax.x && legendMousePos.y <= chipMax.y) {
                hoveredLegendLabel = entry.Label;
                hoveredLegendDetail = entry.Tooltip;
            }

            legendX += itemW + legendItemGap;
        }
        drawList->PopClipRect();
    }

    struct FGraphNode {
        enum class EKind : unsigned char {
            Interface,
            Parameter,
            Query,
            Command
        };

        Slab::Str Key;
        Slab::Str Title;
        Slab::Str SubtitlePrimary;
        Slab::Str SubtitleSecondary;
        Slab::Vector<std::pair<Slab::Str, ImU32>> Badges;
        FGraphNodeV2 *GraphNode = nullptr;
        ImVec2 Size = ImVec2(360.0f, 132.0f);
        EKind Kind = EKind::Parameter;
        Slab::Str RefId;
        int InputSocketCount = 1;
        bool bSelected = false;
    };

    const auto findOrCreatePosition = [this](FGraphNodeV2 &graphNode, const ImVec2 defaultPosition, Slab::Str legacyKey = {}) -> void {
        if (const auto positionIt = BlueprintNodePositionById.find(graphNode.NodeId); positionIt != BlueprintNodePositionById.end()) {
            graphNode.Position.x = positionIt->second.x;
            graphNode.Position.y = positionIt->second.y;
            return;
        }

        if (!legacyKey.empty()) {
            if (const auto legacyIt = BlueprintNodePositionById.find(legacyKey); legacyIt != BlueprintNodePositionById.end()) {
                graphNode.Position.x = legacyIt->second.x;
                graphNode.Position.y = legacyIt->second.y;
                BlueprintNodePositionById.emplace(graphNode.NodeId, legacyIt->second);
                return;
            }
        }

        graphNode.Position.x = defaultPosition.x;
        graphNode.Position.y = defaultPosition.y;
        BlueprintNodePositionById[graphNode.NodeId] = defaultPosition;
    };

    const auto persistNodePosition = [this](const FGraphNode &node) -> void {
        if (node.GraphNode == nullptr) return;
        BlueprintNodePositionById[node.Key] = ImVec2(node.GraphNode->Position.x, node.GraphNode->Position.y);
    };

    Slab::Vector<FGraphNode> nodes;
    nodes.reserve(SchemesBlueprintDocument.Nodes.size());

    const auto computeNodeSize = [this](const FGraphNode &node, const float minWidth, const float maxWidth) -> ImVec2 {
        const float lineHeight = ImGui::GetTextLineHeight();
        const float headerHeight = std::max(BlueprintNodeHeaderMinHeight, std::round(lineHeight * BlueprintNodeHeaderScale));
        constexpr float BodyPaddingX = 9.0f;
        const float bodyTopPadding = BlueprintNodeBodyTopPadding;
        const float bodyBottomPadding = BlueprintNodeBodyBottomPadding;
        const float badgeRowGap = BlueprintNodeBadgeRowGap;
        const float titleWidth = ImGui::CalcTextSize(node.Title.c_str()).x;
        const float subtitleWidth = ImGui::CalcTextSize(node.SubtitlePrimary.c_str()).x;

        float badgesWidth = 0.0f;
        for (const auto &badge : node.Badges) badgesWidth += MeasureBadgeWidth(badge.first);

        float width = std::max(titleWidth, subtitleWidth) + 2.0f * BodyPaddingX + 4.0f;
        width = std::max(width, badgesWidth + 2.0f * BodyPaddingX + 4.0f);
        width = std::clamp(width, minWidth, maxWidth);

        const float contentWidth = std::max(120.0f, width - 2.0f * BodyPaddingX);
        int badgeRows = 0;
        float rowWidth = 0.0f;
        for (const auto &badge : node.Badges) {
            const float badgeWidth = MeasureBadgeWidth(badge.first);
            if (rowWidth > 0.0f && rowWidth + badgeWidth > contentWidth) {
                ++badgeRows;
                rowWidth = 0.0f;
            }
            if (rowWidth <= 0.0f) ++badgeRows;
            rowWidth += badgeWidth;
        }

        const float textBlockHeight = lineHeight + 4.0f + lineHeight;
        const float badgeHeight = badgeRows > 0
            ? static_cast<float>(badgeRows) * (lineHeight + 4.0f) + static_cast<float>(badgeRows - 1) * badgeRowGap
            : 0.0f;
        const float actionReserveHeight = node.Kind == FGraphNode::EKind::Interface ? 0.0f : (lineHeight + BlueprintNodeActionReserve);
        const float height = headerHeight + bodyTopPadding + textBlockHeight + 10.0f + badgeHeight + actionReserveHeight + bodyBottomPadding;
        return ImVec2(width, std::max(116.0f, height));
    };

    float parameterLaneY = 56.0f;
    float queryLaneY = 56.0f;
    float commandLaneY = 56.0f;
    for (auto &graphNode : SchemesBlueprintDocument.Nodes) {
        if (graphNode.Kind == EGraphNodeKindV2::Interface) {
            FGraphNode interfaceNode;
            interfaceNode.Key = graphNode.NodeId;
            interfaceNode.Title = graphNode.DisplayName;
            interfaceNode.SubtitlePrimary = graphNode.SourceInterfaceId.empty() ? graphNode.NodeId : graphNode.SourceInterfaceId;
            interfaceNode.SubtitleSecondary = Slab::Str("parameters=") + std::to_string(interfaceSchema->Parameters.size()) +
                " | operations=" + std::to_string(interfaceSchema->Operations.size());
            interfaceNode.GraphNode = &graphNode;
            interfaceNode.Kind = FGraphNode::EKind::Interface;
            interfaceNode.RefId = graphNode.SourceInterfaceId.empty() ? graphNode.NodeId : graphNode.SourceInterfaceId;
            interfaceNode.bSelected = true;
            interfaceNode.Badges.push_back({WithPolicyPrefix('N', "Interface"), IM_COL32(118, 98, 166, 240)});
            interfaceNode.Size = computeNodeSize(interfaceNode, 300.0f, 520.0f);
            const auto legacyInterfaceKey = interfaceSchema->InterfaceId + "::interface";
            findOrCreatePosition(graphNode, ImVec2(560.0f, 200.0f), legacyInterfaceKey);
            persistNodePosition(interfaceNode);
            nodes.push_back(std::move(interfaceNode));
            continue;
        }

        if (graphNode.Kind == EGraphNodeKindV2::Parameter) {
            const auto *parameterSchema = FindParameterById(*interfaceSchema, graphNode.SourceParameterId);
            if (parameterSchema == nullptr) continue;
            if (!bBlueprintGraphShowParameters) continue;

            const auto mutability = graphNode.Policies.ParameterMutability.value_or(parameterSchema->Mutability);
            const auto exposure = graphNode.Policies.ParameterExposure.value_or(parameterSchema->Exposure);

            if (BlueprintGraphMutabilityFilter == 1 && mutability != EParameterMutability::Const) continue;
            if (BlueprintGraphMutabilityFilter == 2 && mutability != EParameterMutability::RuntimeMutable) continue;
            if (BlueprintGraphMutabilityFilter == 3 && mutability != EParameterMutability::RestartRequired) continue;

            const bool bMatchesText =
                ContainsCaseInsensitive(graphNode.DisplayName, BlueprintGraphFilterText) ||
                ContainsCaseInsensitive(graphNode.SourceParameterId, BlueprintGraphFilterText) ||
                ContainsCaseInsensitive(parameterSchema->TypeId, BlueprintGraphFilterText) ||
                ContainsCaseInsensitive(ToString(mutability), BlueprintGraphFilterText) ||
                ContainsCaseInsensitive(ToString(exposure), BlueprintGraphFilterText);
            if (!bMatchesText) continue;

            const auto draftKey = BuildSchemeParameterDraftKey(interfaceSchema->InterfaceId, parameterSchema->ParameterId);
            const auto draftIt = SchemeParameterDraftByKey.find(draftKey);
            const auto draftValue = draftIt != SchemeParameterDraftByKey.end() ? draftIt->second : Slab::Str{};
            const auto liveValue = catalogRegistry.EncodeCurrentParameterValue(interfaceSchema->InterfaceId, parameterSchema->ParameterId);

            Slab::Str stateLabel = "Unavailable";
            ImU32 stateColor = IM_COL32(120, 124, 136, 220);
            if (liveValue.has_value()) {
                if (draftValue == liveValue->Encoded) {
                    stateLabel = "Live";
                    stateColor = IM_COL32(94, 180, 130, 220);
                } else if (mutability == EParameterMutability::RestartRequired) {
                    stateLabel = "Pending";
                    stateColor = IM_COL32(222, 176, 102, 220);
                } else {
                    stateLabel = "Draft";
                    stateColor = IM_COL32(122, 168, 224, 220);
                }
            }

            FGraphNode node;
            node.Key = graphNode.NodeId;
            node.Title = graphNode.DisplayName;
            node.SubtitlePrimary = Slab::Str(graphNode.SourceParameterId) + " | " + TruncateLabel(parameterSchema->TypeId, 18);
            node.SubtitleSecondary = Slab::Str("State: ") + stateLabel + " | type: " + parameterSchema->TypeId;
            node.GraphNode = &graphNode;
            node.Kind = FGraphNode::EKind::Parameter;
            node.RefId = graphNode.SourceParameterId;
            node.bSelected = SelectedSchemeParameterId == graphNode.SourceParameterId;
            node.Badges.push_back({WithPolicyPrefix('M', ToCompactMutabilityLabel(mutability)), IM_COL32(88, 132, 192, 220)});
            node.Badges.push_back({WithPolicyPrefix('E', ToCompactExposureLabel(exposure)), IM_COL32(96, 120, 150, 220)});
            node.Badges.push_back({WithPolicyPrefix('S', stateLabel), stateColor});
            node.Size = computeNodeSize(node, 340.0f, 620.0f);
            const auto nodeHeight = node.Size.y;
            findOrCreatePosition(graphNode, ImVec2(70.0f, parameterLaneY));
            persistNodePosition(node);
            nodes.push_back(std::move(node));
            parameterLaneY += nodeHeight + 20.0f;
            continue;
        }

        if (graphNode.Kind != EGraphNodeKindV2::Operation) continue;
        const auto *operationSchema = FindOperationById(*interfaceSchema, graphNode.SourceOperationId);
        if (operationSchema == nullptr) continue;
        if (operationSchema->Kind == EOperationKind::Query && !bBlueprintGraphShowQueries) continue;
        if (operationSchema->Kind == EOperationKind::Command && !bBlueprintGraphShowCommands) continue;

        const bool bMatchesText =
            ContainsCaseInsensitive(graphNode.DisplayName, BlueprintGraphFilterText) ||
            ContainsCaseInsensitive(graphNode.SourceOperationId, BlueprintGraphFilterText) ||
            ContainsCaseInsensitive(ToString(operationSchema->Kind), BlueprintGraphFilterText) ||
            ContainsCaseInsensitive(ToString(operationSchema->RunStatePolicy), BlueprintGraphFilterText) ||
            ContainsCaseInsensitive(ToString(operationSchema->ThreadAffinity), BlueprintGraphFilterText);
        if (!bMatchesText) continue;

        FGraphNode node;
        node.Key = graphNode.NodeId;
        node.Title = graphNode.DisplayName;
        node.SubtitlePrimary = Slab::Str(graphNode.SourceOperationId);
        node.SubtitleSecondary = Slab::Str(ToString(operationSchema->Kind)) + " | run=" + ToCompactRunStateLabel(operationSchema->RunStatePolicy) +
            " | thread=" + ToCompactThreadLabel(operationSchema->ThreadAffinity) +
            " | side=" + ToCompactSideEffectLabel(operationSchema->SideEffectClass);
        node.GraphNode = &graphNode;
        const auto requiredInputCount = static_cast<int>(std::count_if(
            graphNode.Ports.begin(),
            graphNode.Ports.end(),
            [](const auto &port) {
                return port.Direction == EGraphPortDirectionV2::Input && port.bRequired;
            }));
        node.InputSocketCount = std::max(1, requiredInputCount);
        node.Size = computeNodeSize(node, 340.0f, 620.0f);
        if (operationSchema->Kind == EOperationKind::Query) {
            node.Kind = FGraphNode::EKind::Query;
            findOrCreatePosition(graphNode, ImVec2(980.0f, queryLaneY), interfaceSchema->InterfaceId + "::operation:" + operationSchema->OperationId);
            queryLaneY += node.Size.y + 20.0f;
        } else {
            node.Kind = FGraphNode::EKind::Command;
            findOrCreatePosition(graphNode, ImVec2(1470.0f, commandLaneY), interfaceSchema->InterfaceId + "::operation:" + operationSchema->OperationId);
            commandLaneY += node.Size.y + 20.0f;
        }
        node.RefId = graphNode.SourceOperationId;
        node.Badges.push_back({WithPolicyPrefix('R', ToCompactRunStateLabel(operationSchema->RunStatePolicy)), IM_COL32(78, 122, 112, 220)});
        node.Badges.push_back({WithPolicyPrefix('T', ToCompactThreadLabel(operationSchema->ThreadAffinity)), IM_COL32(86, 112, 142, 220)});
        node.Badges.push_back({WithPolicyPrefix('X', ToCompactSideEffectLabel(operationSchema->SideEffectClass)), IM_COL32(122, 104, 88, 220)});
        if (requiredInputCount > 0) {
            node.Badges.push_back({WithPolicyPrefix('I', Slab::ToStr(requiredInputCount)), IM_COL32(96, 128, 186, 220)});
        }
        persistNodePosition(node);
        nodes.push_back(std::move(node));
    }

    const auto screenPositionFor = [&](const FGraphNode &node) {
        return ImVec2(
            canvasPos.x + SchemesBlueprintDocument.Canvas.PanX + node.GraphNode->Position.x,
            canvasPos.y + SchemesBlueprintDocument.Canvas.PanY + node.GraphNode->Position.y);
    };

    const auto pinPositionFor = [&](const FGraphNode &node, const bool bOutput, int slot, int totalSlots) {
        const auto nodePos = screenPositionFor(node);
        const float slots = static_cast<float>(std::max(1, totalSlots));
        const float slotRatio = static_cast<float>(slot + 1) / (slots + 1.0f);
        const float y = nodePos.y + 52.0f + slotRatio * (node.Size.y - 64.0f);
        const float x = bOutput ? nodePos.x + node.Size.x : nodePos.x;
        return ImVec2(x, y);
    };

    const auto findVisibleNodeByKind = [&](const FGraphNode::EKind kind) -> const FGraphNode * {
        const auto it = std::find_if(nodes.begin(), nodes.end(), [&](const auto &node) {
            return node.Kind == kind;
        });
        if (it == nodes.end()) return nullptr;
        return &(*it);
    };

    const auto *root = findVisibleNodeByKind(FGraphNode::EKind::Interface);
    if (root == nullptr) {
        drawList->PopClipRect();
        ImGui::SetCursorScreenPos(ImVec2(canvasPos.x, canvasEnd.y));
        ImGui::TextDisabled("Blueprint graph root node unavailable.");
        return;
    }

    std::map<Slab::Str, const FGraphNode *> visibleNodesById;
    for (const auto &node : nodes) {
        visibleNodesById[node.Key] = &node;
    }

    Slab::Vector<const FGraphNode *> parameterNodes;
    Slab::Vector<const FGraphNode *> queryNodes;
    Slab::Vector<const FGraphNode *> commandNodes;
    for (const auto &node : nodes) {
        if (node.Kind == FGraphNode::EKind::Parameter) parameterNodes.push_back(&node);
        else if (node.Kind == FGraphNode::EKind::Query) queryNodes.push_back(&node);
        else if (node.Kind == FGraphNode::EKind::Command) commandNodes.push_back(&node);
    }

    const int parameterCount = static_cast<int>(parameterNodes.size());
    const int operationCount = static_cast<int>(queryNodes.size() + commandNodes.size());
    Slab::Vector<const FGraphNode *> operationNodes;
    operationNodes.reserve(queryNodes.size() + commandNodes.size());
    operationNodes.insert(operationNodes.end(), queryNodes.begin(), queryNodes.end());
    operationNodes.insert(operationNodes.end(), commandNodes.begin(), commandNodes.end());
    std::map<Slab::Str, int> parameterSlotByNodeId;
    std::map<Slab::Str, int> operationSlotByNodeId;
    for (int i = 0; i < parameterCount; ++i) {
        parameterSlotByNodeId[parameterNodes[static_cast<std::size_t>(i)]->Key] = i;
    }
    for (int i = 0; i < operationCount; ++i) {
        operationSlotByNodeId[operationNodes[static_cast<std::size_t>(i)]->Key] = i;
    }

    for (const auto &edge : SchemesBlueprintDocument.Edges) {
        const auto fromIt = visibleNodesById.find(edge.FromNodeId);
        const auto toIt = visibleNodesById.find(edge.ToNodeId);
        if (fromIt == visibleNodesById.end() || toIt == visibleNodesById.end()) continue;

        const auto *fromNode = fromIt->second;
        const auto *toNode = toIt->second;

        const bool bParameterEdge =
            (fromNode->Kind == FGraphNode::EKind::Interface && toNode->Kind == FGraphNode::EKind::Parameter) ||
            (toNode->Kind == FGraphNode::EKind::Interface && fromNode->Kind == FGraphNode::EKind::Parameter);
        if (bParameterEdge) {
            const auto *parameterNode = fromNode->Kind == FGraphNode::EKind::Parameter ? fromNode : toNode;
            const auto slotIt = parameterSlotByNodeId.find(parameterNode->Key);
            if (slotIt == parameterSlotByNodeId.end()) continue;

            const auto source = pinPositionFor(*parameterNode, true, 0, 1);
            const auto target = pinPositionFor(*root, false, slotIt->second, parameterCount);
            const auto cp1 = ImVec2(source.x + 84.0f, source.y);
            const auto cp2 = ImVec2(target.x - 84.0f, target.y);
            drawList->AddBezierCubic(source, cp1, cp2, target, IM_COL32(104, 168, 224, 128), 1.6f);
            continue;
        }

        const bool bOperationEdge =
            (fromNode->Kind == FGraphNode::EKind::Interface &&
                (toNode->Kind == FGraphNode::EKind::Query || toNode->Kind == FGraphNode::EKind::Command)) ||
            (toNode->Kind == FGraphNode::EKind::Interface &&
                (fromNode->Kind == FGraphNode::EKind::Query || fromNode->Kind == FGraphNode::EKind::Command));
        if (!bOperationEdge) continue;

        const auto *operationNode =
            (fromNode->Kind == FGraphNode::EKind::Query || fromNode->Kind == FGraphNode::EKind::Command)
                ? fromNode
                : toNode;
        const auto slotIt = operationSlotByNodeId.find(operationNode->Key);
        if (slotIt == operationSlotByNodeId.end()) continue;

        const auto source = pinPositionFor(*root, true, slotIt->second, operationCount);
        const auto inputSocketCount = std::max(1, operationNode->InputSocketCount);
        for (int socket = 0; socket < inputSocketCount; ++socket) {
            const auto target = pinPositionFor(*operationNode, false, socket, inputSocketCount);
            const auto cp1 = ImVec2(source.x + 108.0f, source.y);
            const auto cp2 = ImVec2(target.x - 108.0f, target.y);
            const auto edgeColor = operationNode->Kind == FGraphNode::EKind::Query
                ? IM_COL32(116, 214, 174, 128)
                : IM_COL32(214, 184, 116, 128);
            drawList->AddBezierCubic(source, cp1, cp2, target, edgeColor, 1.6f);
        }
    }

    struct FNodeAction {
        enum class EKind : unsigned char {
            Invoke,
            CopyId
        };
        Slab::Str Label;
        EKind Kind = EKind::Invoke;
        Slab::Str OperationId;
    };

    struct FActionRect {
        ImVec2 Min;
        ImVec2 Max;
    };

    const auto isPointInside = [](const ImVec2 point, const ImVec2 min, const ImVec2 max) -> bool {
        return point.x >= min.x && point.y >= min.y && point.x <= max.x && point.y <= max.y;
    };

    const auto isNodeSelected = [this](const FGraphNode &node) {
        if (node.Kind == FGraphNode::EKind::Interface) return false;
        if (node.Kind == FGraphNode::EKind::Parameter) return node.RefId == SelectedSchemeParameterId;
        return node.RefId == SelectedSchemeOperationId;
    };

    const auto buildNodeActions = [interfaceSchema](const FGraphNode &node) -> Slab::Vector<FNodeAction> {
        using namespace Slab::Core::Reflection::V2;
        using namespace Slab::Graphics::Plot2D::V2;

        Slab::Vector<FNodeAction> actions;
        const auto hasOperation = [interfaceSchema](const Slab::Str &candidate) -> bool {
            if (candidate.empty()) return false;
            return FindOperationById(*interfaceSchema, candidate) != nullptr;
        };
        const auto selectOperationId = [&hasOperation](const Slab::Str &primary,
                                                       const Slab::Str &alternate) -> Slab::Str {
            if (hasOperation(primary)) return primary;
            if (hasOperation(alternate)) return alternate;
            return {};
        };

        if (node.Kind == FGraphNode::EKind::Parameter) {
            const auto getOperationId = selectOperationId(COperationIdQueryGetParameter, CPlotOperationIdQueryGetParameterV2);
            const auto setOperationId = selectOperationId(COperationIdCommandSetParameter, CPlotOperationIdCommandSetParameterV2);
            const auto applyOperationId = selectOperationId(COperationIdCommandApplyPending, CPlotOperationIdCommandApplyPendingV2);

            if (!getOperationId.empty()) {
                actions.push_back(FNodeAction{"Get", FNodeAction::EKind::Invoke, getOperationId});
            }
            if (!setOperationId.empty()) {
                actions.push_back(FNodeAction{"Set", FNodeAction::EKind::Invoke, setOperationId});
            }
            const auto *parameterSchema = FindParameterById(*interfaceSchema, node.RefId);
            if (parameterSchema != nullptr &&
                parameterSchema->Mutability == EParameterMutability::RestartRequired &&
                !applyOperationId.empty()) {
                actions.push_back(FNodeAction{"Apply", FNodeAction::EKind::Invoke, applyOperationId});
            }
            actions.push_back(FNodeAction{"Copy Id", FNodeAction::EKind::CopyId, {}});
        } else if (node.Kind == FGraphNode::EKind::Query || node.Kind == FGraphNode::EKind::Command) {
            actions.push_back(FNodeAction{"Invoke", FNodeAction::EKind::Invoke, node.RefId});
            actions.push_back(FNodeAction{"Copy Id", FNodeAction::EKind::CopyId, {}});
        }
        return actions;
    };

    const auto buildActionRects = [&](const FGraphNode &node, const Slab::Vector<FNodeAction> &actions) {
        Slab::Vector<FActionRect> rects;
        if (actions.empty()) return rects;

        const auto nodePos = screenPositionFor(node);
        const auto nodeEnd = ImVec2(nodePos.x + node.Size.x, nodePos.y + node.Size.y);
        const float actionHeight = ImGui::GetTextLineHeight() + 6.0f;
        const float startX = nodePos.x + 8.0f;
        float x = startX;
        float y = nodeEnd.y - actionHeight - 6.0f;

        for (const auto &action : actions) {
            const float width = ImGui::CalcTextSize(action.Label.c_str()).x + 12.0f;
            if (x > startX && x + width > nodeEnd.x - 8.0f) {
                x = startX;
                y -= actionHeight + 4.0f;
            }
            if (y < nodePos.y + 58.0f) break;
            rects.push_back(FActionRect{ImVec2(x, y), ImVec2(x + width, y + actionHeight)});
            x += width + 6.0f;
        }
        return rects;
    };

    const auto executeNodeAction = [&](const FGraphNode &node, const FNodeAction &action) {
        if (action.Kind == FNodeAction::EKind::CopyId) {
            ImGui::SetClipboardText(node.RefId.c_str());
            return;
        }

        if (node.Kind == FGraphNode::EKind::Parameter) {
            SelectedSchemeParameterId = node.RefId;
            SelectedSchemeOperationId.clear();
        } else if (node.Kind == FGraphNode::EKind::Query || node.Kind == FGraphNode::EKind::Command) {
            SelectedSchemeOperationId = node.RefId;
        }
        InvokeSelectedSchemeOperation(*interfaceSchema, action.OperationId, context);
    };

    std::map<Slab::Str, FGraphNode *> nodesByKey;
    std::map<Slab::Str, Slab::Vector<FNodeAction>> nodeActionsByKey;
    std::map<Slab::Str, Slab::Vector<FActionRect>> nodeActionRectsByKey;
    Slab::Str hoveredBadgeLabel;
    Slab::Str hoveredBadgeDetail;

    const auto rebuildSelectedActionLayout = [&]() {
        nodeActionsByKey.clear();
        nodeActionRectsByKey.clear();
        for (auto &node : nodes) {
            if (!isNodeSelected(node)) continue;
            const auto actions = buildNodeActions(node);
            nodeActionsByKey[node.Key] = actions;
            nodeActionRectsByKey[node.Key] = buildActionRects(node, actions);
        }
    };

    for (auto &node : nodes) {
        nodesByKey[node.Key] = &node;
    }
    rebuildSelectedActionLayout();

    const auto &io = ImGui::GetIO();
    const auto mousePos = io.MousePos;
    const bool bLeftClicked = ImGui::IsMouseClicked(ImGuiMouseButton_Left);
    const bool bLeftDown = ImGui::IsMouseDown(ImGuiMouseButton_Left);
    const bool bLeftReleased = ImGui::IsMouseReleased(ImGuiMouseButton_Left);

    int hoveredNodeIndex = -1;
    for (int i = static_cast<int>(nodes.size()) - 1; i >= 0; --i) {
        const auto nodePos = screenPositionFor(nodes[static_cast<std::size_t>(i)]);
        const auto nodeEnd = ImVec2(nodePos.x + nodes[static_cast<std::size_t>(i)].Size.x, nodePos.y + nodes[static_cast<std::size_t>(i)].Size.y);
        if (isPointInside(mousePos, nodePos, nodeEnd)) {
            hoveredNodeIndex = i;
            break;
        }
    }

    Slab::Str hoveredActionNodeKey;
    int hoveredActionIndex = -1;
    if (hoveredNodeIndex >= 0) {
        const auto &hoveredNode = nodes[static_cast<std::size_t>(hoveredNodeIndex)];
        if (const auto rectsIt = nodeActionRectsByKey.find(hoveredNode.Key); rectsIt != nodeActionRectsByKey.end()) {
            const auto &rects = rectsIt->second;
            for (std::size_t idx = 0; idx < rects.size(); ++idx) {
                if (isPointInside(mousePos, rects[idx].Min, rects[idx].Max)) {
                    hoveredActionNodeKey = hoveredNode.Key;
                    hoveredActionIndex = static_cast<int>(idx);
                    break;
                }
            }
        }
    }

    if (bCanvasHovered && ImGui::IsMouseReleased(ImGuiMouseButton_Right)) {
        const auto clickPos = io.MouseClickedPos[ImGuiMouseButton_Right];
        const auto drag = ImVec2(mousePos.x - clickPos.x, mousePos.y - clickPos.y);
        const float dragDistance2 = drag.x * drag.x + drag.y * drag.y;
        if (dragDistance2 <= 36.0f) {
            ImGui::OpenPopup(PopupBlueprintGraphContext);
        }
    }

    const auto createBlueprintPlotWindow = [&]() -> FPlot2DWindowV2 * {
        ++BlueprintPlotWindowCreateCount;
        const auto baseTitle = interfaceSchema->DisplayName.empty()
            ? Slab::Str("Blueprint Plot")
            : interfaceSchema->DisplayName + " Plot";
        const auto windowTitle = baseTitle + " " + Slab::ToStr(BlueprintPlotWindowCreateCount);

        auto window = Slab::New<FPlot2DWindowV2>(
            windowTitle,
            Slab::Graphics::RectR{-1.0, 1.0, -1.0, 1.0},
            Slab::Graphics::RectI{0, 1024, 0, 768});
        window->SetAutoFitRanges(true);

        auto background = Slab::New<FBackgroundArtistV2>();
        auto axis = Slab::New<FAxisArtistV2>();
        window->AddArtist(background, -100);
        window->AddArtist(axis, -10);

        PlotWindowsV2.push_back(window);

        SelectedPlotInterfaceId = "v2.plot.window." + window->GetWindowId();
        SelectedPlotParameterId.clear();
        PlotsLastOperationSummary = "[Ok] Created Plot2D window '" + window->GetTitle() + "'.";
        PlotsLastOperationOutput.clear();

        return window.get();
    };

    const auto createPointArtist = [&](FPlot2DWindowV2 *window) {
        if (window == nullptr) return;

        ++BlueprintPlotArtistCreateCount;
        auto artist = Slab::New<FPointSetArtistV2>(
            Slab::Vector<Slab::Graphics::FPoint2D>{
                {-1.0, -0.4},
                {-0.2, 0.9},
                {0.6, -0.1},
                {1.2, 0.55}
            });
        artist->SetLabel("Blueprint Points " + Slab::ToStr(BlueprintPlotArtistCreateCount));

        window->AddArtist(artist, static_cast<int>(window->GetArtists().size()));
        window->FitRegionToArtists();

        SelectedPlotInterfaceId = "v2.plot.artist." + window->GetWindowId() + "." + artist->GetArtistId();
        SelectedPlotParameterId.clear();
        PlotsLastOperationSummary =
            "[Ok] Added point-set artist '" + artist->GetArtistId() + "' to '" + window->GetTitle() + "'.";
        PlotsLastOperationOutput.clear();
    };

    const auto createFunctionArtist = [&](FPlot2DWindowV2 *window) {
        if (window == nullptr) return;

        ++BlueprintPlotArtistCreateCount;
        auto artist = Slab::New<FRtoRFunctionArtistV2>(Slab::New<Slab::Math::RtoR::FSine>(1.0, 1.0));
        artist->SetSampleCount(512);
        artist->SetDomain(-6.28318530718, 6.28318530718);
        artist->SetLabel("Blueprint f(x) " + Slab::ToStr(BlueprintPlotArtistCreateCount));

        window->AddArtist(artist, static_cast<int>(window->GetArtists().size()));
        window->FitRegionToArtists();

        SelectedPlotInterfaceId = "v2.plot.artist." + window->GetWindowId() + "." + artist->GetArtistId();
        SelectedPlotParameterId.clear();
        PlotsLastOperationSummary =
            "[Ok] Added R->R function artist '" + artist->GetArtistId() + "' to '" + window->GetTitle() + "'.";
        PlotsLastOperationOutput.clear();
    };

    const auto drawWindowTargetMenu = [&](const char *menuLabel, const auto &createArtistAction) {
        if (!ImGui::BeginMenu(menuLabel)) return;

        auto windows = FPlot2DWindowV2::GetLiveWindows();
        if (windows.empty()) {
            if (ImGui::MenuItem("Create in New Plot Window")) {
                createArtistAction(createBlueprintPlotWindow());
            }
            ImGui::EndMenu();
            return;
        }

        for (auto *window : windows) {
            if (window == nullptr) continue;
            const auto itemLabel = window->GetTitle().empty()
                ? window->GetWindowId()
                : window->GetTitle() + " (" + window->GetWindowId() + ")";
            if (ImGui::MenuItem(itemLabel.c_str())) {
                createArtistAction(window);
            }
        }

        ImGui::Separator();
        if (ImGui::MenuItem("Create in New Plot Window")) {
            createArtistAction(createBlueprintPlotWindow());
        }

        ImGui::EndMenu();
    };

    if (ImGui::BeginPopup(PopupBlueprintGraphContext)) {
        if (hoveredNodeIndex >= 0) {
            const auto &hoveredNode = nodes[static_cast<std::size_t>(hoveredNodeIndex)];
            ImGui::TextDisabled("Blueprint Node: %s", hoveredNode.Title.c_str());
            ImGui::Separator();
        }

        if (ImGui::MenuItem("Create Plot Window")) {
            createBlueprintPlotWindow();
        }

        drawWindowTargetMenu("Create Point-Set Artist", createPointArtist);
        drawWindowTargetMenu("Create R->R Function Artist", createFunctionArtist);

        ImGui::EndPopup();
    }

    if (bLeftClicked) {
        if (hoveredActionIndex >= 0) {
            BlueprintPressedActionNodeKey = hoveredActionNodeKey;
            BlueprintPressedActionIndex = hoveredActionIndex;
            BlueprintPressedNodeKey.clear();
            bBlueprintNodeDragging = false;
        } else if (hoveredNodeIndex >= 0) {
            auto &pressedNode = nodes[static_cast<std::size_t>(hoveredNodeIndex)];
            BlueprintPressedNodeKey = pressedNode.Key;
            BlueprintPressMousePos = mousePos;
            if (pressedNode.GraphNode != nullptr) {
                BlueprintPressNodePos = ImVec2(pressedNode.GraphNode->Position.x, pressedNode.GraphNode->Position.y);
            } else {
                BlueprintPressNodePos = ImVec2(0.0f, 0.0f);
            }
            bBlueprintNodeDragging = false;
            BlueprintPressedActionNodeKey.clear();
            BlueprintPressedActionIndex = -1;
        } else {
            BlueprintPressedNodeKey.clear();
            BlueprintPressedActionNodeKey.clear();
            BlueprintPressedActionIndex = -1;
            bBlueprintNodeDragging = false;
        }
    }

    if (bLeftDown && !BlueprintPressedNodeKey.empty() && BlueprintPressedActionNodeKey.empty()) {
        if (const auto nodeIt = nodesByKey.find(BlueprintPressedNodeKey); nodeIt != nodesByKey.end()) {
            const auto delta = ImVec2(mousePos.x - BlueprintPressMousePos.x, mousePos.y - BlueprintPressMousePos.y);
            const float distance2 = delta.x * delta.x + delta.y * delta.y;
            if (!bBlueprintNodeDragging && distance2 > 9.0f) bBlueprintNodeDragging = true;
            if (bBlueprintNodeDragging && nodeIt->second->GraphNode != nullptr) {
                nodeIt->second->GraphNode->Position.x = BlueprintPressNodePos.x + delta.x;
                nodeIt->second->GraphNode->Position.y = BlueprintPressNodePos.y + delta.y;
                persistNodePosition(*nodeIt->second);
            }
        }
    }

    if (bLeftReleased) {
        if (!BlueprintPressedActionNodeKey.empty()) {
            if (BlueprintPressedActionNodeKey == hoveredActionNodeKey && BlueprintPressedActionIndex == hoveredActionIndex) {
                if (const auto nodeIt = nodesByKey.find(BlueprintPressedActionNodeKey); nodeIt != nodesByKey.end()) {
                    if (const auto actionsIt = nodeActionsByKey.find(BlueprintPressedActionNodeKey); actionsIt != nodeActionsByKey.end()) {
                        const auto &actions = actionsIt->second;
                        if (BlueprintPressedActionIndex >= 0 &&
                            static_cast<std::size_t>(BlueprintPressedActionIndex) < actions.size()) {
                            executeNodeAction(*nodeIt->second, actions[static_cast<std::size_t>(BlueprintPressedActionIndex)]);
                        }
                    }
                }
            }
            BlueprintPressedActionNodeKey.clear();
            BlueprintPressedActionIndex = -1;
        } else if (!BlueprintPressedNodeKey.empty()) {
            if (!bBlueprintNodeDragging) {
                if (const auto nodeIt = nodesByKey.find(BlueprintPressedNodeKey); nodeIt != nodesByKey.end()) {
                    const auto &node = *(nodeIt->second);
                    if (node.Kind == FGraphNode::EKind::Interface) {
                        SelectedSchemeInterfaceId = node.RefId;
                        SelectedSchemeOperationId.clear();
                    } else if (node.Kind == FGraphNode::EKind::Parameter) {
                        SelectedSchemeParameterId = node.RefId;
                        SelectedSchemeOperationId.clear();
                    } else if (node.Kind == FGraphNode::EKind::Query || node.Kind == FGraphNode::EKind::Command) {
                        SelectedSchemeOperationId = node.RefId;
                    }
                }
            }
            BlueprintPressedNodeKey.clear();
            bBlueprintNodeDragging = false;
        }
    }

    // Keep action-bar geometry in sync with post-drag/post-selection node positions this frame.
    rebuildSelectedActionLayout();

    for (const auto &node : nodes) {
        const auto nodePos = screenPositionFor(node);
        const auto nodeEnd = ImVec2(nodePos.x + node.Size.x, nodePos.y + node.Size.y);
        if (nodeEnd.x < canvasPos.x || nodePos.x > canvasEnd.x || nodeEnd.y < canvasPos.y || nodePos.y > canvasEnd.y) {
            continue;
        }

        ImU32 bodyColor = IM_COL32(38, 44, 55, 238);
        ImU32 headerColor = IM_COL32(92, 106, 132, 255);
        if (node.Kind == FGraphNode::EKind::Query) {
            bodyColor = IM_COL32(34, 48, 46, 238);
            headerColor = IM_COL32(78, 132, 120, 255);
        }
        if (node.Kind == FGraphNode::EKind::Command) {
            bodyColor = IM_COL32(52, 48, 36, 238);
            headerColor = IM_COL32(146, 124, 72, 255);
        }
        if (node.Kind == FGraphNode::EKind::Interface) {
            bodyColor = IM_COL32(44, 40, 58, 238);
            headerColor = IM_COL32(112, 94, 154, 255);
        }

        const float lineHeight = ImGui::GetTextLineHeight();
        const float headerHeight = std::max(BlueprintNodeHeaderMinHeight, std::round(lineHeight * BlueprintNodeHeaderScale));
        drawList->AddRectFilled(nodePos, nodeEnd, bodyColor, 8.0f);
        drawList->AddRectFilled(nodePos, ImVec2(nodeEnd.x, nodePos.y + headerHeight), headerColor, 8.0f, ImDrawFlags_RoundCornersTop);
        drawList->AddLine(
            ImVec2(nodePos.x + 1.0f, nodePos.y + headerHeight),
            ImVec2(nodeEnd.x - 1.0f, nodePos.y + headerHeight),
            IM_COL32(168, 178, 196, 72),
            1.0f);

        const bool bNodeSelected =
            (node.Kind == FGraphNode::EKind::Parameter && node.RefId == SelectedSchemeParameterId) ||
            ((node.Kind == FGraphNode::EKind::Query || node.Kind == FGraphNode::EKind::Command) &&
                node.RefId == SelectedSchemeOperationId);
        drawList->AddRect(nodePos, nodeEnd, bNodeSelected ? IM_COL32(245, 195, 110, 255) : IM_COL32(96, 106, 122, 255), 8.0f, 0, bNodeSelected ? 2.0f : 1.0f);
        const float contentWidth = std::max(120.0f, node.Size.x - 18.0f);
        const std::size_t titleChars = static_cast<std::size_t>(std::max(12.0f, std::floor(contentWidth / 8.3f)));
        const std::size_t subtitleChars = static_cast<std::size_t>(std::max(14.0f, std::floor(contentWidth / 8.0f)));
        drawList->AddText(
            ImVec2(nodePos.x + 9.0f, nodePos.y + 6.0f),
            IM_COL32(240, 244, 252, 255),
            TruncateLabel(node.Title, titleChars).c_str());
        drawList->AddText(
            ImVec2(nodePos.x + 9.0f, nodePos.y + headerHeight + 6.0f),
            IM_COL32(182, 192, 208, 255),
            TruncateLabel(node.SubtitlePrimary, subtitleChars).c_str());

        const float badgeRowHeight = ImGui::GetTextLineHeight() + 4.0f;
        const float badgeRowGap = BlueprintNodeBadgeRowGap;
        const float badgeRowStep = badgeRowHeight + badgeRowGap;
        int badgeRowCount = 0;
        float measureRowWidth = 0.0f;
        for (const auto &badge : node.Badges) {
            const float badgeWidth = MeasureBadgeWidth(badge.first);
            if (measureRowWidth > 0.0f && measureRowWidth + badgeWidth > node.Size.x - 18.0f) {
                ++badgeRowCount;
                measureRowWidth = 0.0f;
            }
            if (measureRowWidth <= 0.0f) ++badgeRowCount;
            measureRowWidth += badgeWidth;
        }

        const float badgeBlockHeight =
            static_cast<float>(badgeRowCount) * badgeRowHeight +
            static_cast<float>(std::max(0, badgeRowCount - 1)) * badgeRowGap;
        float badgeBottomY = nodeEnd.y - 10.0f;
        if (bNodeSelected) {
            if (const auto rectsIt = nodeActionRectsByKey.find(node.Key); rectsIt != nodeActionRectsByKey.end()) {
                if (!rectsIt->second.empty()) {
                    float actionTopY = rectsIt->second.front().Min.y;
                    for (const auto &rect : rectsIt->second) actionTopY = std::min(actionTopY, rect.Min.y);
                    badgeBottomY = std::min(badgeBottomY, actionTopY - BlueprintNodeActionGap);
                }
            }
        }
        const float badgeStartY = std::max(nodePos.y + headerHeight + BlueprintNodeBadgeTopOffset, badgeBottomY - badgeBlockHeight);

        float badgeX = nodePos.x + 9.0f;
        float badgeRowY = badgeStartY;
        const auto setHoveredBadgeTooltip = [&](const Slab::Str &badgeLabel, const FGraphNode &currentNode) {
            const auto separator = badgeLabel.find(':');
            const bool bHasPrefix = separator != Slab::Str::npos && separator > 0;
            const char prefix = bHasPrefix ? badgeLabel[0] : '\0';
            const Slab::Str value = bHasPrefix ? badgeLabel.substr(separator + 1) : badgeLabel;

            if (prefix == 'M') hoveredBadgeLabel = "Mutability = " + value;
            else if (prefix == 'E') hoveredBadgeLabel = "Exposure = " + value;
            else if (prefix == 'S') hoveredBadgeLabel = "State = " + value;
            else if (prefix == 'R') hoveredBadgeLabel = "Run State Policy = " + value;
            else if (prefix == 'T') hoveredBadgeLabel = "Thread Affinity = " + value;
            else if (prefix == 'X') hoveredBadgeLabel = "Side Effect Class = " + value;
            else if (prefix == 'I') hoveredBadgeLabel = "Required Inputs = " + value;
            else if (prefix == 'N') hoveredBadgeLabel = "Node Kind = " + value;
            else hoveredBadgeLabel = value;

            if (prefix == 'M' && value == "Restart") hoveredBadgeDetail = "RestartRequired: changes are staged and need Apply/Restart.";
            else if (prefix == 'M' && value == "Runtime") hoveredBadgeDetail = "RuntimeMutable: can be applied while runtime is active.";
            else if (prefix == 'M' && value == "Const") hoveredBadgeDetail = "Const mutability: runtime read-only.";
            else if (prefix == 'E' && value == "Writable") hoveredBadgeDetail = "WritableExposed: parameter can be edited via reflection.";
            else if (prefix == 'E' && value == "ReadOnly") hoveredBadgeDetail = "ReadOnlyExposed: visible but cannot be changed.";
            else if (prefix == 'E' && value == "Hidden") hoveredBadgeDetail = "Hidden: not intended for routine user editing.";
            else if (prefix == 'S' && value == "Live") hoveredBadgeDetail = "Draft matches current runtime value.";
            else if (prefix == 'S' && value == "Draft") hoveredBadgeDetail = "Draft differs from runtime value.";
            else if (prefix == 'S' && value == "Pending") hoveredBadgeDetail = "Staged value pending Apply/Restart.";
            else if (prefix == 'S' && value == "Unavailable") hoveredBadgeDetail = "Current value provider unavailable.";
            else if (prefix == 'R' && value == "Stopped") hoveredBadgeDetail = "Operation allowed only while runtime is stopped.";
            else if (prefix == 'R' && value == "Running") hoveredBadgeDetail = "Operation allowed only while runtime is running.";
            else if (prefix == 'R' && value == "Any") hoveredBadgeDetail = "No run-state restriction.";
            else if (prefix == 'T' && value == "UI") hoveredBadgeDetail = "Invocation must run on UI thread.";
            else if (prefix == 'T' && value == "Sim") hoveredBadgeDetail = "Invocation must run on simulation thread.";
            else if (prefix == 'T' && value == "Worker") hoveredBadgeDetail = "Invocation must run on worker thread.";
            else if (prefix == 'T' && value == "Any") hoveredBadgeDetail = "No thread-affinity restriction.";
            else if (prefix == 'X' && value == "Local") hoveredBadgeDetail = "Side effects are local state changes.";
            else if (prefix == 'X' && value == "Task") hoveredBadgeDetail = "Side effects include task lifecycle changes.";
            else if (prefix == 'X' && value == "IO") hoveredBadgeDetail = "Side effects include IO.";
            else if (prefix == 'X' && value == "External") hoveredBadgeDetail = "Side effects include external integrations.";
            else if (prefix == 'X' && value == "None") hoveredBadgeDetail = "No side effects declared.";
            else if (prefix == 'I') hoveredBadgeDetail = "Number of required operation inputs.";
            else if (prefix == 'N' && value == "Interface") hoveredBadgeDetail = "Interface root node.";
            else hoveredBadgeDetail = "Policy badge for " + currentNode.Title + ".";
        };
        for (const auto &badge : node.Badges) {
            const float badgeWidth = MeasureBadgeWidth(badge.first);
            if (badgeX > nodePos.x + 9.0f && badgeX + badgeWidth > nodeEnd.x - 9.0f) {
                badgeX = nodePos.x + 9.0f;
                badgeRowY += badgeRowStep;
            }
            if (badgeRowY + badgeRowHeight > badgeBottomY) break;
            const auto compactText = TruncateLabel(badge.first, 14);
            const auto compactWidth = ImGui::CalcTextSize(compactText.c_str()).x + 12.0f;
            const auto badgeMin = ImVec2(badgeX, badgeRowY);
            const auto badgeMax = ImVec2(badgeX + compactWidth, badgeRowY + badgeRowHeight);
            if (isPointInside(mousePos, badgeMin, badgeMax)) {
                setHoveredBadgeTooltip(badge.first, node);
            }
            badgeX += DrawNodeBadge(drawList, ImVec2(badgeX, badgeRowY), badge.first, badge.second);
        }

        if (bNodeSelected && node.Kind != FGraphNode::EKind::Interface) {
            if (const auto rectsIt = nodeActionRectsByKey.find(node.Key); rectsIt != nodeActionRectsByKey.end()) {
                if (const auto actionsIt = nodeActionsByKey.find(node.Key); actionsIt != nodeActionsByKey.end()) {
                    const auto &rects = rectsIt->second;
                    const auto &actions = actionsIt->second;
                    const std::size_t count = std::min(rects.size(), actions.size());
                    for (std::size_t idx = 0; idx < count; ++idx) {
                        const bool bHoveredAction =
                            hoveredActionIndex >= 0 &&
                            static_cast<std::size_t>(hoveredActionIndex) == idx &&
                            hoveredActionNodeKey == node.Key;
                        const bool bPressedAction =
                            bLeftDown &&
                            BlueprintPressedActionNodeKey == node.Key &&
                            BlueprintPressedActionIndex >= 0 &&
                            static_cast<std::size_t>(BlueprintPressedActionIndex) == idx;
                        const ImU32 fill = bPressedAction
                            ? IM_COL32(142, 152, 166, 230)
                            : (bHoveredAction ? IM_COL32(112, 122, 138, 220) : IM_COL32(92, 102, 118, 210));
                        drawList->AddRectFilled(rects[idx].Min, rects[idx].Max, fill, 4.0f);
                        drawList->AddRect(rects[idx].Min, rects[idx].Max, IM_COL32(180, 186, 198, 170), 4.0f, 0, 1.0f);
                        drawList->AddText(ImVec2(rects[idx].Min.x + 6.0f, rects[idx].Min.y + 2.0f), IM_COL32(236, 240, 248, 255), actions[idx].Label.c_str());
                    }
                }
            }
        }

        if (node.Kind == FGraphNode::EKind::Parameter) {
            const auto pin = pinPositionFor(node, true, 0, 1);
            drawList->AddCircleFilled(pin, 5.0f, IM_COL32(138, 196, 246, 255));
        } else if (node.Kind == FGraphNode::EKind::Query) {
            const auto socketCount = std::max(1, node.InputSocketCount);
            for (int socket = 0; socket < socketCount; ++socket) {
                const auto pin = pinPositionFor(node, false, socket, socketCount);
                drawList->AddCircleFilled(pin, 5.0f, IM_COL32(132, 220, 162, 255));
            }
        } else if (node.Kind == FGraphNode::EKind::Command) {
            const auto socketCount = std::max(1, node.InputSocketCount);
            for (int socket = 0; socket < socketCount; ++socket) {
                const auto pin = pinPositionFor(node, false, socket, socketCount);
                drawList->AddCircleFilled(pin, 5.0f, IM_COL32(226, 198, 132, 255));
            }
        } else {
            const auto leftPins = std::max(1, parameterCount);
            const auto rightPins = std::max(1, operationCount);
            for (int i = 0; i < leftPins; ++i) {
                drawList->AddCircleFilled(pinPositionFor(node, false, i, leftPins), 4.0f, IM_COL32(120, 170, 220, 230));
            }
            for (int i = 0; i < rightPins; ++i) {
                drawList->AddCircleFilled(pinPositionFor(node, true, i, rightPins), 4.0f, IM_COL32(120, 220, 160, 230));
            }
        }
    }

    drawList->PopClipRect();
    // Node interaction widgets move the ImGui cursor; explicitly restore flow below the canvas.
    ImGui::SetCursorScreenPos(ImVec2(canvasPos.x, canvasEnd.y));

    ImGui::InvisibleButton("SchemesBlueprintTraceSplitter", ImVec2(canvasSize.x, SplitterHeight), ImGuiButtonFlags_MouseButtonLeft);
    const bool bSplitterHovered = ImGui::IsItemHovered();
    const bool bSplitterActive = ImGui::IsItemActive();
    if (bSplitterHovered || bSplitterActive) {
        ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNS);
    }
    if (bSplitterActive && ImGui::IsMouseDragging(ImGuiMouseButton_Left, 0.0f)) {
        const auto &dragIo = ImGui::GetIO();
        BlueprintGraphTraceHeight = std::clamp(
            BlueprintGraphTraceHeight - dragIo.MouseDelta.y,
            GraphTraceMinHeight,
            GraphTraceMaxHeight);
    }

    const Slab::Str &tooltipLabel = !hoveredBadgeLabel.empty() ? hoveredBadgeLabel : hoveredLegendLabel;
    const Slab::Str &tooltipDetail = !hoveredBadgeLabel.empty() ? hoveredBadgeDetail : hoveredLegendDetail;
    if (!tooltipLabel.empty()) {
        ImGui::SetNextWindowSizeConstraints(ImVec2(280.0f, 0.0f), ImVec2(420.0f, FLT_MAX));
        ImGui::BeginTooltip();
        ImGui::TextUnformatted(tooltipLabel.c_str());
        if (!tooltipDetail.empty()) {
            ImGui::Separator();
            ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + 360.0f);
            ImGui::TextWrapped("%s", tooltipDetail.c_str());
            ImGui::PopTextWrapPos();
        }
        ImGui::EndTooltip();
    }

    if constexpr (CEnableBlueprintGraphTuningUi) {
    if (bShowBlueprintTuningWindow) {
        ImGui::SetNextWindowBgAlpha(0.94f);
        ImGui::SetNextWindowSize(ImVec2(380.0f, 0.0f), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowPos(ImVec2(canvasPos.x + 20.0f, canvasPos.y + 20.0f), ImGuiCond_FirstUseEver);
        constexpr auto tuningFlags =
            ImGuiWindowFlags_NoDocking |
            ImGuiWindowFlags_AlwaysAutoResize;
        if (ImGui::Begin("Graph Tuning (Temporary)", &bShowBlueprintTuningWindow, tuningFlags)) {
            const auto sliderWithReset = [](const char *id,
                                            const char *label,
                                            float *value,
                                            const float minValue,
                                            const float maxValue,
                                            const float defaultValue) {
                ImGui::PushID(id);
                ImGui::SetNextItemWidth(220.0f);
                ImGui::SliderFloat(label, value, minValue, maxValue, "%.2f");
                ImGui::SameLine();
                if (ImGui::SmallButton("Reset")) *value = defaultValue;
                ImGui::PopID();
            };

            ImGui::SeparatorText("Node");
            sliderWithReset("node_header_scale", "Header Scale", &BlueprintNodeHeaderScale, 1.2f, 2.2f, 1.55f);
            sliderWithReset("node_header_min_h", "Header Min Height", &BlueprintNodeHeaderMinHeight, 28.0f, 72.0f, 36.0f);
            sliderWithReset("node_body_top", "Body Top Padding", &BlueprintNodeBodyTopPadding, 2.0f, 20.0f, 8.0f);
            sliderWithReset("node_body_bottom", "Body Bottom Padding", &BlueprintNodeBodyBottomPadding, 2.0f, 24.0f, 10.0f);
            sliderWithReset("node_badge_gap", "Badge Row Gap", &BlueprintNodeBadgeRowGap, 0.0f, 12.0f, 4.0f);
            sliderWithReset("node_action_reserve", "Action Reserve", &BlueprintNodeActionReserve, 6.0f, 36.0f, 14.0f);
            sliderWithReset("node_badge_top", "Badge Top Offset", &BlueprintNodeBadgeTopOffset, 22.0f, 90.0f, 42.0f);
            sliderWithReset("node_action_gap", "Action Gap", &BlueprintNodeActionGap, 2.0f, 24.0f, 10.0f);

            ImGui::SeparatorText("Legend");
            sliderWithReset("legend_padding", "Legend Padding", &BlueprintLegendPadding, 4.0f, 18.0f, 14.0f);
            sliderWithReset("legend_row_gap", "Legend Row Gap", &BlueprintLegendRowGap, 0.0f, 16.0f, 6.0f);
            sliderWithReset("legend_item_gap", "Legend Item Gap", &BlueprintLegendItemGap, 2.0f, 20.0f, 8.0f);
            sliderWithReset("legend_chip_extra", "Legend Chip Extra H", &BlueprintLegendChipExtraHeight, 6.0f, 24.0f, 12.0f);
            sliderWithReset("legend_marker_inset", "Legend Marker Inset", &BlueprintLegendMarkerInset, 1.0f, 12.0f, 4.0f);
        }
        ImGui::End();
    }
    }

    ImGui::SeparatorText("Graph Trace");
    ImGui::BeginChild("SchemesBlueprintTrace", ImVec2(0.0f, BlueprintGraphTraceHeight), true);
    if (SchemesOperationTrace.empty()) {
        ImGui::TextDisabled("No operation invocations yet.");
    } else {
        if (ImGui::Button("Clear")) {
            SchemesOperationTrace.clear();
        }
        for (const auto &entry : SchemesOperationTrace) {
            ImGui::PushID(static_cast<int>(entry.SequenceId));
            ImGui::TextColored(
                entry.bOk ? ImVec4(0.45f, 0.86f, 0.56f, 1.0f) : ImVec4(0.93f, 0.40f, 0.40f, 1.0f),
                "#%zu",
                entry.SequenceId);
            ImGui::SameLine();
            ImGui::Text("%s :: %s", entry.InterfaceId.c_str(), entry.OperationId.c_str());
            if (!entry.LatencyLabel.empty()) {
                ImGui::SameLine();
                ImGui::TextDisabled("(%s)", entry.LatencyLabel.c_str());
            }
            ImGui::TextDisabled("%s", TruncateLabel(entry.Summary, 120).c_str());
            ImGui::PopID();
        }
    }
    ImGui::EndChild();
}

auto FLabV2WindowManager::MarkGraphPlaygroundDirty() -> void {
    bPlaygroundDirty = true;
}

auto FLabV2WindowManager::SaveGraphPlaygroundStateToFile() -> bool {
    if (PlaygroundPersistenceFilePath.empty()) {
        PlaygroundPersistenceStatus = "[Error] Graph state path is empty.";
        return false;
    }

    namespace json = crude_json;
    json::value root(json::object{});
    root["schema_version"] = 1.0;
    root["workspace"] = "GraphPlayground";

    json::value templateState(json::object{});
    templateState["selected_operator"] = PlaygroundTemplateSelectedOperatorId;
    templateState["show_grid"] = PlaygroundTemplateDocument.Canvas.bShowGrid;
    templateState["show_minimap"] = PlaygroundTemplateDocument.Canvas.bShowMinimap;
    templateState["pan"] = JsonWriteVec2(ImVec2(
        PlaygroundTemplateDocument.Canvas.PanX,
        PlaygroundTemplateDocument.Canvas.PanY));
    templateState["node_counter"] = static_cast<double>(PlaygroundTemplateNodeCounter);
    templateState["edge_counter"] = static_cast<double>(PlaygroundTemplateEdgeCounter);
    templateState["coercion_counter"] = static_cast<double>(PlaygroundTemplateCoercionCounter);

    json::array templateNodes;
    templateNodes.reserve(PlaygroundTemplateDocument.Nodes.size());
    for (const auto &node : PlaygroundTemplateDocument.Nodes) {
        json::value nodeValue(json::object{});
        nodeValue["id"] = node.NodeId;
        nodeValue["operator_id"] = node.SemanticOperatorId;
        nodeValue["position"] = JsonWriteVec2(ImVec2(node.Position.x, node.Position.y));
        templateNodes.push_back(std::move(nodeValue));
    }
    templateState["nodes"] = json::value(std::move(templateNodes));

    json::array templateEdges;
    templateEdges.reserve(PlaygroundTemplateDocument.Edges.size());
    for (const auto &edge : PlaygroundTemplateDocument.Edges) {
        json::value edgeValue(json::object{});
        edgeValue["id"] = edge.EdgeId;
        edgeValue["from_node_id"] = edge.FromNodeId;
        edgeValue["from_port_id"] = edge.FromPortId;
        edgeValue["to_node_id"] = edge.ToNodeId;
        edgeValue["to_port_id"] = edge.ToPortId;
        edgeValue["match_reason"] = edge.MatchReason;

        json::array diagnostics;
        diagnostics.reserve(edge.Diagnostics.size());
        for (const auto &diagnostic : edge.Diagnostics) {
            diagnostics.emplace_back(diagnostic);
        }
        edgeValue["diagnostics"] = json::value(std::move(diagnostics));

        json::array suggestions;
        suggestions.reserve(edge.SuggestedCoercionOperatorIds.size());
        for (const auto &suggestion : edge.SuggestedCoercionOperatorIds) {
            suggestions.emplace_back(suggestion);
        }
        edgeValue["suggested_coercions"] = json::value(std::move(suggestions));
        templateEdges.push_back(std::move(edgeValue));
    }
    templateState["edges"] = json::value(std::move(templateEdges));

    json::array selectedNodes;
    selectedNodes.reserve(PlaygroundTemplateSelectedNodeIds.size());
    for (const auto &selectedNodeId : PlaygroundTemplateSelectedNodeIds) {
        selectedNodes.emplace_back(selectedNodeId);
    }
    templateState["selected_nodes"] = json::value(std::move(selectedNodes));
    templateState["selected_node"] = PlaygroundTemplateSelectedNodeId;
    templateState["selected_edge"] = PlaygroundTemplateSelectedEdgeId;

    root["template"] = std::move(templateState);

    json::value routingState(json::object{});
    routingState["edge_counter"] = static_cast<double>(PlaygroundRoutingEdgeCounter);
    routingState["source_endpoint"] = PlaygroundRoutingSourceEndpoint;
    routingState["target_endpoint"] = PlaygroundRoutingTargetEndpoint;
    routingState["edge_kind"] = static_cast<double>(static_cast<int>(PlaygroundRoutingEdgeKind));

    json::array routingEdges;
    routingEdges.reserve(PlaygroundRoutingDocument.Edges.size());
    for (const auto &edge : PlaygroundRoutingDocument.Edges) {
        json::value edgeValue(json::object{});
        edgeValue["id"] = edge.EdgeId;
        edgeValue["source"] = edge.FromNodeId;
        edgeValue["target"] = edge.ToNodeId;
        edgeValue["kind"] = static_cast<double>(static_cast<int>(edge.Kind));
        routingEdges.push_back(std::move(edgeValue));
    }
    routingState["edges"] = json::value(std::move(routingEdges));
    root["routing"] = std::move(routingState);

    try {
        const std::filesystem::path path(PlaygroundPersistenceFilePath);
        if (path.has_parent_path()) {
            std::filesystem::create_directories(path.parent_path());
        }
    } catch (const std::exception &exception) {
        PlaygroundPersistenceStatus = "[Error] Could not create folder for graph state: " + Slab::Str(exception.what());
        return false;
    }

    if (!root.save(PlaygroundPersistenceFilePath, 2, ' ')) {
        PlaygroundPersistenceStatus = "[Error] Failed to save graph state at '" + PlaygroundPersistenceFilePath + "'.";
        return false;
    }

    bPlaygroundDirty = false;
    PlaygroundLastAutosaveTimestampSeconds = ImGui::GetTime();
    PlaygroundPersistenceStatus = "[Ok] Saved graph state to '" + PlaygroundPersistenceFilePath + "'.";
    return true;
}

auto FLabV2WindowManager::LoadGraphPlaygroundStateFromFile() -> bool {
    if (PlaygroundPersistenceFilePath.empty()) {
        PlaygroundPersistenceStatus = "[Error] Graph state path is empty.";
        return false;
    }

    namespace json = crude_json;
    const std::filesystem::path path(PlaygroundPersistenceFilePath);
    if (!std::filesystem::exists(path)) {
        PlaygroundPersistenceStatus = "[Info] Graph state file not found; starting with empty graphs.";
        return false;
    }

    const auto [loadedRoot, ok] = json::value::load(PlaygroundPersistenceFilePath);
    if (!ok || !loadedRoot.is_object()) {
        PlaygroundPersistenceStatus = "[Error] Failed to parse graph state file '" + PlaygroundPersistenceFilePath + "'.";
        return false;
    }

    PlaygroundTemplateDocument.Nodes.clear();
    PlaygroundTemplateDocument.Edges.clear();
    PlaygroundTemplateSelectedNodeIds.clear();
    PlaygroundTemplateSelectedNodeId.clear();
    PlaygroundTemplateSelectedEdgeId.clear();
    PlaygroundTemplatePendingCoercion = {};
    PlaygroundTemplateConnectingNodeId.clear();
    PlaygroundTemplateConnectingPortId.clear();
    bPlaygroundTemplateConnectingFromOutput = false;
    bPlaygroundTemplateMarqueeSelecting = false;
    PlaygroundTemplateMarqueeStart = ImVec2(0.0f, 0.0f);
    PlaygroundTemplateMarqueeEnd = ImVec2(0.0f, 0.0f);

    PlaygroundRoutingDocument.Edges.clear();

    if (const auto *templateState = JsonTryGetValue(loadedRoot, "template"); templateState != nullptr) {
        PlaygroundTemplateSelectedOperatorId = JsonReadString(*templateState, "selected_operator", PlaygroundTemplateSelectedOperatorId);
        PlaygroundTemplateDocument.Canvas.bShowGrid = JsonReadBool(*templateState, "show_grid", PlaygroundTemplateDocument.Canvas.bShowGrid);
        PlaygroundTemplateDocument.Canvas.bShowMinimap = JsonReadBool(*templateState, "show_minimap", PlaygroundTemplateDocument.Canvas.bShowMinimap);
        const auto pan = JsonReadVec2(
            *templateState,
            "pan",
            ImVec2(
                PlaygroundTemplateDocument.Canvas.PanX,
                PlaygroundTemplateDocument.Canvas.PanY));
        PlaygroundTemplateDocument.Canvas.PanX = pan.x;
        PlaygroundTemplateDocument.Canvas.PanY = pan.y;

        if (const auto *nodesValue = JsonTryGetValue(*templateState, "nodes");
            nodesValue != nullptr && nodesValue->is_array()) {
            for (const auto &nodeValue : nodesValue->get<json::array>()) {
                if (!nodeValue.is_object()) continue;

                Slab::Core::Reflection::V2::FGraphNodeV2 node;
                node.NodeId = JsonReadString(nodeValue, "id");
                node.SemanticOperatorId = JsonReadString(nodeValue, "operator_id");
                const auto position = JsonReadVec2(
                    nodeValue,
                    "position",
                    ImVec2(node.Position.x, node.Position.y));
                node.Position.x = position.x;
                node.Position.y = position.y;
                if (node.NodeId.empty() || node.SemanticOperatorId.empty()) continue;
                PlaygroundTemplateDocument.Nodes.push_back(std::move(node));
            }
        }

        if (const auto *edgesValue = JsonTryGetValue(*templateState, "edges");
            edgesValue != nullptr && edgesValue->is_array()) {
            for (const auto &edgeValue : edgesValue->get<json::array>()) {
                if (!edgeValue.is_object()) continue;

                Slab::Core::Reflection::V2::FGraphEdgeV2 edge;
                edge.EdgeId = JsonReadString(edgeValue, "id");
                edge.FromNodeId = JsonReadString(edgeValue, "from_node_id");
                edge.FromPortId = JsonReadString(edgeValue, "from_port_id");
                edge.ToNodeId = JsonReadString(edgeValue, "to_node_id");
                edge.ToPortId = JsonReadString(edgeValue, "to_port_id");
                edge.MatchReason = JsonReadString(edgeValue, "match_reason", "ExactMatch");
                if (edge.EdgeId.empty()) edge.EdgeId = "tmpl.edge." + Slab::ToStr(PlaygroundTemplateDocument.Edges.size() + 1);
                if (edge.FromNodeId.empty() || edge.ToNodeId.empty() || edge.FromPortId.empty() || edge.ToPortId.empty()) continue;

                if (const auto *diagnosticsValue = JsonTryGetValue(edgeValue, "diagnostics");
                    diagnosticsValue != nullptr && diagnosticsValue->is_array()) {
                    for (const auto &diagnosticValue : diagnosticsValue->get<json::array>()) {
                        const auto *diagnostic = diagnosticValue.get_ptr<json::string>();
                        if (diagnostic == nullptr || diagnostic->empty()) continue;
                        edge.Diagnostics.push_back(*diagnostic);
                    }
                }

                if (const auto *suggestionsValue = JsonTryGetValue(edgeValue, "suggested_coercions");
                    suggestionsValue != nullptr && suggestionsValue->is_array()) {
                    for (const auto &suggestionValue : suggestionsValue->get<json::array>()) {
                        const auto *suggestion = suggestionValue.get_ptr<json::string>();
                        if (suggestion == nullptr || suggestion->empty()) continue;
                        edge.SuggestedCoercionOperatorIds.push_back(*suggestion);
                    }
                }

                PlaygroundTemplateDocument.Edges.push_back(std::move(edge));
            }
        }

        if (const auto *selectedNodesValue = JsonTryGetValue(*templateState, "selected_nodes");
            selectedNodesValue != nullptr && selectedNodesValue->is_array()) {
            for (const auto &selectedNodeValue : selectedNodesValue->get<json::array>()) {
                const auto *selectedNodeId = selectedNodeValue.get_ptr<json::string>();
                if (selectedNodeId == nullptr || selectedNodeId->empty()) continue;
                PlaygroundTemplateSelectedNodeIds.push_back(*selectedNodeId);
            }
        }
        PlaygroundTemplateSelectedNodeId = JsonReadString(*templateState, "selected_node");
        PlaygroundTemplateSelectedEdgeId = JsonReadString(*templateState, "selected_edge");

        PlaygroundTemplateNodeCounter = std::max(
            PlaygroundTemplateDocument.Nodes.size(),
            JsonReadUSize(*templateState, "node_counter", PlaygroundTemplateDocument.Nodes.size()));
        PlaygroundTemplateEdgeCounter = std::max(
            PlaygroundTemplateDocument.Edges.size(),
            JsonReadUSize(*templateState, "edge_counter", PlaygroundTemplateDocument.Edges.size()));
        PlaygroundTemplateCoercionCounter = JsonReadUSize(
            *templateState,
            "coercion_counter",
            PlaygroundTemplateCoercionCounter);
    } else {
        PlaygroundTemplateNodeCounter = PlaygroundTemplateDocument.Nodes.size();
        PlaygroundTemplateEdgeCounter = PlaygroundTemplateDocument.Edges.size();
    }

    if (const auto *routingState = JsonTryGetValue(loadedRoot, "routing"); routingState != nullptr) {
        PlaygroundRoutingSourceEndpoint = JsonReadString(*routingState, "source_endpoint");
        PlaygroundRoutingTargetEndpoint = JsonReadString(*routingState, "target_endpoint");
        const auto edgeKind = static_cast<int>(JsonReadDouble(
            *routingState,
            "edge_kind",
            static_cast<double>(static_cast<int>(PlaygroundRoutingEdgeKind))));
        PlaygroundRoutingEdgeKind = static_cast<Slab::Core::Reflection::V2::EGraphEdgeKindV2>(std::clamp(edgeKind, 0, 3));

        if (const auto *edgesValue = JsonTryGetValue(*routingState, "edges");
            edgesValue != nullptr && edgesValue->is_array()) {
            for (const auto &edgeValue : edgesValue->get<json::array>()) {
                if (!edgeValue.is_object()) continue;

                Slab::Core::Reflection::V2::FGraphEdgeV2 edge;
                edge.EdgeId = JsonReadString(edgeValue, "id");
                edge.FromNodeId = JsonReadString(edgeValue, "source");
                edge.ToNodeId = JsonReadString(edgeValue, "target");
                const auto kind = static_cast<int>(JsonReadDouble(edgeValue, "kind", 0.0));
                edge.Kind = static_cast<Slab::Core::Reflection::V2::EGraphEdgeKindV2>(std::clamp(kind, 0, 3));
                if (edge.EdgeId.empty()) edge.EdgeId = "route.edge." + Slab::ToStr(PlaygroundRoutingDocument.Edges.size() + 1);
                if (edge.FromNodeId.empty() || edge.ToNodeId.empty()) continue;
                PlaygroundRoutingDocument.Edges.push_back(std::move(edge));
            }
        }

        PlaygroundRoutingEdgeCounter = std::max(
            PlaygroundRoutingDocument.Edges.size(),
            JsonReadUSize(*routingState, "edge_counter", PlaygroundRoutingDocument.Edges.size()));
    } else {
        PlaygroundRoutingEdgeCounter = PlaygroundRoutingDocument.Edges.size();
    }

    bPlaygroundDirty = false;
    PlaygroundLastAutosaveTimestampSeconds = ImGui::GetTime();
    PlaygroundPersistenceStatus =
        "[Ok] Loaded graph state: " +
        Slab::ToStr(PlaygroundTemplateDocument.Nodes.size()) + " template nodes, " +
        Slab::ToStr(PlaygroundTemplateDocument.Edges.size()) + " template edges, " +
        Slab::ToStr(PlaygroundRoutingDocument.Edges.size()) + " routing edges.";
    return true;
}

auto FLabV2WindowManager::DrawGraphPlaygroundPanel() -> void {
    using namespace Slab::Core::Reflection::V2;

    InitSemanticLayerV1();
    auto &semanticCatalog = GetSemanticCatalogV1();
    const auto semanticSpaces = semanticCatalog.ListSpaces();
    const auto semanticOperators = semanticCatalog.ListOperators();

    if (!bPlaygroundStateLoaded) {
        bPlaygroundStateLoaded = true;
        (void) LoadGraphPlaygroundStateFromFile();
    }

    if (bPlaygroundAutosave && bPlaygroundDirty) {
        const auto now = ImGui::GetTime();
        if (now - PlaygroundLastAutosaveTimestampSeconds >= PlaygroundAutosaveIntervalSeconds) {
            if (SaveGraphPlaygroundStateToFile()) {
                PlaygroundPersistenceStatus = "[Ok] Autosaved graph state.";
            }
        }
    }

    {
        auto pathBuffer = std::vector<char>(512, '\0');
        if (!PlaygroundPersistenceFilePath.empty()) {
            std::strncpy(pathBuffer.data(), PlaygroundPersistenceFilePath.c_str(), pathBuffer.size() - 1);
            pathBuffer.back() = '\0';
        }

        ImGui::TextDisabled("State");
        ImGui::SameLine();
        ImGui::PushItemWidth(360.0f);
        if (ImGui::InputTextWithHint(
                "##GraphPlaygroundStatePath",
                "Graph state path...",
                pathBuffer.data(),
                pathBuffer.size())) {
            PlaygroundPersistenceFilePath = pathBuffer.data();
        }
        ImGui::PopItemWidth();
        ImGui::SameLine();
        if (ImGui::Button("Load")) {
            (void) LoadGraphPlaygroundStateFromFile();
        }
        ImGui::SameLine();
        if (ImGui::Button("Save")) {
            (void) SaveGraphPlaygroundStateToFile();
        }
        ImGui::SameLine();
        ImGui::Checkbox("Autosave", &bPlaygroundAutosave);
        if (bPlaygroundAutosave) {
            ImGui::SameLine();
            ImGui::SetNextItemWidth(120.0f);
            auto autosaveSeconds = static_cast<float>(PlaygroundAutosaveIntervalSeconds);
            ImGui::SliderFloat(
                "Interval (s)",
                &autosaveSeconds,
                0.35f,
                5.0f,
                "%.2f");
            PlaygroundAutosaveIntervalSeconds = std::max(0.25, static_cast<double>(autosaveSeconds));
        }

        if (!PlaygroundPersistenceStatus.empty()) {
            const bool bError = PlaygroundPersistenceStatus.starts_with("[Error]");
            const auto color = bError
                ? ImVec4(0.92f, 0.35f, 0.35f, 1.0f)
                : ImVec4(0.56f, 0.76f, 0.92f, 1.0f);
            ImGui::TextColored(color, "%s", PlaygroundPersistenceStatus.c_str());
        }
    }

    ImGui::Separator();

    if (PlaygroundTemplateSelectedOperatorId.empty()) {
        PlaygroundTemplateSelectedOperatorId = COperatorIdIdentityV1;
    }
    if (semanticCatalog.FindOperatorById(PlaygroundTemplateSelectedOperatorId) == nullptr && !semanticOperators.empty()) {
        PlaygroundTemplateSelectedOperatorId = semanticOperators.front()->OperatorId;
    }

    struct FSpaceMatchResult {
        Slab::Str Reason = "Incompatible";
        Slab::StrVector Diagnostics;
        Slab::Vector<Slab::Str> SuggestedCoercionOperatorIds;
    };

    struct FTemplateCompilePreviewNode {
        Slab::Str NodeId;
        Slab::Str OperatorId;
        Slab::Str DisplayName;
        bool bBound = false;
        Slab::Str BindingLabel;
        Slab::StrVector Diagnostics;
    };

    struct FTemplateCompilePreviewEdge {
        Slab::Str EdgeId;
        Slab::Str MatchReason;
        Slab::Str Summary;
        Slab::StrVector Diagnostics;
        Slab::Vector<Slab::Str> SuggestedCoercionOperatorIds;
    };

    struct FTemplateCompilePreview {
        Slab::Vector<FTemplateCompilePreviewNode> Nodes;
        Slab::Vector<FTemplateCompilePreviewEdge> Edges;
        std::size_t BoundNodes = 0;
        std::size_t UnboundNodes = 0;
        std::size_t ExactEdges = 0;
        std::size_t CoercionEdges = 0;
        std::size_t IncompatibleEdges = 0;
    };

    const auto findTemplateNode = [this](const Slab::Str &nodeId) -> FGraphNodeV2 * {
        const auto it = std::find_if(PlaygroundTemplateDocument.Nodes.begin(), PlaygroundTemplateDocument.Nodes.end(), [&](const auto &node) {
            return node.NodeId == nodeId;
        });
        if (it == PlaygroundTemplateDocument.Nodes.end()) return nullptr;
        return &(*it);
    };

    const auto findTemplateNodeConst = [this](const Slab::Str &nodeId) -> const FGraphNodeV2 * {
        const auto it = std::find_if(PlaygroundTemplateDocument.Nodes.begin(), PlaygroundTemplateDocument.Nodes.end(), [&](const auto &node) {
            return node.NodeId == nodeId;
        });
        if (it == PlaygroundTemplateDocument.Nodes.end()) return nullptr;
        return &(*it);
    };

    const auto findTemplateEdgeConst = [this](const Slab::Str &edgeId) -> const FGraphEdgeV2 * {
        const auto it = std::find_if(PlaygroundTemplateDocument.Edges.begin(), PlaygroundTemplateDocument.Edges.end(), [&](const auto &edge) {
            return edge.EdgeId == edgeId;
        });
        if (it == PlaygroundTemplateDocument.Edges.end()) return nullptr;
        return &(*it);
    };

    const auto findInputPort = [](const FSemanticOperatorSchemaV1 &semanticOperator, const Slab::Str &portId)
        -> const FSemanticPortSchemaV1 * {
        const auto it = std::find_if(semanticOperator.DomainPorts.begin(), semanticOperator.DomainPorts.end(), [&](const auto &port) {
            return port.PortId == portId;
        });
        if (it == semanticOperator.DomainPorts.end()) return nullptr;
        return &(*it);
    };

    const auto findOutputPort = [](const FSemanticOperatorSchemaV1 &semanticOperator, const Slab::Str &portId)
        -> const FSemanticPortSchemaV1 * {
        const auto it = std::find_if(semanticOperator.CodomainPorts.begin(), semanticOperator.CodomainPorts.end(), [&](const auto &port) {
            return port.PortId == portId;
        });
        if (it == semanticOperator.CodomainPorts.end()) return nullptr;
        return &(*it);
    };

    const auto appendUniqueString = [](Slab::Vector<Slab::Str> &target, const Slab::Str &item) {
        if (item.empty()) return;
        if (std::find(target.begin(), target.end(), item) != target.end()) return;
        target.push_back(item);
    };

    const auto appendUniqueNodeSelection = [this](const Slab::Str &nodeId) {
        if (nodeId.empty()) return;
        if (std::find(PlaygroundTemplateSelectedNodeIds.begin(), PlaygroundTemplateSelectedNodeIds.end(), nodeId)
            != PlaygroundTemplateSelectedNodeIds.end()) {
            return;
        }
        PlaygroundTemplateSelectedNodeIds.push_back(nodeId);
    };

    const auto markDirty = [this]() {
        MarkGraphPlaygroundDirty();
    };

    const auto clearTemplateSelection = [this]() {
        PlaygroundTemplateSelectedNodeId.clear();
        PlaygroundTemplateSelectedNodeIds.clear();
        PlaygroundTemplateSelectedEdgeId.clear();
    };

    const auto clearTemplateConnectionState = [this]() {
        PlaygroundTemplateConnectingNodeId.clear();
        PlaygroundTemplateConnectingPortId.clear();
        bPlaygroundTemplateConnectingFromOutput = false;
    };

    const auto deleteTemplateNodes = [this, &markDirty, &clearTemplateSelection, &clearTemplateConnectionState](const Slab::Vector<Slab::Str> &nodeIds) -> bool {
        if (nodeIds.empty()) return false;

        const auto containsNodeId = [&nodeIds](const Slab::Str &nodeId) {
            return std::find(nodeIds.begin(), nodeIds.end(), nodeId) != nodeIds.end();
        };

        const auto previousNodeCount = PlaygroundTemplateDocument.Nodes.size();
        const auto previousEdgeCount = PlaygroundTemplateDocument.Edges.size();

        PlaygroundTemplateDocument.Nodes.erase(
            std::remove_if(
                PlaygroundTemplateDocument.Nodes.begin(),
                PlaygroundTemplateDocument.Nodes.end(),
                [&](const auto &node) { return containsNodeId(node.NodeId); }),
            PlaygroundTemplateDocument.Nodes.end());

        PlaygroundTemplateDocument.Edges.erase(
            std::remove_if(
                PlaygroundTemplateDocument.Edges.begin(),
                PlaygroundTemplateDocument.Edges.end(),
                [&](const auto &edge) {
                    return containsNodeId(edge.FromNodeId) || containsNodeId(edge.ToNodeId);
                }),
            PlaygroundTemplateDocument.Edges.end());

        const bool bChanged =
            previousNodeCount != PlaygroundTemplateDocument.Nodes.size() ||
            previousEdgeCount != PlaygroundTemplateDocument.Edges.size();

        if (!bChanged) return false;

        PlaygroundTemplatePendingCoercion = {};
        clearTemplateSelection();
        clearTemplateConnectionState();
        markDirty();
        return true;
    };

    const auto evaluateSpaceMatch = [&](const Slab::Str &fromSpaceId, const Slab::Str &toSpaceId) -> FSpaceMatchResult {
        FSpaceMatchResult result;
        if (fromSpaceId.empty() || toSpaceId.empty()) {
            result.Reason = "Incompatible";
            result.Diagnostics = {"space id is empty"};
            return result;
        }
        if (fromSpaceId == toSpaceId) {
            result.Reason = "ExactMatch";
            return result;
        }

        bool bCompatibleWithCoercion = false;
        const auto matchResults = semanticCatalog.QueryOperatorMatchesForSignature({fromSpaceId}, {toSpaceId});
        for (const auto &match : matchResults) {
            if (match.Reason == ESignatureMatchReasonV1::ExactMatch) {
                const auto *operatorSchema = semanticCatalog.FindOperatorById(match.OperatorId);
                if (operatorSchema == nullptr) continue;
                if (operatorSchema->Kind == ESemanticOperatorKindV1::Coercion ||
                    operatorSchema->Kind == ESemanticOperatorKindV1::Projection) {
                    appendUniqueString(result.SuggestedCoercionOperatorIds, operatorSchema->OperatorId);
                    bCompatibleWithCoercion = true;
                }
                continue;
            }

            if (match.Reason != ESignatureMatchReasonV1::CompatibleButNeedsCoercion) continue;
            bCompatibleWithCoercion = true;
            for (const auto &diagnostic : match.Diagnostics) {
                appendUniqueString(result.Diagnostics, diagnostic);
            }
            for (const auto &suggestion : match.SuggestedCoercionOperatorIds) {
                appendUniqueString(result.SuggestedCoercionOperatorIds, suggestion);
            }
        }

        if (bCompatibleWithCoercion) {
            result.Reason = "CompatibleButNeedsCoercion";
            if (result.Diagnostics.empty()) {
                result.Diagnostics.push_back("coercion required: '" + fromSpaceId + "' -> '" + toSpaceId + "'");
            }
            return result;
        }

        result.Reason = "Incompatible";
        if (result.Diagnostics.empty()) {
            result.Diagnostics.push_back("incompatible spaces: '" + fromSpaceId + "' -> '" + toSpaceId + "'");
        }
        return result;
    };

    const auto addTemplateEdge = [this,
                                  &markDirty](const Slab::Str &fromNodeId,
                                              const Slab::Str &fromPortId,
                                              const Slab::Str &toNodeId,
                                              const Slab::Str &toPortId,
                                              const Slab::Str &reason,
                                              Slab::StrVector diagnostics = {},
                                              Slab::Vector<Slab::Str> suggestions = {}) -> bool {
        const auto duplicateIt = std::find_if(
            PlaygroundTemplateDocument.Edges.begin(),
            PlaygroundTemplateDocument.Edges.end(),
            [&](const auto &edge) {
                return edge.FromNodeId == fromNodeId &&
                       edge.FromPortId == fromPortId &&
                       edge.ToNodeId == toNodeId &&
                       edge.ToPortId == toPortId;
            });
        if (duplicateIt != PlaygroundTemplateDocument.Edges.end()) return false;

        ++PlaygroundTemplateEdgeCounter;
        FGraphEdgeV2 edge;
        edge.EdgeId = "tmpl.edge." + Slab::ToStr(PlaygroundTemplateEdgeCounter);
        edge.FromNodeId = fromNodeId;
        edge.FromPortId = fromPortId;
        edge.ToNodeId = toNodeId;
        edge.ToPortId = toPortId;
        edge.MatchReason = reason;
        edge.Diagnostics = std::move(diagnostics);
        edge.SuggestedCoercionOperatorIds = std::move(suggestions);
        PlaygroundTemplateDocument.Edges.push_back(std::move(edge));
        markDirty();
        return true;
    };

    const auto insertCoercionNode = [&](const Slab::Str &fromNodeId,
                                        const Slab::Str &fromPortId,
                                        const Slab::Str &toNodeId,
                                        const Slab::Str &toPortId,
                                        const Slab::Vector<Slab::Str> &coercionOperatorIds,
                                        const Slab::Str &edgeIdToReplace = Slab::Str{}) -> bool {
        if (coercionOperatorIds.empty()) {
            PlaygroundTemplateStatus = "[Error] No coercion operator was suggested.";
            return false;
        }

        const auto *fromNode = findTemplateNodeConst(fromNodeId);
        const auto *toNode = findTemplateNodeConst(toNodeId);
        if (fromNode == nullptr || toNode == nullptr) {
            PlaygroundTemplateStatus = "[Error] Could not resolve coercion endpoints.";
            return false;
        }

        const auto *coercionOperator = semanticCatalog.FindOperatorById(coercionOperatorIds.front());
        if (coercionOperator == nullptr ||
            coercionOperator->DomainPorts.empty() ||
            coercionOperator->CodomainPorts.empty()) {
            PlaygroundTemplateStatus = "[Error] Suggested coercion operator is invalid.";
            return false;
        }

        if (!edgeIdToReplace.empty()) {
            const auto previousCount = PlaygroundTemplateDocument.Edges.size();
            PlaygroundTemplateDocument.Edges.erase(
                std::remove_if(
                    PlaygroundTemplateDocument.Edges.begin(),
                    PlaygroundTemplateDocument.Edges.end(),
                    [&](const auto &edge) { return edge.EdgeId == edgeIdToReplace; }),
                PlaygroundTemplateDocument.Edges.end());
            if (previousCount != PlaygroundTemplateDocument.Edges.size()) {
                markDirty();
            }
        }

        ++PlaygroundTemplateCoercionCounter;
        FGraphNodeV2 coercionNode;
        coercionNode.NodeId = "tmpl.coercion." + Slab::ToStr(PlaygroundTemplateCoercionCounter);
        coercionNode.SemanticOperatorId = coercionOperator->OperatorId;
        coercionNode.Position.x = 0.5f * (fromNode->Position.x + toNode->Position.x) + 10.0f;
        coercionNode.Position.y = 0.5f * (fromNode->Position.y + toNode->Position.y) + 30.0f;
        PlaygroundTemplateDocument.Nodes.push_back(std::move(coercionNode));
        markDirty();

        const auto coercionNodeId = PlaygroundTemplateDocument.Nodes.back().NodeId;
        (void) addTemplateEdge(
            fromNodeId,
            fromPortId,
            coercionNodeId,
            coercionOperator->DomainPorts.front().PortId,
            "ExactMatch");
        (void) addTemplateEdge(
            coercionNodeId,
            coercionOperator->CodomainPorts.front().PortId,
            toNodeId,
            toPortId,
            "ExactMatch");

        PlaygroundTemplatePendingCoercion = {};
        PlaygroundTemplateSelectedNodeId = coercionNodeId;
        PlaygroundTemplateSelectedNodeIds = {coercionNodeId};
        PlaygroundTemplateSelectedEdgeId.clear();
        PlaygroundTemplateStatus =
            "[Ok] Inserted coercion node '" + coercionOperator->DisplayName + "'.";
        return true;
    };

    const auto attemptTemplateConnection = [&](const Slab::Str &outputNodeId,
                                               const Slab::Str &outputPortId,
                                               const Slab::Str &inputNodeId,
                                               const Slab::Str &inputPortId) {
        auto *fromNode = findTemplateNode(outputNodeId);
        auto *toNode = findTemplateNode(inputNodeId);
        if (fromNode == nullptr || toNode == nullptr) {
            PlaygroundTemplateStatus = "[Error] Could not resolve source/target nodes.";
            PlaygroundTemplatePendingCoercion = {};
            clearTemplateConnectionState();
            return;
        }

        const auto *fromOperator = semanticCatalog.FindOperatorById(fromNode->SemanticOperatorId);
        const auto *toOperator = semanticCatalog.FindOperatorById(toNode->SemanticOperatorId);
        if (fromOperator == nullptr || toOperator == nullptr) {
            PlaygroundTemplateStatus = "[Error] Could not resolve semantic operator schema for one endpoint.";
            PlaygroundTemplatePendingCoercion = {};
            clearTemplateConnectionState();
            return;
        }

        const auto *fromPort = findOutputPort(*fromOperator, outputPortId);
        const auto *toPort = findInputPort(*toOperator, inputPortId);
        if (fromPort == nullptr || toPort == nullptr) {
            PlaygroundTemplateStatus = "[Error] Could not resolve source/target ports.";
            PlaygroundTemplatePendingCoercion = {};
            clearTemplateConnectionState();
            return;
        }

        const auto &fromSpaceId = fromPort->SpaceId;
        const auto &toSpaceId = toPort->SpaceId;

        PlaygroundTemplatePendingCoercion = {};
        const auto match = evaluateSpaceMatch(fromSpaceId, toSpaceId);
        if (match.Reason == "ExactMatch") {
            if (addTemplateEdge(outputNodeId, outputPortId, inputNodeId, inputPortId, "ExactMatch")) {
                PlaygroundTemplateStatus =
                    "[Ok] Connected " + fromPort->DisplayName + " -> " + toPort->DisplayName + " (exact semantic space match).";
            } else {
                PlaygroundTemplateStatus = "[Warn] Edge already exists.";
            }
            clearTemplateConnectionState();
            return;
        }

        if (match.Reason == "CompatibleButNeedsCoercion") {
            PlaygroundTemplatePendingCoercion.bActive = true;
            PlaygroundTemplatePendingCoercion.FromNodeId = outputNodeId;
            PlaygroundTemplatePendingCoercion.FromPortId = outputPortId;
            PlaygroundTemplatePendingCoercion.ToNodeId = inputNodeId;
            PlaygroundTemplatePendingCoercion.ToPortId = inputPortId;
            PlaygroundTemplatePendingCoercion.SuggestedCoercionOperatorIds = match.SuggestedCoercionOperatorIds;
            PlaygroundTemplatePendingCoercion.Summary =
                "Direct edge requires explicit coercion node: '" + fromSpaceId + "' -> '" + toSpaceId + "'.";
            PlaygroundTemplateStatus =
                "[Warn] Compatible with coercion. Insert one of the suggested coercion operators.";
            clearTemplateConnectionState();
            return;
        }

        PlaygroundTemplateStatus =
            "[Error] Incompatible spaces: '" + fromSpaceId + "' -> '" + toSpaceId + "'.";
        if (!match.Diagnostics.empty()) {
            PlaygroundTemplateStatus += " " + match.Diagnostics.front();
        }
        clearTemplateConnectionState();
    };

    const auto buildCompilePreview = [&]() -> FTemplateCompilePreview {
        FTemplateCompilePreview preview;

        for (const auto &node : PlaygroundTemplateDocument.Nodes) {
            FTemplateCompilePreviewNode nodePreview;
            nodePreview.NodeId = node.NodeId;
            nodePreview.OperatorId = node.SemanticOperatorId;

            const auto *semanticOperator = semanticCatalog.FindOperatorById(node.SemanticOperatorId);
            if (semanticOperator == nullptr) {
                nodePreview.DisplayName = "<missing operator>";
                nodePreview.Diagnostics.push_back("operator schema not found");
                ++preview.UnboundNodes;
                preview.Nodes.push_back(std::move(nodePreview));
                continue;
            }

            nodePreview.DisplayName = semanticOperator->DisplayName;
            const auto bindings = semanticCatalog.QueryBindingsForOperator(semanticOperator->OperatorId);
            if (bindings.empty()) {
                nodePreview.bBound = false;
                nodePreview.Diagnostics.push_back("no implementation bindings");
                ++preview.UnboundNodes;
            } else {
                nodePreview.bBound = true;
                ++preview.BoundNodes;
                const auto &binding = bindings.front();
                nodePreview.BindingLabel = binding.TargetInterfaceId + "::" + binding.TargetOperationId;
            }

            preview.Nodes.push_back(std::move(nodePreview));
        }

        for (const auto &edge : PlaygroundTemplateDocument.Edges) {
            FTemplateCompilePreviewEdge edgePreview;
            edgePreview.EdgeId = edge.EdgeId;
            edgePreview.MatchReason = edge.MatchReason;
            edgePreview.Summary =
                edge.FromNodeId + ":" + edge.FromPortId +
                " -> " +
                edge.ToNodeId + ":" + edge.ToPortId;
            edgePreview.Diagnostics = edge.Diagnostics;
            edgePreview.SuggestedCoercionOperatorIds = edge.SuggestedCoercionOperatorIds;

            const auto *fromNode = findTemplateNodeConst(edge.FromNodeId);
            const auto *toNode = findTemplateNodeConst(edge.ToNodeId);
            if (fromNode == nullptr || toNode == nullptr) {
                edgePreview.MatchReason = "Incompatible";
                edgePreview.Diagnostics.push_back("edge endpoint node missing");
                ++preview.IncompatibleEdges;
                preview.Edges.push_back(std::move(edgePreview));
                continue;
            }

            const auto *fromOperator = semanticCatalog.FindOperatorById(fromNode->SemanticOperatorId);
            const auto *toOperator = semanticCatalog.FindOperatorById(toNode->SemanticOperatorId);
            if (fromOperator == nullptr || toOperator == nullptr) {
                edgePreview.MatchReason = "Incompatible";
                edgePreview.Diagnostics.push_back("edge endpoint operator schema missing");
                ++preview.IncompatibleEdges;
                preview.Edges.push_back(std::move(edgePreview));
                continue;
            }

            const auto *fromPort = findOutputPort(*fromOperator, edge.FromPortId);
            const auto *toPort = findInputPort(*toOperator, edge.ToPortId);
            if (fromPort == nullptr || toPort == nullptr) {
                edgePreview.MatchReason = "Incompatible";
                edgePreview.Diagnostics.push_back("edge endpoint port missing");
                ++preview.IncompatibleEdges;
                preview.Edges.push_back(std::move(edgePreview));
                continue;
            }

            const auto match = evaluateSpaceMatch(fromPort->SpaceId, toPort->SpaceId);
            edgePreview.MatchReason = match.Reason;
            if (edgePreview.Diagnostics.empty()) {
                edgePreview.Diagnostics = match.Diagnostics;
            } else {
                for (const auto &diagnostic : match.Diagnostics) {
                    appendUniqueString(edgePreview.Diagnostics, diagnostic);
                }
            }
            for (const auto &suggestion : match.SuggestedCoercionOperatorIds) {
                appendUniqueString(edgePreview.SuggestedCoercionOperatorIds, suggestion);
            }

            if (match.Reason == "ExactMatch") ++preview.ExactEdges;
            if (match.Reason == "CompatibleButNeedsCoercion") ++preview.CoercionEdges;
            if (match.Reason == "Incompatible") ++preview.IncompatibleEdges;

            if (match.Reason != "ExactMatch") {
                preview.Edges.push_back(std::move(edgePreview));
            }
        }

        return preview;
    };

    if (ImGui::BeginTabBar("GraphPlaygroundModes")) {
        if (ImGui::BeginTabItem("Semantic")) {
            ImGui::Text("Semantic catalog: %zu spaces | %zu operators", semanticSpaces.size(), semanticOperators.size());
            if (ImGui::BeginTable("SemanticCatalogTable", 2, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_Resizable)) {
                ImGui::TableSetupColumn("Spaces", ImGuiTableColumnFlags_WidthStretch, 0.48f);
                ImGui::TableSetupColumn("Operators", ImGuiTableColumnFlags_WidthStretch, 0.52f);
                ImGui::TableNextRow();

                ImGui::TableSetColumnIndex(0);
                if (ImGui::BeginChild("SemanticSpacesChild", ImVec2(0.0f, 0.0f), false)) {
                    for (const auto *space : semanticSpaces) {
                        if (space == nullptr) continue;
                        ImGui::PushID(space->SpaceId.c_str());
                        ImGui::Text("%s", space->DisplayName.c_str());
                        ImGui::TextDisabled("%s", space->SpaceId.c_str());
                        ImGui::TextDisabled("Carrier: %s", space->CarrierTypeId.c_str());
                        if (!space->Latex.empty()) ImGui::TextDisabled("LaTeX: %s", space->Latex.c_str());
                        if (!space->Tags.empty()) {
                            Slab::Str tagsLine = "Tags: ";
                            for (std::size_t i = 0; i < space->Tags.size(); ++i) {
                                tagsLine += space->Tags[i];
                                if (i + 1 < space->Tags.size()) tagsLine += ", ";
                            }
                            ImGui::TextDisabled("%s", TruncateLabel(tagsLine, 90).c_str());
                        }
                        ImGui::Separator();
                        ImGui::PopID();
                    }
                }
                ImGui::EndChild();

                ImGui::TableSetColumnIndex(1);
                if (ImGui::BeginChild("SemanticOperatorsChild", ImVec2(0.0f, 0.0f), false)) {
                    for (const auto *semanticOperator : semanticOperators) {
                        if (semanticOperator == nullptr) continue;
                        ImGui::PushID(semanticOperator->OperatorId.c_str());

                        const bool bPickOperator = ImGui::Selectable(
                            semanticOperator->DisplayName.c_str(),
                            PlaygroundTemplateSelectedOperatorId == semanticOperator->OperatorId);
                        if (bPickOperator) PlaygroundTemplateSelectedOperatorId = semanticOperator->OperatorId;

                        ImGui::TextDisabled("%s", semanticOperator->OperatorId.c_str());
                        if (!semanticOperator->LatexSymbol.empty()) {
                            ImGui::TextDisabled("LaTeX: %s", semanticOperator->LatexSymbol.c_str());
                        }
                        if (!semanticOperator->DomainPorts.empty() || !semanticOperator->CodomainPorts.empty()) {
                            Slab::Str signature = "Signature: ";
                            for (std::size_t i = 0; i < semanticOperator->DomainPorts.size(); ++i) {
                                signature += semanticOperator->DomainPorts[i].SpaceId;
                                if (i + 1 < semanticOperator->DomainPorts.size()) signature += " x ";
                            }
                            signature += " -> ";
                            for (std::size_t i = 0; i < semanticOperator->CodomainPorts.size(); ++i) {
                                signature += semanticOperator->CodomainPorts[i].SpaceId;
                                if (i + 1 < semanticOperator->CodomainPorts.size()) signature += " x ";
                            }
                            ImGui::TextDisabled("%s", TruncateLabel(signature, 108).c_str());
                        }
                        const auto bindings = semanticCatalog.QueryBindingsForOperator(semanticOperator->OperatorId);
                        if (!bindings.empty()) {
                            ImGui::TextDisabled("Bindings: %zu", bindings.size());
                        }
                        ImGui::Separator();
                        ImGui::PopID();
                    }
                }
                ImGui::EndChild();

                ImGui::EndTable();
            }
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Template")) {
            auto *selectedOperator = semanticCatalog.FindOperatorById(PlaygroundTemplateSelectedOperatorId);
            const auto selectedOperatorLabel = selectedOperator == nullptr
                ? "<no operator>"
                : selectedOperator->DisplayName + "###" + selectedOperator->OperatorId;
            if (ImGui::BeginCombo("Operator Seed", selectedOperatorLabel.c_str())) {
                for (const auto *semanticOperator : semanticOperators) {
                    if (semanticOperator == nullptr) continue;
                    const bool bSelected = PlaygroundTemplateSelectedOperatorId == semanticOperator->OperatorId;
                    if (ImGui::Selectable(semanticOperator->DisplayName.c_str(), bSelected)) {
                        PlaygroundTemplateSelectedOperatorId = semanticOperator->OperatorId;
                    }
                    if (bSelected) ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }

            ImGui::SameLine();
            if (ImGui::Button("Add Node")) {
                if (selectedOperator != nullptr) {
                    ++PlaygroundTemplateNodeCounter;
                    FGraphNodeV2 node;
                    node.NodeId = "tmpl.node." + Slab::ToStr(PlaygroundTemplateNodeCounter);
                    node.SemanticOperatorId = selectedOperator->OperatorId;
                    node.Position.x = 80.0f + static_cast<float>((PlaygroundTemplateNodeCounter % 4) * 280);
                    node.Position.y = 70.0f + static_cast<float>((PlaygroundTemplateNodeCounter / 4) * 140);
                    PlaygroundTemplateDocument.Nodes.push_back(std::move(node));
                    PlaygroundTemplateSelectedNodeId = PlaygroundTemplateDocument.Nodes.back().NodeId;
                    PlaygroundTemplateSelectedNodeIds = {PlaygroundTemplateDocument.Nodes.back().NodeId};
                    PlaygroundTemplateSelectedEdgeId.clear();
                    PlaygroundTemplateStatus = "[Ok] Added semantic node '" + selectedOperator->DisplayName + "'.";
                    markDirty();
                } else {
                    PlaygroundTemplateStatus = "[Error] Select a semantic operator first.";
                }
            }

            ImGui::SameLine();
            if (ImGui::Button("Duplicate Selected")) {
                if (PlaygroundTemplateSelectedNodeIds.empty() && PlaygroundTemplateSelectedNodeId.empty()) {
                    PlaygroundTemplateStatus = "[Error] Select a node to duplicate.";
                } else {
                    Slab::Vector<Slab::Str> sourceNodeIds = PlaygroundTemplateSelectedNodeIds;
                    if (sourceNodeIds.empty()) sourceNodeIds.push_back(PlaygroundTemplateSelectedNodeId);

                    Slab::Vector<Slab::Str> duplicatedNodeIds;
                    duplicatedNodeIds.reserve(sourceNodeIds.size());
                    for (const auto &sourceNodeId : sourceNodeIds) {
                        const auto *source = findTemplateNodeConst(sourceNodeId);
                        if (source == nullptr) continue;
                        ++PlaygroundTemplateNodeCounter;
                        FGraphNodeV2 duplicated = *source;
                        duplicated.NodeId = "tmpl.node." + Slab::ToStr(PlaygroundTemplateNodeCounter);
                        duplicated.Position.x += 36.0f;
                        duplicated.Position.y += 24.0f;
                        duplicatedNodeIds.push_back(duplicated.NodeId);
                        PlaygroundTemplateDocument.Nodes.push_back(std::move(duplicated));
                    }

                    if (duplicatedNodeIds.empty()) {
                        PlaygroundTemplateStatus = "[Error] Could not duplicate selected node(s).";
                    } else {
                        PlaygroundTemplateSelectedNodeIds = duplicatedNodeIds;
                        PlaygroundTemplateSelectedNodeId = duplicatedNodeIds.front();
                        PlaygroundTemplateSelectedEdgeId.clear();
                        PlaygroundTemplateStatus =
                            "[Ok] Duplicated " + Slab::ToStr(duplicatedNodeIds.size()) + " node(s).";
                        markDirty();
                    }
                }
            }

            ImGui::SameLine();
            if (ImGui::Button("Auto Layout")) {
                std::size_t idx = 0;
                for (auto &node : PlaygroundTemplateDocument.Nodes) {
                    node.Position.x = 90.0f + static_cast<float>((idx % 4) * 300);
                    node.Position.y = 80.0f + static_cast<float>((idx / 4) * 170);
                    ++idx;
                }
                if (!PlaygroundTemplateDocument.Nodes.empty()) {
                    PlaygroundTemplateStatus = "[Ok] Applied auto layout.";
                    markDirty();
                }
            }

            bool bRequestFitToContent = false;
            ImGui::SameLine();
            if (ImGui::Button("Fit")) {
                bRequestFitToContent = true;
            }

            ImGui::SameLine();
            if (ImGui::Button("Clear")) {
                PlaygroundTemplateDocument.Nodes.clear();
                PlaygroundTemplateDocument.Edges.clear();
                PlaygroundTemplateSelectedNodeIds.clear();
                PlaygroundTemplateSelectedNodeId.clear();
                PlaygroundTemplateSelectedEdgeId.clear();
                PlaygroundTemplatePendingCoercion = {};
                clearTemplateConnectionState();
                PlaygroundTemplateStatus = "[Ok] Template playground graph cleared.";
                markDirty();
            }
            ImGui::SameLine();
            if (ImGui::Button("Delete Selected")) {
                Slab::Vector<Slab::Str> selectedNodes = PlaygroundTemplateSelectedNodeIds;
                if (selectedNodes.empty() && !PlaygroundTemplateSelectedNodeId.empty()) {
                    selectedNodes.push_back(PlaygroundTemplateSelectedNodeId);
                }
                if (deleteTemplateNodes(selectedNodes)) {
                    PlaygroundTemplateStatus = "[Ok] Deleted selected node(s) and connected edges.";
                } else {
                    PlaygroundTemplateStatus = "[Warn] No selected node to delete.";
                }
            }

            ImGui::SameLine();
            if (ImGui::Button("Delete Edge")) {
                if (PlaygroundTemplateSelectedEdgeId.empty()) {
                    PlaygroundTemplateStatus = "[Warn] No selected edge to delete.";
                } else {
                    const auto previousCount = PlaygroundTemplateDocument.Edges.size();
                    PlaygroundTemplateDocument.Edges.erase(
                        std::remove_if(
                            PlaygroundTemplateDocument.Edges.begin(),
                            PlaygroundTemplateDocument.Edges.end(),
                            [&](const auto &edge) { return edge.EdgeId == PlaygroundTemplateSelectedEdgeId; }),
                        PlaygroundTemplateDocument.Edges.end());
                    if (previousCount != PlaygroundTemplateDocument.Edges.size()) {
                        PlaygroundTemplateStatus = "[Ok] Deleted selected edge.";
                        PlaygroundTemplateSelectedEdgeId.clear();
                        markDirty();
                    } else {
                        PlaygroundTemplateStatus = "[Warn] Selected edge was not found.";
                    }
                }
            }

            ImGui::SameLine();
            if (ImGui::Checkbox("Grid", &PlaygroundTemplateDocument.Canvas.bShowGrid)) {
                markDirty();
            }
            ImGui::SameLine();
            if (ImGui::Checkbox("Minimap", &PlaygroundTemplateDocument.Canvas.bShowMinimap)) {
                markDirty();
            }

            const bool bTemplateHotkeysEnabled =
                ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) &&
                !ImGui::GetIO().WantTextInput;
            if (bTemplateHotkeysEnabled && ImGui::IsKeyPressed(ImGuiKey_A, false)) {
                if (selectedOperator != nullptr) {
                    ++PlaygroundTemplateNodeCounter;
                    FGraphNodeV2 node;
                    node.NodeId = "tmpl.node." + Slab::ToStr(PlaygroundTemplateNodeCounter);
                    node.SemanticOperatorId = selectedOperator->OperatorId;
                    node.Position.x = 100.0f + static_cast<float>((PlaygroundTemplateNodeCounter % 3) * 120);
                    node.Position.y = 110.0f + static_cast<float>((PlaygroundTemplateNodeCounter / 3) * 80);
                    PlaygroundTemplateDocument.Nodes.push_back(std::move(node));
                    PlaygroundTemplateSelectedNodeId = PlaygroundTemplateDocument.Nodes.back().NodeId;
                    PlaygroundTemplateSelectedNodeIds = {PlaygroundTemplateDocument.Nodes.back().NodeId};
                    PlaygroundTemplateSelectedEdgeId.clear();
                    PlaygroundTemplateStatus = "[Ok] Added node (shortcut A).";
                    markDirty();
                }
            }
            if (bTemplateHotkeysEnabled && ImGui::IsKeyPressed(ImGuiKey_F, false)) {
                bRequestFitToContent = true;
            }
            if (bTemplateHotkeysEnabled && ImGui::IsKeyPressed(ImGuiKey_Delete, false)) {
                if (!PlaygroundTemplateSelectedEdgeId.empty()) {
                    const auto previousCount = PlaygroundTemplateDocument.Edges.size();
                    PlaygroundTemplateDocument.Edges.erase(
                        std::remove_if(
                            PlaygroundTemplateDocument.Edges.begin(),
                            PlaygroundTemplateDocument.Edges.end(),
                            [&](const auto &edge) { return edge.EdgeId == PlaygroundTemplateSelectedEdgeId; }),
                        PlaygroundTemplateDocument.Edges.end());
                    if (previousCount != PlaygroundTemplateDocument.Edges.size()) {
                        PlaygroundTemplateSelectedEdgeId.clear();
                        PlaygroundTemplateStatus = "[Ok] Deleted selected edge (Del).";
                        markDirty();
                    }
                } else {
                    Slab::Vector<Slab::Str> selectedNodes = PlaygroundTemplateSelectedNodeIds;
                    if (selectedNodes.empty() && !PlaygroundTemplateSelectedNodeId.empty()) {
                        selectedNodes.push_back(PlaygroundTemplateSelectedNodeId);
                    }
                    if (deleteTemplateNodes(selectedNodes)) {
                        PlaygroundTemplateStatus = "[Ok] Deleted selected node(s) (Del).";
                    }
                }
            }

            if (!PlaygroundTemplateStatus.empty()) {
                const bool bError = PlaygroundTemplateStatus.starts_with("[Error]");
                const bool bWarn = PlaygroundTemplateStatus.starts_with("[Warn]");
                const auto color = bError
                    ? ImVec4(0.92f, 0.35f, 0.35f, 1.0f)
                    : (bWarn ? ImVec4(0.93f, 0.73f, 0.30f, 1.0f) : ImVec4(0.42f, 0.83f, 0.52f, 1.0f));
                ImGui::TextColored(color, "%s", PlaygroundTemplateStatus.c_str());
            }

            if (PlaygroundTemplatePendingCoercion.bActive) {
                ImGui::SeparatorText("Pending Coercion");
                ImGui::TextWrapped("%s", PlaygroundTemplatePendingCoercion.Summary.c_str());
                if (!PlaygroundTemplatePendingCoercion.SuggestedCoercionOperatorIds.empty()) {
                    ImGui::TextDisabled("Suggestions: %s",
                        TruncateLabel(
                            Slab::ToStr(PlaygroundTemplatePendingCoercion.SuggestedCoercionOperatorIds),
                            120).c_str());
                }

                if (ImGui::Button("Insert Suggested Coercion")) {
                    (void) insertCoercionNode(
                        PlaygroundTemplatePendingCoercion.FromNodeId,
                        PlaygroundTemplatePendingCoercion.FromPortId,
                        PlaygroundTemplatePendingCoercion.ToNodeId,
                        PlaygroundTemplatePendingCoercion.ToPortId,
                        PlaygroundTemplatePendingCoercion.SuggestedCoercionOperatorIds);
                }
                ImGui::SameLine();
                if (ImGui::Button("Dismiss Coercion")) {
                    PlaygroundTemplatePendingCoercion = {};
                    PlaygroundTemplateStatus = "[Info] Coercion suggestion dismissed.";
                }
            }

            ImGui::Separator();
            if (ImGui::BeginTable(
                    "TemplateGraphLayout",
                    2,
                    ImGuiTableFlags_Resizable |
                    ImGuiTableFlags_SizingStretchSame |
                    ImGuiTableFlags_BordersInnerV)) {
                ImGui::TableSetupColumn("Canvas", ImGuiTableColumnFlags_WidthStretch, 0.70f);
                ImGui::TableSetupColumn("Inspector", ImGuiTableColumnFlags_WidthStretch, 0.30f);
                ImGui::TableNextRow();

                ImGui::TableSetColumnIndex(0);
                const auto canvasPos = ImGui::GetCursorScreenPos();
                auto canvasSize = ImGui::GetContentRegionAvail();
                if (canvasSize.x < 320.0f) canvasSize.x = 320.0f;
                if (canvasSize.y < 240.0f) canvasSize.y = 240.0f;
                const auto canvasEnd = ImVec2(canvasPos.x + canvasSize.x, canvasPos.y + canvasSize.y);

                ImGui::InvisibleButton(
                    "GraphPlaygroundTemplateCanvas",
                    canvasSize,
                    ImGuiButtonFlags_MouseButtonRight);
                ImGui::SetItemAllowOverlap();
                const bool bCanvasHovered = ImGui::IsItemHovered();
                bool bCanvasPanning = false;
                if (ImGui::IsItemActive() && ImGui::IsMouseDragging(ImGuiMouseButton_Right, 0.0f)) {
                    const auto &io = ImGui::GetIO();
                    PlaygroundTemplateDocument.Canvas.PanX += io.MouseDelta.x;
                    PlaygroundTemplateDocument.Canvas.PanY += io.MouseDelta.y;
                    bCanvasPanning = true;
                    markDirty();
                }

                auto *drawList = ImGui::GetWindowDrawList();
                drawList->AddRectFilled(canvasPos, canvasEnd, IM_COL32(18, 21, 28, 255), 6.0f);
                drawList->AddRect(canvasPos, canvasEnd, IM_COL32(74, 82, 96, 255), 6.0f, 0, 1.0f);
                drawList->PushClipRect(canvasPos, canvasEnd, true);

                if (PlaygroundTemplateDocument.Canvas.bShowGrid) {
                    constexpr float GridStep = 52.0f;
                    const float xOffset = std::fmod(PlaygroundTemplateDocument.Canvas.PanX, GridStep);
                    const float yOffset = std::fmod(PlaygroundTemplateDocument.Canvas.PanY, GridStep);
                    for (float x = canvasPos.x + xOffset; x < canvasEnd.x; x += GridStep) {
                        drawList->AddLine(ImVec2(x, canvasPos.y), ImVec2(x, canvasEnd.y), IM_COL32(34, 40, 52, 255), 1.0f);
                    }
                    for (float y = canvasPos.y + yOffset; y < canvasEnd.y; y += GridStep) {
                        drawList->AddLine(ImVec2(canvasPos.x, y), ImVec2(canvasEnd.x, y), IM_COL32(34, 40, 52, 255), 1.0f);
                    }
                }

                const auto nodeScreenPos = [&](const FGraphNodeV2 &node) -> ImVec2 {
                    return ImVec2(
                        canvasPos.x + PlaygroundTemplateDocument.Canvas.PanX + node.Position.x,
                        canvasPos.y + PlaygroundTemplateDocument.Canvas.PanY + node.Position.y);
                };

                const float templateLineHeight = ImGui::GetTextLineHeight();
                const float templateHeaderHeight = std::max(
                    BlueprintNodeHeaderMinHeight,
                    std::round(templateLineHeight * BlueprintNodeHeaderScale));

                const auto nodeScreenSize = [&](const FSemanticOperatorSchemaV1 &semanticOperator) -> ImVec2 {
                    const int rows = std::max(
                        static_cast<int>(semanticOperator.DomainPorts.size()),
                        static_cast<int>(semanticOperator.CodomainPorts.size()));
                    const float height = templateHeaderHeight + 28.0f + static_cast<float>(std::max(1, rows)) * 20.0f;
                    const float titleWidth = ImGui::CalcTextSize(semanticOperator.DisplayName.c_str()).x + 80.0f;
                    const float width = std::max(260.0f, std::min(420.0f, titleWidth));
                    return ImVec2(width, height);
                };

                const auto portPinPosition = [&](const FGraphNodeV2 &node,
                                                 const FSemanticOperatorSchemaV1 &semanticOperator,
                                                 const bool bOutput,
                                                 const int index) -> ImVec2 {
                    const auto nodePos = nodeScreenPos(node);
                    const auto size = nodeScreenSize(semanticOperator);
                    const int total = std::max(
                        1,
                        bOutput
                            ? static_cast<int>(semanticOperator.CodomainPorts.size())
                            : static_cast<int>(semanticOperator.DomainPorts.size()));
                    const float y =
                        nodePos.y + (templateHeaderHeight + 14.0f) +
                        (static_cast<float>(index) + 0.5f) * ((size.y - (templateHeaderHeight + 20.0f)) / static_cast<float>(total));
                    const float x = bOutput ? (nodePos.x + size.x - 8.0f) : (nodePos.x + 8.0f);
                    return ImVec2(x, y);
                };

                if (bRequestFitToContent) {
                    if (PlaygroundTemplateDocument.Nodes.empty()) {
                        PlaygroundTemplateStatus = "[Warn] Nothing to fit.";
                    } else {
                        float minX = std::numeric_limits<float>::max();
                        float minY = std::numeric_limits<float>::max();
                        float maxX = std::numeric_limits<float>::lowest();
                        float maxY = std::numeric_limits<float>::lowest();
                        for (const auto &node : PlaygroundTemplateDocument.Nodes) {
                            const auto *semanticOperator = semanticCatalog.FindOperatorById(node.SemanticOperatorId);
                            if (semanticOperator == nullptr) continue;
                            const auto nodeSize = nodeScreenSize(*semanticOperator);
                            minX = std::min(minX, node.Position.x);
                            minY = std::min(minY, node.Position.y);
                            maxX = std::max(maxX, node.Position.x + nodeSize.x);
                            maxY = std::max(maxY, node.Position.y + nodeSize.y);
                        }
                        if (minX <= maxX && minY <= maxY) {
                            const auto contentWidth = std::max(1.0f, maxX - minX);
                            const auto contentHeight = std::max(1.0f, maxY - minY);
                            PlaygroundTemplateDocument.Canvas.PanX = 0.5f * (canvasSize.x - contentWidth) - minX;
                            PlaygroundTemplateDocument.Canvas.PanY = 0.5f * (canvasSize.y - contentHeight) - minY;
                            PlaygroundTemplateStatus = "[Ok] View fitted to graph bounds.";
                            markDirty();
                        }
                    }
                }

                struct FRenderedTemplateEdge {
                    const FGraphEdgeV2 *Edge = nullptr;
                    ImVec2 P0 = ImVec2(0.0f, 0.0f);
                    ImVec2 P1 = ImVec2(0.0f, 0.0f);
                    ImVec2 P2 = ImVec2(0.0f, 0.0f);
                    ImVec2 P3 = ImVec2(0.0f, 0.0f);
                    ImU32 Color = IM_COL32(110, 198, 255, 255);
                };
                Slab::Vector<FRenderedTemplateEdge> renderedEdges;
                renderedEdges.reserve(PlaygroundTemplateDocument.Edges.size());

                for (const auto &edge : PlaygroundTemplateDocument.Edges) {
                    const auto *fromNode = findTemplateNodeConst(edge.FromNodeId);
                    const auto *toNode = findTemplateNodeConst(edge.ToNodeId);
                    if (fromNode == nullptr || toNode == nullptr) continue;

                    const auto *fromOperator = semanticCatalog.FindOperatorById(fromNode->SemanticOperatorId);
                    const auto *toOperator = semanticCatalog.FindOperatorById(toNode->SemanticOperatorId);
                    if (fromOperator == nullptr || toOperator == nullptr) continue;

                    const auto fromPortIt = std::find_if(
                        fromOperator->CodomainPorts.begin(),
                        fromOperator->CodomainPorts.end(),
                        [&](const auto &port) { return port.PortId == edge.FromPortId; });
                    const auto toPortIt = std::find_if(
                        toOperator->DomainPorts.begin(),
                        toOperator->DomainPorts.end(),
                        [&](const auto &port) { return port.PortId == edge.ToPortId; });
                    if (fromPortIt == fromOperator->CodomainPorts.end() || toPortIt == toOperator->DomainPorts.end()) continue;

                    const int fromIndex = static_cast<int>(std::distance(fromOperator->CodomainPorts.begin(), fromPortIt));
                    const int toIndex = static_cast<int>(std::distance(toOperator->DomainPorts.begin(), toPortIt));
                    const auto p0 = portPinPosition(*fromNode, *fromOperator, true, fromIndex);
                    const auto p3 = portPinPosition(*toNode, *toOperator, false, toIndex);
                    const auto p1 = ImVec2(p0.x + 54.0f, p0.y);
                    const auto p2 = ImVec2(p3.x - 54.0f, p3.y);

                    ImU32 color = IM_COL32(110, 198, 255, 255);
                    if (edge.MatchReason == "CompatibleButNeedsCoercion") color = IM_COL32(236, 194, 92, 255);
                    if (edge.MatchReason == "Incompatible") color = IM_COL32(226, 98, 98, 255);
                    const bool bSelected = (PlaygroundTemplateSelectedEdgeId == edge.EdgeId);
                    const float thickness = bSelected ? 3.2f : 2.0f;
                    if (bSelected) color = IM_COL32(246, 202, 116, 255);
                    drawList->AddBezierCubic(p0, p1, p2, p3, color, thickness);

                    renderedEdges.push_back(FRenderedTemplateEdge{
                        .Edge = &edge,
                        .P0 = p0,
                        .P1 = p1,
                        .P2 = p2,
                        .P3 = p3,
                        .Color = color
                    });
                }

                bool bAnyNodeHovered = false;
                bool bAnyPortHovered = false;
                bool bAnyNodeClicked = false;
                bool bAnyPortClicked = false;
                bool bAnyNodeDragging = false;

                for (auto &node : PlaygroundTemplateDocument.Nodes) {
                    const auto *semanticOperator = semanticCatalog.FindOperatorById(node.SemanticOperatorId);
                    if (semanticOperator == nullptr) continue;

                    const auto nodePos = nodeScreenPos(node);
                    const auto nodeSize = nodeScreenSize(*semanticOperator);
                    const auto nodeEnd = ImVec2(nodePos.x + nodeSize.x, nodePos.y + nodeSize.y);
                    if (nodeEnd.x < canvasPos.x || nodePos.x > canvasEnd.x || nodeEnd.y < canvasPos.y || nodePos.y > canvasEnd.y) {
                        continue;
                    }

                    ImGui::SetCursorScreenPos(nodePos);
                    ImGui::InvisibleButton(("##TemplateNode" + node.NodeId).c_str(), nodeSize, ImGuiButtonFlags_MouseButtonLeft);
                    ImGui::SetItemAllowOverlap();
                    const bool bNodeHovered = ImGui::IsItemHovered();
                    bAnyNodeHovered = bAnyNodeHovered || bNodeHovered;
                    if (ImGui::IsItemClicked(ImGuiMouseButton_Left)) {
                        bAnyNodeClicked = true;
                        PlaygroundTemplateSelectedNodeId = node.NodeId;
                        PlaygroundTemplateSelectedNodeIds = {node.NodeId};
                        PlaygroundTemplateSelectedEdgeId.clear();
                    }
                    if (ImGui::IsItemActive() && ImGui::IsMouseDragging(ImGuiMouseButton_Left, 0.0f)) {
                        const auto &io = ImGui::GetIO();
                        node.Position.x += io.MouseDelta.x;
                        node.Position.y += io.MouseDelta.y;
                        bAnyNodeDragging = true;
                        markDirty();
                    }

                    bool bSelected = PlaygroundTemplateSelectedNodeId == node.NodeId;
                    bSelected = bSelected ||
                        std::find(
                            PlaygroundTemplateSelectedNodeIds.begin(),
                            PlaygroundTemplateSelectedNodeIds.end(),
                            node.NodeId) != PlaygroundTemplateSelectedNodeIds.end();
                    drawList->AddRectFilled(nodePos, nodeEnd, IM_COL32(36, 43, 56, 246), 7.0f);
                    drawList->AddRectFilled(
                        nodePos,
                        ImVec2(nodeEnd.x, nodePos.y + templateHeaderHeight),
                        IM_COL32(84, 104, 148, 255),
                        7.0f,
                        ImDrawFlags_RoundCornersTop);
                    drawList->AddRect(
                        nodePos,
                        nodeEnd,
                        bSelected ? IM_COL32(246, 202, 116, 255) : IM_COL32(116, 126, 146, 255),
                        7.0f,
                        0,
                        bSelected ? 2.0f : 1.0f);
                    drawList->AddText(
                        ImVec2(nodePos.x + 10.0f, nodePos.y + 6.0f),
                        IM_COL32(240, 246, 252, 255),
                        TruncateLabel(semanticOperator->DisplayName, 34).c_str());
                    drawList->AddText(
                        ImVec2(nodePos.x + 10.0f, nodePos.y + templateHeaderHeight + 4.0f),
                        IM_COL32(174, 186, 204, 255),
                        TruncateLabel(node.NodeId, 34).c_str());

                    for (std::size_t i = 0; i < semanticOperator->DomainPorts.size(); ++i) {
                        const auto &port = semanticOperator->DomainPorts[i];
                        const auto pin = portPinPosition(node, *semanticOperator, false, static_cast<int>(i));
                        drawList->AddCircleFilled(pin, 5.0f, IM_COL32(116, 212, 176, 255));
                        drawList->AddText(
                            ImVec2(pin.x + 8.0f, pin.y - 7.0f),
                            IM_COL32(212, 220, 232, 255),
                            TruncateLabel(port.DisplayName.empty() ? port.PortId : port.DisplayName, 22).c_str());

                        ImGui::SetCursorScreenPos(ImVec2(pin.x - 7.0f, pin.y - 7.0f));
                        ImGui::InvisibleButton(
                            ("##TemplateInPort" + node.NodeId + ":" + port.PortId).c_str(),
                            ImVec2(14.0f, 14.0f));
                        ImGui::SetItemAllowOverlap();
                        bAnyPortHovered = bAnyPortHovered || ImGui::IsItemHovered();
                        if (ImGui::IsItemHovered()) {
                            ImGui::SetTooltip("Input %s\nSpace: %s", port.PortId.c_str(), port.SpaceId.c_str());
                        }
                        if (ImGui::IsItemClicked(ImGuiMouseButton_Left)) {
                            bAnyPortClicked = true;
                            if (!PlaygroundTemplateConnectingNodeId.empty() && bPlaygroundTemplateConnectingFromOutput) {
                                attemptTemplateConnection(
                                    PlaygroundTemplateConnectingNodeId,
                                    PlaygroundTemplateConnectingPortId,
                                    node.NodeId,
                                    port.PortId);
                            } else {
                                PlaygroundTemplateConnectingNodeId = node.NodeId;
                                PlaygroundTemplateConnectingPortId = port.PortId;
                                bPlaygroundTemplateConnectingFromOutput = false;
                                PlaygroundTemplateStatus = "[Info] Selected input port. Click an output port to connect.";
                            }
                        }
                    }

                    for (std::size_t i = 0; i < semanticOperator->CodomainPorts.size(); ++i) {
                        const auto &port = semanticOperator->CodomainPorts[i];
                        const auto pin = portPinPosition(node, *semanticOperator, true, static_cast<int>(i));
                        drawList->AddCircleFilled(pin, 5.0f, IM_COL32(122, 198, 255, 255));
                        const auto label = TruncateLabel(port.DisplayName.empty() ? port.PortId : port.DisplayName, 22);
                        const auto labelWidth = ImGui::CalcTextSize(label.c_str()).x;
                        drawList->AddText(
                            ImVec2(pin.x - 10.0f - labelWidth, pin.y - 7.0f),
                            IM_COL32(212, 220, 232, 255),
                            label.c_str());

                        ImGui::SetCursorScreenPos(ImVec2(pin.x - 7.0f, pin.y - 7.0f));
                        ImGui::InvisibleButton(
                            ("##TemplateOutPort" + node.NodeId + ":" + port.PortId).c_str(),
                            ImVec2(14.0f, 14.0f));
                        ImGui::SetItemAllowOverlap();
                        bAnyPortHovered = bAnyPortHovered || ImGui::IsItemHovered();
                        if (ImGui::IsItemHovered()) {
                            ImGui::SetTooltip("Output %s\nSpace: %s", port.PortId.c_str(), port.SpaceId.c_str());
                        }
                        if (ImGui::IsItemClicked(ImGuiMouseButton_Left)) {
                            bAnyPortClicked = true;
                            if (!PlaygroundTemplateConnectingNodeId.empty() && !bPlaygroundTemplateConnectingFromOutput) {
                                attemptTemplateConnection(
                                    node.NodeId,
                                    port.PortId,
                                    PlaygroundTemplateConnectingNodeId,
                                    PlaygroundTemplateConnectingPortId);
                            } else {
                                PlaygroundTemplateConnectingNodeId = node.NodeId;
                                PlaygroundTemplateConnectingPortId = port.PortId;
                                bPlaygroundTemplateConnectingFromOutput = true;
                                PlaygroundTemplateStatus = "[Info] Selected output port. Click an input port to connect.";
                            }
                        }
                    }
                }

                const auto mousePos = ImGui::GetIO().MousePos;
                const bool bMouseInCanvas =
                    mousePos.x >= canvasPos.x && mousePos.x <= canvasEnd.x &&
                    mousePos.y >= canvasPos.y && mousePos.y <= canvasEnd.y;

                if (bCanvasPanning || bAnyNodeDragging || bAnyNodeClicked || bAnyPortClicked) {
                    bPlaygroundTemplateMarqueeSelecting = false;
                }

                if (bCanvasHovered &&
                    bMouseInCanvas &&
                    ImGui::IsMouseClicked(ImGuiMouseButton_Left) &&
                    !bAnyNodeHovered &&
                    !bAnyPortHovered &&
                    !bAnyNodeClicked &&
                    !bAnyPortClicked) {
                    constexpr float EdgePickDistanceSq = 11.0f * 11.0f;
                    const FRenderedTemplateEdge *pickedEdge = nullptr;
                    float minDistanceSq = EdgePickDistanceSq;
                    for (const auto &renderedEdge : renderedEdges) {
                        const auto distanceSq = DistanceSquaredToBezier(
                            renderedEdge.P0,
                            renderedEdge.P1,
                            renderedEdge.P2,
                            renderedEdge.P3,
                            mousePos);
                        if (distanceSq < minDistanceSq) {
                            minDistanceSq = distanceSq;
                            pickedEdge = &renderedEdge;
                        }
                    }

                    if (pickedEdge != nullptr && pickedEdge->Edge != nullptr) {
                        PlaygroundTemplateSelectedEdgeId = pickedEdge->Edge->EdgeId;
                        PlaygroundTemplateSelectedNodeId.clear();
                        PlaygroundTemplateSelectedNodeIds.clear();
                        bPlaygroundTemplateMarqueeSelecting = false;
                    } else {
                        PlaygroundTemplateSelectedEdgeId.clear();
                        PlaygroundTemplateSelectedNodeId.clear();
                        PlaygroundTemplateSelectedNodeIds.clear();
                        bPlaygroundTemplateMarqueeSelecting = true;
                        PlaygroundTemplateMarqueeStart = mousePos;
                        PlaygroundTemplateMarqueeEnd = mousePos;
                    }
                }

                if (bPlaygroundTemplateMarqueeSelecting) {
                    if (ImGui::IsMouseDown(ImGuiMouseButton_Right)) {
                        bPlaygroundTemplateMarqueeSelecting = false;
                    } else if (ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
                        PlaygroundTemplateMarqueeEnd = mousePos;
                    } else {
                        constexpr float MarqueeMinDistanceSq = 8.0f * 8.0f;
                        const bool bTinyMarquee =
                            DistanceSquared(PlaygroundTemplateMarqueeStart, PlaygroundTemplateMarqueeEnd) < MarqueeMinDistanceSq;

                        if (bTinyMarquee) {
                            PlaygroundTemplateSelectedNodeId.clear();
                            PlaygroundTemplateSelectedNodeIds.clear();
                            PlaygroundTemplateSelectedEdgeId.clear();
                        } else {
                            const auto rectMin = ImVec2(
                                std::min(PlaygroundTemplateMarqueeStart.x, PlaygroundTemplateMarqueeEnd.x),
                                std::min(PlaygroundTemplateMarqueeStart.y, PlaygroundTemplateMarqueeEnd.y));
                            const auto rectMax = ImVec2(
                                std::max(PlaygroundTemplateMarqueeStart.x, PlaygroundTemplateMarqueeEnd.x),
                                std::max(PlaygroundTemplateMarqueeStart.y, PlaygroundTemplateMarqueeEnd.y));

                            PlaygroundTemplateSelectedNodeIds.clear();
                            for (const auto &node : PlaygroundTemplateDocument.Nodes) {
                                const auto *semanticOperator = semanticCatalog.FindOperatorById(node.SemanticOperatorId);
                                if (semanticOperator == nullptr) continue;
                                const auto nodePos = nodeScreenPos(node);
                                const auto nodeSize = nodeScreenSize(*semanticOperator);
                                const auto nodeEnd = ImVec2(nodePos.x + nodeSize.x, nodePos.y + nodeSize.y);
                                const bool bIntersect =
                                    !(nodeEnd.x < rectMin.x || nodePos.x > rectMax.x || nodeEnd.y < rectMin.y || nodePos.y > rectMax.y);
                                if (!bIntersect) continue;
                                appendUniqueNodeSelection(node.NodeId);
                            }
                            if (!PlaygroundTemplateSelectedNodeIds.empty()) {
                                PlaygroundTemplateSelectedNodeId = PlaygroundTemplateSelectedNodeIds.front();
                                PlaygroundTemplateStatus =
                                    "[Info] Selected " + Slab::ToStr(PlaygroundTemplateSelectedNodeIds.size()) + " node(s).";
                            } else {
                                PlaygroundTemplateSelectedNodeId.clear();
                            }
                        }

                        bPlaygroundTemplateMarqueeSelecting = false;
                    }
                }

                if (bPlaygroundTemplateMarqueeSelecting && ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
                    const auto rectMin = ImVec2(
                        std::min(PlaygroundTemplateMarqueeStart.x, PlaygroundTemplateMarqueeEnd.x),
                        std::min(PlaygroundTemplateMarqueeStart.y, PlaygroundTemplateMarqueeEnd.y));
                    const auto rectMax = ImVec2(
                        std::max(PlaygroundTemplateMarqueeStart.x, PlaygroundTemplateMarqueeEnd.x),
                        std::max(PlaygroundTemplateMarqueeStart.y, PlaygroundTemplateMarqueeEnd.y));
                    drawList->AddRectFilled(rectMin, rectMax, IM_COL32(236, 194, 92, 32), 3.0f);
                    drawList->AddRect(rectMin, rectMax, IM_COL32(236, 194, 92, 200), 3.0f, 0, 1.25f);
                }

                if (!PlaygroundTemplateConnectingNodeId.empty()) {
                    const auto *sourceNode = findTemplateNodeConst(PlaygroundTemplateConnectingNodeId);
                    if (sourceNode != nullptr) {
                        const auto *sourceOperator = semanticCatalog.FindOperatorById(sourceNode->SemanticOperatorId);
                        if (sourceOperator != nullptr) {
                            const auto portVec = bPlaygroundTemplateConnectingFromOutput
                                ? sourceOperator->CodomainPorts
                                : sourceOperator->DomainPorts;
                            const auto portIt = std::find_if(portVec.begin(), portVec.end(), [&](const auto &port) {
                                return port.PortId == PlaygroundTemplateConnectingPortId;
                            });
                            if (portIt != portVec.end()) {
                                const int index = static_cast<int>(std::distance(portVec.begin(), portIt));
                                const auto sourcePin = portPinPosition(
                                    *sourceNode,
                                    *sourceOperator,
                                    bPlaygroundTemplateConnectingFromOutput,
                                    index);
                                drawList->AddBezierCubic(
                                    sourcePin,
                                    ImVec2(sourcePin.x + (bPlaygroundTemplateConnectingFromOutput ? 48.0f : -48.0f), sourcePin.y),
                                    ImVec2(mousePos.x + (bPlaygroundTemplateConnectingFromOutput ? -24.0f : 24.0f), mousePos.y),
                                    mousePos,
                                    IM_COL32(232, 206, 118, 220),
                                    2.0f);
                            }
                        }
                    }

                    if (bMouseInCanvas && ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
                        clearTemplateConnectionState();
                        PlaygroundTemplateStatus = "[Info] Connection cancelled.";
                    }
                }

                if (PlaygroundTemplateDocument.Canvas.bShowMinimap && !PlaygroundTemplateDocument.Nodes.empty()) {
                    const auto minimapSize = ImVec2(
                        std::clamp(canvasSize.x * 0.20f, 140.0f, 220.0f),
                        std::clamp(canvasSize.y * 0.25f, 90.0f, 140.0f));
                    const auto minimapPos = ImVec2(
                        canvasEnd.x - minimapSize.x - 12.0f,
                        canvasEnd.y - minimapSize.y - 12.0f);
                    const auto minimapEnd = ImVec2(minimapPos.x + minimapSize.x, minimapPos.y + minimapSize.y);
                    drawList->AddRectFilled(minimapPos, minimapEnd, IM_COL32(18, 22, 30, 220), 5.0f);
                    drawList->AddRect(minimapPos, minimapEnd, IM_COL32(96, 110, 138, 220), 5.0f, 0, 1.0f);

                    float minX = std::numeric_limits<float>::max();
                    float minY = std::numeric_limits<float>::max();
                    float maxX = std::numeric_limits<float>::lowest();
                    float maxY = std::numeric_limits<float>::lowest();
                    for (const auto &node : PlaygroundTemplateDocument.Nodes) {
                        const auto *semanticOperator = semanticCatalog.FindOperatorById(node.SemanticOperatorId);
                        if (semanticOperator == nullptr) continue;
                        const auto nodeSize = nodeScreenSize(*semanticOperator);
                        minX = std::min(minX, node.Position.x);
                        minY = std::min(minY, node.Position.y);
                        maxX = std::max(maxX, node.Position.x + nodeSize.x);
                        maxY = std::max(maxY, node.Position.y + nodeSize.y);
                    }

                    if (minX <= maxX && minY <= maxY) {
                        const auto viewMin = ImVec2(-PlaygroundTemplateDocument.Canvas.PanX, -PlaygroundTemplateDocument.Canvas.PanY);
                        const auto viewMax = ImVec2(viewMin.x + canvasSize.x, viewMin.y + canvasSize.y);
                        const float worldMinX = std::min(minX, viewMin.x);
                        const float worldMinY = std::min(minY, viewMin.y);
                        const float worldMaxX = std::max(maxX, viewMax.x);
                        const float worldMaxY = std::max(maxY, viewMax.y);
                        const auto graphWidth = std::max(1.0f, worldMaxX - worldMinX);
                        const auto graphHeight = std::max(1.0f, worldMaxY - worldMinY);
                        const auto mapX = [worldMinX, graphWidth, minimapPos, minimapSize](const float x) {
                            return minimapPos.x + (x - worldMinX) * (minimapSize.x / graphWidth);
                        };
                        const auto mapY = [worldMinY, graphHeight, minimapPos, minimapSize](const float y) {
                            return minimapPos.y + (y - worldMinY) * (minimapSize.y / graphHeight);
                        };

                        drawList->PushClipRect(minimapPos, minimapEnd, true);
                        for (const auto &node : PlaygroundTemplateDocument.Nodes) {
                            const auto *semanticOperator = semanticCatalog.FindOperatorById(node.SemanticOperatorId);
                            if (semanticOperator == nullptr) continue;
                            const auto nodeSize = nodeScreenSize(*semanticOperator);
                            const auto p0 = ImVec2(mapX(node.Position.x), mapY(node.Position.y));
                            const auto p1 = ImVec2(
                                mapX(node.Position.x + nodeSize.x),
                                mapY(node.Position.y + nodeSize.y));
                            drawList->AddRectFilled(p0, p1, IM_COL32(98, 136, 194, 180), 2.0f);
                        }

                        auto vp0 = ImVec2(mapX(viewMin.x), mapY(viewMin.y));
                        auto vp1 = ImVec2(mapX(viewMax.x), mapY(viewMax.y));
                        vp0.x = std::clamp(vp0.x, minimapPos.x, minimapEnd.x);
                        vp0.y = std::clamp(vp0.y, minimapPos.y, minimapEnd.y);
                        vp1.x = std::clamp(vp1.x, minimapPos.x, minimapEnd.x);
                        vp1.y = std::clamp(vp1.y, minimapPos.y, minimapEnd.y);
                        drawList->AddRect(vp0, vp1, IM_COL32(246, 202, 116, 240), 2.0f, 0, 1.3f);
                        drawList->PopClipRect();
                    }
                }

                drawList->PopClipRect();
                ImGui::SetCursorScreenPos(ImVec2(canvasPos.x, canvasEnd.y));
                ImGui::TextDisabled(
                    "Template graph: %zu nodes, %zu edges | Shortcuts: A add, Del delete, F fit | Right-drag pans.",
                    PlaygroundTemplateDocument.Nodes.size(),
                    PlaygroundTemplateDocument.Edges.size());

                ImGui::TableSetColumnIndex(1);
                if (ImGui::BeginChild("TemplateInspectorChild", ImVec2(0.0f, 0.0f), false)) {
                    ImGui::TextDisabled("Inspector");
                    ImGui::Separator();
                    ImGui::Text("Selected nodes: %zu", PlaygroundTemplateSelectedNodeIds.size());
                    ImGui::Text("Selected edge: %s",
                        PlaygroundTemplateSelectedEdgeId.empty()
                            ? "<none>"
                            : PlaygroundTemplateSelectedEdgeId.c_str());

                    const auto *selectedEdge = findTemplateEdgeConst(PlaygroundTemplateSelectedEdgeId);
                    if (!PlaygroundTemplateSelectedEdgeId.empty() && selectedEdge == nullptr) {
                        PlaygroundTemplateSelectedEdgeId.clear();
                    }

                    if (selectedEdge != nullptr) {
                        ImGui::SeparatorText("Edge");
                        ImGui::Text("Id: %s", selectedEdge->EdgeId.c_str());
                        ImGui::TextDisabled(
                            "%s:%s -> %s:%s",
                            selectedEdge->FromNodeId.c_str(),
                            selectedEdge->FromPortId.c_str(),
                            selectedEdge->ToNodeId.c_str(),
                            selectedEdge->ToPortId.c_str());
                        ImGui::Text("Match: %s", selectedEdge->MatchReason.c_str());
                        if (!selectedEdge->Diagnostics.empty()) {
                            for (const auto &diagnostic : selectedEdge->Diagnostics) {
                                ImGui::TextDisabled("%s", diagnostic.c_str());
                            }
                        }

                        Slab::Vector<Slab::Str> suggestions = selectedEdge->SuggestedCoercionOperatorIds;
                        if (suggestions.empty()) {
                            const auto *fromNode = findTemplateNodeConst(selectedEdge->FromNodeId);
                            const auto *toNode = findTemplateNodeConst(selectedEdge->ToNodeId);
                            if (fromNode != nullptr && toNode != nullptr) {
                                const auto *fromOperator = semanticCatalog.FindOperatorById(fromNode->SemanticOperatorId);
                                const auto *toOperator = semanticCatalog.FindOperatorById(toNode->SemanticOperatorId);
                                if (fromOperator != nullptr && toOperator != nullptr) {
                                    const auto *fromPort = findOutputPort(*fromOperator, selectedEdge->FromPortId);
                                    const auto *toPort = findInputPort(*toOperator, selectedEdge->ToPortId);
                                    if (fromPort != nullptr && toPort != nullptr) {
                                        const auto match = evaluateSpaceMatch(fromPort->SpaceId, toPort->SpaceId);
                                        suggestions = match.SuggestedCoercionOperatorIds;
                                    }
                                }
                            }
                        }

                        if (!suggestions.empty()) {
                            ImGui::TextDisabled("Suggested coercions:");
                            for (std::size_t i = 0; i < suggestions.size(); ++i) {
                                const auto buttonLabel = "Insert " + TruncateLabel(suggestions[i], 46) + "##edge_suggestion_" + Slab::ToStr(i);
                                if (ImGui::Button(buttonLabel.c_str())) {
                                    (void) insertCoercionNode(
                                        selectedEdge->FromNodeId,
                                        selectedEdge->FromPortId,
                                        selectedEdge->ToNodeId,
                                        selectedEdge->ToPortId,
                                        suggestions,
                                        selectedEdge->EdgeId);
                                }
                            }
                        }

                        if (ImGui::Button("Delete This Edge")) {
                            const auto previousCount = PlaygroundTemplateDocument.Edges.size();
                            PlaygroundTemplateDocument.Edges.erase(
                                std::remove_if(
                                    PlaygroundTemplateDocument.Edges.begin(),
                                    PlaygroundTemplateDocument.Edges.end(),
                                    [&](const auto &edge) { return edge.EdgeId == selectedEdge->EdgeId; }),
                                PlaygroundTemplateDocument.Edges.end());
                            if (previousCount != PlaygroundTemplateDocument.Edges.size()) {
                                PlaygroundTemplateStatus = "[Ok] Deleted selected edge.";
                                PlaygroundTemplateSelectedEdgeId.clear();
                                markDirty();
                            }
                        }
                    }

                    const auto *selectedNode = findTemplateNodeConst(PlaygroundTemplateSelectedNodeId);
                    if (selectedNode != nullptr) {
                        const auto *selectedNodeOperator = semanticCatalog.FindOperatorById(selectedNode->SemanticOperatorId);
                        ImGui::SeparatorText("Node");
                        ImGui::Text("Id: %s", selectedNode->NodeId.c_str());
                        if (selectedNodeOperator != nullptr) {
                            ImGui::Text("Operator: %s", selectedNodeOperator->DisplayName.c_str());
                            ImGui::TextDisabled("%s", selectedNodeOperator->OperatorId.c_str());
                            if (!selectedNodeOperator->LatexSymbol.empty()) {
                                ImGui::TextDisabled("LaTeX: %s", selectedNodeOperator->LatexSymbol.c_str());
                            }
                            ImGui::Text("Inputs: %zu | Outputs: %zu",
                                selectedNodeOperator->DomainPorts.size(),
                                selectedNodeOperator->CodomainPorts.size());
                            const auto bindings = semanticCatalog.QueryBindingsForOperator(selectedNodeOperator->OperatorId);
                            ImGui::Text("Bindings: %zu", bindings.size());
                            for (std::size_t i = 0; i < bindings.size() && i < 6; ++i) {
                                const auto &binding = bindings[i];
                                ImGui::TextDisabled(
                                    "%s::%s",
                                    TruncateLabel(binding.TargetInterfaceId, 42).c_str(),
                                    TruncateLabel(binding.TargetOperationId, 30).c_str());
                            }
                        } else {
                            ImGui::TextDisabled("Operator schema missing.");
                        }
                    }

                    const auto compilePreview = buildCompilePreview();
                    ImGui::SeparatorText("Compile Preview");
                    ImGui::Text("Bound nodes: %zu", compilePreview.BoundNodes);
                    ImGui::Text("Unbound nodes: %zu", compilePreview.UnboundNodes);
                    ImGui::Text("Exact edges: %zu", compilePreview.ExactEdges);
                    ImGui::Text("Needs coercion: %zu", compilePreview.CoercionEdges);
                    ImGui::Text("Incompatible edges: %zu", compilePreview.IncompatibleEdges);

                    if (ImGui::BeginTable(
                            "TemplateCompileNodesTable",
                            3,
                            ImGuiTableFlags_BordersInnerV |
                            ImGuiTableFlags_RowBg |
                            ImGuiTableFlags_SizingStretchProp)) {
                        ImGui::TableSetupColumn("Node");
                        ImGui::TableSetupColumn("Operator");
                        ImGui::TableSetupColumn("Binding");
                        ImGui::TableHeadersRow();
                        for (const auto &compiledNode : compilePreview.Nodes) {
                            ImGui::TableNextRow();
                            ImGui::TableSetColumnIndex(0);
                            ImGui::TextUnformatted(compiledNode.NodeId.c_str());
                            ImGui::TableSetColumnIndex(1);
                            ImGui::TextUnformatted(compiledNode.DisplayName.c_str());
                            ImGui::TableSetColumnIndex(2);
                            if (compiledNode.bBound) {
                                ImGui::TextUnformatted(TruncateLabel(compiledNode.BindingLabel, 54).c_str());
                            } else {
                                ImGui::TextColored(ImVec4(0.93f, 0.72f, 0.30f, 1.0f), "Unbound");
                            }
                        }
                        ImGui::EndTable();
                    }

                    if (!compilePreview.Edges.empty()) {
                        ImGui::SeparatorText("Edge Diagnostics");
                        for (const auto &compiledEdge : compilePreview.Edges) {
                            const bool bIncompatible = compiledEdge.MatchReason == "Incompatible";
                            const auto color = bIncompatible
                                ? ImVec4(0.93f, 0.40f, 0.40f, 1.0f)
                                : ImVec4(0.93f, 0.73f, 0.30f, 1.0f);
                            ImGui::TextColored(color, "%s (%s)", compiledEdge.EdgeId.c_str(), compiledEdge.MatchReason.c_str());
                            ImGui::TextDisabled("%s", TruncateLabel(compiledEdge.Summary, 84).c_str());
                            for (const auto &diagnostic : compiledEdge.Diagnostics) {
                                ImGui::TextDisabled("%s", TruncateLabel(diagnostic, 84).c_str());
                            }
                            if (!compiledEdge.SuggestedCoercionOperatorIds.empty()) {
                                ImGui::TextDisabled(
                                    "Coercions: %s",
                                    TruncateLabel(Slab::ToStr(compiledEdge.SuggestedCoercionOperatorIds), 84).c_str());
                            }
                            ImGui::Separator();
                        }
                    }
                }
                ImGui::EndChild();
                ImGui::EndTable();
            }

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Runtime")) {
            auto &catalogRegistry = FReflectionCatalogRegistryV2::Get();
            catalogRegistry.RefreshAll();
            const auto mergedCatalog = catalogRegistry.BuildMergedCatalog();
            const auto runtimeGraphDocument = MakeGraphDocumentFromReflectionCatalog(mergedCatalog, EGraphModeV2::Runtime);
            SyncReflectionSemanticsOverlayV1(mergedCatalog);

            ImGui::Text(
                "Runtime reflection snapshot: %zu interfaces | %zu graph nodes | %zu graph members",
                mergedCatalog.Interfaces.size(),
                runtimeGraphDocument.Nodes.size(),
                CountGraphMembers(runtimeGraphDocument));
            {
                auto buffer = std::vector<char>(256, '\0');
                if (!PlaygroundRuntimeFilter.empty()) {
                    std::strncpy(buffer.data(), PlaygroundRuntimeFilter.c_str(), buffer.size() - 1);
                    buffer.back() = '\0';
                }
                if (ImGui::InputTextWithHint("##RuntimeFilter", "Filter interfaces...", buffer.data(), buffer.size())) {
                    PlaygroundRuntimeFilter = buffer.data();
                }
            }

            if (ImGui::BeginTable(
                    "RuntimeGraphInterfaces",
                    4,
                    ImGuiTableFlags_RowBg |
                    ImGuiTableFlags_BordersInnerV |
                    ImGuiTableFlags_ScrollY)) {
                ImGui::TableSetupColumn("Interface");
                ImGui::TableSetupColumn("Source", ImGuiTableColumnFlags_WidthFixed, 180.0f);
                ImGui::TableSetupColumn("Properties", ImGuiTableColumnFlags_WidthFixed, 80.0f);
                ImGui::TableSetupColumn("Operations", ImGuiTableColumnFlags_WidthFixed, 80.0f);
                ImGui::TableHeadersRow();

                for (const auto &interfaceSchema : mergedCatalog.Interfaces) {
                    const bool bMatches =
                        ContainsCaseInsensitive(interfaceSchema.DisplayName, PlaygroundRuntimeFilter) ||
                        ContainsCaseInsensitive(interfaceSchema.InterfaceId, PlaygroundRuntimeFilter);
                    if (!bMatches) continue;

                    auto sourceLabel = Slab::Str("unknown");
                    if (const auto source = catalogRegistry.FindSourceForInterface(interfaceSchema.InterfaceId); source.has_value()) {
                        sourceLabel = source->DisplayName;
                    }

                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("%s", interfaceSchema.DisplayName.c_str());
                    ImGui::TextDisabled("%s", TruncateLabel(interfaceSchema.InterfaceId, 68).c_str());

                    ImGui::TableSetColumnIndex(1);
                    ImGui::TextUnformatted(sourceLabel.c_str());

                    ImGui::TableSetColumnIndex(2);
                    ImGui::Text("%zu", interfaceSchema.Parameters.size());

                    ImGui::TableSetColumnIndex(3);
                    ImGui::Text("%zu", interfaceSchema.Operations.size());
                }

                ImGui::EndTable();
            }
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Routing")) {
            const auto routeKindLabel = [](const EGraphEdgeKindV2 kind) -> const char * {
                switch (kind) {
                    case EGraphEdgeKindV2::ValueFlow:
                        return "ValueFlow";
                    case EGraphEdgeKindV2::HandleBinding:
                        return "HandleBinding";
                    case EGraphEdgeKindV2::StreamSubscription:
                        return "StreamSubscription";
                    case EGraphEdgeKindV2::ControlDependency:
                        return "ControlDependency";
                }
                return "Unknown";
            };

            ImGui::TextDisabled("Patchbay playground (in-memory only).");
            {
                auto sourceBuffer = std::vector<char>(256, '\0');
                auto targetBuffer = std::vector<char>(256, '\0');
                if (!PlaygroundRoutingSourceEndpoint.empty()) {
                    std::strncpy(sourceBuffer.data(), PlaygroundRoutingSourceEndpoint.c_str(), sourceBuffer.size() - 1);
                }
                if (!PlaygroundRoutingTargetEndpoint.empty()) {
                    std::strncpy(targetBuffer.data(), PlaygroundRoutingTargetEndpoint.c_str(), targetBuffer.size() - 1);
                }

                ImGui::InputText("Source Endpoint", sourceBuffer.data(), sourceBuffer.size());
                ImGui::InputText("Target Endpoint", targetBuffer.data(), targetBuffer.size());
                const auto previousSource = PlaygroundRoutingSourceEndpoint;
                const auto previousTarget = PlaygroundRoutingTargetEndpoint;
                PlaygroundRoutingSourceEndpoint = sourceBuffer.data();
                PlaygroundRoutingTargetEndpoint = targetBuffer.data();
                if (PlaygroundRoutingSourceEndpoint != previousSource ||
                    PlaygroundRoutingTargetEndpoint != previousTarget) {
                    markDirty();
                }
            }

            const auto previousRouteKind = PlaygroundRoutingEdgeKind;
            int routeKindIndex = static_cast<int>(PlaygroundRoutingEdgeKind);
            const char *routeKindLabels[] = {
                "ValueFlow",
                "HandleBinding",
                "StreamSubscription",
                "ControlDependency"
            };
            ImGui::Combo(
                "Edge Kind",
                &routeKindIndex,
                routeKindLabels,
                static_cast<int>(std::size(routeKindLabels)));
            routeKindIndex = std::clamp(routeKindIndex, 0, 3);
            PlaygroundRoutingEdgeKind = static_cast<EGraphEdgeKindV2>(routeKindIndex);
            if (PlaygroundRoutingEdgeKind != previousRouteKind) markDirty();

            if (ImGui::Button("Add Route")) {
                if (PlaygroundRoutingSourceEndpoint.empty() || PlaygroundRoutingTargetEndpoint.empty()) {
                    PlaygroundRoutingStatus = "[Error] Source and target endpoint are required.";
                } else {
                    ++PlaygroundRoutingEdgeCounter;
                    FGraphEdgeV2 edge;
                    edge.EdgeId = "route.edge." + Slab::ToStr(PlaygroundRoutingEdgeCounter);
                    edge.FromNodeId = PlaygroundRoutingSourceEndpoint;
                    edge.ToNodeId = PlaygroundRoutingTargetEndpoint;
                    edge.Kind = PlaygroundRoutingEdgeKind;
                    PlaygroundRoutingDocument.Edges.push_back(std::move(edge));
                    PlaygroundRoutingStatus = "[Ok] Added routing edge (" + Slab::Str(routeKindLabel(PlaygroundRoutingEdgeKind)) + ").";
                    markDirty();
                }
            }
            ImGui::SameLine();
            if (ImGui::Button("Clear Routes")) {
                PlaygroundRoutingDocument.Edges.clear();
                PlaygroundRoutingStatus = "[Ok] Cleared routing playground edges.";
                markDirty();
            }

            if (!PlaygroundRoutingStatus.empty()) {
                const bool bError = PlaygroundRoutingStatus.starts_with("[Error]");
                ImGui::TextColored(
                    bError ? ImVec4(0.92f, 0.36f, 0.36f, 1.0f) : ImVec4(0.42f, 0.83f, 0.52f, 1.0f),
                    "%s",
                    PlaygroundRoutingStatus.c_str());
            }

            if (ImGui::BeginTable("RoutingGraphTable", 4, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_RowBg)) {
                ImGui::TableSetupColumn("Id", ImGuiTableColumnFlags_WidthFixed, 130.0f);
                ImGui::TableSetupColumn("Kind", ImGuiTableColumnFlags_WidthFixed, 160.0f);
                ImGui::TableSetupColumn("Source");
                ImGui::TableSetupColumn("Target");
                ImGui::TableHeadersRow();

                for (const auto &edge : PlaygroundRoutingDocument.Edges) {
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::TextUnformatted(edge.EdgeId.c_str());
                    ImGui::TableSetColumnIndex(1);
                    ImGui::TextUnformatted(routeKindLabel(edge.Kind));
                    ImGui::TableSetColumnIndex(2);
                    ImGui::TextUnformatted(edge.FromNodeId.c_str());
                    ImGui::TableSetColumnIndex(3);
                    ImGui::TextUnformatted(edge.ToNodeId.c_str());
                }

                ImGui::EndTable();
            }
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }
}

auto FLabV2WindowManager::ArrangeTopLevelSlabWindows() -> bool {
    if (SlabWindows.empty()) return true;

    constexpr int MinTileWidth = 420;

    const int gap = Slab::Graphics::WindowStyle::TilingGapSize;
    const int menuHeight = static_cast<int>(std::ceil(GetTopMenuInset()));
    const bool bDockingMode = IsDockingEnabled();
    if (!ShouldRenderSlabWindowsInWorkspace()) {
        HideSlabWindowsOffscreen();
        return true;
    }
    const int sidePaneInset = bDockingMode ? 0 : SidePaneWidth;
    const int tabsHeight = bDockingMode ? static_cast<int>(std::ceil(WorkspaceTabsHeight)) : 0;
    const int stripHeight = bDockingMode ? static_cast<int>(std::ceil(WorkspaceStripHeight)) : 0;

    int xWorkspace = sidePaneInset + gap;
    int yWorkspace = menuHeight + tabsHeight + stripHeight + gap;
    int wWorkspace = std::max(0, WidthSysWin - xWorkspace - gap);
    int hWorkspace = std::max(0, HeightSysWin - yWorkspace - gap);

#ifdef IMGUI_HAS_DOCK
    if (bDockingMode && DockspaceId != 0) {
        if (const auto *centralNode = ImGui::DockBuilderGetCentralNode(static_cast<ImGuiID>(DockspaceId));
            centralNode != nullptr) {
            if (centralNode->Size.x <= 1.0f || centralNode->Size.y <= 1.0f) {
                return false;
            }
            xWorkspace = static_cast<int>(std::floor(centralNode->Pos.x)) + gap;
            yWorkspace = static_cast<int>(std::floor(centralNode->Pos.y)) + gap;
            wWorkspace = std::max(0, static_cast<int>(std::ceil(centralNode->Size.x)) - 2 * gap);
            hWorkspace = std::max(0, static_cast<int>(std::ceil(centralNode->Size.y)) - 2 * gap);
        } else {
            return false;
        }
    }
#endif

    if (wWorkspace <= 0 || hWorkspace <= 0) return false;

    auto activeWorkspaceWindows = GetWorkspaceWindows(ActiveWorkspace);
    for (const auto &window : SlabWindows) {
        if (window == nullptr) continue;
        if (GetWorkspaceForWindow(window) == ActiveWorkspace) continue;
        window->Set_x(-10000);
        window->Set_y(-10000);
        window->NotifyReshape(64, 64);
    }

    const int nWindows = static_cast<int>(activeWorkspaceWindows.size());
    if (nWindows <= 0) {
        CapturedMouseWindow.reset();
        return true;
    }

    if (bDockingMode && nWindows == 1) {
        if (const auto &window = activeWorkspaceWindows.front(); window != nullptr) {
            window->Set_x(xWorkspace);
            window->Set_y(yWorkspace);
            window->NotifyReshape(wWorkspace, hWorkspace);
        }
        return true;
    }

    auto canFitCols = [&](const int nColsCandidate) {
        if (nColsCandidate <= 0) return false;
        const int wTilesCandidate = std::max(1, wWorkspace - (nColsCandidate + 1) * gap);
        return (wTilesCandidate / nColsCandidate) >= MinTileWidth;
    };

    int nCols = std::max(1, std::min(2, nWindows));
    while (nCols > 1 && !canFitCols(nCols)) --nCols;

    const int nRows = std::max(1, static_cast<int>(std::ceil(static_cast<double>(nWindows) / nCols)));
    const int wTiles = std::max(1, wWorkspace - (nCols + 1) * gap);
    const int hTiles = std::max(1, hWorkspace - (nRows + 1) * gap);
    const int tileW = std::max(1, wTiles / nCols);
    const int tileH = std::max(1, hTiles / nRows);

    int iWindow = 0;
    for (const auto &window : activeWorkspaceWindows) {
        if (window == nullptr) continue;

        const int iCol = iWindow % nCols;
        const int iRow = iWindow / nCols;

        const int x = xWorkspace + gap + iCol * (tileW + gap);
        const int y = yWorkspace + gap + iRow * (tileH + gap);

        const int w = std::max(1, std::min(tileW, WidthSysWin - x - gap));
        const int h = std::max(1, std::min(tileH, HeightSysWin - y - gap));

        window->Set_x(x);
        window->Set_y(y);
        window->NotifyReshape(w, h);

        ++iWindow;
    }

    return true;
}

auto FLabV2WindowManager::IsDockingEnabled() const -> bool {
    if (!bUseDockspaceLayout) return false;
    if (ImGui::GetCurrentContext() == nullptr) return false;
    const auto &io = ImGui::GetIO();
    return (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) != 0;
}

auto FLabV2WindowManager::ShouldRenderSlabWindowsInWorkspace() const -> bool {
    if (!IsDockingEnabled()) return true;
    return ActiveWorkspace == EWorkspaceTab::Simulations || ActiveWorkspace == EWorkspaceTab::Plots;
}

auto FLabV2WindowManager::HideSlabWindowsOffscreen() -> void {
    for (const auto &window : SlabWindows) {
        if (window == nullptr) continue;
        window->Set_x(-10000);
        window->Set_y(-10000);
        // Keep non-zero dimensions so nested panel children never collapse to zero viewports.
        window->NotifyReshape(64, 64);
    }
    CapturedMouseWindow.reset();
}

auto FLabV2WindowManager::SaveWorkspacePanelVisibility(const EWorkspaceTab workspace) -> void {
    const auto index = static_cast<std::size_t>(workspace);
    WorkspacePanels[index] = FWorkspacePanelVisibility{
        bShowWindowLab,
        bShowWindowSimulationLauncher,
        bShowWindowTasks,
        bShowWindowLiveData,
        bShowWindowLiveControl,
        bShowWindowLiveInteraction,
        bShowWindowViews,
        bShowWindowSchemeInspector,
        bShowWindowBlueprintGraph,
        bShowWindowGraphPlayground,
        bShowWindowPlotInspector
    };
}

auto FLabV2WindowManager::LoadWorkspacePanelVisibility(const EWorkspaceTab workspace) -> void {
    const auto index = static_cast<std::size_t>(workspace);
    const auto &cfg = WorkspacePanels[index];
    bShowWindowLab = cfg.bShowWindowLab;
    bShowWindowSimulationLauncher = cfg.bShowWindowSimulationLauncher;
    bShowWindowTasks = cfg.bShowWindowTasks;
    bShowWindowLiveData = cfg.bShowWindowLiveData;
    bShowWindowLiveControl = cfg.bShowWindowLiveControl;
    bShowWindowLiveInteraction = cfg.bShowWindowLiveInteraction;
    bShowWindowViews = cfg.bShowWindowViews;
    bShowWindowSchemeInspector = cfg.bShowWindowSchemeInspector;
    bShowWindowBlueprintGraph = cfg.bShowWindowBlueprintGraph;
    bShowWindowGraphPlayground = cfg.bShowWindowGraphPlayground;
    bShowWindowPlotInspector = cfg.bShowWindowPlotInspector;
}

auto FLabV2WindowManager::RequestSimulationLauncherVisible() -> void {
    if (ActiveWorkspace != EWorkspaceTab::Simulations) {
        SetActiveWorkspace(EWorkspaceTab::Simulations);
    }
    bShowWindowSimulationLauncher = true;
    if (LauncherInitialDockId != 0) {
        bRequestLauncherInitialDock = true;
    }
}

auto FLabV2WindowManager::SetActiveWorkspace(const EWorkspaceTab workspace) -> void {
    if (ActiveWorkspace == workspace) return;
    const auto previousWorkspace = ActiveWorkspace;
    SaveWorkspacePanelVisibility(ActiveWorkspace);
    ActiveWorkspace = workspace;
    const bool bPreviousHasSlabWindows =
        previousWorkspace == EWorkspaceTab::Simulations || previousWorkspace == EWorkspaceTab::Plots;
    const bool bCurrentHasSlabWindows =
        ActiveWorkspace == EWorkspaceTab::Simulations || ActiveWorkspace == EWorkspaceTab::Plots;
    if (!bCurrentHasSlabWindows || previousWorkspace != ActiveWorkspace) {
        CapturedMouseWindow.reset();
    }
    LoadWorkspacePanelVisibility(ActiveWorkspace);
    if (bPreviousHasSlabWindows || bCurrentHasSlabWindows) {
        RequestViewRetile();
    }
}

auto FLabV2WindowManager::GetTopMenuInset() const -> float {
    if (ImGuiContext == nullptr) {
        return static_cast<float>(Slab::Graphics::WindowStyle::GlobalMenuHeight);
    }

    if (ImGuiContext->GetMainMenuPresentation() == Slab::Graphics::FImGuiContext::EMainMenuPresentation::Hidden) {
        return 0.0f;
    }

    const auto measured = ImGui::GetFrameHeight();
    if (measured > 1.0f) return measured;
    return static_cast<float>(Slab::Graphics::WindowStyle::GlobalMenuHeight);
}

auto FLabV2WindowManager::DrawWorkspaceLauncher() -> void {
    if (!IsDockingEnabled()) return;
    if (ImGuiContext == nullptr) return;

    const auto *viewport = ImGui::GetMainViewport();
    if (viewport == nullptr) return;

    const auto menuHeight = GetTopMenuInset();
    const auto tabsHeight = ImGui::GetFrameHeight() + 8.0f;
    const auto stripHeight = ImGui::GetFrameHeight() + 8.0f;
    const auto totalHeight = tabsHeight + stripHeight;
    const auto desiredWidth = std::max(44.0f, totalHeight);

    ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x, viewport->Pos.y + menuHeight), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(desiredWidth, totalHeight), ImGuiCond_Always);
    ImGui::SetNextWindowViewport(viewport->ID);

    constexpr auto flags =
        ImGuiWindowFlags_NoDocking |
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoSavedSettings |
        ImGuiWindowFlags_NoNavFocus;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(4.0f, 4.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(0.0f, 0.0f));

    const auto previousWidth = WorkspaceLauncherWidth;
    if (ImGui::Begin("##LabWorkspaceLauncher", nullptr, flags)) {
        const auto buttonSize = ImVec2(
            std::max(16.0f, ImGui::GetContentRegionAvail().x),
            std::max(16.0f, ImGui::GetContentRegionAvail().y));
        (void) ImGuiContext->DrawMainMenuLauncher("LabMainMenuLauncher", buttonSize);
        WorkspaceLauncherWidth = std::max(0.0f, ImGui::GetWindowWidth());
    }
    ImGui::End();

    ImGui::PopStyleVar(4);

    if (std::abs(previousWidth - WorkspaceLauncherWidth) > 0.5f) {
        RequestViewRetile();
    }
}

auto FLabV2WindowManager::DrawWorkspaceTabs() -> void {
    if (!IsDockingEnabled()) return;

    const auto *viewport = ImGui::GetMainViewport();
    if (viewport == nullptr) return;

    const auto menuHeight = GetTopMenuInset();
    const float estimatedHeight = ImGui::GetFrameHeight() + 8.0f;
    const auto leftInset = std::max(0.0f, WorkspaceLauncherWidth);

    ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x + leftInset, viewport->Pos.y + menuHeight), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(std::max(0.0f, viewport->Size.x - leftInset), estimatedHeight), ImGuiCond_Always);
    ImGui::SetNextWindowViewport(viewport->ID);

    constexpr auto flags =
        ImGuiWindowFlags_NoDocking |
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoScrollWithMouse |
        ImGuiWindowFlags_NoSavedSettings |
        ImGuiWindowFlags_NoNavFocus;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8.0f, 3.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(0.0f, 0.0f));

    const auto previousHeight = WorkspaceTabsHeight;
    WorkspaceTabsHeight = estimatedHeight;
    if (ImGui::Begin("##LabWorkspaceTabs", nullptr, flags)) {
        auto selected = ActiveWorkspace;
        if (ImGui::BeginTabBar("##WorkspaceTabBar",
            ImGuiTabBarFlags_FittingPolicyResizeDown | ImGuiTabBarFlags_NoCloseWithMiddleMouseButton)) {

            const auto drawTab = [&selected](const EWorkspaceTab workspace, const char *label) {
                if (ImGui::BeginTabItem(label)) {
                    selected = workspace;
                    ImGui::EndTabItem();
                }
            };

            drawTab(EWorkspaceTab::Simulations, WorkspaceTabSimulations);
            drawTab(EWorkspaceTab::Schemes, WorkspaceTabSchemes);
            drawTab(EWorkspaceTab::GraphPlayground, WorkspaceTabGraphPlayground);
            drawTab(EWorkspaceTab::Plots, WorkspaceTabPlots);

            ImGui::EndTabBar();
        }

        if (selected != ActiveWorkspace) SetActiveWorkspace(selected);
    }
    ImGui::End();
    ImGui::PopStyleVar(4);

    if (std::abs(previousHeight - WorkspaceTabsHeight) > 0.5f) {
        RequestViewRetile();
    }
}

auto FLabV2WindowManager::DrawWorkspaceStrip() -> void {
    if (!IsDockingEnabled()) return;

    const auto *viewport = ImGui::GetMainViewport();
    if (viewport == nullptr) return;

    const auto menuHeight = GetTopMenuInset();
    const float estimatedHeight = ImGui::GetFrameHeight() + 8.0f;
    const auto yOffset = menuHeight + WorkspaceTabsHeight;
    const auto leftInset = std::max(0.0f, WorkspaceLauncherWidth);

    ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x + leftInset, viewport->Pos.y + yOffset), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(std::max(0.0f, viewport->Size.x - leftInset), estimatedHeight), ImGuiCond_Always);
    ImGui::SetNextWindowViewport(viewport->ID);

    constexpr auto flags =
        ImGuiWindowFlags_NoDocking |
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoSavedSettings |
        ImGuiWindowFlags_NoNavFocus;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8.0f, 2.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(0.0f, 0.0f));

    const auto previousHeight = WorkspaceStripHeight;
    WorkspaceStripHeight = estimatedHeight;
    if (ImGui::Begin("##LabWorkspaceStrip", nullptr, flags)) {
        const char *workspaceLabel = WorkspaceTabSimulations;
        if (ActiveWorkspace == EWorkspaceTab::Schemes) workspaceLabel = WorkspaceTabSchemes;
        if (ActiveWorkspace == EWorkspaceTab::GraphPlayground) workspaceLabel = WorkspaceTabGraphPlayground;
        if (ActiveWorkspace == EWorkspaceTab::Plots) workspaceLabel = WorkspaceTabPlots;

        ImGui::TextDisabled("%s", workspaceLabel);
        ImGui::SameLine();
        ImGui::TextDisabled("|");
        ImGui::SameLine();

        const auto drawToggle = [](const char *label, bool *value) {
            ImGui::Checkbox(label, value);
            ImGui::SameLine();
        };

        if (ActiveWorkspace == EWorkspaceTab::Simulations) {
            drawToggle("Launcher", &bShowWindowSimulationLauncher);
            drawToggle("Views", &bShowWindowViews);
            drawToggle("Tasks", &bShowWindowTasks);
            drawToggle("Live Data", &bShowWindowLiveData);
            drawToggle("Live Control", &bShowWindowLiveControl);
            drawToggle("Live Interaction", &bShowWindowLiveInteraction);
        } else if (ActiveWorkspace == EWorkspaceTab::Schemes) {
            drawToggle("Inspector", &bShowWindowSchemeInspector);
            drawToggle("Blueprint Graph", &bShowWindowBlueprintGraph);
        } else if (ActiveWorkspace == EWorkspaceTab::GraphPlayground) {
            drawToggle("Graph Playground", &bShowWindowGraphPlayground);
        } else {
            drawToggle("Plot Inspector", &bShowWindowPlotInspector);
        }

        if (ImGui::Button("Reset Layout")) {
            bResetDockLayoutRequested = true;
        }
    }
    ImGui::End();

    ImGui::PopStyleVar(4);

    if (std::abs(previousHeight - WorkspaceStripHeight) > 0.5f) {
        RequestViewRetile();
    }
}

auto FLabV2WindowManager::BuildDefaultDockLayout(const unsigned int dockspaceId,
                                                 const EWorkspaceTab workspace) -> void {
#ifdef IMGUI_HAS_DOCK
    const auto dockId = static_cast<ImGuiID>(dockspaceId);
    if (dockId == 0) return;

    const auto *viewport = ImGui::GetMainViewport();
    if (viewport == nullptr) return;

    const auto menuHeight = GetTopMenuInset();
    const auto topChromeHeight = menuHeight + WorkspaceTabsHeight + WorkspaceStripHeight;
    const auto dockSize = ImVec2(
        viewport->Size.x,
        std::max(0.0f, viewport->Size.y - topChromeHeight));

    ImGui::DockBuilderRemoveNode(dockId);
    ImGui::DockBuilderAddNode(
        dockId,
        static_cast<ImGuiDockNodeFlags>(
            static_cast<int>(ImGuiDockNodeFlags_DockSpace) |
            static_cast<int>(ImGuiDockNodeFlags_PassthruCentralNode) |
            static_cast<int>(ImGuiDockNodeFlags_AutoHideTabBar)));
    ImGui::DockBuilderSetNodeSize(dockId, dockSize);

    ImGuiID dockMain = dockId;
    if (workspace == EWorkspaceTab::Simulations) {
        const auto dockLeft = ImGui::DockBuilderSplitNode(dockMain, ImGuiDir_Left, 0.24f, nullptr, &dockMain);
        const auto dockRight = ImGui::DockBuilderSplitNode(dockMain, ImGuiDir_Right, 0.28f, nullptr, &dockMain);
        const auto dockBottom = ImGui::DockBuilderSplitNode(dockMain, ImGuiDir_Down, 0.30f, nullptr, &dockMain);

        LauncherInitialDockId = static_cast<unsigned int>(dockLeft);
        bRequestLauncherInitialDock = LauncherInitialDockId != 0;

        ImGui::DockBuilderDockWindow(WindowTitleViews, dockLeft);
        ImGui::DockBuilderDockWindow(WindowTitleSimulationLauncher, dockLeft);
        ImGui::DockBuilderDockWindow(WindowTitleLiveInteraction, dockRight);
        ImGui::DockBuilderDockWindow(WindowTitleTasks, dockBottom);
        ImGui::DockBuilderDockWindow(WindowTitleLiveData, dockBottom);
        ImGui::DockBuilderDockWindow(WindowTitleLiveControl, dockBottom);
    } else if (workspace == EWorkspaceTab::Schemes) {
        const auto dockLeft = ImGui::DockBuilderSplitNode(dockMain, ImGuiDir_Left, 0.35f, nullptr, &dockMain);
        ImGui::DockBuilderDockWindow(WindowTitleSchemesInspector, dockLeft);
        ImGui::DockBuilderDockWindow(WindowTitleBlueprintGraph, dockMain);
    } else if (workspace == EWorkspaceTab::GraphPlayground) {
        ImGui::DockBuilderDockWindow(WindowTitleGraphPlayground, dockMain);
    } else {
        ImGui::DockBuilderDockWindow(WindowTitlePlotInspector, dockMain);
    }

    ImGui::DockBuilderFinish(dockId);
#else
    (void) dockspaceId;
    (void) workspace;
#endif
}

auto FLabV2WindowManager::DrawDockspaceHost() -> void {
    if (!IsDockingEnabled()) return;

    const auto menuHeight = GetTopMenuInset();
    const auto topChromeHeight = menuHeight + WorkspaceTabsHeight + WorkspaceStripHeight;
    const auto *viewport = ImGui::GetMainViewport();
    if (viewport == nullptr) return;

    ImGui::SetNextWindowPos(
        ImVec2(viewport->Pos.x, viewport->Pos.y + topChromeHeight),
        ImGuiCond_Always);
    ImGui::SetNextWindowSize(
        ImVec2(viewport->Size.x, std::max(0.0f, viewport->Size.y - topChromeHeight)),
        ImGuiCond_Always);
    ImGui::SetNextWindowViewport(viewport->ID);

    constexpr auto hostFlags =
        ImGuiWindowFlags_NoDocking |
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoBringToFrontOnFocus |
        ImGuiWindowFlags_NoNavFocus |
        ImGuiWindowFlags_NoBackground;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

    if (ImGui::Begin(DockspaceHostName, nullptr, hostFlags)) {
        const auto dockspaceIdSimulations =
            static_cast<unsigned int>(ImGui::GetID(DockspaceNameSimulations));
        const auto dockspaceIdSchemes =
            static_cast<unsigned int>(ImGui::GetID(DockspaceNameSchemes));
        const auto dockspaceIdGraphPlayground =
            static_cast<unsigned int>(ImGui::GetID(DockspaceNameGraphPlayground));
        const auto dockspaceIdPlots =
            static_cast<unsigned int>(ImGui::GetID(DockspaceNamePlots));

        if (!bWorkspaceLayoutsBootstrapped) {
            BuildDefaultDockLayout(dockspaceIdSimulations, EWorkspaceTab::Simulations);
            BuildDefaultDockLayout(dockspaceIdSchemes, EWorkspaceTab::Schemes);
            BuildDefaultDockLayout(dockspaceIdGraphPlayground, EWorkspaceTab::GraphPlayground);
            BuildDefaultDockLayout(dockspaceIdPlots, EWorkspaceTab::Plots);
            WorkspaceLayoutInitialized[static_cast<std::size_t>(EWorkspaceTab::Simulations)] = true;
            WorkspaceLayoutInitialized[static_cast<std::size_t>(EWorkspaceTab::Schemes)] = true;
            WorkspaceLayoutInitialized[static_cast<std::size_t>(EWorkspaceTab::GraphPlayground)] = true;
            WorkspaceLayoutInitialized[static_cast<std::size_t>(EWorkspaceTab::Plots)] = true;
            bWorkspaceLayoutsBootstrapped = true;
            RequestViewRetile();
        }

        const auto dockspaceIdFor = [dockspaceIdSimulations, dockspaceIdSchemes, dockspaceIdGraphPlayground, dockspaceIdPlots]
            (const EWorkspaceTab workspace) -> unsigned int {
                if (workspace == EWorkspaceTab::Schemes) return dockspaceIdSchemes;
                if (workspace == EWorkspaceTab::GraphPlayground) return dockspaceIdGraphPlayground;
                if (workspace == EWorkspaceTab::Plots) return dockspaceIdPlots;
                return dockspaceIdSimulations;
            };

        DockspaceId = dockspaceIdFor(ActiveWorkspace);
        const auto workspaceIndex = static_cast<std::size_t>(ActiveWorkspace);
        if (bResetDockLayoutRequested) {
            BuildDefaultDockLayout(DockspaceId, ActiveWorkspace);
            WorkspaceLayoutInitialized[workspaceIndex] = true;
            bResetDockLayoutRequested = false;
            RequestViewRetile();
        }
#ifdef IMGUI_HAS_DOCK
        const auto drawWorkspaceDockspace = [this](const unsigned int id, const EWorkspaceTab workspace) {
            ImGuiDockNodeFlags dockFlags = static_cast<ImGuiDockNodeFlags>(
                static_cast<int>(ImGuiDockNodeFlags_PassthruCentralNode) |
                static_cast<int>(ImGuiDockNodeFlags_AutoHideTabBar));
            if (workspace != ActiveWorkspace) {
                // Keep inactive workspaces alive so docked windows don't detach on tab switch.
                dockFlags = static_cast<ImGuiDockNodeFlags>(
                    static_cast<int>(ImGuiDockNodeFlags_KeepAliveOnly) |
                    static_cast<int>(ImGuiDockNodeFlags_AutoHideTabBar));
            }

            ImGui::DockSpace(static_cast<ImGuiID>(id), ImVec2(0.0f, 0.0f), dockFlags);
        };

        drawWorkspaceDockspace(dockspaceIdSimulations, EWorkspaceTab::Simulations);
        drawWorkspaceDockspace(dockspaceIdSchemes, EWorkspaceTab::Schemes);
        drawWorkspaceDockspace(dockspaceIdGraphPlayground, EWorkspaceTab::GraphPlayground);
        drawWorkspaceDockspace(dockspaceIdPlots, EWorkspaceTab::Plots);
#endif
    }
    ImGui::End();

    ImGui::PopStyleVar(3);
}

auto FLabV2WindowManager::BuildPanelSurfaceRegistry() -> std::vector<FPanelSurfaceRegistration> {
    std::vector<FPanelSurfaceRegistration> registry;
    registry.reserve(12);

    registry.push_back(FPanelSurfaceRegistration{
        WindowTitleLab,
        EWorkspaceTab::Simulations,
        &bShowWindowLab,
        false,
        true,
        [this]() {
            ImGui::TextDisabled("Observability + launcher shell");
            ImGui::SeparatorText("Workspace");
            ImGui::Checkbox("Enable dockspace layout", &bUseDockspaceLayout);
            if (ImGui::Button("Reset Dock Layout")) {
                bResetDockLayoutRequested = true;
            }
            ImGui::SeparatorText("Panels");
            if (ActiveWorkspace == EWorkspaceTab::Simulations) {
                ImGui::Checkbox("Simulation Launcher", &bShowWindowSimulationLauncher);
            }
            ImGui::Checkbox("Tasks", &bShowWindowTasks);
            ImGui::Checkbox("Live Data", &bShowWindowLiveData);
            ImGui::Checkbox("Live Control", &bShowWindowLiveControl);
            ImGui::Checkbox("Live Interaction", &bShowWindowLiveInteraction);
            ImGui::Checkbox("Views", &bShowWindowViews);
            if (ActiveWorkspace == EWorkspaceTab::Schemes) {
                ImGui::Checkbox("Interface Inspector", &bShowWindowSchemeInspector);
                ImGui::Checkbox("Blueprint Graph", &bShowWindowBlueprintGraph);
            } else if (ActiveWorkspace == EWorkspaceTab::GraphPlayground) {
                ImGui::Checkbox("Graph Playground", &bShowWindowGraphPlayground);
            } else if (ActiveWorkspace == EWorkspaceTab::Plots) {
                ImGui::Checkbox("Plot Inspector", &bShowWindowPlotInspector);
            }
        }
    });

    registry.push_back(FPanelSurfaceRegistration{
        WindowTitleSimulationLauncher,
        EWorkspaceTab::Simulations,
        &bShowWindowSimulationLauncher,
        false,
        false,
        [this]() {
            if (SimulationManager != nullptr) {
                SimulationManager->DrawLauncherContents();
            }
        }
    });

    registry.push_back(FPanelSurfaceRegistration{
        WindowTitleViews,
        EWorkspaceTab::Simulations,
        &bShowWindowViews,
        false,
        true,
        [this]() {
            DrawViewManagerPanel();
        }
    });

    registry.push_back(FPanelSurfaceRegistration{
        WindowTitleTasks,
        EWorkspaceTab::Simulations,
        &bShowWindowTasks,
        false,
        false,
        [this]() {
            Slab::Studios::LabV2::Panels::ShowTasksPanel(TaskNameFilter, bTaskOnlyRunning, bTaskHideSuccess, bTaskOnlyNumeric);
        }
    });

    registry.push_back(FPanelSurfaceRegistration{
        WindowTitleLiveData,
        EWorkspaceTab::Simulations,
        &bShowWindowLiveData,
        false,
        true,
        [this]() {
            Slab::Studios::LabV2::Panels::ShowLiveDataV2Panel(LiveDataHub, LiveDataTopicFilter, bLiveDataOnlyBound, SelectedLiveDataTopic);
        }
    });

    registry.push_back(FPanelSurfaceRegistration{
        WindowTitleLiveControl,
        EWorkspaceTab::Simulations,
        &bShowWindowLiveControl,
        false,
        true,
        [this]() {
            Slab::Studios::LabV2::Panels::ShowLiveControlV2Panel(
                LiveControlHub,
                LiveControlTopicFilter,
                bLiveControlLevelsOnly,
                SelectedLiveControlTopic);
        }
    });

    registry.push_back(FPanelSurfaceRegistration{
        WindowTitleLiveInteraction,
        EWorkspaceTab::Simulations,
        &bShowWindowLiveInteraction,
        false,
        true,
        [this]() {
            Slab::Studios::LabV2::Panels::ShowLiveInteractionPanel(
                LiveControlHub,
                LiveInteractionTopicFilter);
        }
    });

    registry.push_back(FPanelSurfaceRegistration{
        WindowTitleSchemesInspector,
        EWorkspaceTab::Schemes,
        &bShowWindowSchemeInspector,
        false,
        false,
        [this]() {
            DrawSchemesInspectorPanel();
        }
    });

    registry.push_back(FPanelSurfaceRegistration{
        WindowTitleBlueprintGraph,
        EWorkspaceTab::Schemes,
        &bShowWindowBlueprintGraph,
        false,
        false,
        [this]() {
            DrawSchemesBlueprintGraphPanel();
        }
    });

    registry.push_back(FPanelSurfaceRegistration{
        WindowTitleGraphPlayground,
        EWorkspaceTab::GraphPlayground,
        &bShowWindowGraphPlayground,
        false,
        false,
        [this]() {
            DrawGraphPlaygroundPanel();
        }
    });

    registry.push_back(FPanelSurfaceRegistration{
        WindowTitlePlotInspector,
        EWorkspaceTab::Plots,
        &bShowWindowPlotInspector,
        false,
        false,
        [this]() {
            DrawPlotsInspectorPanel();
        }
    });

    return registry;
}

auto FLabV2WindowManager::DrawPanelSurface(const FPanelSurfaceRegistration &registration) -> void {
    if (registration.Workspace != ActiveWorkspace) return;

    const bool bVisible = registration.bForceVisibleInWorkspace ||
        (registration.bVisible != nullptr && *registration.bVisible);
    if (!bVisible) return;

    if (std::strcmp(registration.WindowTitle, WindowTitleSimulationLauncher) == 0) {
#ifdef IMGUI_HAS_DOCK
        if (bRequestLauncherInitialDock && LauncherInitialDockId != 0) {
            ImGui::SetNextWindowDockID(static_cast<ImGuiID>(LauncherInitialDockId), ImGuiCond_Always);
            ImGui::SetNextWindowFocus();
        }
#endif
    }

    const auto openPtr = registration.bForceVisibleInWorkspace ? nullptr : registration.bVisible;
    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoCollapse;
    if (IsDockingEnabled() && registration.bHideTitleBarWhenDocked) {
        windowFlags |= ImGuiWindowFlags_NoTitleBar;
    }

    if (ImGui::Begin(registration.WindowTitle, openPtr, windowFlags)) {
        if (registration.DrawContents) registration.DrawContents();

        if (std::strcmp(registration.WindowTitle, WindowTitleSimulationLauncher) == 0) {
#ifdef IMGUI_HAS_DOCK
            if (bRequestLauncherInitialDock && ImGui::IsWindowDocked()) {
                bRequestLauncherInitialDock = false;
            }
#else
            bRequestLauncherInitialDock = false;
#endif
        }
    }
    ImGui::End();
}

auto FLabV2WindowManager::DrawDockedToolWindows() -> void {
    const auto registry = BuildPanelSurfaceRegistry();
    for (const auto &registration : registry) {
        DrawPanelSurface(registration);
    }
    SaveWorkspacePanelVisibility(ActiveWorkspace);
}

auto FLabV2WindowManager::DrawLegacySidePane() -> void {
    const auto menuHeight = GetTopMenuInset();
    ImGui::SetNextWindowPos(ImVec2(0, menuHeight));
    ImGui::SetNextWindowSize(
        ImVec2(static_cast<float>(SidePaneWidth), std::max(0.0f, static_cast<float>(HeightSysWin) - menuHeight)),
        ImGuiCond_Appearing);

    constexpr auto flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar;
    if (ImGui::Begin(FStudioConfigV2::SidePaneId, nullptr, flags)) {
        ImGui::SeparatorText("Lab");
        ImGui::TextDisabled("Observability + launcher shell");

        ImGui::SeparatorText("Panels");
        ImGui::Checkbox("Simulation Launcher", &bShowWindowSimulationLauncher);
        ImGui::Checkbox("Tasks", &bShowWindowTasks);
        ImGui::Checkbox("Live Data", &bShowWindowLiveData);
        ImGui::Checkbox("Live Control", &bShowWindowLiveControl);
        ImGui::Checkbox("Live Interaction", &bShowWindowLiveInteraction);
        ImGui::Checkbox("Views", &bShowWindowViews);

        if (bShowWindowTasks) {
            Slab::Studios::LabV2::Panels::ShowTasksPanel(TaskNameFilter, bTaskOnlyRunning, bTaskHideSuccess, bTaskOnlyNumeric);
        }

        if (bShowWindowLiveData) {
            Slab::Studios::LabV2::Panels::ShowLiveDataV2Panel(LiveDataHub, LiveDataTopicFilter, bLiveDataOnlyBound, SelectedLiveDataTopic);
        }

        if (bShowWindowLiveControl) {
            Slab::Studios::LabV2::Panels::ShowLiveControlV2Panel(
                LiveControlHub,
                LiveControlTopicFilter,
                bLiveControlLevelsOnly,
                SelectedLiveControlTopic);
        }

        if (bShowWindowLiveInteraction) {
            Slab::Studios::LabV2::Panels::ShowLiveInteractionPanel(
                LiveControlHub,
                LiveInteractionTopicFilter);
        }

        if (bShowWindowViews) {
            DrawViewManagerPanel();
        }

        if (const auto windowWidth = static_cast<int>(ImGui::GetWindowWidth()); SidePaneWidth != windowWidth) {
            SidePaneWidth = windowWidth;
            NotifySystemWindowReshape(WidthSysWin, HeightSysWin);
        }
    }
    ImGui::End();

    SaveWorkspacePanelVisibility(ActiveWorkspace);
}

bool FLabV2WindowManager::NotifyRender(const Slab::Graphics::FPlatformWindow &platformWindow) {
    FlushPendingSlabWindows();

    platformWindow.Clear(Slab::Graphics::WindowStyle::PlatformWindow_BackgroundColor);

    ImGuiContext->NewFrame();
    ImGuiContext->SetupOptionalMenuItems();
    if (SimulationManager != nullptr) {
        SimulationManager->AddMenus(platformWindow);
    }
    AddExitMenuEntry(platformWindow, *ImGuiContext);

    if (IsDockingEnabled()) {
        DrawWorkspaceLauncher();
        DrawWorkspaceTabs();
        DrawWorkspaceStrip();
        DrawDockspaceHost();
        DrawDockedToolWindows();
    } else {
        WorkspaceTabsHeight = 0.0f;
        WorkspaceStripHeight = 0.0f;
        WorkspaceLauncherWidth = 52.0f;
        DrawLegacySidePane();
        if (bShowWindowSimulationLauncher) {
            ImGui::SetNextWindowPos(
                ImVec2(
                    static_cast<float>(FStudioConfigV2::SidePaneWidth + 24),
                    GetTopMenuInset() + 24.0f),
                ImGuiCond_Appearing);
            ImGui::SetNextWindowSize(ImVec2(520, 560), ImGuiCond_Appearing);
            if (ImGui::Begin(WindowTitleSimulationLauncher, &bShowWindowSimulationLauncher)) {
                if (SimulationManager != nullptr) {
                    SimulationManager->DrawLauncherContents();
                }
            }
            ImGui::End();
        }
    }

    SyncPlotWorkspaceWindows();
    FlushPendingSlabWindows();

    if (PruneClosedSlabWindows()) {
        RequestViewRetile();
    }

    const bool bRenderSlabWindows = ShouldRenderSlabWindowsInWorkspace();
    const bool bForceWorkspaceDockRetile =
        IsDockingEnabled() &&
        (ActiveWorkspace == EWorkspaceTab::Simulations || ActiveWorkspace == EWorkspaceTab::Plots);

    if (bRenderSlabWindows && (bForceWorkspaceDockRetile || bPendingViewRetile || RetileStabilizationFramesRemaining > 0)) {
        if (ArrangeTopLevelSlabWindows()) {
            if (RetileStabilizationFramesRemaining > 0) {
                --RetileStabilizationFramesRemaining;
            }

            if (RetileStabilizationFramesRemaining == 0) {
                bPendingViewRetile = false;
            }
        } else {
            bPendingViewRetile = true;
            RetileStabilizationFramesRemaining = std::max(RetileStabilizationFramesRemaining, 1);
        }
    } else if (!bRenderSlabWindows) {
        HideSlabWindowsOffscreen();
    }

    if (bRenderSlabWindows) {
        FWindowManager::NotifyRender(platformWindow);
    }
    FlushPendingSlabWindows();
    ImGuiContext->Render();
    return true;
}

bool FLabV2WindowManager::NotifySystemWindowReshape(const int w, const int h) {
    WidthSysWin = w;
    HeightSysWin = h;
    const auto responded = FWindowManager::NotifySystemWindowReshape(w, h);
    RequestViewRetile();
    return responded;
}

bool FLabV2WindowManager::NotifyKeyboard(const Slab::Graphics::EKeyMap key,
                                         const Slab::Graphics::EKeyState state,
                                         const Slab::Graphics::EModKeys modKeys) {
    const bool bGuiHandled =
        ImGuiContext != nullptr &&
        ImGuiContext->NotifyKeyboard(key, state, modKeys);

    if (bGuiHandled) return true;

    const auto target = FindKeyboardTargetWindow();
    if (target == nullptr) return false;
    return target->NotifyKeyboard(key, state, modKeys);
}

bool FLabV2WindowManager::NotifyCharacter(const Slab::UInt codepoint) {
    const bool bGuiHandled =
        ImGuiContext != nullptr &&
        ImGuiContext->NotifyCharacter(codepoint);

    if (bGuiHandled) return true;

    const auto target = FindKeyboardTargetWindow();
    if (target == nullptr) return false;
    return target->NotifyCharacter(codepoint);
}

bool FLabV2WindowManager::NotifyMouseButton(const Slab::Graphics::EMouseButton button,
                                            const Slab::Graphics::EKeyState state,
                                            const Slab::Graphics::EModKeys modKeys) {
    SyncMousePositionFromImGui();

    const bool bGuiHandled =
        ImGuiContext != nullptr &&
        ImGuiContext->NotifyMouseButton(button, state, modKeys);

    // Always release an already captured slab window to keep button-state tracking coherent,
    // even when an overlay ImGui window captures this release event.
    if (state == Slab::Graphics::EKeyState::Release) {
        const auto captured = CapturedMouseWindow.lock();
        CapturedMouseWindow.reset();

        const bool bCapturedReleaseHandled =
            captured != nullptr &&
            captured->NotifyMouseButton(button, state, modKeys);

        if (bGuiHandled) return true;
        if (bCapturedReleaseHandled) return true;

        if (!bHasLastMousePosition) return false;
        const auto target = FindTopWindowAtPoint(LastMouseX, LastMouseY);
        return target != nullptr && target->NotifyMouseButton(button, state, modKeys);
    }

    if (bGuiHandled) return true;
    if (!bHasLastMousePosition) return false;

    const auto target = FindTopWindowAtPoint(LastMouseX, LastMouseY);
    if (target != nullptr) {
        CapturedMouseWindow = target;
    }

    return target != nullptr && target->NotifyMouseButton(button, state, modKeys);
}

bool FLabV2WindowManager::NotifyMouseMotion(const int x, const int y, const int dx, const int dy) {
    bHasLastMousePosition = true;
    LastMouseX = x;
    LastMouseY = y;

    const bool bGuiHandled =
        ImGuiContext != nullptr &&
        ImGuiContext->NotifyMouseMotion(x, y, dx, dy);

    if (bGuiHandled) return true;

    auto target = CapturedMouseWindow.lock();
    if (target == nullptr) {
        target = FindTopWindowAtPoint(x, y);
    }

    const bool bWindowHandled =
        target != nullptr &&
        target->NotifyMouseMotion(x, y, dx, dy);

    return bWindowHandled;
}

bool FLabV2WindowManager::NotifyMouseWheel(const double dx, const double dy) {
    SyncMousePositionFromImGui();

    const bool bGuiHandled =
        ImGuiContext != nullptr &&
        ImGuiContext->NotifyMouseWheel(dx, dy);

    if (bGuiHandled) return true;

    auto target = CapturedMouseWindow.lock();
    if (target == nullptr && bHasLastMousePosition) {
        target = FindTopWindowAtPoint(LastMouseX, LastMouseY);
    }

    const bool bWindowHandled =
        target != nullptr &&
        target->NotifyMouseWheel(dx, dy);

    return bWindowHandled;
}

auto FLabV2WindowManager::GetImGuiContext() const -> Slab::TPointer<Slab::Graphics::FImGuiContext> {
    return ImGuiContext;
}

auto FLabV2WindowManager::GetLiveDataHub() const -> Slab::TPointer<Slab::Math::LiveData::V2::FLiveDataHubV2> {
    return LiveDataHub;
}

auto FLabV2WindowManager::GetLiveControlHub() const -> Slab::TPointer<Slab::Math::LiveControl::V2::FLiveControlHubV2> {
    return LiveControlHub;
}
