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
#include <functional>
#include <limits>
#include <set>
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

    using FPlaygroundGraphVisualNode = FLabV2SubstrateGraphVisualNode;
    using FPlaygroundGraphCanvasAction = FLabV2SubstrateGraphCanvasAction;

    auto ComputePlaygroundGraphNodeSize(const FPlaygroundGraphVisualNode &node,
                                        const float minWidth,
                                        const float maxWidth,
                                        const float headerMinHeight,
                                        const float headerScale,
                                        const float bodyTopPadding,
                                        const float bodyBottomPadding,
                                        const float badgeRowGap) -> ImVec2 {
        const float lineHeight = ImGui::GetTextLineHeight();
        const float headerHeight = std::max(headerMinHeight, std::round(lineHeight * headerScale));
        constexpr float BodyPaddingX = 9.0f;
        const float titleWidth = ImGui::CalcTextSize(node.Title.c_str()).x;
        const float subtitleWidth = ImGui::CalcTextSize(node.Subtitle.c_str()).x;

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
        const float height = headerHeight + bodyTopPadding + textBlockHeight + 10.0f + badgeHeight + bodyBottomPadding;
        return ImVec2(width, std::max(116.0f, height));
    }

    auto ColorForGraphNodeKind(const Slab::Core::Reflection::V2::EGraphNodeKindV2 kind) -> std::pair<ImU32, ImU32> {
        using Slab::Core::Reflection::V2::EGraphNodeKindV2;

        switch (kind) {
            case EGraphNodeKindV2::Interface:
                return {IM_COL32(44, 40, 58, 238), IM_COL32(112, 94, 154, 255)};
            case EGraphNodeKindV2::Parameter:
                return {IM_COL32(38, 44, 55, 238), IM_COL32(92, 106, 132, 255)};
            case EGraphNodeKindV2::Operation:
                return {IM_COL32(34, 48, 46, 238), IM_COL32(78, 132, 120, 255)};
            case EGraphNodeKindV2::SemanticOperator:
                return {IM_COL32(34, 46, 56, 238), IM_COL32(86, 122, 152, 255)};
            case EGraphNodeKindV2::Coercion:
                return {IM_COL32(54, 46, 34, 238), IM_COL32(164, 132, 72, 255)};
            case EGraphNodeKindV2::RoutingEndpoint:
                return {IM_COL32(30, 52, 56, 238), IM_COL32(72, 146, 154, 255)};
            default:
                return {IM_COL32(40, 44, 52, 238), IM_COL32(96, 108, 126, 255)};
        }
    }

    auto ColorForGraphEdgeKind(const Slab::Core::Reflection::V2::EGraphEdgeKindV2 kind) -> ImU32 {
        using Slab::Core::Reflection::V2::EGraphEdgeKindV2;

        switch (kind) {
            case EGraphEdgeKindV2::Containment:
                return IM_COL32(106, 172, 226, 148);
            case EGraphEdgeKindV2::ValueFlow:
                return IM_COL32(116, 214, 174, 148);
            case EGraphEdgeKindV2::SemanticCoercion:
                return IM_COL32(236, 194, 92, 180);
            case EGraphEdgeKindV2::HandleBinding:
                return IM_COL32(192, 154, 234, 168);
            case EGraphEdgeKindV2::StreamSubscription:
                return IM_COL32(126, 210, 236, 168);
            case EGraphEdgeKindV2::ControlDependency:
                return IM_COL32(226, 168, 106, 168);
            default:
                return IM_COL32(138, 146, 162, 132);
        }
    }

}

