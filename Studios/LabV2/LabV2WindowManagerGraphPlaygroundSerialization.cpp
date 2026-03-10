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

namespace {

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

    struct FRuntimeEndpointCandidate {
        Slab::Str EndpointId;
        Slab::Str DisplayLabel;
        Slab::Str NodeId;
        Slab::Str PortId;
        Slab::Core::Reflection::V2::EGraphPortDirectionV2 Direction =
            Slab::Core::Reflection::V2::EGraphPortDirectionV2::Output;
        Slab::Str SpaceId;
        Slab::Str TypeId;
        Slab::Str SourceInterfaceId;
        Slab::Str SourceOperationId;
        Slab::Str SourceParameterId;
    };

    struct FPlaygroundGraphVisualNode {
        Slab::Core::Reflection::V2::FGraphNodeV2 *Node = nullptr;
        Slab::Str Title;
        Slab::Str Subtitle;
        Slab::Vector<std::pair<Slab::Str, ImU32>> Badges;
        int InputPins = 1;
        int OutputPins = 1;
        ImVec2 Size = ImVec2(320.0f, 132.0f);
    };

    struct FPlaygroundGraphCanvasAction {
        Slab::Str Label;
        Slab::Str Hint;
        bool bEnabled = true;
        std::function<void()> Execute;
    };

}

auto FLabV2WindowManager::MarkGraphPlaygroundDirtyImpl() -> void {
    bPlaygroundDirty = true;
}

