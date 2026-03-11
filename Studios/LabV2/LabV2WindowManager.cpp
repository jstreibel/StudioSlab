#include "LabV2WindowManager.h"

#include "LabV2GraphPlaygroundController.h"
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
#include "Core/Model/V2/ModelSeedsV2.h"
#include "Core/Reflection/V2/ReflectionCodecsV2.h"
#include "Core/Reflection/V2/GraphSubstrateV2.h"
#include "Core/Reflection/V2/ReflectionInvokeV2.h"
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
#include "Graphics/Typesetting/TypesettingService.h"
#include "Graphics/Window/WindowStyles.h"
#include "Math/Data/V2/SessionLiveViewV2.h"
#include "Math/Function/RtoR/Model/FunctionsCollection/Trigonometric.h"

#include <algorithm>
#include <cctype>
#include <cfloat>
#include <cmath>
#include <cstring>
#include <filesystem>
#include <functional>
#include <limits>
#include <sstream>
#include <utility>

namespace {

    namespace ModelV2 = Slab::Core::Model::V2;
    namespace Typesetting = Slab::Graphics::Typesetting;

    constexpr bool CEnableBlueprintGraphTuningUi = false;

    auto PrewarmModelTypesettingCache(Typesetting::FTypesettingService &service,
                                      const Slab::Vector<ModelV2::FModelV2> &models) -> void {
        const float baseFontSize =
            ImGui::GetCurrentContext() != nullptr && ImGui::GetFontSize() > 0.0f
                ? ImGui::GetFontSize()
                : 18.0f;

        const auto makeMathRequest = [&](const Slab::Str &latex, const float fontSize) {
            Typesetting::FTypesetStyle style;
            style.FontPixelSize = fontSize;
            return Typesetting::MakeMathRequest(latex, style);
        };

        for (const auto &model : models) {
            for (const auto &definition : model.Definitions) {
                service.ResolveRenderable(makeMathRequest(ModelV2::RenderDialectDefinitionV2(definition, &model), baseFontSize * 1.02f));
                if (definition.DeclaredType.has_value()) {
                    service.ResolveRenderable(makeMathRequest(ModelV2::RenderDialectTypeExprV2(*definition.DeclaredType, &model), baseFontSize));
                }
                if (!definition.SourceText.empty()) {
                    service.ResolveRenderable(makeMathRequest(definition.SourceText, baseFontSize * 1.12f));
                }
            }

            for (const auto &relation : model.Relations) {
                service.ResolveRenderable(makeMathRequest(ModelV2::RenderDialectRelationV2(relation, &model), baseFontSize * 1.04f));
                service.ResolveRenderable(makeMathRequest(ModelV2::RenderDialectExpressionV2(relation.Left, &model), baseFontSize));
                service.ResolveRenderable(makeMathRequest(ModelV2::RenderDialectExpressionV2(relation.Right, &model), baseFontSize));
                if (!relation.SourceText.empty()) {
                    service.ResolveRenderable(makeMathRequest(relation.SourceText, baseFontSize * 1.12f));
                }
            }
        }
    }

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
    constexpr auto WindowTitleModelInspector = "Model Layer";
    constexpr auto WindowTitleGraphPlayground = "Graph Playground";
    constexpr auto WindowTitlePlotInspector = "Plot Inspector";
    constexpr auto PopupBlueprintGraphContext = "SchemesBlueprintGraphContext";
    constexpr auto DockspaceHostName = "##LabDockspaceHost";
    constexpr auto DockspaceNameSimulations = "##LabDockspace-Simulations";
    constexpr auto DockspaceNameSchemes = "##LabDockspace-Schemes";
    constexpr auto DockspaceNameModels = "##LabDockspace-Models";
    constexpr auto DockspaceNameGraphPlayground = "##LabDockspace-GraphPlayground";
    constexpr auto DockspaceNamePlots = "##LabDockspace-Plots";
    constexpr auto WorkspaceTabSimulations = "Simulations";
    constexpr auto WorkspaceTabSchemes = "Schemes";
    constexpr auto WorkspaceTabModels = "Model";
    constexpr auto WorkspaceTabGraphPlayground = "Graph Playground";
    constexpr auto WorkspaceTabPlots = "Plots";
    constexpr auto PlotWindowInterfaceIdPrefix = "v2.plot.window.";

