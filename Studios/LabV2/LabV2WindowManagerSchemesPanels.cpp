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

    struct FNodeAction {
        enum class EKind : unsigned char {
            Invoke,
            CopyId
        };
        Slab::Str Label;
        EKind Kind = EKind::Invoke;
        Slab::Str OperationId;
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

    int parameterCount = 0;
    int operationCount = 0;
    for (const auto &node : nodes) {
        if (node.Kind == FGraphNode::EKind::Parameter) ++parameterCount;
        if (node.Kind == FGraphNode::EKind::Query || node.Kind == FGraphNode::EKind::Command) ++operationCount;
    }

    std::map<Slab::Str, FGraphNode *> nodesById;
    Slab::Vector<FLabV2SubstrateGraphVisualNode> blueprintVisualNodes;
    blueprintVisualNodes.reserve(nodes.size());
    for (auto &node : nodes) {
        nodesById[node.Key] = &node;

        FLabV2SubstrateGraphVisualNode visualNode;
        visualNode.Node = node.GraphNode;
        visualNode.Title = node.Title;
        visualNode.Subtitle = node.SubtitlePrimary;
        visualNode.Badges = node.Badges;
        visualNode.Size = node.Size;

        int inputPins = 0;
        int outputPins = 0;
        if (node.GraphNode != nullptr) {
            inputPins = static_cast<int>(std::count_if(
                node.GraphNode->Ports.begin(),
                node.GraphNode->Ports.end(),
                [](const auto &port) { return port.Direction == EGraphPortDirectionV2::Input; }));
            outputPins = static_cast<int>(std::count_if(
                node.GraphNode->Ports.begin(),
                node.GraphNode->Ports.end(),
                [](const auto &port) { return port.Direction == EGraphPortDirectionV2::Output; }));
        }

        if (node.Kind == FGraphNode::EKind::Interface) {
            inputPins = std::max(inputPins, std::max(1, parameterCount));
            outputPins = std::max(outputPins, std::max(1, operationCount));
        } else if (node.Kind == FGraphNode::EKind::Parameter) {
            outputPins = std::max(outputPins, 1);
        } else if (node.Kind == FGraphNode::EKind::Query || node.Kind == FGraphNode::EKind::Command) {
            inputPins = std::max(inputPins, std::max(1, node.InputSocketCount));
        }

        visualNode.InputPins = inputPins;
        visualNode.OutputPins = outputPins;
        blueprintVisualNodes.push_back(std::move(visualNode));
    }

    const auto buildBlueprintAddNodeActions = [](const ImVec2 &) -> Slab::Vector<FLabV2SubstrateGraphCanvasAction> {
        Slab::Vector<FLabV2SubstrateGraphCanvasAction> actions;
        actions.push_back(FLabV2SubstrateGraphCanvasAction{
            .Label = "Catalog-derived blueprint graph",
            .Hint = "Interface, parameter, and operation nodes are projected from reflection catalog.",
            .bEnabled = false
        });
        return actions;
    };

    const auto buildBlueprintConnectNodeActions = [](const FGraphNodeV2 &,
                                                     FGraphPortV2 *,
                                                     const EGraphPortDirectionV2,
                                                     const ImVec2 &) -> Slab::Vector<FLabV2SubstrateGraphCanvasAction> {
        Slab::Vector<FLabV2SubstrateGraphCanvasAction> actions;
        actions.push_back(FLabV2SubstrateGraphCanvasAction{
            .Label = "Connections are projection-only",
            .Hint = "Blueprint graph edges are derived from interface schema relationships.",
            .bEnabled = false
        });
        return actions;
    };

    const auto persistBlueprintNodePositions = [this]() {
        for (const auto &graphNode : SchemesBlueprintDocument.Nodes) {
            BlueprintNodePositionById[graphNode.NodeId] = ImVec2(graphNode.Position.x, graphNode.Position.y);
        }
    };

    FLabV2SubstrateGraphCanvasInteraction blueprintCanvasInteraction;
    DrawSubstrateGraphCanvasCommon(
        "SchemesBlueprintCanvas",
        SchemesBlueprintDocument,
        blueprintVisualNodes,
        BlueprintGraphTraceHeight,
        buildBlueprintAddNodeActions,
        buildBlueprintConnectNodeActions,
        persistBlueprintNodePositions,
        &blueprintCanvasInteraction);

    if (!blueprintCanvasInteraction.ClickedNodeId.empty()) {
        const auto nodeIt = nodesById.find(blueprintCanvasInteraction.ClickedNodeId);
        if (nodeIt != nodesById.end() && nodeIt->second != nullptr) {
            const auto &node = *nodeIt->second;
            if (node.Kind == FGraphNode::EKind::Interface) {
                SelectedSchemeInterfaceId = node.RefId;
                SelectedSchemeParameterId.clear();
                SelectedSchemeOperationId.clear();
            } else if (node.Kind == FGraphNode::EKind::Parameter) {
                SelectedSchemeParameterId = node.RefId;
                SelectedSchemeOperationId.clear();
            } else if (node.Kind == FGraphNode::EKind::Query || node.Kind == FGraphNode::EKind::Command) {
                SelectedSchemeOperationId = node.RefId;
                SelectedSchemeParameterId.clear();
            }
        }
    }

    const FGraphNode *selectedActionNode = nullptr;
    for (const auto &node : nodes) {
        if (node.Kind == FGraphNode::EKind::Parameter && node.RefId == SelectedSchemeParameterId) {
            selectedActionNode = &node;
            break;
        }
        if ((node.Kind == FGraphNode::EKind::Query || node.Kind == FGraphNode::EKind::Command) &&
            node.RefId == SelectedSchemeOperationId) {
            selectedActionNode = &node;
            break;
        }
    }

    if constexpr (CEnableBlueprintGraphTuningUi) {
        if (bShowBlueprintTuningWindow) {
            ImGui::SetNextWindowBgAlpha(0.94f);
            ImGui::SetNextWindowSize(ImVec2(380.0f, 0.0f), ImGuiCond_FirstUseEver);
            ImGui::SetNextWindowPos(ImVec2(120.0f, 120.0f), ImGuiCond_FirstUseEver);
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

    ImGui::BeginChild("SchemesBlueprintTrace", ImVec2(0.0f, BlueprintGraphTraceHeight), true);

    if (bShowBlueprintLegend) {
        ImGui::SeparatorText("Legend");
        ImGui::TextDisabled("N: node kind | M: mutability | E: exposure | S: state | R: run-state | T: thread | X: side-effect | I: required inputs");
    }

    if (selectedActionNode != nullptr && selectedActionNode->Kind != FGraphNode::EKind::Interface) {
        ImGui::SeparatorText("Node Actions");
        const auto actions = buildNodeActions(*selectedActionNode);
        for (std::size_t i = 0; i < actions.size(); ++i) {
            if (i > 0) ImGui::SameLine();
            if (ImGui::Button(actions[i].Label.c_str())) {
                executeNodeAction(*selectedActionNode, actions[i]);
            }
        }
    }

    ImGui::SeparatorText("Graph Trace");
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