auto FLabV2WindowManager::DrawGraphPlaygroundPanelImpl() -> void {
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

    Slab::Vector<const FSemanticOperatorSchemaV1 *> templateWorkingOperators;
    templateWorkingOperators.reserve(semanticOperators.size());
    for (const auto *semanticOperator : semanticOperators) {
        if (semanticOperator == nullptr || semanticOperator->OperatorId.empty()) continue;
        const auto bindings = semanticCatalog.QueryBindingsForOperator(semanticOperator->OperatorId);
        if (bindings.empty()) continue;
        templateWorkingOperators.push_back(semanticOperator);
    }

    const auto isTemplateOperatorWorking = [&templateWorkingOperators](const Slab::Str &operatorId) -> bool {
        if (operatorId.empty()) return false;
        return std::find_if(
                   templateWorkingOperators.begin(),
                   templateWorkingOperators.end(),
                   [&](const auto *semanticOperator) {
                       return semanticOperator != nullptr && semanticOperator->OperatorId == operatorId;
                   }) != templateWorkingOperators.end();
    };

    const auto buildTemplateNodePortsFromOperator = [](const FSemanticOperatorSchemaV1 &semanticOperator) -> Slab::Vector<FGraphPortV2> {
        Slab::Vector<FGraphPortV2> ports;
        ports.reserve(semanticOperator.DomainPorts.size() + semanticOperator.CodomainPorts.size());

        for (const auto &inputPort : semanticOperator.DomainPorts) {
            FGraphPortV2 port;
            port.PortId = inputPort.PortId;
            port.DisplayName = inputPort.DisplayName;
            port.Direction = EGraphPortDirectionV2::Input;
            port.bRequired = inputPort.bRequired;
            port.Handle.SpaceId = inputPort.SpaceId;
            ports.push_back(std::move(port));
        }

        for (const auto &outputPort : semanticOperator.CodomainPorts) {
            FGraphPortV2 port;
            port.PortId = outputPort.PortId;
            port.DisplayName = outputPort.DisplayName;
            port.Direction = EGraphPortDirectionV2::Output;
            port.bRequired = outputPort.bRequired;
            port.Handle.SpaceId = outputPort.SpaceId;
            ports.push_back(std::move(port));
        }

        return ports;
    };

    const auto makeSemanticSpaceNodeId = [](const Slab::Str &spaceId) -> Slab::Str {
        return "semantic.space." + spaceId;
    };

    const auto tryExtractSemanticSpaceId = [](const Slab::Str &nodeId) -> Slab::Str {
        constexpr auto Prefix = "semantic.space.";
        if (nodeId.rfind(Prefix, 0) != 0) return {};
        return nodeId.substr(std::char_traits<char>::length(Prefix));
    };

    const auto isSemanticNodeVisible = [this](const Slab::Str &nodeId) -> bool {
        if (nodeId.empty()) return false;
        return std::find(
                   PlaygroundSemanticVisibleNodeIds.begin(),
                   PlaygroundSemanticVisibleNodeIds.end(),
                   nodeId) != PlaygroundSemanticVisibleNodeIds.end();
    };

    const auto seedSemanticNodePosition = [this](const Slab::Str &nodeId, const ImVec2 worldPosition) -> void {
        if (nodeId.empty()) return;

        const auto existingIt = std::find_if(
            PlaygroundSemanticDocument.Nodes.begin(),
            PlaygroundSemanticDocument.Nodes.end(),
            [&](const auto &node) { return node.NodeId == nodeId; });
        if (existingIt != PlaygroundSemanticDocument.Nodes.end()) {
            existingIt->Position.x = worldPosition.x;
            existingIt->Position.y = worldPosition.y;
            return;
        }

        FGraphNodeV2 placeholder;
        placeholder.NodeId = nodeId;
        placeholder.Position.x = worldPosition.x;
        placeholder.Position.y = worldPosition.y;
        PlaygroundSemanticDocument.Nodes.push_back(std::move(placeholder));
    };

    const auto addSemanticVisibleNodeAt = [this,
                                           &isSemanticNodeVisible,
                                           &seedSemanticNodePosition](const Slab::Str &nodeId,
                                                                      const ImVec2 worldPosition) -> bool {
        if (nodeId.empty()) return false;
        if (isSemanticNodeVisible(nodeId)) return false;
        PlaygroundSemanticVisibleNodeIds.push_back(nodeId);
        seedSemanticNodePosition(nodeId, worldPosition);
        return true;
    };

    if (!isTemplateOperatorWorking(PlaygroundTemplateSelectedOperatorId)) {
        if (!templateWorkingOperators.empty()) {
            PlaygroundTemplateSelectedOperatorId = templateWorkingOperators.front()->OperatorId;
        } else {
            PlaygroundTemplateSelectedOperatorId.clear();
        }
    }

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

    const auto buildRuntimeEndpointCandidates = [this]() -> Slab::Vector<FRuntimeEndpointCandidate> {
        Slab::Vector<FRuntimeEndpointCandidate> candidates;
        if (PlaygroundRuntimeInstanceDocument.Nodes.empty()) return candidates;

        const auto instanceId =
            PlaygroundRuntimeInstanceId.empty()
                ? Slab::Str("runtime.instance.unknown")
                : PlaygroundRuntimeInstanceId;

        for (const auto &node : PlaygroundRuntimeInstanceDocument.Nodes) {
            for (const auto &port : node.Ports) {
                FRuntimeEndpointCandidate candidate;
                candidate.NodeId = node.NodeId;
                candidate.PortId = port.PortId;
                candidate.Direction = port.Direction;
                candidate.SpaceId = port.Handle.SpaceId;
                candidate.TypeId = port.Handle.TypeId;
                candidate.SourceInterfaceId = node.SourceInterfaceId;
                candidate.SourceOperationId = node.SourceOperationId;
                candidate.SourceParameterId = node.SourceParameterId;
                candidate.EndpointId =
                    "runtime.endpoint." + instanceId + "::" + node.NodeId + "::" + port.PortId;
                candidate.DisplayLabel =
                    (node.DisplayName.empty() ? node.NodeId : node.DisplayName) +
                    " :: " +
                    (port.DisplayName.empty() ? port.PortId : port.DisplayName) +
                    (port.Direction == EGraphPortDirectionV2::Output ? " [out]" : " [in]");
                candidates.push_back(std::move(candidate));
            }
        }
        return candidates;
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

    const auto syncTemplateNodePortsFromSemanticSchema = [this,
                                                           &semanticCatalog,
                                                           &buildTemplateNodePortsFromOperator,
                                                           &markDirty]() {
        const auto areEquivalentPorts = [](const FGraphPortV2 &lhs, const FGraphPortV2 &rhs) -> bool {
            return lhs.PortId == rhs.PortId &&
                   lhs.DisplayName == rhs.DisplayName &&
                   lhs.Direction == rhs.Direction &&
                   lhs.bRequired == rhs.bRequired &&
                   lhs.Handle.SpaceId == rhs.Handle.SpaceId;
        };

        bool bUpdatedAnyNode = false;
        for (auto &node : PlaygroundTemplateDocument.Nodes) {
            const auto *semanticOperator = semanticCatalog.FindOperatorById(node.SemanticOperatorId);
            if (semanticOperator == nullptr) continue;

            auto expectedPorts = buildTemplateNodePortsFromOperator(*semanticOperator);
            const bool bNeedsUpdate =
                node.Ports.size() != expectedPorts.size() ||
                !std::equal(
                    node.Ports.begin(),
                    node.Ports.end(),
                    expectedPorts.begin(),
                    expectedPorts.end(),
                    areEquivalentPorts);
            if (!bNeedsUpdate) continue;

            node.Ports = std::move(expectedPorts);
            bUpdatedAnyNode = true;
        }

        if (bUpdatedAnyNode) {
            markDirty();
        }
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

    const auto pruneTemplateNodesWithoutBindings = [this,
                                                    &isTemplateOperatorWorking,
                                                    &clearTemplateSelection,
                                                    &clearTemplateConnectionState,
                                                    &markDirty]() {
        Slab::Vector<Slab::Str> removedNodeIds;
        for (const auto &node : PlaygroundTemplateDocument.Nodes) {
            if (isTemplateOperatorWorking(node.SemanticOperatorId)) continue;
            removedNodeIds.push_back(node.NodeId);
        }
        if (removedNodeIds.empty()) return;

        const auto containsNodeId = [&removedNodeIds](const Slab::Str &nodeId) {
            return std::find(removedNodeIds.begin(), removedNodeIds.end(), nodeId) != removedNodeIds.end();
        };

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

        PlaygroundTemplatePendingCoercion = {};
        clearTemplateSelection();
        clearTemplateConnectionState();
        PlaygroundTemplateStatus =
            "[Warn] Removed " + Slab::ToStr(removedNodeIds.size()) +
            " non-working template node(s) (missing bindings).";
        markDirty();
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
        coercionNode.Ports = buildTemplateNodePortsFromOperator(*coercionOperator);
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

    const auto instantiateTemplateRuntimeGraph = [this,
                                                  &semanticCatalog,
                                                  &buildCompilePreview,
                                                  &markDirty]() -> bool {
        if (PlaygroundTemplateDocument.Nodes.empty()) {
            PlaygroundTemplateStatus = "[Error] Template graph is empty. Add at least one semantic node first.";
            return false;
        }

        const auto compilePreview = buildCompilePreview();
        ++PlaygroundRuntimeInstanceCounter;
        PlaygroundRuntimeInstanceId = "runtime.instance." + Slab::ToStr(PlaygroundRuntimeInstanceCounter);

        std::map<Slab::Str, FTemplateCompilePreviewNode> previewNodeByTemplateId;
        for (const auto &previewNode : compilePreview.Nodes) {
            previewNodeByTemplateId[previewNode.NodeId] = previewNode;
        }

        FGraphDocumentV2 instanceDocument;
        instanceDocument.Mode = EGraphModeV2::Runtime;
        if (PlaygroundRuntimeInstanceDocument.Nodes.empty()) {
            instanceDocument.Canvas = PlaygroundRuntimeDocument.Canvas;
        } else {
            instanceDocument.Canvas = PlaygroundRuntimeInstanceDocument.Canvas;
        }

        FGraphNodeV2 instanceRootNode;
        instanceRootNode.NodeId = PlaygroundRuntimeInstanceId;
        instanceRootNode.DisplayName = "Runtime Instance";
        instanceRootNode.Kind = EGraphNodeKindV2::Other;
        instanceRootNode.Metadata.Description = "Instantiated runtime graph from template.";
        instanceRootNode.Metadata.Tags.push_back("instantiated-runtime");
        instanceRootNode.Metadata.Tags.push_back("template-derived");
        instanceRootNode.Metadata.Attrs["instance_id"] = MakeStringValue(PlaygroundRuntimeInstanceId);
        instanceRootNode.Position.x = 120.0f;
        instanceRootNode.Position.y = 120.0f;
        instanceDocument.Nodes.push_back(std::move(instanceRootNode));

        std::map<Slab::Str, Slab::Str> templateNodeToRuntimeNode;
        templateNodeToRuntimeNode.clear();
        std::size_t containmentEdgeCounter = 0;
        for (const auto &templateNode : PlaygroundTemplateDocument.Nodes) {
            FGraphNodeV2 runtimeNode;
            runtimeNode.NodeId = PlaygroundRuntimeInstanceId + "::" + templateNode.NodeId;
            runtimeNode.DisplayName = templateNode.NodeId;
            runtimeNode.Kind = templateNode.Kind == EGraphNodeKindV2::Coercion
                ? EGraphNodeKindV2::Coercion
                : EGraphNodeKindV2::SemanticOperator;
            runtimeNode.SemanticOperatorId = templateNode.SemanticOperatorId;
            runtimeNode.Position.x = templateNode.Position.x + 360.0f;
            runtimeNode.Position.y = templateNode.Position.y + 30.0f;
            runtimeNode.Metadata.Attrs["template_node_id"] = MakeStringValue(templateNode.NodeId);
            runtimeNode.Metadata.Attrs["semantic_operator_id"] = MakeStringValue(templateNode.SemanticOperatorId);
            runtimeNode.Metadata.Tags.push_back("template-derived");

            if (const auto *semanticOperator = semanticCatalog.FindOperatorById(templateNode.SemanticOperatorId);
                semanticOperator != nullptr) {
                runtimeNode.DisplayName = semanticOperator->DisplayName.empty()
                    ? templateNode.NodeId
                    : semanticOperator->DisplayName;

                runtimeNode.Ports.reserve(semanticOperator->DomainPorts.size() + semanticOperator->CodomainPorts.size());
                for (const auto &inputPort : semanticOperator->DomainPorts) {
                    FGraphPortV2 graphPort;
                    graphPort.PortId = inputPort.PortId;
                    graphPort.DisplayName = inputPort.DisplayName;
                    graphPort.Direction = EGraphPortDirectionV2::Input;
                    graphPort.bRequired = inputPort.bRequired;
                    graphPort.Handle.SpaceId = inputPort.SpaceId;
                    runtimeNode.Ports.push_back(std::move(graphPort));
                }
                for (const auto &outputPort : semanticOperator->CodomainPorts) {
                    FGraphPortV2 graphPort;
                    graphPort.PortId = outputPort.PortId;
                    graphPort.DisplayName = outputPort.DisplayName;
                    graphPort.Direction = EGraphPortDirectionV2::Output;
                    graphPort.bRequired = outputPort.bRequired;
                    graphPort.Handle.SpaceId = outputPort.SpaceId;
                    runtimeNode.Ports.push_back(std::move(graphPort));
                }
            }

            const auto previewNodeIt = previewNodeByTemplateId.find(templateNode.NodeId);
            if (previewNodeIt != previewNodeByTemplateId.end()) {
                const auto &previewNode = previewNodeIt->second;
                if (previewNode.bBound) {
                    runtimeNode.Metadata.Tags.push_back("bound");
                    runtimeNode.Metadata.Attrs["binding"] = MakeStringValue(previewNode.BindingLabel);
                    const auto separator = previewNode.BindingLabel.find("::");
                    if (separator != Slab::Str::npos) {
                        runtimeNode.SourceInterfaceId = previewNode.BindingLabel.substr(0, separator);
                        runtimeNode.SourceOperationId = previewNode.BindingLabel.substr(separator + 2);
                    }
                } else {
                    runtimeNode.Metadata.Tags.push_back("unbound");
                }
            } else {
                runtimeNode.Metadata.Tags.push_back("unknown-binding");
            }

            templateNodeToRuntimeNode[templateNode.NodeId] = runtimeNode.NodeId;
            instanceDocument.Nodes.push_back(std::move(runtimeNode));

            FGraphEdgeV2 containmentEdge;
            containmentEdge.EdgeId = PlaygroundRuntimeInstanceId + "::contains::" + Slab::ToStr(containmentEdgeCounter++);
            containmentEdge.FromNodeId = PlaygroundRuntimeInstanceId;
            containmentEdge.ToNodeId = templateNodeToRuntimeNode[templateNode.NodeId];
            containmentEdge.Kind = EGraphEdgeKindV2::Containment;
            instanceDocument.Edges.push_back(std::move(containmentEdge));
        }

        std::size_t droppedEdges = 0;
        for (const auto &templateEdge : PlaygroundTemplateDocument.Edges) {
            const auto fromIt = templateNodeToRuntimeNode.find(templateEdge.FromNodeId);
            const auto toIt = templateNodeToRuntimeNode.find(templateEdge.ToNodeId);
            if (fromIt == templateNodeToRuntimeNode.end() || toIt == templateNodeToRuntimeNode.end()) {
                ++droppedEdges;
                continue;
            }

            FGraphEdgeV2 runtimeEdge;
            runtimeEdge.EdgeId = PlaygroundRuntimeInstanceId + "::edge::" + templateEdge.EdgeId;
            runtimeEdge.FromNodeId = fromIt->second;
            runtimeEdge.FromPortId = templateEdge.FromPortId;
            runtimeEdge.ToNodeId = toIt->second;
            runtimeEdge.ToPortId = templateEdge.ToPortId;
            runtimeEdge.Kind = EGraphEdgeKindV2::ValueFlow;
            runtimeEdge.MatchReason = templateEdge.MatchReason;
            runtimeEdge.Diagnostics = templateEdge.Diagnostics;
            runtimeEdge.SuggestedCoercionOperatorIds = templateEdge.SuggestedCoercionOperatorIds;
            runtimeEdge.Metadata.Attrs["template_edge_id"] = MakeStringValue(templateEdge.EdgeId);
            instanceDocument.Edges.push_back(std::move(runtimeEdge));
        }

        PlaygroundRuntimeInstanceDocument = std::move(instanceDocument);
        PlaygroundRuntimeInstanceDocument.Mode = EGraphModeV2::Runtime;
        bPlaygroundRuntimeUseInstantiatedGraph = true;

        PlaygroundRuntimeInstanceDiagnostics.clear();
        PlaygroundRuntimeInstanceDiagnostics.push_back(
            "Bound nodes: " + Slab::ToStr(compilePreview.BoundNodes) +
            ", unbound nodes: " + Slab::ToStr(compilePreview.UnboundNodes) + ".");
        PlaygroundRuntimeInstanceDiagnostics.push_back(
            "Edge summary: exact " + Slab::ToStr(compilePreview.ExactEdges) +
            ", coercion " + Slab::ToStr(compilePreview.CoercionEdges) +
            ", incompatible " + Slab::ToStr(compilePreview.IncompatibleEdges) + ".");
        if (droppedEdges > 0) {
            PlaygroundRuntimeInstanceDiagnostics.push_back(
                "Dropped " + Slab::ToStr(droppedEdges) + " edge(s) due to missing node mapping.");
        }

        if (compilePreview.UnboundNodes > 0 || compilePreview.IncompatibleEdges > 0 || droppedEdges > 0) {
            PlaygroundRuntimeInstanceStatus =
                "[Warn] Instantiated '" + PlaygroundRuntimeInstanceId + "' with diagnostics.";
            PlaygroundTemplateStatus =
                "[Warn] Instantiated runtime graph with diagnostics. Check Runtime tab details.";
        } else {
            PlaygroundRuntimeInstanceStatus =
                "[Ok] Instantiated '" + PlaygroundRuntimeInstanceId + "' with " +
                Slab::ToStr(PlaygroundRuntimeInstanceDocument.Nodes.size()) + " nodes and " +
                Slab::ToStr(PlaygroundRuntimeInstanceDocument.Edges.size()) + " edges.";
            PlaygroundTemplateStatus =
                "[Ok] Runtime graph instantiated as '" + PlaygroundRuntimeInstanceId + "'.";
        }

        markDirty();
        return true;
    };

    const auto computePlaygroundGraphNodeSize = [this](const FPlaygroundGraphVisualNode &node,
                                                       const float minWidth,
                                                       const float maxWidth) -> ImVec2 {
        return ComputePlaygroundGraphNodeSize(
            node,
            minWidth,
            maxWidth,
            BlueprintNodeHeaderMinHeight,
            BlueprintNodeHeaderScale,
            BlueprintNodeBodyTopPadding,
            BlueprintNodeBodyBottomPadding,
            BlueprintNodeBadgeRowGap);
    };

    pruneTemplateNodesWithoutBindings();
    syncTemplateNodePortsFromSemanticSchema();

    if (ImGui::BeginTabBar("GraphPlaygroundModes")) {
        if (ImGui::BeginTabItem("Semantic")) {
            const auto previousSemanticCanvas = PlaygroundSemanticDocument.Canvas;
            std::map<Slab::Str, FGraphPointV2> previousSemanticPositions;
            for (const auto &node : PlaygroundSemanticDocument.Nodes) {
                previousSemanticPositions[node.NodeId] = node.Position;
            }

            std::map<Slab::Str, const FSpaceSchemaV1 *> semanticSpacesById;
            for (const auto *space : semanticSpaces) {
                if (space == nullptr || space->SpaceId.empty()) continue;
                semanticSpacesById[space->SpaceId] = space;
            }

            std::map<Slab::Str, const FSemanticOperatorSchemaV1 *> semanticOperatorsById;
            for (const auto *semanticOperator : semanticOperators) {
                if (semanticOperator == nullptr || semanticOperator->OperatorId.empty()) continue;
                semanticOperatorsById[semanticOperator->OperatorId] = semanticOperator;
            }

            const auto buildSemanticSpaceNode = [&semanticSpacesById,
                                                 &makeSemanticSpaceNodeId](const Slab::Str &spaceId) -> FGraphNodeV2 {
                FGraphNodeV2 node;
                node.NodeId = makeSemanticSpaceNodeId(spaceId);
                node.DisplayName = spaceId;
                node.Kind = EGraphNodeKindV2::Other;
                node.Metadata.Tags.push_back("semantic-space");

                if (const auto spaceIt = semanticSpacesById.find(spaceId); spaceIt != semanticSpacesById.end()) {
                    const auto *space = spaceIt->second;
                    node.DisplayName = space->DisplayName.empty() ? spaceId : space->DisplayName;
                    node.Metadata.Description = space->Latex;
                    node.Metadata.Tags.insert(node.Metadata.Tags.end(), space->Tags.begin(), space->Tags.end());
                    node.Metadata.Attrs = space->Attrs;
                    node.Metadata.Attrs["carrier_type_id"] = MakeStringValue(space->CarrierTypeId);
                    node.Metadata.Attrs["space_id"] = MakeStringValue(space->SpaceId);
                } else {
                    node.Metadata.Attrs["carrier_type_id"] = MakeStringValue("unknown");
                    node.Metadata.Attrs["space_id"] = MakeStringValue(spaceId);
                }

                FGraphPortV2 inputPort;
                inputPort.PortId = "in";
                inputPort.DisplayName = "in";
                inputPort.Direction = EGraphPortDirectionV2::Input;
                inputPort.bRequired = true;
                inputPort.Handle.SpaceId = spaceId;
                if (const auto carrierIt = node.Metadata.Attrs.find("carrier_type_id"); carrierIt != node.Metadata.Attrs.end()) {
                    inputPort.Handle.TypeId = carrierIt->second.Encoded;
                }
                node.Ports.push_back(std::move(inputPort));

                FGraphPortV2 outputPort;
                outputPort.PortId = "out";
                outputPort.DisplayName = "out";
                outputPort.Direction = EGraphPortDirectionV2::Output;
                outputPort.bRequired = true;
                outputPort.Handle.SpaceId = spaceId;
                if (const auto carrierIt = node.Metadata.Attrs.find("carrier_type_id"); carrierIt != node.Metadata.Attrs.end()) {
                    outputPort.Handle.TypeId = carrierIt->second.Encoded;
                }
                node.Ports.push_back(std::move(outputPort));

                return node;
            };

            FGraphDocumentV2 semanticDocument;
            semanticDocument.Mode = EGraphModeV2::Semantic;
            semanticDocument.Canvas = previousSemanticCanvas;

            std::set<Slab::Str> visibleSemanticNodeIds;
            Slab::Vector<Slab::Str> resolvedVisibleSemanticNodeIds;
            resolvedVisibleSemanticNodeIds.reserve(PlaygroundSemanticVisibleNodeIds.size());

            for (const auto &nodeId : PlaygroundSemanticVisibleNodeIds) {
                if (nodeId.empty()) continue;
                if (!visibleSemanticNodeIds.insert(nodeId).second) continue;

                const auto visibleSpaceId = tryExtractSemanticSpaceId(nodeId);
                if (!visibleSpaceId.empty()) {
                    semanticDocument.Nodes.push_back(buildSemanticSpaceNode(visibleSpaceId));
                    resolvedVisibleSemanticNodeIds.push_back(nodeId);
                    continue;
                }

                if (const auto operatorIt = semanticOperatorsById.find(nodeId); operatorIt != semanticOperatorsById.end()) {
                    semanticDocument.Nodes.push_back(MakeGraphNodeFromSemanticOperatorSchema(*operatorIt->second));
                    resolvedVisibleSemanticNodeIds.push_back(nodeId);
                }
            }

            PlaygroundSemanticVisibleNodeIds = resolvedVisibleSemanticNodeIds;

            for (const auto &nodeId : PlaygroundSemanticVisibleNodeIds) {
                const auto operatorIt = semanticOperatorsById.find(nodeId);
                if (operatorIt == semanticOperatorsById.end()) continue;

                const auto &semanticOperator = *operatorIt->second;
                for (const auto &inputPort : semanticOperator.DomainPorts) {
                    const auto fromNodeId = makeSemanticSpaceNodeId(inputPort.SpaceId);
                    if (!visibleSemanticNodeIds.contains(fromNodeId)) continue;
                    semanticDocument.Edges.push_back(FGraphEdgeV2{
                        .EdgeId = semanticOperator.OperatorId + "::in::" + inputPort.PortId,
                        .FromNodeId = fromNodeId,
                        .FromPortId = "out",
                        .ToNodeId = semanticOperator.OperatorId,
                        .ToPortId = inputPort.PortId,
                        .Kind = EGraphEdgeKindV2::ValueFlow
                    });
                }
                for (const auto &outputPort : semanticOperator.CodomainPorts) {
                    const auto toNodeId = makeSemanticSpaceNodeId(outputPort.SpaceId);
                    if (!visibleSemanticNodeIds.contains(toNodeId)) continue;
                    semanticDocument.Edges.push_back(FGraphEdgeV2{
                        .EdgeId = semanticOperator.OperatorId + "::out::" + outputPort.PortId,
                        .FromNodeId = semanticOperator.OperatorId,
                        .FromPortId = outputPort.PortId,
                        .ToNodeId = toNodeId,
                        .ToPortId = "in",
                        .Kind = EGraphEdgeKindV2::ValueFlow
                    });
                }
            }

            int spaceRow = 0;
            int operatorRow = 0;
            int coercionRow = 0;
            int otherRow = 0;
            for (auto &node : semanticDocument.Nodes) {
                if (const auto previousIt = previousSemanticPositions.find(node.NodeId); previousIt != previousSemanticPositions.end()) {
                    node.Position = previousIt->second;
                    continue;
                }

                if (node.Kind == EGraphNodeKindV2::SemanticOperator) {
                    node.Position.x = 560.0f;
                    node.Position.y = 70.0f + static_cast<float>(operatorRow++) * 140.0f;
                } else if (node.Kind == EGraphNodeKindV2::Coercion) {
                    node.Position.x = 900.0f;
                    node.Position.y = 70.0f + static_cast<float>(coercionRow++) * 140.0f;
                } else if (node.Metadata.Tags.end() != std::find(node.Metadata.Tags.begin(), node.Metadata.Tags.end(), "semantic-space")) {
                    node.Position.x = 110.0f;
                    node.Position.y = 70.0f + static_cast<float>(spaceRow++) * 120.0f;
                } else {
                    node.Position.x = 360.0f;
                    node.Position.y = 70.0f + static_cast<float>(otherRow++) * 130.0f;
                }
            }
            PlaygroundSemanticDocument = std::move(semanticDocument);

            {
                auto filterBuffer = std::vector<char>(256, '\0');
                if (!PlaygroundSemanticFilter.empty()) {
                    std::strncpy(filterBuffer.data(), PlaygroundSemanticFilter.c_str(), filterBuffer.size() - 1);
                    filterBuffer.back() = '\0';
                }
                ImGui::SetNextItemWidth(260.0f);
                if (ImGui::InputTextWithHint("##PlaygroundSemanticFilter", "Filter spaces/operators...", filterBuffer.data(), filterBuffer.size())) {
                    const auto previousFilter = PlaygroundSemanticFilter;
                    PlaygroundSemanticFilter = filterBuffer.data();
                    if (PlaygroundSemanticFilter != previousFilter) markDirty();
                }
            }
            ImGui::SameLine();
            if (ImGui::Button("Clear##SemanticGraph")) {
                PlaygroundSemanticVisibleNodeIds.clear();
                PlaygroundSemanticDocument.Nodes.clear();
                PlaygroundSemanticDocument.Edges.clear();
                markDirty();
            }
            ImGui::SameLine();
            if (ImGui::Checkbox("Grid##SemanticGraph", &PlaygroundSemanticDocument.Canvas.bShowGrid)) {
                markDirty();
            }
            ImGui::SameLine();
            if (ImGui::Button("Auto Layout##SemanticGraph")) {
                int spaceLane = 0;
                int opLane = 0;
                int coercionLane = 0;
                int otherLane = 0;
                for (auto &node : PlaygroundSemanticDocument.Nodes) {
                    if (node.Kind == EGraphNodeKindV2::SemanticOperator) {
                        node.Position.x = 560.0f;
                        node.Position.y = 70.0f + static_cast<float>(opLane++) * 140.0f;
                    } else if (node.Kind == EGraphNodeKindV2::Coercion) {
                        node.Position.x = 900.0f;
                        node.Position.y = 70.0f + static_cast<float>(coercionLane++) * 140.0f;
                    } else if (node.Metadata.Tags.end() != std::find(node.Metadata.Tags.begin(), node.Metadata.Tags.end(), "semantic-space")) {
                        node.Position.x = 110.0f;
                        node.Position.y = 70.0f + static_cast<float>(spaceLane++) * 120.0f;
                    } else {
                        node.Position.x = 360.0f;
                        node.Position.y = 70.0f + static_cast<float>(otherLane++) * 130.0f;
                    }
                }
                markDirty();
            }

            const auto resolveSemanticSourceSpaceId =
                [&tryExtractSemanticSpaceId](const FGraphNodeV2 &sourceNode,
                                             FGraphPortV2 *sourcePort,
                                             const EGraphPortDirectionV2 sourceDirection) -> Slab::Str {
                if (sourcePort != nullptr && !sourcePort->Handle.SpaceId.empty()) {
                    return sourcePort->Handle.SpaceId;
                }

                if (const auto spaceId = tryExtractSemanticSpaceId(sourceNode.NodeId); !spaceId.empty()) {
                    return spaceId;
                }

                const auto expectedDirection = sourceDirection;
                const auto portIt = std::find_if(sourceNode.Ports.begin(), sourceNode.Ports.end(), [&](const auto &port) {
                    return port.Direction == expectedDirection && !port.Handle.SpaceId.empty();
                });
                if (portIt == sourceNode.Ports.end()) return {};
                return portIt->Handle.SpaceId;
            };

            Slab::Vector<FPlaygroundGraphVisualNode> semanticVisualNodes;
            semanticVisualNodes.reserve(PlaygroundSemanticDocument.Nodes.size());
            for (auto &node : PlaygroundSemanticDocument.Nodes) {
                const bool bMatches =
                    ContainsCaseInsensitive(node.DisplayName, PlaygroundSemanticFilter) ||
                    ContainsCaseInsensitive(node.NodeId, PlaygroundSemanticFilter);
                if (!bMatches) continue;

                FPlaygroundGraphVisualNode visualNode;
                visualNode.Node = &node;
                visualNode.Title = node.DisplayName.empty() ? node.NodeId : node.DisplayName;
                visualNode.Subtitle = node.NodeId;

                if (node.Kind == EGraphNodeKindV2::SemanticOperator || node.Kind == EGraphNodeKindV2::Coercion) {
                    visualNode.Badges.push_back({
                        WithPolicyPrefix('N', node.Kind == EGraphNodeKindV2::Coercion ? "Coercion" : "Operator"),
                        node.Kind == EGraphNodeKindV2::Coercion ? IM_COL32(192, 154, 96, 220) : IM_COL32(102, 142, 188, 220)});
                    const auto inputPins = static_cast<int>(std::count_if(
                        node.Ports.begin(),
                        node.Ports.end(),
                        [](const auto &port) { return port.Direction == EGraphPortDirectionV2::Input; }));
                    const auto outputPins = static_cast<int>(std::count_if(
                        node.Ports.begin(),
                        node.Ports.end(),
                        [](const auto &port) { return port.Direction == EGraphPortDirectionV2::Output; }));
                    visualNode.InputPins = std::max(1, inputPins);
                    visualNode.OutputPins = std::max(1, outputPins);
                    visualNode.Badges.push_back({WithPolicyPrefix('I', Slab::ToStr(inputPins)), IM_COL32(104, 148, 204, 220)});
                    visualNode.Badges.push_back({WithPolicyPrefix('O', Slab::ToStr(outputPins)), IM_COL32(86, 164, 140, 220)});
                } else {
                    visualNode.Badges.push_back({WithPolicyPrefix('N', "Space"), IM_COL32(116, 126, 146, 220)});
                    if (const auto carrierIt = node.Metadata.Attrs.find("carrier_type_id"); carrierIt != node.Metadata.Attrs.end()) {
                        visualNode.Badges.push_back({WithPolicyPrefix('C', TruncateLabel(carrierIt->second.Encoded, 14)), IM_COL32(94, 120, 156, 220)});
                    }
                    visualNode.InputPins = 1;
                    visualNode.OutputPins = 1;
                }
                visualNode.Size = computePlaygroundGraphNodeSize(visualNode, 280.0f, 520.0f);
                semanticVisualNodes.push_back(std::move(visualNode));
            }

            ImGui::Text(
                "Semantic graph: %zu nodes | %zu edges | %zu filtered nodes",
                PlaygroundSemanticDocument.Nodes.size(),
                PlaygroundSemanticDocument.Edges.size(),
                semanticVisualNodes.size());
            ImGui::TextDisabled("Right-click canvas to add spaces/operators. Drag from a pin to expand along matching semantic spaces.");

            const auto buildSemanticAddNodeActions =
                [this,
                 &semanticSpaces,
                 &semanticOperators,
                 &makeSemanticSpaceNodeId,
                 &isSemanticNodeVisible,
                 &addSemanticVisibleNodeAt](const ImVec2 &worldPos) -> Slab::Vector<FPlaygroundGraphCanvasAction> {
                Slab::Vector<FPlaygroundGraphCanvasAction> actions;
                const auto matchesActionFilter = [this](const Slab::Str &label, const Slab::Str &id) -> bool {
                    return PlaygroundSemanticFilter.empty() ||
                           ContainsCaseInsensitive(label, PlaygroundSemanticFilter) ||
                           ContainsCaseInsensitive(id, PlaygroundSemanticFilter);
                };

                bool bAddedSpaceHeader = false;
                for (const auto *space : semanticSpaces) {
                    if (space == nullptr || space->SpaceId.empty()) continue;
                    const auto nodeId = makeSemanticSpaceNodeId(space->SpaceId);
                    if (isSemanticNodeVisible(nodeId)) continue;
                    const auto label = space->DisplayName.empty() ? space->SpaceId : space->DisplayName;
                    if (!matchesActionFilter(label, space->SpaceId)) continue;

                    if (!bAddedSpaceHeader) {
                        actions.push_back(FPlaygroundGraphCanvasAction{
                            .Label = "Spaces",
                            .Hint = "Semantic spaces from the catalog.",
                            .bEnabled = false
                        });
                        bAddedSpaceHeader = true;
                    }

                    actions.push_back(FPlaygroundGraphCanvasAction{
                        .Label = Slab::Str("Add Space: ") + label,
                        .Hint = "Adds the semantic space node at the clicked position.",
                        .Execute = [&, nodeId, worldPos]() {
                            (void) addSemanticVisibleNodeAt(nodeId, worldPos);
                        }
                    });
                }

                bool bAddedOperatorHeader = false;
                for (const auto *semanticOperator : semanticOperators) {
                    if (semanticOperator == nullptr || semanticOperator->OperatorId.empty()) continue;
                    if (isSemanticNodeVisible(semanticOperator->OperatorId)) continue;
                    const auto label = semanticOperator->DisplayName.empty()
                        ? semanticOperator->OperatorId
                        : semanticOperator->DisplayName;
                    if (!matchesActionFilter(label, semanticOperator->OperatorId)) continue;

                    if (!bAddedOperatorHeader) {
                        actions.push_back(FPlaygroundGraphCanvasAction{
                            .Label = "Operators",
                            .Hint = "Semantic operators and coercions from the catalog.",
                            .bEnabled = false
                        });
                        bAddedOperatorHeader = true;
                    }

                    actions.push_back(FPlaygroundGraphCanvasAction{
                        .Label = Slab::Str(semanticOperator->Kind == ESemanticOperatorKindV1::Coercion ? "Add Coercion: " : "Add Operator: ") + label,
                        .Hint = "Adds the semantic operator node at the clicked position.",
                        .Execute = [&, operatorId = semanticOperator->OperatorId, worldPos]() {
                            (void) addSemanticVisibleNodeAt(operatorId, worldPos);
                        }
                    });
                }

                if (actions.empty()) {
                    actions.push_back(FPlaygroundGraphCanvasAction{
                        .Label = "No hidden semantic nodes",
                        .Hint = "All matching semantic spaces/operators are already shown.",
                        .bEnabled = false
                    });
                }
                return actions;
            };
            const auto buildSemanticConnectNodeActions =
                [this,
                 &semanticOperators,
                 &semanticSpacesById,
                 &makeSemanticSpaceNodeId,
                 &isSemanticNodeVisible,
                 &addSemanticVisibleNodeAt,
                 &resolveSemanticSourceSpaceId](const FGraphNodeV2 &sourceNode,
                                                FGraphPortV2 *sourcePort,
                                                const EGraphPortDirectionV2 sourceDirection,
                                                const ImVec2 &worldPos) -> Slab::Vector<FPlaygroundGraphCanvasAction> {
                Slab::Vector<FPlaygroundGraphCanvasAction> actions;
                const auto sourceSpaceId = resolveSemanticSourceSpaceId(sourceNode, sourcePort, sourceDirection);
                if (sourceSpaceId.empty()) {
                    actions.push_back(FPlaygroundGraphCanvasAction{
                        .Label = "Source space unresolved",
                        .Hint = "The dragged pin does not map to a semantic space.",
                        .bEnabled = false
                    });
                    return actions;
                }

                const auto spaceNodeId = makeSemanticSpaceNodeId(sourceSpaceId);
                if (!isSemanticNodeVisible(spaceNodeId)) {
                    Slab::Str spaceLabel = sourceSpaceId;
                    if (const auto spaceIt = semanticSpacesById.find(sourceSpaceId); spaceIt != semanticSpacesById.end()) {
                        const auto *space = spaceIt->second;
                        if (space != nullptr && !space->DisplayName.empty()) spaceLabel = space->DisplayName;
                    }

                    actions.push_back(FPlaygroundGraphCanvasAction{
                        .Label = Slab::Str("Add Space: ") + spaceLabel,
                        .Hint = "Adds the semantic space node carried by the dragged pin.",
                        .Execute = [&, spaceNodeId, worldPos]() {
                            (void) addSemanticVisibleNodeAt(spaceNodeId, worldPos);
                        }
                    });
                }

                const auto addConnectedOperatorAction =
                    [&](const FSemanticOperatorSchemaV1 &semanticOperator,
                        const Slab::Str &matchedPortLabel) {
                    if (semanticOperator.OperatorId.empty()) return;
                    if (semanticOperator.OperatorId == sourceNode.SemanticOperatorId) return;
                    if (isSemanticNodeVisible(semanticOperator.OperatorId)) return;

                    const auto label = semanticOperator.DisplayName.empty()
                        ? semanticOperator.OperatorId
                        : semanticOperator.DisplayName;
                    const auto midpoint = ImVec2(
                        0.5f * (sourceNode.Position.x + worldPos.x),
                        0.5f * (sourceNode.Position.y + worldPos.y));

                    actions.push_back(FPlaygroundGraphCanvasAction{
                        .Label = Slab::Str(semanticOperator.Kind == ESemanticOperatorKindV1::Coercion ? "Add Coercion: " : "Add Operator: ") + label,
                        .Hint = matchedPortLabel.empty()
                            ? Slab::Str("Adds a node connected through semantic space '") + sourceSpaceId + "'."
                            : Slab::Str("Matches port '") + matchedPortLabel + "' on semantic space '" + sourceSpaceId + "'.",
                        .Execute = [&, operatorId = semanticOperator.OperatorId, spaceNodeId, midpoint, worldPos]() {
                            (void) addSemanticVisibleNodeAt(operatorId, worldPos);
                            if (!isSemanticNodeVisible(spaceNodeId)) {
                                (void) addSemanticVisibleNodeAt(spaceNodeId, midpoint);
                            }
                        }
                    });
                };

                for (const auto *semanticOperator : semanticOperators) {
                    if (semanticOperator == nullptr || semanticOperator->OperatorId.empty()) continue;

                    if (sourceDirection == EGraphPortDirectionV2::Output) {
                        for (const auto &inputPort : semanticOperator->DomainPorts) {
                            if (inputPort.SpaceId != sourceSpaceId) continue;
                            addConnectedOperatorAction(*semanticOperator, inputPort.DisplayName.empty() ? inputPort.PortId : inputPort.DisplayName);
                            break;
                        }
                    } else {
                        for (const auto &outputPort : semanticOperator->CodomainPorts) {
                            if (outputPort.SpaceId != sourceSpaceId) continue;
                            addConnectedOperatorAction(*semanticOperator, outputPort.DisplayName.empty() ? outputPort.PortId : outputPort.DisplayName);
                            break;
                        }
                    }
                }

                if (actions.empty()) {
                    actions.push_back(FPlaygroundGraphCanvasAction{
                        .Label = "No hidden connected nodes",
                        .Hint = "All matching semantic spaces/operators are already shown.",
                        .bEnabled = false
                    });
                }
                return actions;
            };
            DrawSubstrateGraphCanvasCommon(
                "SemanticPlaygroundCanvas",
                PlaygroundSemanticDocument,
                semanticVisualNodes,
                PlaygroundSemanticBottomPanelHeight,
                buildSemanticAddNodeActions,
                buildSemanticConnectNodeActions,
                markDirty);

            if (ImGui::BeginChild("SemanticCatalogPane", ImVec2(0.0f, PlaygroundSemanticBottomPanelHeight), true)) {
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
            }
            ImGui::EndChild();
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Template")) {
            const auto *selectedOperator = semanticCatalog.FindOperatorById(PlaygroundTemplateSelectedOperatorId);
            if (selectedOperator != nullptr && !isTemplateOperatorWorking(selectedOperator->OperatorId)) {
                selectedOperator = nullptr;
            }
            const auto selectedOperatorLabel = selectedOperator == nullptr
                ? "<no working operator>"
                : selectedOperator->DisplayName + "###" + selectedOperator->OperatorId;
            ImGui::TextDisabled(
                "Working semantic operators: %zu / %zu",
                templateWorkingOperators.size(),
                semanticOperators.size());
            if (ImGui::BeginCombo("Operator Seed", selectedOperatorLabel.c_str())) {
                for (const auto *semanticOperator : templateWorkingOperators) {
                    if (semanticOperator == nullptr) continue;
                    const bool bSelected = PlaygroundTemplateSelectedOperatorId == semanticOperator->OperatorId;
                    if (ImGui::Selectable(semanticOperator->DisplayName.c_str(), bSelected)) {
                        PlaygroundTemplateSelectedOperatorId = semanticOperator->OperatorId;
                        selectedOperator = semanticOperator;
                    }
                    if (bSelected) ImGui::SetItemDefaultFocus();
                }
                if (templateWorkingOperators.empty()) {
                    ImGui::TextDisabled("No bound semantic operators are currently available.");
                }
                ImGui::EndCombo();
            }

            const auto addTemplateNode =
                [this, &selectedOperator, &buildTemplateNodePortsFromOperator, &markDirty](const ImVec2 *worldPosition = nullptr) -> bool {
                if (selectedOperator == nullptr) {
                    PlaygroundTemplateStatus = "[Error] Select a working semantic operator first.";
                    return false;
                }

                ++PlaygroundTemplateNodeCounter;
                FGraphNodeV2 node;
                node.NodeId = "tmpl.node." + Slab::ToStr(PlaygroundTemplateNodeCounter);
                node.SemanticOperatorId = selectedOperator->OperatorId;
                node.Kind = selectedOperator->Kind == ESemanticOperatorKindV1::Coercion
                    ? EGraphNodeKindV2::Coercion
                    : EGraphNodeKindV2::SemanticOperator;
                node.Ports = buildTemplateNodePortsFromOperator(*selectedOperator);
                if (worldPosition != nullptr) {
                    node.Position.x = worldPosition->x;
                    node.Position.y = worldPosition->y;
                } else {
                    node.Position.x = 80.0f + static_cast<float>((PlaygroundTemplateNodeCounter % 4) * 280);
                    node.Position.y = 70.0f + static_cast<float>((PlaygroundTemplateNodeCounter / 4) * 140);
                }
                PlaygroundTemplateDocument.Nodes.push_back(std::move(node));
                PlaygroundTemplateSelectedNodeId = PlaygroundTemplateDocument.Nodes.back().NodeId;
                PlaygroundTemplateSelectedNodeIds = {PlaygroundTemplateDocument.Nodes.back().NodeId};
                PlaygroundTemplateSelectedEdgeId.clear();
                PlaygroundTemplateStatus = "[Ok] Added semantic node '" + selectedOperator->DisplayName + "'.";
                markDirty();
                return true;
            };

            ImGui::SameLine();
            if (ImGui::Button("Add Node")) {
                (void) addTemplateNode();
            }

            ImGui::SameLine();
            if (ImGui::Button("Instantiate Runtime")) {
                (void) instantiateTemplateRuntimeGraph();
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

            const bool bTemplateHotkeysEnabled =
                ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) &&
                !ImGui::GetIO().WantTextInput;
            if (bTemplateHotkeysEnabled && ImGui::IsKeyPressed(ImGuiKey_A, false)) {
                (void) addTemplateNode();
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
            Slab::Vector<FPlaygroundGraphVisualNode> templateVisualNodes;
            templateVisualNodes.reserve(PlaygroundTemplateDocument.Nodes.size());

            for (auto &node : PlaygroundTemplateDocument.Nodes) {
                const auto *semanticOperator = semanticCatalog.FindOperatorById(node.SemanticOperatorId);
                if (semanticOperator == nullptr) continue;

                FPlaygroundGraphVisualNode visualNode;
                visualNode.Node = &node;
                visualNode.Title = semanticOperator->DisplayName.empty() ? node.NodeId : semanticOperator->DisplayName;
                visualNode.Subtitle = node.NodeId;

                const bool bCoercion =
                    semanticOperator->Kind == ESemanticOperatorKindV1::Coercion ||
                    node.Kind == EGraphNodeKindV2::Coercion;
                visualNode.Badges.push_back({
                    WithPolicyPrefix('N', bCoercion ? "Coercion" : "Operator"),
                    bCoercion ? IM_COL32(192, 154, 96, 220) : IM_COL32(102, 142, 188, 220)});
                visualNode.Badges.push_back({
                    WithPolicyPrefix('I', Slab::ToStr(semanticOperator->DomainPorts.size())),
                    IM_COL32(104, 148, 204, 220)});
                visualNode.Badges.push_back({
                    WithPolicyPrefix('O', Slab::ToStr(semanticOperator->CodomainPorts.size())),
                    IM_COL32(86, 164, 140, 220)});

                const auto bindings = semanticCatalog.QueryBindingsForOperator(semanticOperator->OperatorId);
                visualNode.Badges.push_back({WithPolicyPrefix('B', Slab::ToStr(bindings.size())), IM_COL32(124, 132, 156, 220)});

                visualNode.InputPins = static_cast<int>(semanticOperator->DomainPorts.size());
                visualNode.OutputPins = static_cast<int>(semanticOperator->CodomainPorts.size());
                visualNode.Size = computePlaygroundGraphNodeSize(visualNode, 280.0f, 520.0f);
                templateVisualNodes.push_back(std::move(visualNode));
            }

            if (bRequestFitToContent) {
                if (templateVisualNodes.empty()) {
                    PlaygroundTemplateStatus = "[Warn] Nothing to fit.";
                } else {
                    float minX = std::numeric_limits<float>::max();
                    float minY = std::numeric_limits<float>::max();
                    float maxX = std::numeric_limits<float>::lowest();
                    float maxY = std::numeric_limits<float>::lowest();
                    for (const auto &visualNode : templateVisualNodes) {
                        if (visualNode.Node == nullptr) continue;
                        minX = std::min(minX, visualNode.Node->Position.x);
                        minY = std::min(minY, visualNode.Node->Position.y);
                        maxX = std::max(maxX, visualNode.Node->Position.x + visualNode.Size.x);
                        maxY = std::max(maxY, visualNode.Node->Position.y + visualNode.Size.y);
                    }
                    if (minX <= maxX && minY <= maxY) {
                        const auto contentWidth = std::max(1.0f, maxX - minX);
                        const auto contentHeight = std::max(1.0f, maxY - minY);
                        const auto available = ImGui::GetContentRegionAvail();
                        const auto viewportWidth = std::max(320.0f, available.x);
                        const auto viewportHeight = std::max(220.0f, available.y - PlaygroundTemplateBottomPanelHeight - 6.0f);
                        PlaygroundTemplateDocument.Canvas.PanX = 0.5f * (viewportWidth - contentWidth) - minX;
                        PlaygroundTemplateDocument.Canvas.PanY = 0.5f * (viewportHeight - contentHeight) - minY;
                        PlaygroundTemplateStatus = "[Ok] View fitted to graph bounds.";
                        markDirty();
                    }
                }
            }

            const auto buildTemplateAddNodeActions =
                [this, &selectedOperator, &addTemplateNode](const ImVec2 &worldPos) -> Slab::Vector<FPlaygroundGraphCanvasAction> {
                Slab::Vector<FPlaygroundGraphCanvasAction> actions;
                actions.push_back(FPlaygroundGraphCanvasAction{
                    .Label = selectedOperator != nullptr
                        ? Slab::Str("Add ") + selectedOperator->DisplayName
                        : Slab::Str("Add Selected Operator Node"),
                    .Hint = "Adds a semantic operator node at the clicked position.",
                    .bEnabled = selectedOperator != nullptr,
                    .Execute = [this, &addTemplateNode, worldPos]() {
                        (void) addTemplateNode(&worldPos);
                    }
                });
                return actions;
            };

            const auto buildTemplateConnectNodeActions =
                [this,
                 &semanticCatalog,
                 &attemptTemplateConnection,
                 &evaluateSpaceMatch](const FGraphNodeV2 &sourceNode,
                                      FGraphPortV2 *sourcePort,
                                      const EGraphPortDirectionV2 sourceDirection,
                                      const ImVec2 &) -> Slab::Vector<FPlaygroundGraphCanvasAction> {
                Slab::Vector<FPlaygroundGraphCanvasAction> actions;

                const auto *sourceOperator = semanticCatalog.FindOperatorById(sourceNode.SemanticOperatorId);
                if (sourceOperator == nullptr) {
                    actions.push_back(FPlaygroundGraphCanvasAction{
                        .Label = "Source operator missing",
                        .Hint = "Cannot resolve semantic operator schema for source node.",
                        .bEnabled = false
                    });
                    return actions;
                }

                const auto findSemanticPortById =
                    [](const Slab::Vector<FSemanticPortSchemaV1> &ports,
                       const Slab::Str &portId) -> const FSemanticPortSchemaV1 * {
                    const auto it = std::find_if(ports.begin(), ports.end(), [&](const auto &port) {
                        return port.PortId == portId;
                    });
                    if (it == ports.end()) return nullptr;
                    return &(*it);
                };

                const auto *sourceSemanticPort = static_cast<const FSemanticPortSchemaV1 *>(nullptr);
                if (sourcePort != nullptr && !sourcePort->PortId.empty()) {
                    sourceSemanticPort =
                        sourceDirection == EGraphPortDirectionV2::Output
                            ? findSemanticPortById(sourceOperator->CodomainPorts, sourcePort->PortId)
                            : findSemanticPortById(sourceOperator->DomainPorts, sourcePort->PortId);
                }
                if (sourceSemanticPort == nullptr) {
                    if (sourceDirection == EGraphPortDirectionV2::Output) {
                        if (!sourceOperator->CodomainPorts.empty()) sourceSemanticPort = &sourceOperator->CodomainPorts.front();
                    } else {
                        if (!sourceOperator->DomainPorts.empty()) sourceSemanticPort = &sourceOperator->DomainPorts.front();
                    }
                }
                if (sourceSemanticPort == nullptr) {
                    actions.push_back(FPlaygroundGraphCanvasAction{
                        .Label = "Source port missing",
                        .Hint = "Source node does not expose a compatible semantic port.",
                        .bEnabled = false
                    });
                    return actions;
                }

                for (const auto &candidateNode : PlaygroundTemplateDocument.Nodes) {
                    if (candidateNode.NodeId == sourceNode.NodeId) continue;
                    const auto *candidateOperator = semanticCatalog.FindOperatorById(candidateNode.SemanticOperatorId);
                    if (candidateOperator == nullptr) continue;

                    if (sourceDirection == EGraphPortDirectionV2::Output) {
                        for (const auto &targetPort : candidateOperator->DomainPorts) {
                            const auto match = evaluateSpaceMatch(sourceSemanticPort->SpaceId, targetPort.SpaceId);
                            Slab::Str hint = "Match: " + match.Reason;
                            if (!match.Diagnostics.empty()) hint += " | " + match.Diagnostics.front();
                            const auto label =
                                sourceNode.NodeId + ":" + sourceSemanticPort->PortId +
                                " -> " + candidateNode.NodeId + ":" + targetPort.PortId;
                            actions.push_back(FPlaygroundGraphCanvasAction{
                                .Label = Slab::Str("Connect ") + label,
                                .Hint = std::move(hint),
                                .bEnabled = true,
                                .Execute = [&, fromNodeId = sourceNode.NodeId,
                                            fromPortId = sourceSemanticPort->PortId,
                                            toNodeId = candidateNode.NodeId,
                                            toPortId = targetPort.PortId]() {
                                    attemptTemplateConnection(fromNodeId, fromPortId, toNodeId, toPortId);
                                }
                            });
                        }
                    } else {
                        for (const auto &candidatePort : candidateOperator->CodomainPorts) {
                            const auto match = evaluateSpaceMatch(candidatePort.SpaceId, sourceSemanticPort->SpaceId);
                            Slab::Str hint = "Match: " + match.Reason;
                            if (!match.Diagnostics.empty()) hint += " | " + match.Diagnostics.front();
                            const auto label =
                                candidateNode.NodeId + ":" + candidatePort.PortId +
                                " -> " + sourceNode.NodeId + ":" + sourceSemanticPort->PortId;
                            actions.push_back(FPlaygroundGraphCanvasAction{
                                .Label = Slab::Str("Connect ") + label,
                                .Hint = std::move(hint),
                                .bEnabled = true,
                                .Execute = [&, fromNodeId = candidateNode.NodeId,
                                            fromPortId = candidatePort.PortId,
                                            toNodeId = sourceNode.NodeId,
                                            toPortId = sourceSemanticPort->PortId]() {
                                    attemptTemplateConnection(fromNodeId, fromPortId, toNodeId, toPortId);
                                }
                            });
                        }
                    }
                }

                if (actions.empty()) {
                    actions.push_back(FPlaygroundGraphCanvasAction{
                        .Label = "No connectable nodes suggested",
                        .Hint = "No candidate ports were found for the dragged pin.",
                        .bEnabled = false
                    });
                }
                return actions;
            };

            FLabV2SubstrateGraphCanvasInteraction templateCanvasInteraction;
            DrawSubstrateGraphCanvasCommon(
                "TemplatePlaygroundCanvas",
                PlaygroundTemplateDocument,
                templateVisualNodes,
                PlaygroundTemplateBottomPanelHeight,
                buildTemplateAddNodeActions,
                buildTemplateConnectNodeActions,
                markDirty,
                &templateCanvasInteraction);

            if (!templateCanvasInteraction.ClickedEdgeId.empty()) {
                PlaygroundTemplateSelectedEdgeId = templateCanvasInteraction.ClickedEdgeId;
                PlaygroundTemplateSelectedNodeId.clear();
                PlaygroundTemplateSelectedNodeIds.clear();
            } else if (!templateCanvasInteraction.ClickedNodeId.empty()) {
                PlaygroundTemplateSelectedNodeId = templateCanvasInteraction.ClickedNodeId;
                PlaygroundTemplateSelectedNodeIds = {templateCanvasInteraction.ClickedNodeId};
                PlaygroundTemplateSelectedEdgeId.clear();
            } else if (templateCanvasInteraction.bBackgroundClicked) {
                PlaygroundTemplateSelectedNodeId.clear();
                PlaygroundTemplateSelectedNodeIds.clear();
                PlaygroundTemplateSelectedEdgeId.clear();
            }

            ImGui::TextDisabled(
                "Template graph: %zu nodes, %zu edges | Shortcuts: A add, Del delete, F fit | Right-drag pans.",
                PlaygroundTemplateDocument.Nodes.size(),
                PlaygroundTemplateDocument.Edges.size());

            if (ImGui::BeginChild("TemplateInspectorPane", ImVec2(0.0f, PlaygroundTemplateBottomPanelHeight), true)) {
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

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Runtime")) {
            auto &catalogRegistry = FReflectionCatalogRegistryV2::Get();
            catalogRegistry.RefreshAll();
            const auto mergedCatalog = catalogRegistry.BuildMergedCatalog();
            SyncReflectionSemanticsOverlayV1(mergedCatalog);

            const bool bHasInstantiatedRuntimeGraph = !PlaygroundRuntimeInstanceDocument.Nodes.empty();
            if (!bHasInstantiatedRuntimeGraph && bPlaygroundRuntimeUseInstantiatedGraph) {
                bPlaygroundRuntimeUseInstantiatedGraph = false;
            }

            ImGui::TextDisabled("View");
            ImGui::SameLine();
            if (ImGui::RadioButton("Reflection Snapshot", !bPlaygroundRuntimeUseInstantiatedGraph)) {
                bPlaygroundRuntimeUseInstantiatedGraph = false;
                markDirty();
            }
            ImGui::SameLine();
            if (!bHasInstantiatedRuntimeGraph) ImGui::BeginDisabled();
            if (ImGui::RadioButton("Instantiated Graph", bPlaygroundRuntimeUseInstantiatedGraph)) {
                bPlaygroundRuntimeUseInstantiatedGraph = true;
                markDirty();
            }
            if (!bHasInstantiatedRuntimeGraph) {
                ImGui::EndDisabled();
                ImGui::SameLine();
                ImGui::TextDisabled("(instantiate from Template)");
            }

            if (!bPlaygroundRuntimeUseInstantiatedGraph) {
                const auto previousRuntimeCanvas = PlaygroundRuntimeDocument.Canvas;
                std::map<Slab::Str, FGraphPointV2> previousRuntimePositions;
                for (const auto &node : PlaygroundRuntimeDocument.Nodes) {
                    previousRuntimePositions[node.NodeId] = node.Position;
                }

                PlaygroundRuntimeDocument = MakeGraphDocumentFromReflectionCatalog(mergedCatalog, EGraphModeV2::Runtime);
                PlaygroundRuntimeDocument.Canvas = previousRuntimeCanvas;
                PlaygroundRuntimeDocument.Mode = EGraphModeV2::Runtime;

                int interfaceLane = 0;
                int parameterLane = 0;
                int operationLane = 0;
                int otherLane = 0;
                for (auto &node : PlaygroundRuntimeDocument.Nodes) {
                    if (const auto previousIt = previousRuntimePositions.find(node.NodeId); previousIt != previousRuntimePositions.end()) {
                        node.Position = previousIt->second;
                        continue;
                    }

                    if (node.Kind == EGraphNodeKindV2::Interface) {
                        node.Position.x = 560.0f;
                        node.Position.y = 80.0f + static_cast<float>(interfaceLane++) * 160.0f;
                    } else if (node.Kind == EGraphNodeKindV2::Parameter) {
                        node.Position.x = 110.0f;
                        node.Position.y = 80.0f + static_cast<float>(parameterLane++) * 128.0f;
                    } else if (node.Kind == EGraphNodeKindV2::Operation) {
                        node.Position.x = 980.0f;
                        node.Position.y = 80.0f + static_cast<float>(operationLane++) * 140.0f;
                    } else {
                        node.Position.x = 320.0f;
                        node.Position.y = 80.0f + static_cast<float>(otherLane++) * 132.0f;
                    }
                }
            }

            auto &activeRuntimeDocument = bPlaygroundRuntimeUseInstantiatedGraph
                ? PlaygroundRuntimeInstanceDocument
                : PlaygroundRuntimeDocument;

            if (bPlaygroundRuntimeUseInstantiatedGraph) {
                ImGui::Text(
                    "Runtime instance: %s | %zu nodes | %zu edges",
                    PlaygroundRuntimeInstanceId.empty() ? "<unknown>" : PlaygroundRuntimeInstanceId.c_str(),
                    activeRuntimeDocument.Nodes.size(),
                    activeRuntimeDocument.Edges.size());
                if (!PlaygroundRuntimeInstanceStatus.empty()) {
                    const bool bWarn = PlaygroundRuntimeInstanceStatus.starts_with("[Warn]");
                    const bool bError = PlaygroundRuntimeInstanceStatus.starts_with("[Error]");
                    const auto statusColor = bError
                        ? ImVec4(0.92f, 0.36f, 0.36f, 1.0f)
                        : (bWarn ? ImVec4(0.93f, 0.73f, 0.30f, 1.0f) : ImVec4(0.42f, 0.83f, 0.52f, 1.0f));
                    ImGui::TextColored(statusColor, "%s", PlaygroundRuntimeInstanceStatus.c_str());
                }
            } else {
                ImGui::Text(
                    "Runtime reflection snapshot: %zu interfaces | %zu graph nodes | %zu graph members",
                    mergedCatalog.Interfaces.size(),
                    activeRuntimeDocument.Nodes.size(),
                    CountGraphMembers(activeRuntimeDocument));
            }

            {
                auto buffer = std::vector<char>(256, '\0');
                if (!PlaygroundRuntimeFilter.empty()) {
                    std::strncpy(buffer.data(), PlaygroundRuntimeFilter.c_str(), buffer.size() - 1);
                    buffer.back() = '\0';
                }
                if (ImGui::InputTextWithHint("##RuntimeFilter", "Filter nodes...", buffer.data(), buffer.size())) {
                    const auto previousFilter = PlaygroundRuntimeFilter;
                    PlaygroundRuntimeFilter = buffer.data();
                    if (PlaygroundRuntimeFilter != previousFilter) markDirty();
                }
            }
            ImGui::SameLine();
            if (ImGui::Checkbox("Grid##RuntimeGraph", &activeRuntimeDocument.Canvas.bShowGrid)) {
                markDirty();
            }
            ImGui::SameLine();
            if (ImGui::Button("Auto Layout##RuntimeGraph")) {
                int interfaceRow = 0;
                int parameterRow = 0;
                int operationRow = 0;
                int semanticOperatorRow = 0;
                int coercionRow = 0;
                int otherRow = 0;
                for (auto &node : activeRuntimeDocument.Nodes) {
                    if (node.Kind == EGraphNodeKindV2::Interface) {
                        node.Position.x = 560.0f;
                        node.Position.y = 80.0f + static_cast<float>(interfaceRow++) * 160.0f;
                    } else if (node.Kind == EGraphNodeKindV2::Parameter) {
                        node.Position.x = 110.0f;
                        node.Position.y = 80.0f + static_cast<float>(parameterRow++) * 128.0f;
                    } else if (node.Kind == EGraphNodeKindV2::Operation) {
                        node.Position.x = 980.0f;
                        node.Position.y = 80.0f + static_cast<float>(operationRow++) * 140.0f;
                    } else if (node.Kind == EGraphNodeKindV2::SemanticOperator) {
                        node.Position.x = 980.0f;
                        node.Position.y = 80.0f + static_cast<float>(semanticOperatorRow++) * 140.0f;
                    } else if (node.Kind == EGraphNodeKindV2::Coercion) {
                        node.Position.x = 1320.0f;
                        node.Position.y = 80.0f + static_cast<float>(coercionRow++) * 140.0f;
                    } else {
                        node.Position.x = 320.0f;
                        node.Position.y = 80.0f + static_cast<float>(otherRow++) * 132.0f;
                    }
                }
                markDirty();
            }

            Slab::Vector<FPlaygroundGraphVisualNode> runtimeVisualNodes;
            runtimeVisualNodes.reserve(activeRuntimeDocument.Nodes.size());
            for (auto &node : activeRuntimeDocument.Nodes) {
                const bool bMatches =
                    ContainsCaseInsensitive(node.DisplayName, PlaygroundRuntimeFilter) ||
                    ContainsCaseInsensitive(node.NodeId, PlaygroundRuntimeFilter) ||
                    ContainsCaseInsensitive(node.SourceInterfaceId, PlaygroundRuntimeFilter) ||
                    ContainsCaseInsensitive(node.SourceOperationId, PlaygroundRuntimeFilter) ||
                    ContainsCaseInsensitive(node.SourceParameterId, PlaygroundRuntimeFilter);
                if (!bMatches) continue;

                FPlaygroundGraphVisualNode visualNode;
                visualNode.Node = &node;
                visualNode.Title = node.DisplayName.empty() ? node.NodeId : node.DisplayName;
                visualNode.Subtitle = node.NodeId;

                const bool bRuntimeInstanceRoot =
                    std::find(node.Metadata.Tags.begin(), node.Metadata.Tags.end(), "instantiated-runtime") !=
                    node.Metadata.Tags.end();
                const bool bBoundRuntimeNode =
                    !node.SourceInterfaceId.empty() && !node.SourceOperationId.empty();

                if (bRuntimeInstanceRoot) {
                    visualNode.Badges.push_back({WithPolicyPrefix('N', "Runtime"), IM_COL32(124, 114, 84, 220)});
                    visualNode.Badges.push_back({
                        WithPolicyPrefix('M', Slab::ToStr(std::count_if(
                            activeRuntimeDocument.Edges.begin(),
                            activeRuntimeDocument.Edges.end(),
                            [&](const auto &edge) { return edge.FromNodeId == node.NodeId; }))),
                        IM_COL32(98, 138, 188, 220)});
                    visualNode.InputPins = 1;
                    visualNode.OutputPins = std::max(1, static_cast<int>(std::count_if(
                        activeRuntimeDocument.Edges.begin(),
                        activeRuntimeDocument.Edges.end(),
                        [&](const auto &edge) { return edge.FromNodeId == node.NodeId; })));
                } else if (node.Kind == EGraphNodeKindV2::Interface) {
                    visualNode.Badges.push_back({WithPolicyPrefix('N', "Interface"), IM_COL32(118, 98, 166, 220)});
                    visualNode.Badges.push_back({WithPolicyPrefix('M', Slab::ToStr(node.Members.size())), IM_COL32(98, 138, 188, 220)});
                    visualNode.InputPins = 1;
                    visualNode.OutputPins = std::max(1, static_cast<int>(std::count_if(
                        activeRuntimeDocument.Edges.begin(),
                        activeRuntimeDocument.Edges.end(),
                        [&](const auto &edge) { return edge.FromNodeId == node.NodeId; })));
                } else if (node.Kind == EGraphNodeKindV2::Parameter) {
                    visualNode.Badges.push_back({
                        WithPolicyPrefix('M',
                            node.Policies.ParameterMutability.has_value()
                                ? ToCompactMutabilityLabel(*node.Policies.ParameterMutability)
                                : Slab::Str("Unknown")),
                        IM_COL32(88, 132, 192, 220)});
                    visualNode.Badges.push_back({
                        WithPolicyPrefix('E',
                            node.Policies.ParameterExposure.has_value()
                                ? ToCompactExposureLabel(*node.Policies.ParameterExposure)
                                : Slab::Str("Unknown")),
                        IM_COL32(96, 120, 150, 220)});
                    visualNode.InputPins = 1;
                    visualNode.OutputPins = 1;
                } else if (node.Kind == EGraphNodeKindV2::Operation) {
                    if (node.Policies.RunStatePolicy.has_value()) {
                        visualNode.Badges.push_back({
                            WithPolicyPrefix('R', ToCompactRunStateLabel(*node.Policies.RunStatePolicy)),
                            IM_COL32(78, 122, 112, 220)});
                    }
                    if (node.Policies.ThreadAffinity.has_value()) {
                        visualNode.Badges.push_back({
                            WithPolicyPrefix('T', ToCompactThreadLabel(*node.Policies.ThreadAffinity)),
                            IM_COL32(86, 112, 142, 220)});
                    }
                    if (node.Policies.SideEffectClass.has_value()) {
                        visualNode.Badges.push_back({
                            WithPolicyPrefix('X', ToCompactSideEffectLabel(*node.Policies.SideEffectClass)),
                            IM_COL32(122, 104, 88, 220)});
                    }
                    visualNode.InputPins = std::max(
                        1,
                        static_cast<int>(std::count_if(
                            node.Ports.begin(),
                            node.Ports.end(),
                            [](const auto &port) { return port.Direction == EGraphPortDirectionV2::Input; })));
                    visualNode.OutputPins = std::max(
                        1,
                        static_cast<int>(std::count_if(
                            node.Ports.begin(),
                            node.Ports.end(),
                            [](const auto &port) { return port.Direction == EGraphPortDirectionV2::Output; })));
                } else if (node.Kind == EGraphNodeKindV2::SemanticOperator ||
                           node.Kind == EGraphNodeKindV2::Coercion) {
                    visualNode.Badges.push_back({
                        WithPolicyPrefix('N', node.Kind == EGraphNodeKindV2::Coercion ? "Coercion" : "Operator"),
                        node.Kind == EGraphNodeKindV2::Coercion ? IM_COL32(192, 154, 96, 220) : IM_COL32(102, 142, 188, 220)});
                    visualNode.Badges.push_back({
                        WithPolicyPrefix('B', bBoundRuntimeNode ? "Bound" : "Graph"),
                        bBoundRuntimeNode ? IM_COL32(86, 164, 140, 220) : IM_COL32(118, 124, 142, 220)});
                    visualNode.InputPins = std::max(
                        1,
                        static_cast<int>(std::count_if(
                            node.Ports.begin(),
                            node.Ports.end(),
                            [](const auto &port) { return port.Direction == EGraphPortDirectionV2::Input; })));
                    visualNode.OutputPins = std::max(
                        1,
                        static_cast<int>(std::count_if(
                            node.Ports.begin(),
                            node.Ports.end(),
                            [](const auto &port) { return port.Direction == EGraphPortDirectionV2::Output; })));
                } else {
                    visualNode.Badges.push_back({WithPolicyPrefix('N', "Node"), IM_COL32(120, 128, 142, 220)});
                    visualNode.InputPins = 1;
                    visualNode.OutputPins = 1;
                }

                visualNode.Size = computePlaygroundGraphNodeSize(visualNode, 280.0f, 520.0f);
                runtimeVisualNodes.push_back(std::move(visualNode));
            }

            const auto buildRuntimeAddNodeActions = [this](const ImVec2 &) -> Slab::Vector<FPlaygroundGraphCanvasAction> {
                Slab::Vector<FPlaygroundGraphCanvasAction> actions;
                if (bPlaygroundRuntimeUseInstantiatedGraph) {
                    actions.push_back(FPlaygroundGraphCanvasAction{
                        .Label = "Instantiated runtime graph",
                        .Hint = "Instantiate from Template tab; runtime editing is read-mostly in this slice.",
                        .bEnabled = false
                    });
                } else {
                    actions.push_back(FPlaygroundGraphCanvasAction{
                        .Label = "Reflection-derived runtime graph",
                        .Hint = "Runtime nodes are rebuilt from merged reflection catalogs.",
                        .bEnabled = false
                    });
                }
                return actions;
            };
            const auto buildRuntimeConnectNodeActions = [this](const FGraphNodeV2 &,
                                                               FGraphPortV2 *,
                                                               const EGraphPortDirectionV2,
                                                               const ImVec2 &) -> Slab::Vector<FPlaygroundGraphCanvasAction> {
                Slab::Vector<FPlaygroundGraphCanvasAction> actions;
                actions.push_back(FPlaygroundGraphCanvasAction{
                    .Label = "Connections are projection-only",
                    .Hint = bPlaygroundRuntimeUseInstantiatedGraph
                        ? "Use Routing tab for connect/disconnect operations."
                        : "Runtime links come from reflection structure and are not edited here.",
                    .bEnabled = false
                });
                return actions;
            };
            DrawSubstrateGraphCanvasCommon(
                "RuntimePlaygroundCanvas",
                activeRuntimeDocument,
                runtimeVisualNodes,
                PlaygroundRuntimeBottomPanelHeight,
                buildRuntimeAddNodeActions,
                buildRuntimeConnectNodeActions,
                markDirty);

            if (ImGui::BeginChild("RuntimeCatalogPane", ImVec2(0.0f, PlaygroundRuntimeBottomPanelHeight), true)) {
                if (bPlaygroundRuntimeUseInstantiatedGraph) {
                    if (!PlaygroundRuntimeInstanceDiagnostics.empty()) {
                        ImGui::SeparatorText("Instantiation Diagnostics");
                        for (const auto &diagnostic : PlaygroundRuntimeInstanceDiagnostics) {
                            ImGui::TextDisabled("%s", diagnostic.c_str());
                        }
                    }

                    const auto endpointCandidates = buildRuntimeEndpointCandidates();
                    ImGui::SeparatorText("Runtime Endpoints");
                    if (endpointCandidates.empty()) {
                        ImGui::TextDisabled("No runtime endpoints available.");
                    } else if (ImGui::BeginTable(
                                   "RuntimeInstanceEndpointsTable",
                                   4,
                                   ImGuiTableFlags_RowBg |
                                       ImGuiTableFlags_BordersInnerV |
                                       ImGuiTableFlags_ScrollY)) {
                        ImGui::TableSetupColumn("Endpoint");
                        ImGui::TableSetupColumn("Node");
                        ImGui::TableSetupColumn("Direction", ImGuiTableColumnFlags_WidthFixed, 90.0f);
                        ImGui::TableSetupColumn("Space", ImGuiTableColumnFlags_WidthFixed, 160.0f);
                        ImGui::TableHeadersRow();
                        for (const auto &candidate : endpointCandidates) {
                            ImGui::TableNextRow();
                            ImGui::TableSetColumnIndex(0);
                            ImGui::TextUnformatted(candidate.EndpointId.c_str());
                            ImGui::TableSetColumnIndex(1);
                            ImGui::TextUnformatted(candidate.DisplayLabel.c_str());
                            ImGui::TableSetColumnIndex(2);
                            ImGui::TextUnformatted(
                                candidate.Direction == EGraphPortDirectionV2::Output
                                    ? "Output"
                                    : "Input");
                            ImGui::TableSetColumnIndex(3);
                            ImGui::TextUnformatted(
                                candidate.SpaceId.empty()
                                    ? "<unspecified>"
                                    : candidate.SpaceId.c_str());
                        }
                        ImGui::EndTable();
                    }
                } else if (ImGui::BeginTable(
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
            }
            ImGui::EndChild();
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

            struct FRoutingEndpointDescriptor {
                Slab::Str EndpointId;
                Slab::Str DisplayLabel;
                std::optional<EGraphPortDirectionV2> DirectionHint;
                Slab::Str SpaceId;
                Slab::Str TypeId;
                Slab::Str SourceInterfaceId;
                Slab::Str SourceOperationId;
                bool bFromRuntimeInstance = false;
            };

            const auto runtimeEndpointCandidates = buildRuntimeEndpointCandidates();
            std::map<Slab::Str, FRoutingEndpointDescriptor> endpointDescriptors;
            endpointDescriptors.clear();

            for (const auto &candidate : runtimeEndpointCandidates) {
                FRoutingEndpointDescriptor descriptor;
                descriptor.EndpointId = candidate.EndpointId;
                descriptor.DisplayLabel = candidate.DisplayLabel;
                descriptor.DirectionHint = candidate.Direction;
                descriptor.SpaceId = candidate.SpaceId;
                descriptor.TypeId = candidate.TypeId;
                descriptor.SourceInterfaceId = candidate.SourceInterfaceId;
                descriptor.SourceOperationId = candidate.SourceOperationId;
                descriptor.bFromRuntimeInstance = true;
                endpointDescriptors[descriptor.EndpointId] = std::move(descriptor);
            }
            for (const auto &endpointId : PlaygroundRoutingStandaloneEndpoints) {
                if (endpointId.empty()) continue;
                auto &descriptor = endpointDescriptors[endpointId];
                descriptor.EndpointId = endpointId;
                if (descriptor.DisplayLabel.empty()) descriptor.DisplayLabel = endpointId;
            }
            if (!PlaygroundRoutingSourceEndpoint.empty()) {
                auto &descriptor = endpointDescriptors[PlaygroundRoutingSourceEndpoint];
                descriptor.EndpointId = PlaygroundRoutingSourceEndpoint;
                if (descriptor.DisplayLabel.empty()) descriptor.DisplayLabel = PlaygroundRoutingSourceEndpoint;
            }
            if (!PlaygroundRoutingTargetEndpoint.empty()) {
                auto &descriptor = endpointDescriptors[PlaygroundRoutingTargetEndpoint];
                descriptor.EndpointId = PlaygroundRoutingTargetEndpoint;
                if (descriptor.DisplayLabel.empty()) descriptor.DisplayLabel = PlaygroundRoutingTargetEndpoint;
            }
            for (const auto &edge : PlaygroundRoutingDocument.Edges) {
                if (!edge.FromNodeId.empty()) {
                    auto &descriptor = endpointDescriptors[edge.FromNodeId];
                    descriptor.EndpointId = edge.FromNodeId;
                    if (descriptor.DisplayLabel.empty()) descriptor.DisplayLabel = edge.FromNodeId;
                }
                if (!edge.ToNodeId.empty()) {
                    auto &descriptor = endpointDescriptors[edge.ToNodeId];
                    descriptor.EndpointId = edge.ToNodeId;
                    if (descriptor.DisplayLabel.empty()) descriptor.DisplayLabel = edge.ToNodeId;
                }
            }

            const auto resolveEndpointDescriptor =
                [&endpointDescriptors](const Slab::Str &endpointId) -> const FRoutingEndpointDescriptor * {
                const auto it = endpointDescriptors.find(endpointId);
                if (it == endpointDescriptors.end()) return nullptr;
                return &it->second;
            };
            const auto endpointCanSource = [](const FRoutingEndpointDescriptor &descriptor) -> bool {
                return !descriptor.DirectionHint.has_value() || *descriptor.DirectionHint == EGraphPortDirectionV2::Output;
            };
            const auto endpointCanTarget = [](const FRoutingEndpointDescriptor &descriptor) -> bool {
                return !descriptor.DirectionHint.has_value() || *descriptor.DirectionHint == EGraphPortDirectionV2::Input;
            };

            const auto appendStandaloneEndpoint = [this](const Slab::Str &endpointId) -> bool {
                if (endpointId.empty()) return false;
                if (std::find(
                        PlaygroundRoutingStandaloneEndpoints.begin(),
                        PlaygroundRoutingStandaloneEndpoints.end(),
                        endpointId) != PlaygroundRoutingStandaloneEndpoints.end()) {
                    return false;
                }
                PlaygroundRoutingStandaloneEndpoints.push_back(endpointId);
                return true;
            };

            const auto rebuildRoutingNodes = [this, &endpointDescriptors]() {
                std::map<Slab::Str, FGraphPointV2> previousPositions;
                for (const auto &node : PlaygroundRoutingDocument.Nodes) {
                    previousPositions[node.NodeId] = node.Position;
                }

                std::map<Slab::Str, int> endpointRoles;
                std::map<Slab::Str, int> endpointOutDegree;
                std::map<Slab::Str, int> endpointInDegree;
                for (const auto &edge : PlaygroundRoutingDocument.Edges) {
                    if (!edge.FromNodeId.empty()) {
                        endpointRoles[edge.FromNodeId] |= 0x1;
                        endpointOutDegree[edge.FromNodeId] += 1;
                    }
                    if (!edge.ToNodeId.empty()) {
                        endpointRoles[edge.ToNodeId] |= 0x2;
                        endpointInDegree[edge.ToNodeId] += 1;
                    }
                }
                for (const auto &[endpointId, descriptor] : endpointDescriptors) {
                    if (endpointId.empty()) continue;
                    if (descriptor.DirectionHint.has_value()) {
                        endpointRoles[endpointId] |=
                            *descriptor.DirectionHint == EGraphPortDirectionV2::Output ? 0x1 : 0x2;
                    } else {
                        endpointRoles[endpointId] |= 0x3;
                    }
                }
                if (!PlaygroundRoutingSourceEndpoint.empty()) endpointRoles[PlaygroundRoutingSourceEndpoint] |= 0x1;
                if (!PlaygroundRoutingTargetEndpoint.empty()) endpointRoles[PlaygroundRoutingTargetEndpoint] |= 0x2;
                for (const auto &standaloneEndpoint : PlaygroundRoutingStandaloneEndpoints) {
                    if (standaloneEndpoint.empty()) continue;
                    endpointRoles.try_emplace(standaloneEndpoint, 0x3);
                }

                PlaygroundRoutingDocument.Nodes.clear();
                PlaygroundRoutingDocument.Nodes.reserve(endpointRoles.size());

                int sourceLane = 0;
                int bothLane = 0;
                int targetLane = 0;
                for (const auto &[endpointId, roleBits] : endpointRoles) {
                    const auto descriptorIt = endpointDescriptors.find(endpointId);
                    const auto *descriptor = descriptorIt != endpointDescriptors.end() ? &descriptorIt->second : nullptr;

                    FGraphNodeV2 node;
                    node.NodeId = endpointId;
                    node.DisplayName =
                        descriptor != nullptr && !descriptor->DisplayLabel.empty()
                            ? descriptor->DisplayLabel
                            : endpointId;
                    node.Kind = EGraphNodeKindV2::RoutingEndpoint;
                    if (descriptor != nullptr && descriptor->bFromRuntimeInstance) {
                        node.Metadata.Tags.push_back("runtime-endpoint");
                    }
                    node.Metadata.Attrs["routing.in_degree"] = MakeIntValue(endpointInDegree[endpointId]);
                    node.Metadata.Attrs["routing.out_degree"] = MakeIntValue(endpointOutDegree[endpointId]);
                    if (descriptor != nullptr) {
                        if (descriptor->DirectionHint.has_value()) {
                            node.Metadata.Attrs["routing.direction"] = MakeStringValue(
                                *descriptor->DirectionHint == EGraphPortDirectionV2::Output ? "Output" : "Input");
                        }
                        if (!descriptor->SpaceId.empty()) {
                            node.Metadata.Attrs["routing.space_id"] = MakeStringValue(descriptor->SpaceId);
                        }
                        if (!descriptor->TypeId.empty()) {
                            node.Metadata.Attrs["routing.type_id"] = MakeStringValue(descriptor->TypeId);
                        }
                        if (!descriptor->SourceInterfaceId.empty()) {
                            node.Metadata.Attrs["routing.source_interface_id"] = MakeStringValue(descriptor->SourceInterfaceId);
                        }
                        if (!descriptor->SourceOperationId.empty()) {
                            node.Metadata.Attrs["routing.source_operation_id"] = MakeStringValue(descriptor->SourceOperationId);
                        }
                    }

                    bool bAllowInput = (roleBits & 0x2) != 0;
                    bool bAllowOutput = (roleBits & 0x1) != 0;
                    if (descriptor != nullptr && descriptor->DirectionHint.has_value()) {
                        bAllowInput = *descriptor->DirectionHint == EGraphPortDirectionV2::Input;
                        bAllowOutput = *descriptor->DirectionHint == EGraphPortDirectionV2::Output;
                    }
                    if (!bAllowInput && !bAllowOutput) {
                        bAllowInput = true;
                        bAllowOutput = true;
                    }

                    if (bAllowInput) {
                        FGraphPortV2 inputPort;
                        inputPort.PortId = "in";
                        inputPort.DisplayName = "in";
                        inputPort.Direction = EGraphPortDirectionV2::Input;
                        inputPort.bRequired = true;
                        if (descriptor != nullptr) {
                            inputPort.Handle.SpaceId = descriptor->SpaceId;
                            inputPort.Handle.TypeId = descriptor->TypeId;
                        }
                        node.Ports.push_back(std::move(inputPort));
                    }

                    if (bAllowOutput) {
                        FGraphPortV2 outputPort;
                        outputPort.PortId = "out";
                        outputPort.DisplayName = "out";
                        outputPort.Direction = EGraphPortDirectionV2::Output;
                        outputPort.bRequired = true;
                        if (descriptor != nullptr) {
                            outputPort.Handle.SpaceId = descriptor->SpaceId;
                            outputPort.Handle.TypeId = descriptor->TypeId;
                        }
                        node.Ports.push_back(std::move(outputPort));
                    }

                    if (const auto previousIt = previousPositions.find(endpointId); previousIt != previousPositions.end()) {
                        node.Position = previousIt->second;
                    } else if (roleBits == 0x1) {
                        node.Position.x = 120.0f;
                        node.Position.y = 80.0f + static_cast<float>(sourceLane++) * 126.0f;
                    } else if (roleBits == 0x2) {
                        node.Position.x = 940.0f;
                        node.Position.y = 80.0f + static_cast<float>(targetLane++) * 126.0f;
                    } else {
                        node.Position.x = 540.0f;
                        node.Position.y = 80.0f + static_cast<float>(bothLane++) * 126.0f;
                    }

                    PlaygroundRoutingDocument.Nodes.push_back(std::move(node));
                }

                PlaygroundRoutingEndpointCounter = std::max(
                    PlaygroundRoutingEndpointCounter,
                    PlaygroundRoutingStandaloneEndpoints.size());
            };

            const auto validateRoutingConnectRequest =
                [this,
                 &resolveEndpointDescriptor,
                 &routeKindLabel](const Slab::Str &sourceEndpoint,
                                  const Slab::Str &targetEndpoint) -> FOperationResultV2 {
                const auto *sourceDescriptor = resolveEndpointDescriptor(sourceEndpoint);
                const auto *targetDescriptor = resolveEndpointDescriptor(targetEndpoint);
                const auto edgeKind = PlaygroundRoutingEdgeKind;
                const auto edgeKindLabel = Slab::Str(routeKindLabel(edgeKind));

                if (sourceEndpoint.empty() || targetEndpoint.empty()) {
                    return FOperationResultV2::Error(
                        "routing.connect.missing_endpoint",
                        "Source and target endpoints are required.");
                }
                if (sourceEndpoint == targetEndpoint) {
                    return FOperationResultV2::Error(
                        "routing.connect.same_endpoint",
                        "Cannot connect an endpoint to itself.");
                }
                if (sourceDescriptor != nullptr &&
                    sourceDescriptor->DirectionHint.has_value() &&
                    *sourceDescriptor->DirectionHint != EGraphPortDirectionV2::Output) {
                    return FOperationResultV2::Error(
                        "routing.connect.invalid_source_direction",
                        "Source endpoint is not an output endpoint.");
                }
                if (targetDescriptor != nullptr &&
                    targetDescriptor->DirectionHint.has_value() &&
                    *targetDescriptor->DirectionHint != EGraphPortDirectionV2::Input) {
                    return FOperationResultV2::Error(
                        "routing.connect.invalid_target_direction",
                        "Target endpoint is not an input endpoint.");
                }

                if ((edgeKind == EGraphEdgeKindV2::ValueFlow || edgeKind == EGraphEdgeKindV2::StreamSubscription) &&
                    sourceDescriptor != nullptr &&
                    targetDescriptor != nullptr &&
                    !sourceDescriptor->SpaceId.empty() &&
                    !targetDescriptor->SpaceId.empty() &&
                    sourceDescriptor->SpaceId != targetDescriptor->SpaceId) {
                    return FOperationResultV2::Error(
                        "routing.connect.space_mismatch",
                        "Semantic space mismatch: '" + sourceDescriptor->SpaceId +
                            "' -> '" + targetDescriptor->SpaceId + "'.");
                }

                FValueMapV2 outputs;
                outputs["source_endpoint"] = MakeStringValue(sourceEndpoint);
                outputs["target_endpoint"] = MakeStringValue(targetEndpoint);
                outputs["edge_kind"] = MakeStringValue(edgeKindLabel);

                auto result = FOperationResultV2::Ok(std::move(outputs));
                if (sourceDescriptor == nullptr || targetDescriptor == nullptr) {
                    result.Warnings.push_back(
                        "One or both endpoints have no descriptor metadata; validation was partial.");
                }
                if (sourceDescriptor != nullptr &&
                    targetDescriptor != nullptr &&
                    !sourceDescriptor->TypeId.empty() &&
                    !targetDescriptor->TypeId.empty() &&
                    sourceDescriptor->TypeId != targetDescriptor->TypeId &&
                    edgeKind == EGraphEdgeKindV2::HandleBinding) {
                    result.Warnings.push_back(
                        "HandleBinding type mismatch tolerated: '" + sourceDescriptor->TypeId +
                            "' -> '" + targetDescriptor->TypeId + "'.");
                }
                return result;
            };

            const auto addRoutingEdge = [this,
                                         &rebuildRoutingNodes,
                                         &validateRoutingConnectRequest,
                                         &routeKindLabel,
                                         &markDirty](const Slab::Str &sourceEndpoint,
                                                     const Slab::Str &targetEndpoint) -> bool {
                ++PlaygroundRoutingConnectAttemptCount;
                PlaygroundRoutingLastOperationDiagnostics.clear();

                const auto operationResult = validateRoutingConnectRequest(sourceEndpoint, targetEndpoint);
                PlaygroundRoutingLastOperationSummary = BuildOperationSummary(operationResult);
                for (const auto &warning : operationResult.Warnings) {
                    PlaygroundRoutingLastOperationDiagnostics.push_back(warning);
                }

                if (!operationResult.IsOk()) {
                    ++PlaygroundRoutingConnectFailureCount;
                    PlaygroundRoutingStatus = "[Error] Connect failed: " +
                        (operationResult.ErrorCode.empty() ? Slab::Str("routing.connect.failed") : operationResult.ErrorCode) +
                        ". " + operationResult.ErrorMessage;
                    markDirty();
                    return false;
                }

                const auto duplicateIt = std::find_if(
                    PlaygroundRoutingDocument.Edges.begin(),
                    PlaygroundRoutingDocument.Edges.end(),
                    [&](const auto &edge) {
                        return edge.FromNodeId == sourceEndpoint &&
                               edge.ToNodeId == targetEndpoint &&
                               edge.Kind == PlaygroundRoutingEdgeKind;
                    });
                if (duplicateIt != PlaygroundRoutingDocument.Edges.end()) {
                    ++PlaygroundRoutingConnectSuccessCount;
                    PlaygroundRoutingStatus = "[Warn] Route already exists (" + Slab::Str(routeKindLabel(PlaygroundRoutingEdgeKind)) + ").";
                    markDirty();
                    return false;
                }

                ++PlaygroundRoutingConnectSuccessCount;
                ++PlaygroundRoutingEdgeCounter;
                FGraphEdgeV2 edge;
                edge.EdgeId = "route.edge." + Slab::ToStr(PlaygroundRoutingEdgeCounter);
                edge.FromNodeId = sourceEndpoint;
                edge.FromPortId = "out";
                edge.ToNodeId = targetEndpoint;
                edge.ToPortId = "in";
                edge.Kind = PlaygroundRoutingEdgeKind;
                edge.Diagnostics = PlaygroundRoutingLastOperationDiagnostics;
                PlaygroundRoutingDocument.Edges.push_back(std::move(edge));
                rebuildRoutingNodes();

                PlaygroundRoutingStatus = "[Ok] Connected " + sourceEndpoint + " -> " + targetEndpoint +
                    " (" + Slab::Str(routeKindLabel(PlaygroundRoutingEdgeKind)) + ").";
                markDirty();
                return true;
            };

            rebuildRoutingNodes();

            ImGui::TextDisabled("Patchbay playground (connect via operation path).");
            ImGui::TextDisabled(
                "Runtime endpoint candidates: %zu",
                runtimeEndpointCandidates.size());
            if (runtimeEndpointCandidates.empty()) {
                ImGui::BeginDisabled();
            }
            if (ImGui::Button("Import Runtime Endpoints")) {
                std::size_t importedCount = 0;
                for (const auto &candidate : runtimeEndpointCandidates) {
                    if (appendStandaloneEndpoint(candidate.EndpointId)) ++importedCount;
                }
                rebuildRoutingNodes();
                if (importedCount == 0) {
                    PlaygroundRoutingStatus = "[Warn] Runtime endpoints already imported.";
                } else {
                    PlaygroundRoutingStatus =
                        "[Ok] Imported " + Slab::ToStr(importedCount) + " runtime endpoint(s).";
                }
                markDirty();
            }
            if (runtimeEndpointCandidates.empty()) {
                ImGui::EndDisabled();
            }

            const auto drawEndpointSelector =
                [&](const char *label, Slab::Str &selectedEndpoint, const bool bSelectSource) -> bool {
                const auto previewValue = selectedEndpoint.empty() ? "<none>" : selectedEndpoint.c_str();
                bool bChanged = false;
                if (ImGui::BeginCombo(label, previewValue)) {
                    const bool bEmptySelected = selectedEndpoint.empty();
                    if (ImGui::Selectable("<none>", bEmptySelected)) {
                        selectedEndpoint.clear();
                        bChanged = true;
                    }
                    if (bEmptySelected) ImGui::SetItemDefaultFocus();

                    for (const auto &[endpointId, descriptor] : endpointDescriptors) {
                        if (endpointId.empty()) continue;
                        if (bSelectSource && !endpointCanSource(descriptor)) continue;
                        if (!bSelectSource && !endpointCanTarget(descriptor)) continue;

                        const bool bSelected = selectedEndpoint == endpointId;
                        auto itemLabel = descriptor.DisplayLabel.empty() ? endpointId : descriptor.DisplayLabel;
                        if (itemLabel != endpointId) itemLabel += "##" + endpointId;
                        if (ImGui::Selectable(itemLabel.c_str(), bSelected)) {
                            selectedEndpoint = endpointId;
                            bChanged = true;
                        }
                        if (bSelected) ImGui::SetItemDefaultFocus();
                        if (ImGui::IsItemHovered()) {
                            ImGui::BeginTooltip();
                            ImGui::TextUnformatted(endpointId.c_str());
                            ImGui::TextDisabled(
                                "Direction: %s",
                                !descriptor.DirectionHint.has_value()
                                    ? "Unknown"
                                    : (*descriptor.DirectionHint == EGraphPortDirectionV2::Output ? "Output" : "Input"));
                            if (!descriptor.SpaceId.empty()) {
                                ImGui::TextDisabled("Space: %s", descriptor.SpaceId.c_str());
                            }
                            if (!descriptor.TypeId.empty()) {
                                ImGui::TextDisabled("Type: %s", descriptor.TypeId.c_str());
                            }
                            if (!descriptor.SourceInterfaceId.empty() || !descriptor.SourceOperationId.empty()) {
                                ImGui::TextDisabled(
                                    "Path: %s::%s",
                                    descriptor.SourceInterfaceId.empty() ? "<unknown>" : descriptor.SourceInterfaceId.c_str(),
                                    descriptor.SourceOperationId.empty() ? "<unknown>" : descriptor.SourceOperationId.c_str());
                            }
                            ImGui::EndTooltip();
                        }
                    }
                    ImGui::EndCombo();
                }
                return bChanged;
            };

            bool bEndpointSelectionChanged = false;
            bEndpointSelectionChanged |= drawEndpointSelector("Source Endpoint", PlaygroundRoutingSourceEndpoint, true);
            bEndpointSelectionChanged |= drawEndpointSelector("Target Endpoint", PlaygroundRoutingTargetEndpoint, false);
            if (bEndpointSelectionChanged) {
                rebuildRoutingNodes();
                markDirty();
            }

            const auto previousRouteKind = PlaygroundRoutingEdgeKind;
            int routeKindIndex = RoutingEdgeKindToSelectionIndex(PlaygroundRoutingEdgeKind);
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
            routeKindIndex = std::clamp(routeKindIndex, 0, static_cast<int>(std::size(routeKindLabels)) - 1);
            PlaygroundRoutingEdgeKind = RoutingEdgeKindFromSelectionIndex(routeKindIndex);
            if (PlaygroundRoutingEdgeKind != previousRouteKind) markDirty();

            if (ImGui::Button("Add Route")) {
                (void) addRoutingEdge(PlaygroundRoutingSourceEndpoint, PlaygroundRoutingTargetEndpoint);
            }
            ImGui::SameLine();
            if (ImGui::Button("Clear Routes")) {
                PlaygroundRoutingDocument.Edges.clear();
                rebuildRoutingNodes();
                PlaygroundRoutingStatus = "[Ok] Cleared routing playground edges.";
                markDirty();
            }
            ImGui::SameLine();
            if (ImGui::Checkbox("Grid##RoutingGraph", &PlaygroundRoutingDocument.Canvas.bShowGrid)) {
                markDirty();
            }
            ImGui::SameLine();
            if (ImGui::Button("Auto Layout##RoutingGraph")) {
                PlaygroundRoutingDocument.Nodes.clear();
                rebuildRoutingNodes();
                markDirty();
            }

            if (!PlaygroundRoutingStatus.empty()) {
                const bool bError = PlaygroundRoutingStatus.starts_with("[Error]");
                ImGui::TextColored(
                    bError ? ImVec4(0.92f, 0.36f, 0.36f, 1.0f) : ImVec4(0.42f, 0.83f, 0.52f, 1.0f),
                    "%s",
                    PlaygroundRoutingStatus.c_str());
            }
            ImGui::TextDisabled(
                "Connect ops: %zu attempts | %zu ok | %zu failed",
                PlaygroundRoutingConnectAttemptCount,
                PlaygroundRoutingConnectSuccessCount,
                PlaygroundRoutingConnectFailureCount);
            if (!PlaygroundRoutingLastOperationSummary.empty()) {
                ImGui::TextDisabled("Last op: %s", PlaygroundRoutingLastOperationSummary.c_str());
            }
            for (const auto &diagnostic : PlaygroundRoutingLastOperationDiagnostics) {
                ImGui::TextDisabled(" - %s", diagnostic.c_str());
            }

            Slab::Vector<FPlaygroundGraphVisualNode> routingVisualNodes;
            routingVisualNodes.reserve(PlaygroundRoutingDocument.Nodes.size());
            for (auto &node : PlaygroundRoutingDocument.Nodes) {
                FPlaygroundGraphVisualNode visualNode;
                visualNode.Node = &node;
                visualNode.Title = node.DisplayName.empty() ? node.NodeId : node.DisplayName;
                visualNode.Subtitle = node.NodeId;
                const auto inDegreeIt = node.Metadata.Attrs.find("routing.in_degree");
                const auto outDegreeIt = node.Metadata.Attrs.find("routing.out_degree");
                const auto inDegree = inDegreeIt != node.Metadata.Attrs.end() ? inDegreeIt->second.Encoded : "0";
                const auto outDegree = outDegreeIt != node.Metadata.Attrs.end() ? outDegreeIt->second.Encoded : "0";
                visualNode.Badges.push_back({WithPolicyPrefix('N', "Endpoint"), IM_COL32(86, 146, 154, 220)});
                visualNode.Badges.push_back({WithPolicyPrefix('I', inDegree), IM_COL32(92, 132, 176, 220)});
                visualNode.Badges.push_back({WithPolicyPrefix('O', outDegree), IM_COL32(92, 166, 132, 220)});
                if (const auto directionIt = node.Metadata.Attrs.find("routing.direction");
                    directionIt != node.Metadata.Attrs.end() && !directionIt->second.Encoded.empty()) {
                    const auto direction = directionIt->second.Encoded;
                    visualNode.Badges.push_back({
                        WithPolicyPrefix('D', direction == "Output" ? "Out" : "In"),
                        IM_COL32(134, 148, 176, 220)});
                }
                if (const auto spaceIt = node.Metadata.Attrs.find("routing.space_id");
                    spaceIt != node.Metadata.Attrs.end() && !spaceIt->second.Encoded.empty()) {
                    visualNode.Badges.push_back({
                        WithPolicyPrefix('S', TruncateLabel(spaceIt->second.Encoded, 12)),
                        IM_COL32(112, 152, 136, 220)});
                }
                visualNode.InputPins = static_cast<int>(std::count_if(
                    node.Ports.begin(),
                    node.Ports.end(),
                    [](const auto &port) { return port.Direction == EGraphPortDirectionV2::Input; }));
                visualNode.OutputPins = static_cast<int>(std::count_if(
                    node.Ports.begin(),
                    node.Ports.end(),
                    [](const auto &port) { return port.Direction == EGraphPortDirectionV2::Output; }));
                visualNode.Size = computePlaygroundGraphNodeSize(visualNode, 280.0f, 520.0f);
                routingVisualNodes.push_back(std::move(visualNode));
            }

            const auto buildRoutingAddNodeActions = [this,
                                                     &appendStandaloneEndpoint,
                                                     &rebuildRoutingNodes](const ImVec2 &) -> Slab::Vector<FPlaygroundGraphCanvasAction> {
                Slab::Vector<FPlaygroundGraphCanvasAction> actions;
                actions.push_back(FPlaygroundGraphCanvasAction{
                    .Label = "Add Endpoint Node",
                    .Hint = "Creates a standalone routing endpoint.",
                    .bEnabled = true,
                    .Execute = [this, &appendStandaloneEndpoint, &rebuildRoutingNodes]() {
                        ++PlaygroundRoutingEndpointCounter;
                        const auto endpointId = "route.endpoint." + Slab::ToStr(PlaygroundRoutingEndpointCounter);
                        if (!appendStandaloneEndpoint(endpointId)) {
                            PlaygroundRoutingStatus = "[Warn] Endpoint already exists.";
                            return;
                        }
                        rebuildRoutingNodes();
                        PlaygroundRoutingStatus = "[Ok] Added endpoint node '" + endpointId + "'.";
                    }
                });
                return actions;
            };

            const auto buildRoutingConnectNodeActions = [this,
                                                         &appendStandaloneEndpoint,
                                                         &addRoutingEdge,
                                                         &resolveEndpointDescriptor](const FGraphNodeV2 &sourceNode,
                                                                               FGraphPortV2 *,
                                                                               const EGraphPortDirectionV2 sourceDirection,
                                                                               const ImVec2 &) -> Slab::Vector<FPlaygroundGraphCanvasAction> {
                Slab::Vector<FPlaygroundGraphCanvasAction> actions;
                const bool bFromOutput = sourceDirection == EGraphPortDirectionV2::Output;

                for (const auto &candidateNode : PlaygroundRoutingDocument.Nodes) {
                    if (candidateNode.NodeId == sourceNode.NodeId) continue;
                    const auto sourceEndpoint = bFromOutput ? sourceNode.NodeId : candidateNode.NodeId;
                    const auto targetEndpoint = bFromOutput ? candidateNode.NodeId : sourceNode.NodeId;
                    const auto *sourceDescriptor = resolveEndpointDescriptor(sourceEndpoint);
                    const auto *targetDescriptor = resolveEndpointDescriptor(targetEndpoint);
                    const bool bDirectionalCompatible =
                        (sourceDescriptor == nullptr || !sourceDescriptor->DirectionHint.has_value() ||
                         *sourceDescriptor->DirectionHint == EGraphPortDirectionV2::Output) &&
                        (targetDescriptor == nullptr || !targetDescriptor->DirectionHint.has_value() ||
                         *targetDescriptor->DirectionHint == EGraphPortDirectionV2::Input);
                    actions.push_back(FPlaygroundGraphCanvasAction{
                        .Label = Slab::Str("Connect ") + sourceEndpoint + " -> " + targetEndpoint,
                        .Hint = bDirectionalCompatible
                            ? "Executes routing connect operation and adds edge on success."
                            : "Directional mismatch (source must be output and target must be input).",
                        .bEnabled = bDirectionalCompatible,
                        .Execute = [this, &addRoutingEdge, sourceEndpoint, targetEndpoint]() {
                            (void) addRoutingEdge(sourceEndpoint, targetEndpoint);
                        }
                    });
                }

                actions.push_back(FPlaygroundGraphCanvasAction{
                    .Label = "Connect via New Endpoint",
                    .Hint = "Creates a new endpoint and wires to it.",
                    .bEnabled = true,
                    .Execute = [this, &appendStandaloneEndpoint, &addRoutingEdge, sourceNode, bFromOutput]() {
                        ++PlaygroundRoutingEndpointCounter;
                        const auto newEndpointId = "route.endpoint." + Slab::ToStr(PlaygroundRoutingEndpointCounter);
                        if (!appendStandaloneEndpoint(newEndpointId)) {
                            PlaygroundRoutingStatus = "[Warn] Endpoint already exists.";
                            return;
                        }
                        const auto sourceEndpoint = bFromOutput ? sourceNode.NodeId : newEndpointId;
                        const auto targetEndpoint = bFromOutput ? newEndpointId : sourceNode.NodeId;
                        if (!addRoutingEdge(sourceEndpoint, targetEndpoint)) {
                            PlaygroundRoutingStatus =
                                "[Warn] Added endpoint '" + newEndpointId + "' but connect operation failed.";
                            return;
                        }
                        PlaygroundRoutingStatus =
                            "[Ok] Added endpoint '" + newEndpointId + "' and connected " +
                            sourceEndpoint + " -> " + targetEndpoint + ".";
                    }
                });

                return actions;
            };

            DrawSubstrateGraphCanvasCommon(
                "RoutingPlaygroundCanvas",
                PlaygroundRoutingDocument,
                routingVisualNodes,
                PlaygroundRoutingBottomPanelHeight,
                buildRoutingAddNodeActions,
                buildRoutingConnectNodeActions,
                markDirty);

            if (ImGui::BeginChild("RoutingCatalogPane", ImVec2(0.0f, PlaygroundRoutingBottomPanelHeight), true)) {
                ImGui::SeparatorText("Endpoints");
                if (ImGui::BeginTable(
                        "RoutingEndpointsTable",
                        5,
                        ImGuiTableFlags_RowBg |
                            ImGuiTableFlags_BordersInnerV |
                            ImGuiTableFlags_ScrollY)) {
                    ImGui::TableSetupColumn("Endpoint");
                    ImGui::TableSetupColumn("Direction", ImGuiTableColumnFlags_WidthFixed, 90.0f);
                    ImGui::TableSetupColumn("Space", ImGuiTableColumnFlags_WidthFixed, 160.0f);
                    ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed, 150.0f);
                    ImGui::TableSetupColumn("Origin", ImGuiTableColumnFlags_WidthFixed, 120.0f);
                    ImGui::TableHeadersRow();
                    for (const auto &[endpointId, descriptor] : endpointDescriptors) {
                        ImGui::TableNextRow();
                        ImGui::TableSetColumnIndex(0);
                        ImGui::TextUnformatted(endpointId.c_str());
                        ImGui::TableSetColumnIndex(1);
                        ImGui::TextUnformatted(
                            !descriptor.DirectionHint.has_value()
                                ? "Unknown"
                                : (*descriptor.DirectionHint == EGraphPortDirectionV2::Output ? "Output" : "Input"));
                        ImGui::TableSetColumnIndex(2);
                        ImGui::TextUnformatted(
                            descriptor.SpaceId.empty()
                                ? "<unspecified>"
                                : descriptor.SpaceId.c_str());
                        ImGui::TableSetColumnIndex(3);
                        ImGui::TextUnformatted(
                            descriptor.TypeId.empty()
                                ? "<unspecified>"
                                : descriptor.TypeId.c_str());
                        ImGui::TableSetColumnIndex(4);
                        ImGui::TextUnformatted(descriptor.bFromRuntimeInstance ? "Runtime" : "Standalone");
                    }
                    ImGui::EndTable();
                }

                ImGui::SeparatorText("Routes");
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
            }
            ImGui::EndChild();
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }
}