    [[nodiscard]] auto RoutingEdgeKindFromSelectionIndex(const int selectionIndex)
        -> Slab::Core::Reflection::V2::EGraphEdgeKindV2 {
        using Slab::Core::Reflection::V2::EGraphEdgeKindV2;
        switch (selectionIndex) {
            case 0:
                return EGraphEdgeKindV2::ValueFlow;
            case 1:
                return EGraphEdgeKindV2::HandleBinding;
            case 2:
                return EGraphEdgeKindV2::StreamSubscription;
            case 3:
                return EGraphEdgeKindV2::ControlDependency;
            default:
                return EGraphEdgeKindV2::ValueFlow;
        }
    }

    [[nodiscard]] auto RoutingEdgeKindToSelectionIndex(const Slab::Core::Reflection::V2::EGraphEdgeKindV2 kind) -> int {
        using Slab::Core::Reflection::V2::EGraphEdgeKindV2;
        switch (kind) {
            case EGraphEdgeKindV2::ValueFlow:
                return 0;
            case EGraphEdgeKindV2::HandleBinding:
                return 1;
            case EGraphEdgeKindV2::StreamSubscription:
                return 2;
            case EGraphEdgeKindV2::ControlDependency:
                return 3;
            default:
                return 0;
        }
    }

    [[nodiscard]] auto TryDecodeRoutingEdgeKindEnumValue(const int encodedValue,
                                                         Slab::Core::Reflection::V2::EGraphEdgeKindV2 &outKind) -> bool {
        using Slab::Core::Reflection::V2::EGraphEdgeKindV2;
        switch (encodedValue) {
            case static_cast<int>(EGraphEdgeKindV2::ValueFlow):
                outKind = EGraphEdgeKindV2::ValueFlow;
                return true;
            case static_cast<int>(EGraphEdgeKindV2::HandleBinding):
                outKind = EGraphEdgeKindV2::HandleBinding;
                return true;
            case static_cast<int>(EGraphEdgeKindV2::StreamSubscription):
                outKind = EGraphEdgeKindV2::StreamSubscription;
                return true;
            case static_cast<int>(EGraphEdgeKindV2::ControlDependency):
                outKind = EGraphEdgeKindV2::ControlDependency;
                return true;
            default:
                return false;
        }
    }

    [[nodiscard]] auto DecodeRoutingEdgeKindFromStoredValue(const int encodedValue,
                                                            const bool bEncodedAsEnumValue,
                                                            const Slab::Core::Reflection::V2::EGraphEdgeKindV2 fallback)
        -> Slab::Core::Reflection::V2::EGraphEdgeKindV2 {
        using Slab::Core::Reflection::V2::EGraphEdgeKindV2;
        EGraphEdgeKindV2 decodedKind = fallback;

        if (bEncodedAsEnumValue) {
            if (TryDecodeRoutingEdgeKindEnumValue(encodedValue, decodedKind)) return decodedKind;
            return fallback;
        }

        switch (encodedValue) {
            case 0:
                return EGraphEdgeKindV2::ValueFlow;
            case 1:
                return EGraphEdgeKindV2::HandleBinding;
            case 2:
                return EGraphEdgeKindV2::StreamSubscription;
            case 3:
                return EGraphEdgeKindV2::ControlDependency;
            default:
                break;
        }

        if (TryDecodeRoutingEdgeKindEnumValue(encodedValue, decodedKind)) return decodedKind;
        return fallback;
    }

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

