#include "LabV2WindowManager.h"

#include "LabV2Panels.h"
#include "SimulationManagerV2.h"
#include "StudioConfigV2.h"

#include "Core/Backend/Modules/TaskManager/TaskManager.h"
#include "imgui.h"
#ifdef IMGUI_HAS_DOCK
#include "imgui_internal.h"
#endif

#include "Core/SlabCore.h"
#include "Core/Reflection/V2/ReflectionCodecsV2.h"
#include "Graphics/Modules/ImGui/ImGuiModule.h"
#include "Graphics/SlabGraphics.h"
#include "Graphics/Plot2D/Plot2DWindow.h"
#include "Graphics/Plot2D/Plotter.h"
#include "Graphics/Plot2D/PlotThemeManager.h"
#include "Graphics/Plot2D/V2/PlotReflectionSchemaV2.h"
#include "Graphics/Window/WindowStyles.h"
#include "Math/Data/V2/SessionLiveViewV2.h"
#include "Math/Function/RtoR/Model/FunctionsCollection/Trigonometric.h"

#include <algorithm>
#include <cctype>
#include <cfloat>
#include <cstring>
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
    constexpr auto WindowTitlePlotInspector = "Plot Inspector";
    constexpr auto DockspaceHostName = "##LabDockspaceHost";
    constexpr auto DockspaceNameSimulations = "##LabDockspace-Simulations";
    constexpr auto DockspaceNameSchemes = "##LabDockspace-Schemes";
    constexpr auto DockspaceNamePlots = "##LabDockspace-Plots";
    constexpr auto WorkspaceTabSimulations = "Simulations";
    constexpr auto WorkspaceTabSchemes = "Schemes";
    constexpr auto WorkspaceTabPlots = "Plots";

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

} // namespace

FLabV2WindowManager::FLabV2WindowManager()
: SidePaneWidth(FStudioConfigV2::SidePaneWidth) {
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

    AddResponder(ImGuiContext);
    LoadWorkspacePanelVisibility(ActiveWorkspace);

    // No default monitor window; monitors are attached per simulation on demand.
}

void FLabV2WindowManager::AddSlabWindow(const Slab::TPointer<Slab::Graphics::FSlabWindow> &window) {
    AddSlabWindow(window, false);
}

void FLabV2WindowManager::AddSlabWindow(const Slab::TPointer<Slab::Graphics::FSlabWindow> &window, bool hidden) {
    (void) hidden;
    if (window == nullptr) return;
    AddResponder(window);
    SlabWindows.emplace_back(window);
    if (SelectedViewUniqueName.empty()) {
        SelectedViewUniqueName = window->GetUniqueName();
    }
    RequestViewRetile();
    NotifySystemWindowReshape(WidthSysWin, HeightSysWin);
}

auto FLabV2WindowManager::QueueSlabWindow(const Slab::TPointer<Slab::Graphics::FSlabWindow> &window) -> void {
    if (window == nullptr) return;
    PendingSlabWindows.emplace_back(window);
}

auto FLabV2WindowManager::FlushPendingSlabWindows() -> void {
    if (PendingSlabWindows.empty()) return;

    auto pending = std::move(PendingSlabWindows);
    PendingSlabWindows.clear();
    for (const auto &window : pending) AddSlabWindow(window, false);
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
        if (window->WantsClose()) continue;
        if (window->GetWidth() <= 0 || window->GetHeight() <= 0) continue;
        if (window->IsPointWithin(point)) return window;
    }

    return nullptr;
}