auto FLabV2WindowManager::SaveGraphPlaygroundStateToFileImpl() -> bool {
    if (PlaygroundPersistenceFilePath.empty()) {
        PlaygroundPersistenceStatus = "[Error] Graph state path is empty.";
        return false;
    }

    namespace json = crude_json;
    json::value root(json::object{});
    root["schema_version"] = 1.0;
    root["workspace"] = "GraphPlayground";

    json::value semanticState(json::object{});
    semanticState["show_grid"] = PlaygroundSemanticDocument.Canvas.bShowGrid;
    semanticState["pan"] = JsonWriteVec2(ImVec2(
        PlaygroundSemanticDocument.Canvas.PanX,
        PlaygroundSemanticDocument.Canvas.PanY));
    semanticState["filter"] = PlaygroundSemanticFilter;
    semanticState["bottom_panel_height"] = static_cast<double>(PlaygroundSemanticBottomPanelHeight);
    json::array semanticNodes;
    semanticNodes.reserve(PlaygroundSemanticDocument.Nodes.size());
    for (const auto &node : PlaygroundSemanticDocument.Nodes) {
        json::value nodeValue(json::object{});
        nodeValue["id"] = node.NodeId;
        nodeValue["position"] = JsonWriteVec2(ImVec2(node.Position.x, node.Position.y));
        semanticNodes.push_back(std::move(nodeValue));
    }
    semanticState["nodes"] = json::value(std::move(semanticNodes));
    root["semantic"] = std::move(semanticState);

    json::value templateState(json::object{});
    templateState["selected_operator"] = PlaygroundTemplateSelectedOperatorId;
    templateState["show_grid"] = PlaygroundTemplateDocument.Canvas.bShowGrid;
    templateState["show_minimap"] = PlaygroundTemplateDocument.Canvas.bShowMinimap;
    templateState["pan"] = JsonWriteVec2(ImVec2(
        PlaygroundTemplateDocument.Canvas.PanX,
        PlaygroundTemplateDocument.Canvas.PanY));
    templateState["bottom_panel_height"] = static_cast<double>(PlaygroundTemplateBottomPanelHeight);
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
    routingState["endpoint_counter"] = static_cast<double>(PlaygroundRoutingEndpointCounter);
    routingState["source_endpoint"] = PlaygroundRoutingSourceEndpoint;
    routingState["target_endpoint"] = PlaygroundRoutingTargetEndpoint;
    routingState["edge_kind"] = static_cast<double>(static_cast<int>(PlaygroundRoutingEdgeKind));
    routingState["edge_kind_encoding"] = "enum_value";
    routingState["connect_attempt_count"] = static_cast<double>(PlaygroundRoutingConnectAttemptCount);
    routingState["connect_success_count"] = static_cast<double>(PlaygroundRoutingConnectSuccessCount);
    routingState["connect_failure_count"] = static_cast<double>(PlaygroundRoutingConnectFailureCount);
    routingState["last_operation_summary"] = PlaygroundRoutingLastOperationSummary;
    routingState["show_grid"] = PlaygroundRoutingDocument.Canvas.bShowGrid;
    routingState["pan"] = JsonWriteVec2(ImVec2(
        PlaygroundRoutingDocument.Canvas.PanX,
        PlaygroundRoutingDocument.Canvas.PanY));
    routingState["bottom_panel_height"] = static_cast<double>(PlaygroundRoutingBottomPanelHeight);

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

    json::array routingStandaloneEndpoints;
    routingStandaloneEndpoints.reserve(PlaygroundRoutingStandaloneEndpoints.size());
    for (const auto &endpointId : PlaygroundRoutingStandaloneEndpoints) {
        if (endpointId.empty()) continue;
        routingStandaloneEndpoints.emplace_back(endpointId);
    }
    routingState["standalone_endpoints"] = json::value(std::move(routingStandaloneEndpoints));

    json::array routingOperationDiagnostics;
    routingOperationDiagnostics.reserve(PlaygroundRoutingLastOperationDiagnostics.size());
    for (const auto &diagnostic : PlaygroundRoutingLastOperationDiagnostics) {
        if (diagnostic.empty()) continue;
        routingOperationDiagnostics.emplace_back(diagnostic);
    }
    routingState["last_operation_diagnostics"] = json::value(std::move(routingOperationDiagnostics));
    root["routing"] = std::move(routingState);

    json::value runtimeState(json::object{});
    runtimeState["show_grid"] = PlaygroundRuntimeDocument.Canvas.bShowGrid;
    runtimeState["pan"] = JsonWriteVec2(ImVec2(
        PlaygroundRuntimeDocument.Canvas.PanX,
        PlaygroundRuntimeDocument.Canvas.PanY));
    runtimeState["filter"] = PlaygroundRuntimeFilter;
    runtimeState["bottom_panel_height"] = static_cast<double>(PlaygroundRuntimeBottomPanelHeight);

    json::value runtimeInstanceState(json::object{});
    runtimeInstanceState["id"] = PlaygroundRuntimeInstanceId;
    runtimeInstanceState["counter"] = static_cast<double>(PlaygroundRuntimeInstanceCounter);
    runtimeInstanceState["status"] = PlaygroundRuntimeInstanceStatus;
    runtimeInstanceState["use_instantiated_graph"] = bPlaygroundRuntimeUseInstantiatedGraph;
    runtimeInstanceState["show_grid"] = PlaygroundRuntimeInstanceDocument.Canvas.bShowGrid;
    runtimeInstanceState["pan"] = JsonWriteVec2(ImVec2(
        PlaygroundRuntimeInstanceDocument.Canvas.PanX,
        PlaygroundRuntimeInstanceDocument.Canvas.PanY));

    json::array runtimeInstanceDiagnostics;
    runtimeInstanceDiagnostics.reserve(PlaygroundRuntimeInstanceDiagnostics.size());
    for (const auto &diagnostic : PlaygroundRuntimeInstanceDiagnostics) {
        if (diagnostic.empty()) continue;
        runtimeInstanceDiagnostics.emplace_back(diagnostic);
    }
    runtimeInstanceState["diagnostics"] = json::value(std::move(runtimeInstanceDiagnostics));

    json::array runtimeInstanceNodes;
    runtimeInstanceNodes.reserve(PlaygroundRuntimeInstanceDocument.Nodes.size());
    for (const auto &node : PlaygroundRuntimeInstanceDocument.Nodes) {
        runtimeInstanceNodes.push_back(JsonWriteGraphNodeV2(node));
    }
    runtimeInstanceState["nodes"] = json::value(std::move(runtimeInstanceNodes));

    json::array runtimeInstanceEdges;
    runtimeInstanceEdges.reserve(PlaygroundRuntimeInstanceDocument.Edges.size());
    for (const auto &edge : PlaygroundRuntimeInstanceDocument.Edges) {
        runtimeInstanceEdges.push_back(JsonWriteGraphEdgeV2(edge));
    }
    runtimeInstanceState["edges"] = json::value(std::move(runtimeInstanceEdges));
    runtimeState["instance"] = std::move(runtimeInstanceState);
    root["runtime"] = std::move(runtimeState);

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

auto FLabV2WindowManager::LoadGraphPlaygroundStateFromFileImpl() -> bool {
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

    PlaygroundSemanticDocument.Nodes.clear();
    PlaygroundSemanticDocument.Edges.clear();
    PlaygroundSemanticVisibleNodeIds.clear();
    PlaygroundRoutingDocument.Edges.clear();
    PlaygroundRoutingDocument.Nodes.clear();
    PlaygroundRoutingStandaloneEndpoints.clear();
    PlaygroundRoutingLastOperationSummary.clear();
    PlaygroundRoutingLastOperationDiagnostics.clear();
    PlaygroundRoutingConnectAttemptCount = 0;
    PlaygroundRoutingConnectSuccessCount = 0;
    PlaygroundRoutingConnectFailureCount = 0;
    PlaygroundRuntimeDocument.Nodes.clear();
    PlaygroundRuntimeDocument.Edges.clear();
    PlaygroundRuntimeInstanceDocument.Nodes.clear();
    PlaygroundRuntimeInstanceDocument.Edges.clear();
    PlaygroundRuntimeInstanceDocument.Canvas = PlaygroundRuntimeDocument.Canvas;
    PlaygroundRuntimeInstanceCounter = 0;
    PlaygroundRuntimeInstanceId.clear();
    PlaygroundRuntimeInstanceStatus.clear();
    PlaygroundRuntimeInstanceDiagnostics.clear();
    bPlaygroundRuntimeUseInstantiatedGraph = false;

    if (const auto *semanticState = JsonTryGetValue(loadedRoot, "semantic"); semanticState != nullptr) {
        PlaygroundSemanticDocument.Canvas.bShowGrid = JsonReadBool(*semanticState, "show_grid", PlaygroundSemanticDocument.Canvas.bShowGrid);
        const auto semanticPan = JsonReadVec2(
            *semanticState,
            "pan",
            ImVec2(
                PlaygroundSemanticDocument.Canvas.PanX,
                PlaygroundSemanticDocument.Canvas.PanY));
        PlaygroundSemanticDocument.Canvas.PanX = semanticPan.x;
        PlaygroundSemanticDocument.Canvas.PanY = semanticPan.y;
        PlaygroundSemanticFilter = JsonReadString(*semanticState, "filter", PlaygroundSemanticFilter);
        PlaygroundSemanticBottomPanelHeight = static_cast<float>(JsonReadDouble(
            *semanticState,
            "bottom_panel_height",
            static_cast<double>(PlaygroundSemanticBottomPanelHeight)));
        if (const auto *semanticNodesValue = JsonTryGetValue(*semanticState, "nodes");
            semanticNodesValue != nullptr && semanticNodesValue->is_array()) {
            for (const auto &nodeValue : semanticNodesValue->get<json::array>()) {
                if (!nodeValue.is_object()) continue;

                const auto nodeId = JsonReadString(nodeValue, "id");
                if (nodeId.empty()) continue;

                Slab::Core::Reflection::V2::FGraphNodeV2 node;
                node.NodeId = nodeId;
                const auto position = JsonReadVec2(nodeValue, "position", ImVec2(node.Position.x, node.Position.y));
                node.Position.x = position.x;
                node.Position.y = position.y;
                PlaygroundSemanticDocument.Nodes.push_back(std::move(node));

                if (std::find(
                        PlaygroundSemanticVisibleNodeIds.begin(),
                        PlaygroundSemanticVisibleNodeIds.end(),
                        nodeId) == PlaygroundSemanticVisibleNodeIds.end()) {
                    PlaygroundSemanticVisibleNodeIds.push_back(nodeId);
                }
            }
        }
    }

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
        PlaygroundTemplateBottomPanelHeight = static_cast<float>(JsonReadDouble(
            *templateState,
            "bottom_panel_height",
            static_cast<double>(PlaygroundTemplateBottomPanelHeight)));

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
        PlaygroundRoutingLastOperationSummary = JsonReadString(*routingState, "last_operation_summary");
        const auto edgeKindEncoding = JsonReadString(*routingState, "edge_kind_encoding");
        const bool bRouteKindsStoredAsEnumValues = edgeKindEncoding == "enum_value";
        const auto edgeKindStored = static_cast<int>(JsonReadDouble(
            *routingState,
            "edge_kind",
            static_cast<double>(static_cast<int>(PlaygroundRoutingEdgeKind))));
        PlaygroundRoutingEdgeKind = DecodeRoutingEdgeKindFromStoredValue(
            edgeKindStored,
            bRouteKindsStoredAsEnumValues,
            PlaygroundRoutingEdgeKind);
        PlaygroundRoutingDocument.Canvas.bShowGrid = JsonReadBool(*routingState, "show_grid", PlaygroundRoutingDocument.Canvas.bShowGrid);
        const auto routingPan = JsonReadVec2(
            *routingState,
            "pan",
            ImVec2(
                PlaygroundRoutingDocument.Canvas.PanX,
                PlaygroundRoutingDocument.Canvas.PanY));
        PlaygroundRoutingDocument.Canvas.PanX = routingPan.x;
        PlaygroundRoutingDocument.Canvas.PanY = routingPan.y;
        PlaygroundRoutingBottomPanelHeight = static_cast<float>(JsonReadDouble(
            *routingState,
            "bottom_panel_height",
            static_cast<double>(PlaygroundRoutingBottomPanelHeight)));
        PlaygroundRoutingConnectAttemptCount = JsonReadUSize(*routingState, "connect_attempt_count", 0);
        PlaygroundRoutingConnectSuccessCount = JsonReadUSize(*routingState, "connect_success_count", 0);
        PlaygroundRoutingConnectFailureCount = JsonReadUSize(*routingState, "connect_failure_count", 0);

        if (const auto *diagnosticsValue = JsonTryGetValue(*routingState, "last_operation_diagnostics");
            diagnosticsValue != nullptr && diagnosticsValue->is_array()) {
            for (const auto &diagnosticValue : diagnosticsValue->get<json::array>()) {
                const auto *diagnostic = diagnosticValue.get_ptr<json::string>();
                if (diagnostic == nullptr || diagnostic->empty()) continue;
                PlaygroundRoutingLastOperationDiagnostics.push_back(*diagnostic);
            }
        }

        if (const auto *standaloneEndpoints = JsonTryGetValue(*routingState, "standalone_endpoints");
            standaloneEndpoints != nullptr && standaloneEndpoints->is_array()) {
            for (const auto &endpointValue : standaloneEndpoints->get<json::array>()) {
                const auto *endpointId = endpointValue.get_ptr<json::string>();
                if (endpointId == nullptr || endpointId->empty()) continue;
                if (std::find(
                        PlaygroundRoutingStandaloneEndpoints.begin(),
                        PlaygroundRoutingStandaloneEndpoints.end(),
                        *endpointId) == PlaygroundRoutingStandaloneEndpoints.end()) {
                    PlaygroundRoutingStandaloneEndpoints.push_back(*endpointId);
                }
            }
        }

        if (const auto *edgesValue = JsonTryGetValue(*routingState, "edges");
            edgesValue != nullptr && edgesValue->is_array()) {
            for (const auto &edgeValue : edgesValue->get<json::array>()) {
                if (!edgeValue.is_object()) continue;

                Slab::Core::Reflection::V2::FGraphEdgeV2 edge;
                edge.EdgeId = JsonReadString(edgeValue, "id");
                edge.FromNodeId = JsonReadString(edgeValue, "source");
                edge.ToNodeId = JsonReadString(edgeValue, "target");
                edge.FromPortId = "out";
                edge.ToPortId = "in";
                const auto kindStored = static_cast<int>(JsonReadDouble(
                    edgeValue,
                    "kind",
                    static_cast<double>(static_cast<int>(PlaygroundRoutingEdgeKind))));
                edge.Kind = DecodeRoutingEdgeKindFromStoredValue(
                    kindStored,
                    bRouteKindsStoredAsEnumValues,
                    Slab::Core::Reflection::V2::EGraphEdgeKindV2::ValueFlow);
                if (edge.EdgeId.empty()) edge.EdgeId = "route.edge." + Slab::ToStr(PlaygroundRoutingDocument.Edges.size() + 1);
                if (edge.FromNodeId.empty() || edge.ToNodeId.empty()) continue;
                PlaygroundRoutingDocument.Edges.push_back(std::move(edge));
            }
        }

        PlaygroundRoutingEdgeCounter = std::max(
            PlaygroundRoutingDocument.Edges.size(),
            JsonReadUSize(*routingState, "edge_counter", PlaygroundRoutingDocument.Edges.size()));
        PlaygroundRoutingEndpointCounter = std::max(
            PlaygroundRoutingStandaloneEndpoints.size(),
            JsonReadUSize(*routingState, "endpoint_counter", PlaygroundRoutingStandaloneEndpoints.size()));
    } else {
        PlaygroundRoutingEdgeCounter = PlaygroundRoutingDocument.Edges.size();
        PlaygroundRoutingEndpointCounter = PlaygroundRoutingStandaloneEndpoints.size();
    }

    if (const auto *runtimeState = JsonTryGetValue(loadedRoot, "runtime"); runtimeState != nullptr) {
        PlaygroundRuntimeDocument.Canvas.bShowGrid = JsonReadBool(*runtimeState, "show_grid", PlaygroundRuntimeDocument.Canvas.bShowGrid);
        const auto runtimePan = JsonReadVec2(
            *runtimeState,
            "pan",
            ImVec2(
                PlaygroundRuntimeDocument.Canvas.PanX,
                PlaygroundRuntimeDocument.Canvas.PanY));
        PlaygroundRuntimeDocument.Canvas.PanX = runtimePan.x;
        PlaygroundRuntimeDocument.Canvas.PanY = runtimePan.y;
        PlaygroundRuntimeFilter = JsonReadString(*runtimeState, "filter", PlaygroundRuntimeFilter);
        PlaygroundRuntimeBottomPanelHeight = static_cast<float>(JsonReadDouble(
            *runtimeState,
            "bottom_panel_height",
            static_cast<double>(PlaygroundRuntimeBottomPanelHeight)));

        if (const auto *runtimeInstanceState = JsonTryGetValue(*runtimeState, "instance"); runtimeInstanceState != nullptr) {
            PlaygroundRuntimeInstanceId = JsonReadString(*runtimeInstanceState, "id");
            PlaygroundRuntimeInstanceCounter = JsonReadUSize(*runtimeInstanceState, "counter", PlaygroundRuntimeInstanceCounter);
            PlaygroundRuntimeInstanceStatus = JsonReadString(*runtimeInstanceState, "status");
            bPlaygroundRuntimeUseInstantiatedGraph = JsonReadBool(
                *runtimeInstanceState,
                "use_instantiated_graph",
                bPlaygroundRuntimeUseInstantiatedGraph);

            PlaygroundRuntimeInstanceDocument.Mode = Slab::Core::Reflection::V2::EGraphModeV2::Runtime;
            PlaygroundRuntimeInstanceDocument.Canvas.bShowGrid = JsonReadBool(
                *runtimeInstanceState,
                "show_grid",
                PlaygroundRuntimeInstanceDocument.Canvas.bShowGrid);
            const auto runtimeInstancePan = JsonReadVec2(
                *runtimeInstanceState,
                "pan",
                ImVec2(
                    PlaygroundRuntimeInstanceDocument.Canvas.PanX,
                    PlaygroundRuntimeInstanceDocument.Canvas.PanY));
            PlaygroundRuntimeInstanceDocument.Canvas.PanX = runtimeInstancePan.x;
            PlaygroundRuntimeInstanceDocument.Canvas.PanY = runtimeInstancePan.y;

            if (const auto *diagnosticsValue = JsonTryGetValue(*runtimeInstanceState, "diagnostics");
                diagnosticsValue != nullptr && diagnosticsValue->is_array()) {
                for (const auto &diagnosticValue : diagnosticsValue->get<json::array>()) {
                    const auto *diagnostic = diagnosticValue.get_ptr<json::string>();
                    if (diagnostic == nullptr || diagnostic->empty()) continue;
                    PlaygroundRuntimeInstanceDiagnostics.push_back(*diagnostic);
                }
            }

            if (const auto *nodesValue = JsonTryGetValue(*runtimeInstanceState, "nodes");
                nodesValue != nullptr && nodesValue->is_array()) {
                for (const auto &nodeValue : nodesValue->get<json::array>()) {
                    Slab::Core::Reflection::V2::FGraphNodeV2 node;
                    if (!JsonReadGraphNodeV2(nodeValue, node)) continue;
                    PlaygroundRuntimeInstanceDocument.Nodes.push_back(std::move(node));
                }
            }

            if (const auto *edgesValue = JsonTryGetValue(*runtimeInstanceState, "edges");
                edgesValue != nullptr && edgesValue->is_array()) {
                for (const auto &edgeValue : edgesValue->get<json::array>()) {
                    Slab::Core::Reflection::V2::FGraphEdgeV2 edge;
                    if (!JsonReadGraphEdgeV2(edgeValue, edge)) continue;
                    if (edge.EdgeId.empty()) {
                        edge.EdgeId = "runtime.edge." + Slab::ToStr(PlaygroundRuntimeInstanceDocument.Edges.size() + 1);
                    }
                    PlaygroundRuntimeInstanceDocument.Edges.push_back(std::move(edge));
                }
            }
        }
    }

    if (PlaygroundRuntimeInstanceDocument.Nodes.empty()) {
        bPlaygroundRuntimeUseInstantiatedGraph = false;
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