    auto JsonWriteReflectionValueV2(const Slab::Core::Reflection::V2::FReflectionValueV2 &value) -> json::value {
        json::value encodedValue(json::object{});
        encodedValue["type_id"] = value.TypeId;
        encodedValue["encoded"] = value.Encoded;
        return encodedValue;
    }

    auto JsonReadReflectionValueV2(const json::value &value) -> Slab::Core::Reflection::V2::FReflectionValueV2 {
        using namespace Slab::Core::Reflection::V2;

        FReflectionValueV2 reflectionValue;
        if (!value.is_object()) return reflectionValue;
        reflectionValue.TypeId = JsonReadString(value, "type_id");
        reflectionValue.Encoded = JsonReadString(value, "encoded");
        return reflectionValue;
    }

    auto JsonWriteGraphMetadataV2(const Slab::Core::Reflection::V2::FGraphMetadataV2 &metadata) -> json::value {
        using namespace Slab::Core::Reflection::V2;

        json::value metadataValue(json::object{});
        metadataValue["description"] = metadata.Description;

        json::array tagsValue;
        tagsValue.reserve(metadata.Tags.size());
        for (const auto &tag : metadata.Tags) {
            if (tag.empty()) continue;
            tagsValue.emplace_back(tag);
        }
        metadataValue["tags"] = json::value(std::move(tagsValue));

        json::value attrsValue(json::object{});
        for (const auto &[attrKey, attrValue] : metadata.Attrs) {
            if (attrKey.empty()) continue;
            attrsValue[attrKey] = JsonWriteReflectionValueV2(attrValue);
        }
        metadataValue["attrs"] = std::move(attrsValue);
        return metadataValue;
    }

    auto JsonReadGraphMetadataV2(const json::value &value) -> Slab::Core::Reflection::V2::FGraphMetadataV2 {
        using namespace Slab::Core::Reflection::V2;

        FGraphMetadataV2 metadata;
        if (!value.is_object()) return metadata;

        metadata.Description = JsonReadString(value, "description");

        if (const auto *tagsValue = JsonTryGetValue(value, "tags"); tagsValue != nullptr && tagsValue->is_array()) {
            for (const auto &tagValue : tagsValue->get<json::array>()) {
                const auto *tag = tagValue.get_ptr<json::string>();
                if (tag == nullptr || tag->empty()) continue;
                metadata.Tags.push_back(*tag);
            }
        }

        if (const auto *attrsValue = JsonTryGetValue(value, "attrs"); attrsValue != nullptr && attrsValue->is_object()) {
            for (const auto &[attrKey, attrValue] : attrsValue->get<json::object>()) {
                if (attrKey.empty()) continue;
                metadata.Attrs[attrKey] = JsonReadReflectionValueV2(attrValue);
            }
        }

        return metadata;
    }

    auto JsonWriteGraphPortV2(const Slab::Core::Reflection::V2::FGraphPortV2 &port) -> json::value {
        using namespace Slab::Core::Reflection::V2;

        json::value portValue(json::object{});
        portValue["id"] = port.PortId;
        portValue["display_name"] = port.DisplayName;
        portValue["direction"] = static_cast<double>(static_cast<int>(port.Direction));
        portValue["required"] = port.bRequired;
        portValue["handle_id"] = port.Handle.HandleId;
        portValue["type_id"] = port.Handle.TypeId;
        portValue["space_id"] = port.Handle.SpaceId;
        portValue["metadata"] = JsonWriteGraphMetadataV2(port.Metadata);
        return portValue;
    }