auto FLabV2WindowManager::FindKeyboardTargetWindow() const -> Slab::TPointer<Slab::Graphics::FSlabWindow> {
    if (!ShouldRenderSlabWindowsInWorkspace()) return nullptr;

    if (const auto selected = FindWindowByUniqueName(SelectedViewUniqueName); selected != nullptr) {
        return selected;
    }

    for (auto it = SlabWindows.rbegin(); it != SlabWindows.rend(); ++it) {
        if (*it != nullptr && !(*it)->WantsClose()) return *it;
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
    ImGui::TextUnformatted("Views");
    if (SlabWindows.empty()) {
        ImGui::TextDisabled("No active view windows.");
        return;
    }

    const auto selected = FindWindowByUniqueName(SelectedViewUniqueName);
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

        for (const auto &window : SlabWindows) {
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

auto FLabV2WindowManager::EnsureSchemeSelectionIsValid() -> void {
    using namespace Slab::Core::Reflection::V2;

    const auto &catalog = ReflectionAdapter.GetCatalog();
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
        if (const auto current = ReflectionAdapter.EncodeCurrentParameterValue(
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
    const bool bNeedsParameterSelection =
        operationId == COperationIdQueryGetParameter ||
        operationId == COperationIdCommandSetParameter;

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

    if (operationId == COperationIdCommandSetParameter) {
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

    const auto result = ReflectionAdapter.Invoke(interfaceSchema.InterfaceId, operationId, inputs, context);
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
    using namespace Slab::Graphics::Plot2D::V2;

    FValueMapV2 inputs;
    const bool bNeedsParameterSelection =
        operationId == CPlotOperationIdQueryGetParameterV2 ||
        operationId == CPlotOperationIdCommandSetParameterV2;

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

    if (operationId == CPlotOperationIdCommandSetParameterV2) {
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

    ReflectionAdapter.RefreshFromLegacyInterfaces();
    EnsureSchemeSelectionIsValid();

    const auto &catalog = ReflectionAdapter.GetCatalog();
    const auto context = BuildReflectionInvocationContext();

    ImGui::Text("Catalog version: %s", catalog.CatalogVersion.c_str());
    ImGui::SameLine();
    ImGui::TextDisabled("| interfaces: %zu", catalog.Interfaces.size());
    ImGui::SameLine();
    ImGui::TextDisabled("| runtime: %s", context.bRuntimeRunning ? "running" : "stopped");

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
            for (const auto &interfaceSchema : catalog.Interfaces) {
                const bool bSelected = interfaceSchema.InterfaceId == SelectedSchemeInterfaceId;
                ImGui::PushID(interfaceSchema.InterfaceId.c_str());
                if (ImGui::Selectable(interfaceSchema.DisplayName.c_str(), bSelected)) {
                    SelectedSchemeInterfaceId = interfaceSchema.InterfaceId;
                    SelectedSchemeParameterId.clear();
                    EnsureSchemeSelectionIsValid();
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
        if (ImGui::BeginChild("SchemesInterfaceInspector", ImVec2(0.0f, 0.0f), false)) {
            const auto *interfaceSchema = FindInterfaceById(catalog, SelectedSchemeInterfaceId);
            if (interfaceSchema == nullptr) {
                ImGui::TextDisabled("Select an interface.");
            } else {
                ImGui::TextUnformatted(interfaceSchema->DisplayName.c_str());
                ImGui::TextDisabled("%s", interfaceSchema->InterfaceId.c_str());
                if (!interfaceSchema->Description.empty()) {
                    ImGui::TextWrapped("%s", interfaceSchema->Description.c_str());
                }

                ImGui::SeparatorText("Operations");
                for (const auto &operation : interfaceSchema->Operations) {
                    const bool bNeedsParameterSelection =
                        operation.OperationId == COperationIdQueryGetParameter ||
                        operation.OperationId == COperationIdCommandSetParameter;
                    const bool bCanInvoke = !bNeedsParameterSelection || !SelectedSchemeParameterId.empty();

                    ImGui::PushID(operation.OperationId.c_str());
                    ImGui::BeginDisabled(!bCanInvoke);
                    if (ImGui::Button(operation.DisplayName.c_str())) {
                        InvokeSelectedSchemeOperation(*interfaceSchema, operation.OperationId, context);
                    }
                    ImGui::EndDisabled();
                    ImGui::SameLine();
                    ImGui::TextDisabled(
                        "%s | %s | %s",
                        ToString(operation.Kind),
                        ToString(operation.RunStatePolicy),
                        ToString(operation.ThreadAffinity));
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
                                    if (const auto liveValue = ReflectionAdapter.EncodeCurrentParameterValue(
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
                            if (const auto liveValue = ReflectionAdapter.EncodeCurrentParameterValue(
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
                            if (const auto liveValue = ReflectionAdapter.EncodeCurrentParameterValue(
                                    interfaceSchema->InterfaceId,
                                    selectedParameterSchema->ParameterId); liveValue.has_value()) {
                                draft = liveValue->Encoded;
                            }
                        }
                        ImGui::SameLine();
                        if (ImGui::Button("Set Parameter")) {
                            SelectedSchemeParameterId = selectedParameterSchema->ParameterId;
                            InvokeSelectedSchemeOperation(*interfaceSchema, COperationIdCommandSetParameter, context);
                        }

                        if (selectedParameterSchema->Mutability == EParameterMutability::RestartRequired) {
                            ImGui::SameLine();
                            if (ImGui::Button("Apply Pending")) {
                                InvokeSelectedSchemeOperation(*interfaceSchema, COperationIdCommandApplyPending, context);
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
        ImGui::TextDisabled("No Plot2D V2 windows/artists are currently registered.");
        ImGui::TextDisabled("Create Plot2D V2 windows to populate this catalog.");
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
                    const bool bNeedsParameterSelection =
                        operation.OperationId == CPlotOperationIdQueryGetParameterV2 ||
                        operation.OperationId == CPlotOperationIdCommandSetParameterV2;
                    const bool bCanInvoke = !bNeedsParameterSelection || !SelectedPlotParameterId.empty();

                    ImGui::PushID(operation.OperationId.c_str());
                    ImGui::BeginDisabled(!bCanInvoke);
                    if (ImGui::Button(operation.DisplayName.c_str())) {
                        InvokeSelectedPlotOperation(*interfaceSchema, operation.OperationId, context);
                    }
                    ImGui::EndDisabled();
                    ImGui::SameLine();
                    ImGui::TextDisabled(
                        "%s | %s | %s",
                        ToString(operation.Kind),
                        ToString(operation.RunStatePolicy),
                        ToString(operation.ThreadAffinity));
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

    ReflectionAdapter.RefreshFromLegacyInterfaces();
    EnsureSchemeSelectionIsValid();

    const auto &catalog = ReflectionAdapter.GetCatalog();
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

    ImGui::Text("Graph interface: %s", interfaceSchema->DisplayName.c_str());
    ImGui::SameLine();
    ImGui::TextDisabled("(%s)", interfaceSchema->InterfaceId.c_str());
    ImGui::SameLine();
    ImGui::Checkbox("Grid", &bShowBlueprintGrid);
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
            if (it->first.rfind(keyPrefix, 0) == 0) {
                it = BlueprintNodePositionById.erase(it);
            } else {
                ++it;
            }
        }
    }
    ImGui::SameLine();
    if (ImGui::Button("Reset View")) {
        BlueprintGraphPan = ImVec2(80.0f, 80.0f);
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
        BlueprintGraphPan.x += io.MouseDelta.x;
        BlueprintGraphPan.y += io.MouseDelta.y;
    }

    auto *drawList = ImGui::GetWindowDrawList();
    drawList->AddRectFilled(canvasPos, canvasEnd, IM_COL32(20, 24, 30, 255), 6.0f);
    drawList->AddRect(canvasPos, canvasEnd, IM_COL32(72, 82, 92, 255), 6.0f, 0, 1.0f);

    drawList->PushClipRect(canvasPos, canvasEnd, true);

    if (bShowBlueprintGrid) {
        constexpr float GridStep = 64.0f;
        const float xOffset = std::fmod(BlueprintGraphPan.x, GridStep);
        const float yOffset = std::fmod(BlueprintGraphPan.y, GridStep);

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
            {"legend-prefix", ImVec4(168.0f / 255.0f, 174.0f / 255.0f, 188.0f / 255.0f, 1.0f), "Prefixes", "M=Mutability, E=Exposure, S=State, R=Run State, T=Thread, X=Side Effect, N=Node Kind."}
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
        ImVec2 *Position = nullptr;
        ImVec2 Size = ImVec2(360.0f, 132.0f);
        EKind Kind = EKind::Parameter;
        Slab::Str RefId;
        bool bSelected = false;
    };

    const auto findOrCreatePosition = [this](const Slab::Str &nodeKey, const ImVec2 defaultPosition) -> ImVec2 * {
        const auto [it, inserted] = BlueprintNodePositionById.emplace(nodeKey, defaultPosition);
        (void) inserted;
        return &it->second;
    };

    Slab::Vector<FGraphNode> nodes;
    nodes.reserve(1 + interfaceSchema->Parameters.size() + interfaceSchema->Operations.size());

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

    const auto keyPrefix = interfaceSchema->InterfaceId + "::";
    FGraphNode interfaceNode;
    interfaceNode.Key = keyPrefix + "interface";
    interfaceNode.Title = interfaceSchema->DisplayName;
    interfaceNode.SubtitlePrimary = interfaceSchema->InterfaceId;
    interfaceNode.SubtitleSecondary = Slab::Str("parameters=") + std::to_string(interfaceSchema->Parameters.size()) +
        " | operations=" + std::to_string(interfaceSchema->Operations.size());
    interfaceNode.Kind = FGraphNode::EKind::Interface;
    interfaceNode.RefId = interfaceSchema->InterfaceId;
    interfaceNode.bSelected = true;
    interfaceNode.Badges.push_back({WithPolicyPrefix('N', "Interface"), IM_COL32(118, 98, 166, 240)});
    interfaceNode.Size = computeNodeSize(interfaceNode, 300.0f, 520.0f);
    interfaceNode.Position = findOrCreatePosition(interfaceNode.Key, ImVec2(560.0f, 200.0f));
    nodes.push_back(interfaceNode);

    float parameterLaneY = 56.0f;
    for (const auto &parameter : interfaceSchema->Parameters) {
        if (!bBlueprintGraphShowParameters) continue;
        if (BlueprintGraphMutabilityFilter == 1 && parameter.Mutability != EParameterMutability::Const) continue;
        if (BlueprintGraphMutabilityFilter == 2 && parameter.Mutability != EParameterMutability::RuntimeMutable) continue;
        if (BlueprintGraphMutabilityFilter == 3 && parameter.Mutability != EParameterMutability::RestartRequired) continue;

        const bool bMatchesText =
            ContainsCaseInsensitive(parameter.DisplayName, BlueprintGraphFilterText) ||
            ContainsCaseInsensitive(parameter.ParameterId, BlueprintGraphFilterText) ||
            ContainsCaseInsensitive(parameter.TypeId, BlueprintGraphFilterText) ||
            ContainsCaseInsensitive(ToString(parameter.Mutability), BlueprintGraphFilterText) ||
            ContainsCaseInsensitive(ToString(parameter.Exposure), BlueprintGraphFilterText);
        if (!bMatchesText) continue;

        const auto draftKey = BuildSchemeParameterDraftKey(interfaceSchema->InterfaceId, parameter.ParameterId);
        const auto draftIt = SchemeParameterDraftByKey.find(draftKey);
        const auto draftValue = draftIt != SchemeParameterDraftByKey.end() ? draftIt->second : Slab::Str{};
        const auto liveValue = ReflectionAdapter.EncodeCurrentParameterValue(interfaceSchema->InterfaceId, parameter.ParameterId);

        Slab::Str stateLabel = "Unavailable";
        ImU32 stateColor = IM_COL32(120, 124, 136, 220);
        if (liveValue.has_value()) {
            if (draftValue == liveValue->Encoded) {
                stateLabel = "Live";
                stateColor = IM_COL32(94, 180, 130, 220);
            } else if (parameter.Mutability == EParameterMutability::RestartRequired) {
                stateLabel = "Pending";
                stateColor = IM_COL32(222, 176, 102, 220);
            } else {
                stateLabel = "Draft";
                stateColor = IM_COL32(122, 168, 224, 220);
            }
        }

        FGraphNode node;
        node.Key = keyPrefix + "param:" + parameter.ParameterId;
        node.Title = parameter.DisplayName;
        node.SubtitlePrimary = Slab::Str(parameter.ParameterId) + " | " + TruncateLabel(parameter.TypeId, 18);
        node.SubtitleSecondary = Slab::Str("State: ") + stateLabel + " | type: " + parameter.TypeId;
        node.Kind = FGraphNode::EKind::Parameter;
        node.RefId = parameter.ParameterId;
        node.bSelected = SelectedSchemeParameterId == parameter.ParameterId;
        node.Badges.push_back({WithPolicyPrefix('M', ToCompactMutabilityLabel(parameter.Mutability)), IM_COL32(88, 132, 192, 220)});
        node.Badges.push_back({WithPolicyPrefix('E', ToCompactExposureLabel(parameter.Exposure)), IM_COL32(96, 120, 150, 220)});
        node.Badges.push_back({WithPolicyPrefix('S', stateLabel), stateColor});
        node.Size = computeNodeSize(node, 340.0f, 620.0f);
        node.Position = findOrCreatePosition(node.Key, ImVec2(70.0f, parameterLaneY));
        nodes.push_back(node);
        parameterLaneY += node.Size.y + 20.0f;
    }

    float queryLaneY = 56.0f;
    float commandLaneY = 56.0f;
    for (const auto &operation : interfaceSchema->Operations) {
        if (operation.Kind == EOperationKind::Query && !bBlueprintGraphShowQueries) continue;
        if (operation.Kind == EOperationKind::Command && !bBlueprintGraphShowCommands) continue;

        const bool bMatchesText =
            ContainsCaseInsensitive(operation.DisplayName, BlueprintGraphFilterText) ||
            ContainsCaseInsensitive(operation.OperationId, BlueprintGraphFilterText) ||
            ContainsCaseInsensitive(ToString(operation.Kind), BlueprintGraphFilterText) ||
            ContainsCaseInsensitive(ToString(operation.RunStatePolicy), BlueprintGraphFilterText) ||
            ContainsCaseInsensitive(ToString(operation.ThreadAffinity), BlueprintGraphFilterText);
        if (!bMatchesText) continue;

        FGraphNode node;
        node.Key = keyPrefix + "operation:" + operation.OperationId;
        node.Title = operation.DisplayName;
        node.SubtitlePrimary = Slab::Str(operation.OperationId);
        node.SubtitleSecondary = Slab::Str(ToString(operation.Kind)) + " | run=" + ToCompactRunStateLabel(operation.RunStatePolicy) +
            " | thread=" + ToCompactThreadLabel(operation.ThreadAffinity) +
            " | side=" + ToCompactSideEffectLabel(operation.SideEffectClass);
        node.Size = computeNodeSize(node, 340.0f, 620.0f);
        if (operation.Kind == EOperationKind::Query) {
            node.Position = findOrCreatePosition(node.Key, ImVec2(980.0f, queryLaneY));
            node.Kind = FGraphNode::EKind::Query;
            queryLaneY += node.Size.y + 20.0f;
        } else {
            node.Position = findOrCreatePosition(node.Key, ImVec2(1470.0f, commandLaneY));
            node.Kind = FGraphNode::EKind::Command;
            commandLaneY += node.Size.y + 20.0f;
        }
        node.RefId = operation.OperationId;
        node.Badges.push_back({WithPolicyPrefix('R', ToCompactRunStateLabel(operation.RunStatePolicy)), IM_COL32(78, 122, 112, 220)});
        node.Badges.push_back({WithPolicyPrefix('T', ToCompactThreadLabel(operation.ThreadAffinity)), IM_COL32(86, 112, 142, 220)});
        node.Badges.push_back({WithPolicyPrefix('X', ToCompactSideEffectLabel(operation.SideEffectClass)), IM_COL32(122, 104, 88, 220)});
        nodes.push_back(node);
    }

    const auto screenPositionFor = [&](const FGraphNode &node) {
        return ImVec2(
            canvasPos.x + BlueprintGraphPan.x + node.Position->x,
            canvasPos.y + BlueprintGraphPan.y + node.Position->y);
    };

    const auto pinPositionFor = [&](const FGraphNode &node, const bool bOutput, int slot, int totalSlots) {
        const auto nodePos = screenPositionFor(node);
        const float slots = static_cast<float>(std::max(1, totalSlots));
        const float slotRatio = static_cast<float>(slot + 1) / (slots + 1.0f);
        const float y = nodePos.y + 52.0f + slotRatio * (node.Size.y - 64.0f);
        const float x = bOutput ? nodePos.x + node.Size.x : nodePos.x;
        return ImVec2(x, y);
    };

    auto &root = nodes.front();
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
    for (int i = 0; i < parameterCount; ++i) {
        const auto &paramNode = *parameterNodes[i];
        const auto source = pinPositionFor(paramNode, true, 0, 1);
        const auto target = pinPositionFor(root, false, i, parameterCount);
        const auto cp1 = ImVec2(source.x + 84.0f, source.y);
        const auto cp2 = ImVec2(target.x - 84.0f, target.y);
        drawList->AddBezierCubic(source, cp1, cp2, target, IM_COL32(104, 168, 224, 128), 1.6f);
    }

    Slab::Vector<const FGraphNode *> operationNodes;
    operationNodes.reserve(queryNodes.size() + commandNodes.size());
    operationNodes.insert(operationNodes.end(), queryNodes.begin(), queryNodes.end());
    operationNodes.insert(operationNodes.end(), commandNodes.begin(), commandNodes.end());
    for (int i = 0; i < operationCount; ++i) {
        const auto &operationNode = *operationNodes[i];
        const auto source = pinPositionFor(root, true, i, operationCount);
        const auto target = pinPositionFor(operationNode, false, 0, 1);
        const auto cp1 = ImVec2(source.x + 108.0f, source.y);
        const auto cp2 = ImVec2(target.x - 108.0f, target.y);
        const auto edgeColor = operationNode.Kind == FGraphNode::EKind::Query
            ? IM_COL32(116, 214, 174, 128)
            : IM_COL32(214, 184, 116, 128);
        drawList->AddBezierCubic(source, cp1, cp2, target, edgeColor, 1.6f);
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

        Slab::Vector<FNodeAction> actions;
        if (node.Kind == FGraphNode::EKind::Parameter) {
            actions.push_back(FNodeAction{"Get", FNodeAction::EKind::Invoke, COperationIdQueryGetParameter});
            actions.push_back(FNodeAction{"Set", FNodeAction::EKind::Invoke, COperationIdCommandSetParameter});
            const auto *parameterSchema = FindParameterById(*interfaceSchema, node.RefId);
            if (parameterSchema != nullptr && parameterSchema->Mutability == EParameterMutability::RestartRequired) {
                actions.push_back(FNodeAction{"Apply", FNodeAction::EKind::Invoke, COperationIdCommandApplyPending});
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
            BlueprintPressNodePos = *(pressedNode.Position);
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
            if (bBlueprintNodeDragging) {
                nodeIt->second->Position->x = BlueprintPressNodePos.x + delta.x;
                nodeIt->second->Position->y = BlueprintPressNodePos.y + delta.y;
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
            const auto pin = pinPositionFor(node, false, 0, 1);
            drawList->AddCircleFilled(pin, 5.0f, IM_COL32(132, 220, 162, 255));
        } else if (node.Kind == FGraphNode::EKind::Command) {
            const auto pin = pinPositionFor(node, false, 0, 1);
            drawList->AddCircleFilled(pin, 5.0f, IM_COL32(226, 198, 132, 255));
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

    const int nWindows = static_cast<int>(SlabWindows.size());

    if (bDockingMode && ActiveWorkspace == EWorkspaceTab::Simulations && nWindows == 1) {
        if (const auto &window = SlabWindows.front(); window != nullptr) {
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
    for (const auto &window : SlabWindows) {
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
    return ActiveWorkspace == EWorkspaceTab::Simulations;
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
    if (previousWorkspace == EWorkspaceTab::Simulations && ActiveWorkspace != EWorkspaceTab::Simulations) {
        CapturedMouseWindow.reset();
    }
    LoadWorkspacePanelVisibility(ActiveWorkspace);
    if (previousWorkspace == EWorkspaceTab::Simulations || ActiveWorkspace == EWorkspaceTab::Simulations) {
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
        const auto dockspaceIdPlots =
            static_cast<unsigned int>(ImGui::GetID(DockspaceNamePlots));

        if (!bWorkspaceLayoutsBootstrapped) {
            BuildDefaultDockLayout(dockspaceIdSimulations, EWorkspaceTab::Simulations);
            BuildDefaultDockLayout(dockspaceIdSchemes, EWorkspaceTab::Schemes);
            BuildDefaultDockLayout(dockspaceIdPlots, EWorkspaceTab::Plots);
            WorkspaceLayoutInitialized[static_cast<std::size_t>(EWorkspaceTab::Simulations)] = true;
            WorkspaceLayoutInitialized[static_cast<std::size_t>(EWorkspaceTab::Schemes)] = true;
            WorkspaceLayoutInitialized[static_cast<std::size_t>(EWorkspaceTab::Plots)] = true;
            bWorkspaceLayoutsBootstrapped = true;
            RequestViewRetile();
        }

        const auto dockspaceIdFor = [dockspaceIdSimulations, dockspaceIdSchemes, dockspaceIdPlots]
            (const EWorkspaceTab workspace) -> unsigned int {
                if (workspace == EWorkspaceTab::Schemes) return dockspaceIdSchemes;
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

    if (PruneClosedSlabWindows()) {
        RequestViewRetile();
    }

    const bool bRenderSlabWindows = ShouldRenderSlabWindowsInWorkspace();
    const bool bForceSimDockRetile =
        IsDockingEnabled() &&
        ActiveWorkspace == EWorkspaceTab::Simulations;

    if (bRenderSlabWindows && (bForceSimDockRetile || bPendingViewRetile || RetileStabilizationFramesRemaining > 0)) {
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