    auto JsonReadGraphPortV2(const json::value &value,
                             Slab::Core::Reflection::V2::FGraphPortV2 &outPort) -> bool {
        using namespace Slab::Core::Reflection::V2;

        if (!value.is_object()) return false;

        outPort = FGraphPortV2{};
        outPort.PortId = JsonReadString(value, "id");
        outPort.DisplayName = JsonReadString(value, "display_name");
        const auto encodedDirection = static_cast<int>(JsonReadDouble(
            value,
            "direction",
            static_cast<double>(static_cast<int>(EGraphPortDirectionV2::Input))));
        outPort.Direction = encodedDirection == static_cast<int>(EGraphPortDirectionV2::Output)
            ? EGraphPortDirectionV2::Output
            : EGraphPortDirectionV2::Input;
        outPort.bRequired = JsonReadBool(value, "required", true);
        outPort.Handle.HandleId = JsonReadString(value, "handle_id");
        outPort.Handle.TypeId = JsonReadString(value, "type_id");
        outPort.Handle.SpaceId = JsonReadString(value, "space_id");
        if (const auto *metadataValue = JsonTryGetValue(value, "metadata"); metadataValue != nullptr) {
            outPort.Metadata = JsonReadGraphMetadataV2(*metadataValue);
        }

        return !outPort.PortId.empty();
    }

    auto JsonWriteGraphNodeV2(const Slab::Core::Reflection::V2::FGraphNodeV2 &node) -> json::value {
        using namespace Slab::Core::Reflection::V2;

        json::value nodeValue(json::object{});
        nodeValue["id"] = node.NodeId;
        nodeValue["display_name"] = node.DisplayName;
        nodeValue["kind"] = static_cast<double>(static_cast<int>(node.Kind));
        nodeValue["semantic_operator_id"] = node.SemanticOperatorId;
        nodeValue["source_interface_id"] = node.SourceInterfaceId;
        nodeValue["source_operation_id"] = node.SourceOperationId;
        nodeValue["source_parameter_id"] = node.SourceParameterId;
        nodeValue["position"] = JsonWriteVec2(ImVec2(node.Position.x, node.Position.y));
        nodeValue["metadata"] = JsonWriteGraphMetadataV2(node.Metadata);

        json::array portsValue;
        portsValue.reserve(node.Ports.size());
        for (const auto &port : node.Ports) {
            portsValue.push_back(JsonWriteGraphPortV2(port));
        }
        nodeValue["ports"] = json::value(std::move(portsValue));
        return nodeValue;
    }

    auto JsonReadGraphNodeV2(const json::value &value,
                             Slab::Core::Reflection::V2::FGraphNodeV2 &outNode) -> bool {
        using namespace Slab::Core::Reflection::V2;

        if (!value.is_object()) return false;

        outNode = FGraphNodeV2{};
        outNode.NodeId = JsonReadString(value, "id");
        if (outNode.NodeId.empty()) return false;

        outNode.DisplayName = JsonReadString(value, "display_name");
        outNode.SemanticOperatorId = JsonReadString(value, "semantic_operator_id");
        outNode.SourceInterfaceId = JsonReadString(value, "source_interface_id");
        outNode.SourceOperationId = JsonReadString(value, "source_operation_id");
        outNode.SourceParameterId = JsonReadString(value, "source_parameter_id");

        const auto encodedKind = static_cast<int>(JsonReadDouble(
            value,
            "kind",
            static_cast<double>(static_cast<int>(EGraphNodeKindV2::Other))));
        switch (encodedKind) {
            case static_cast<int>(EGraphNodeKindV2::Interface):
                outNode.Kind = EGraphNodeKindV2::Interface;
                break;
            case static_cast<int>(EGraphNodeKindV2::Parameter):
                outNode.Kind = EGraphNodeKindV2::Parameter;
                break;
            case static_cast<int>(EGraphNodeKindV2::Operation):
                outNode.Kind = EGraphNodeKindV2::Operation;
                break;
            case static_cast<int>(EGraphNodeKindV2::SemanticOperator):
                outNode.Kind = EGraphNodeKindV2::SemanticOperator;
                break;
            case static_cast<int>(EGraphNodeKindV2::Coercion):
                outNode.Kind = EGraphNodeKindV2::Coercion;
                break;
            case static_cast<int>(EGraphNodeKindV2::RoutingEndpoint):
                outNode.Kind = EGraphNodeKindV2::RoutingEndpoint;
                break;
            case static_cast<int>(EGraphNodeKindV2::Other):
            default:
                outNode.Kind = EGraphNodeKindV2::Other;
                break;
        }

        const auto position = JsonReadVec2(value, "position", ImVec2(outNode.Position.x, outNode.Position.y));
        outNode.Position.x = position.x;
        outNode.Position.y = position.y;

        if (const auto *metadataValue = JsonTryGetValue(value, "metadata"); metadataValue != nullptr) {
            outNode.Metadata = JsonReadGraphMetadataV2(*metadataValue);
        }

        if (const auto *portsValue = JsonTryGetValue(value, "ports"); portsValue != nullptr && portsValue->is_array()) {
            for (const auto &portValue : portsValue->get<json::array>()) {
                FGraphPortV2 port;
                if (!JsonReadGraphPortV2(portValue, port)) continue;
                outNode.Ports.push_back(std::move(port));
            }
        }

        return true;
    }

    auto JsonWriteGraphEdgeV2(const Slab::Core::Reflection::V2::FGraphEdgeV2 &edge) -> json::value {
        using namespace Slab::Core::Reflection::V2;

        json::value edgeValue(json::object{});
        edgeValue["id"] = edge.EdgeId;
        edgeValue["from_node_id"] = edge.FromNodeId;
        edgeValue["from_port_id"] = edge.FromPortId;
        edgeValue["to_node_id"] = edge.ToNodeId;
        edgeValue["to_port_id"] = edge.ToPortId;
        edgeValue["kind"] = static_cast<double>(static_cast<int>(edge.Kind));
        edgeValue["match_reason"] = edge.MatchReason;
        edgeValue["metadata"] = JsonWriteGraphMetadataV2(edge.Metadata);

        json::array diagnostics;
        diagnostics.reserve(edge.Diagnostics.size());
        for (const auto &diagnostic : edge.Diagnostics) {
            if (diagnostic.empty()) continue;
            diagnostics.emplace_back(diagnostic);
        }
        edgeValue["diagnostics"] = json::value(std::move(diagnostics));

        json::array suggestions;
        suggestions.reserve(edge.SuggestedCoercionOperatorIds.size());
        for (const auto &suggestion : edge.SuggestedCoercionOperatorIds) {
            if (suggestion.empty()) continue;
            suggestions.emplace_back(suggestion);
        }
        edgeValue["suggested_coercions"] = json::value(std::move(suggestions));

        return edgeValue;
    }

    auto JsonReadGraphEdgeV2(const json::value &value,
                             Slab::Core::Reflection::V2::FGraphEdgeV2 &outEdge) -> bool {
        using namespace Slab::Core::Reflection::V2;

        if (!value.is_object()) return false;

        outEdge = FGraphEdgeV2{};
        outEdge.EdgeId = JsonReadString(value, "id");
        outEdge.FromNodeId = JsonReadString(value, "from_node_id");
        outEdge.FromPortId = JsonReadString(value, "from_port_id");
        outEdge.ToNodeId = JsonReadString(value, "to_node_id");
        outEdge.ToPortId = JsonReadString(value, "to_port_id");
        outEdge.MatchReason = JsonReadString(value, "match_reason");

        const auto encodedKind = static_cast<int>(JsonReadDouble(
            value,
            "kind",
            static_cast<double>(static_cast<int>(EGraphEdgeKindV2::Other))));
        switch (encodedKind) {
            case static_cast<int>(EGraphEdgeKindV2::Containment):
                outEdge.Kind = EGraphEdgeKindV2::Containment;
                break;
            case static_cast<int>(EGraphEdgeKindV2::ValueFlow):
                outEdge.Kind = EGraphEdgeKindV2::ValueFlow;
                break;
            case static_cast<int>(EGraphEdgeKindV2::SemanticCoercion):
                outEdge.Kind = EGraphEdgeKindV2::SemanticCoercion;
                break;
            case static_cast<int>(EGraphEdgeKindV2::HandleBinding):
                outEdge.Kind = EGraphEdgeKindV2::HandleBinding;
                break;
            case static_cast<int>(EGraphEdgeKindV2::StreamSubscription):
                outEdge.Kind = EGraphEdgeKindV2::StreamSubscription;
                break;
            case static_cast<int>(EGraphEdgeKindV2::ControlDependency):
                outEdge.Kind = EGraphEdgeKindV2::ControlDependency;
                break;
            case static_cast<int>(EGraphEdgeKindV2::Other):
            default:
                outEdge.Kind = EGraphEdgeKindV2::Other;
                break;
        }

        if (const auto *metadataValue = JsonTryGetValue(value, "metadata"); metadataValue != nullptr) {
            outEdge.Metadata = JsonReadGraphMetadataV2(*metadataValue);
        }

        if (const auto *diagnosticsValue = JsonTryGetValue(value, "diagnostics");
            diagnosticsValue != nullptr && diagnosticsValue->is_array()) {
            for (const auto &diagnosticValue : diagnosticsValue->get<json::array>()) {
                const auto *diagnostic = diagnosticValue.get_ptr<json::string>();
                if (diagnostic == nullptr || diagnostic->empty()) continue;
                outEdge.Diagnostics.push_back(*diagnostic);
            }
        }

        if (const auto *suggestionsValue = JsonTryGetValue(value, "suggested_coercions");
            suggestionsValue != nullptr && suggestionsValue->is_array()) {
            for (const auto &suggestionValue : suggestionsValue->get<json::array>()) {
                const auto *suggestion = suggestionValue.get_ptr<json::string>();
                if (suggestion == nullptr || suggestion->empty()) continue;
                outEdge.SuggestedCoercionOperatorIds.push_back(*suggestion);
            }
        }

        return !outEdge.FromNodeId.empty() && !outEdge.ToNodeId.empty();
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
    ModelDemoCatalog = Slab::Core::Model::V2::BuildDemoModelsV2();
    ModelScratchInput = "\\ddot x + \\omega^2 x = 0";

    SchemesBlueprintDocument.Mode = Slab::Core::Reflection::V2::EGraphModeV2::SchemesBlueprint;
    SchemesBlueprintDocument.Canvas.PanX = 80.0f;
    SchemesBlueprintDocument.Canvas.PanY = 80.0f;
    SchemesBlueprintDocument.Canvas.bShowGrid = true;
    SchemesBlueprintDocument.Canvas.bShowMinimap = false;
    PlaygroundSemanticDocument.Mode = Slab::Core::Reflection::V2::EGraphModeV2::Semantic;
    PlaygroundSemanticDocument.Canvas.PanX = 90.0f;
    PlaygroundSemanticDocument.Canvas.PanY = 70.0f;
    PlaygroundSemanticDocument.Canvas.bShowGrid = true;
    PlaygroundSemanticDocument.Canvas.bShowMinimap = false;
    PlaygroundTemplateDocument.Mode = Slab::Core::Reflection::V2::EGraphModeV2::Template;
    PlaygroundTemplateDocument.Canvas.PanX = 90.0f;
    PlaygroundTemplateDocument.Canvas.PanY = 70.0f;
    PlaygroundTemplateDocument.Canvas.bShowGrid = true;
    PlaygroundTemplateDocument.Canvas.bShowMinimap = true;
    PlaygroundRoutingDocument.Mode = Slab::Core::Reflection::V2::EGraphModeV2::Routing;
    PlaygroundRoutingDocument.Canvas.PanX = 90.0f;
    PlaygroundRoutingDocument.Canvas.PanY = 70.0f;
    PlaygroundRoutingDocument.Canvas.bShowGrid = true;
    PlaygroundRoutingDocument.Canvas.bShowMinimap = false;
    PlaygroundRuntimeDocument.Mode = Slab::Core::Reflection::V2::EGraphModeV2::Runtime;
    PlaygroundRuntimeDocument.Canvas.PanX = 90.0f;
    PlaygroundRuntimeDocument.Canvas.PanY = 70.0f;
    PlaygroundRuntimeDocument.Canvas.bShowGrid = true;
    PlaygroundRuntimeDocument.Canvas.bShowMinimap = false;
    PlaygroundRuntimeInstanceDocument.Mode = Slab::Core::Reflection::V2::EGraphModeV2::Runtime;
    PlaygroundRuntimeInstanceDocument.Canvas = PlaygroundRuntimeDocument.Canvas;

    const auto imGuiModule = Slab::Core::GetModule<Slab::Graphics::FImGuiModule>("ImGui");
    const auto platformWindow = Slab::Graphics::GetGraphicsBackend()->GetMainSystemWindow();

    ImGuiContext = Slab::DynamicPointerCast<Slab::Graphics::FImGuiContext>(imGuiModule->CreateContext(platformWindow));
    ImGuiContext->SetManualRender(true);
    ImGuiContext->SetMainMenuPresentation(Slab::Graphics::FImGuiContext::EMainMenuPresentation::Hidden);
    UiTypesettingService = std::make_unique<Slab::Graphics::Typesetting::FTypesettingService>();
    PrewarmModelTypesettingCache(*UiTypesettingService, ModelDemoCatalog);

    LiveDataHub = Slab::New<Slab::Math::LiveData::V2::FLiveDataHubV2>();
    LiveControlHub = Slab::New<Slab::Math::LiveControl::V2::FLiveControlHubV2>();
    SimulationManager = Slab::New<FSimulationManagerV2>(
        ImGuiContext,
        LiveDataHub,
        LiveControlHub,
        [this](const Slab::TPointer<Slab::Graphics::FSlabWindow> &window) { QueueSlabWindow(window); },
        [this]() { RequestSimulationLauncherVisible(); });
    GraphPlaygroundController = std::make_unique<FLabV2GraphPlaygroundController>(*this);

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

auto FLabV2WindowManager::MarkGraphPlaygroundDirty() -> void {
    if (GraphPlaygroundController != nullptr) {
        GraphPlaygroundController->MarkDirty();
        return;
    }
    MarkGraphPlaygroundDirtyImpl();
}

auto FLabV2WindowManager::SaveGraphPlaygroundStateToFile() -> bool {
    if (GraphPlaygroundController != nullptr) {
        return GraphPlaygroundController->SaveStateToFile();
    }
    return SaveGraphPlaygroundStateToFileImpl();
}

auto FLabV2WindowManager::LoadGraphPlaygroundStateFromFile() -> bool {
    if (GraphPlaygroundController != nullptr) {
        return GraphPlaygroundController->LoadStateFromFile();
    }
    return LoadGraphPlaygroundStateFromFileImpl();
}

auto FLabV2WindowManager::DrawGraphPlaygroundPanel() -> void {
    if (GraphPlaygroundController != nullptr) {
        GraphPlaygroundController->DrawPanel();
        return;
    }
    DrawGraphPlaygroundPanelImpl();
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
        bShowWindowModelInspector,
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
    bShowWindowModelInspector = cfg.bShowWindowModelInspector;
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
            drawTab(EWorkspaceTab::Models, WorkspaceTabModels);
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
        if (ActiveWorkspace == EWorkspaceTab::Models) workspaceLabel = WorkspaceTabModels;
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
        } else if (ActiveWorkspace == EWorkspaceTab::Models) {
            drawToggle("Model Layer", &bShowWindowModelInspector);
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
    } else if (workspace == EWorkspaceTab::Models) {
        ImGui::DockBuilderDockWindow(WindowTitleModelInspector, dockMain);
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
        const auto dockspaceIdModels =
            static_cast<unsigned int>(ImGui::GetID(DockspaceNameModels));
        const auto dockspaceIdGraphPlayground =
            static_cast<unsigned int>(ImGui::GetID(DockspaceNameGraphPlayground));
        const auto dockspaceIdPlots =
            static_cast<unsigned int>(ImGui::GetID(DockspaceNamePlots));

        if (!bWorkspaceLayoutsBootstrapped) {
            BuildDefaultDockLayout(dockspaceIdSimulations, EWorkspaceTab::Simulations);
            BuildDefaultDockLayout(dockspaceIdSchemes, EWorkspaceTab::Schemes);
            BuildDefaultDockLayout(dockspaceIdModels, EWorkspaceTab::Models);
            BuildDefaultDockLayout(dockspaceIdGraphPlayground, EWorkspaceTab::GraphPlayground);
            BuildDefaultDockLayout(dockspaceIdPlots, EWorkspaceTab::Plots);
            WorkspaceLayoutInitialized[static_cast<std::size_t>(EWorkspaceTab::Simulations)] = true;
            WorkspaceLayoutInitialized[static_cast<std::size_t>(EWorkspaceTab::Schemes)] = true;
            WorkspaceLayoutInitialized[static_cast<std::size_t>(EWorkspaceTab::Models)] = true;
            WorkspaceLayoutInitialized[static_cast<std::size_t>(EWorkspaceTab::GraphPlayground)] = true;
            WorkspaceLayoutInitialized[static_cast<std::size_t>(EWorkspaceTab::Plots)] = true;
            bWorkspaceLayoutsBootstrapped = true;
            RequestViewRetile();
        }

        const auto dockspaceIdFor = [dockspaceIdSimulations, dockspaceIdSchemes, dockspaceIdModels, dockspaceIdGraphPlayground, dockspaceIdPlots]
            (const EWorkspaceTab workspace) -> unsigned int {
                if (workspace == EWorkspaceTab::Schemes) return dockspaceIdSchemes;
                if (workspace == EWorkspaceTab::Models) return dockspaceIdModels;
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
        drawWorkspaceDockspace(dockspaceIdModels, EWorkspaceTab::Models);
        drawWorkspaceDockspace(dockspaceIdGraphPlayground, EWorkspaceTab::GraphPlayground);
        drawWorkspaceDockspace(dockspaceIdPlots, EWorkspaceTab::Plots);
#endif
    }
    ImGui::End();

    ImGui::PopStyleVar(3);
}

auto FLabV2WindowManager::BuildPanelSurfaceRegistry() -> std::vector<FPanelSurfaceRegistration> {
    std::vector<FPanelSurfaceRegistration> registry;
    registry.reserve(13);

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
            } else if (ActiveWorkspace == EWorkspaceTab::Models) {
                ImGui::Checkbox("Model Layer", &bShowWindowModelInspector);
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
        WindowTitleModelInspector,
        EWorkspaceTab::Models,
        &bShowWindowModelInspector,
        false,
        false,
        [this]() {
            DrawModelInspectorPanel();
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
        ImGui::Checkbox("Model Layer", &bShowWindowModelInspector);

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

        if (bShowWindowModelInspector) {
            DrawModelInspectorPanel();
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

        if (bShowWindowModelInspector) {
            ImGui::SetNextWindowPos(
                ImVec2(
                    static_cast<float>(FStudioConfigV2::SidePaneWidth + 580),
                    GetTopMenuInset() + 24.0f),
                ImGuiCond_Appearing);
            ImGui::SetNextWindowSize(ImVec2(720, 720), ImGuiCond_Appearing);
            if (ImGui::Begin(WindowTitleModelInspector, &bShowWindowModelInspector)) {
                DrawModelInspectorPanel();
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
