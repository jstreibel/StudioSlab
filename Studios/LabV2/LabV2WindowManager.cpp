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
#include "Graphics/Window/WindowStyles.h"
#include "Math/Data/V2/SessionLiveViewV2.h"
#include "Math/Function/RtoR/Model/FunctionsCollection/Trigonometric.h"

#include <algorithm>
#include <cfloat>
#include <cstring>
#include <sstream>
#include <utility>

namespace {

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
    constexpr auto DockspaceHostName = "##LabDockspaceHost";
    constexpr auto DockspaceNameSimulations = "##LabDockspace-Simulations";
    constexpr auto DockspaceNameSchemes = "##LabDockspace-Schemes";
    constexpr auto WorkspaceTabSimulations = "Simulations";
    constexpr auto WorkspaceTabSchemes = "Schemes";

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
    }

    if (interfaceSchema->Parameters.empty()) {
        SelectedSchemeParameterId.clear();
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
                        FValueMapV2 inputs;
                        if (operation.OperationId == COperationIdQueryGetParameter ||
                            operation.OperationId == COperationIdCommandSetParameter) {
                            inputs["parameter_id"] = MakeStringValue(SelectedSchemeParameterId);
                        }

                        if (operation.OperationId == COperationIdCommandSetParameter) {
                            const auto *parameterSchema = FindParameterById(*interfaceSchema, SelectedSchemeParameterId);
                            if (parameterSchema == nullptr) {
                                SchemesLastOperationSummary = "[Error] reflection.parameter.not_found";
                            } else {
                                const auto draftKey = BuildSchemeParameterDraftKey(
                                    interfaceSchema->InterfaceId,
                                    parameterSchema->ParameterId);
                                const auto draftIt = SchemeParameterDraftByKey.find(draftKey);
                                const auto draft = draftIt != SchemeParameterDraftByKey.end() ? draftIt->second : Slab::Str{};
                                inputs["value"] = FReflectionValueV2(parameterSchema->TypeId, draft);
                            }
                        }

                        const auto result = ReflectionAdapter.Invoke(
                            interfaceSchema->InterfaceId,
                            operation.OperationId,
                            inputs,
                            context);
                        SchemesLastOperationSummary = BuildOperationSummary(result);
                        SchemesLastOperationOutput = result.OutputMap;

                        if (result.IsOk()) {
                            if (const auto it = result.OutputMap.find("value"); it != result.OutputMap.end() &&
                                !SelectedSchemeParameterId.empty()) {
                                const auto draftKey = BuildSchemeParameterDraftKey(
                                    interfaceSchema->InterfaceId,
                                    SelectedSchemeParameterId);
                                SchemeParameterDraftByKey[draftKey] = it->second.Encoded;
                            }
                        }
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
                            FValueMapV2 inputs;
                            inputs["parameter_id"] = MakeStringValue(selectedParameterSchema->ParameterId);
                            inputs["value"] = FReflectionValueV2(selectedParameterSchema->TypeId, draft);

                            const auto result = ReflectionAdapter.Invoke(
                                interfaceSchema->InterfaceId,
                                COperationIdCommandSetParameter,
                                inputs,
                                context);
                            SchemesLastOperationSummary = BuildOperationSummary(result);
                            SchemesLastOperationOutput = result.OutputMap;
                        }

                        if (selectedParameterSchema->Mutability == EParameterMutability::RestartRequired) {
                            ImGui::SameLine();
                            if (ImGui::Button("Apply Pending")) {
                                const auto result = ReflectionAdapter.Invoke(
                                    interfaceSchema->InterfaceId,
                                    COperationIdCommandApplyPending,
                                    {},
                                    context);
                                SchemesLastOperationSummary = BuildOperationSummary(result);
                                SchemesLastOperationOutput = result.OutputMap;
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

auto FLabV2WindowManager::DrawSchemesBlueprintGraphPanel() -> void {
    using namespace Slab::Core::Reflection::V2;

    ReflectionAdapter.RefreshFromLegacyInterfaces();
    EnsureSchemeSelectionIsValid();

    const auto &catalog = ReflectionAdapter.GetCatalog();
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

    ImGui::Separator();

    const auto canvasPos = ImGui::GetCursorScreenPos();
    auto canvasSize = ImGui::GetContentRegionAvail();
    if (canvasSize.x < 64.0f) canvasSize.x = 64.0f;
    if (canvasSize.y < 180.0f) canvasSize.y = 180.0f;
    const auto canvasEnd = ImVec2(canvasPos.x + canvasSize.x, canvasPos.y + canvasSize.y);

    ImGui::InvisibleButton(
        "SchemesBlueprintCanvas",
        canvasSize,
        ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);

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

    struct FGraphNode {
        enum class EKind : unsigned char {
            Interface,
            Parameter,
            Operation
        };

        Slab::Str Key;
        Slab::Str Title;
        Slab::Str Subtitle;
        ImVec2 *Position = nullptr;
        ImVec2 Size = ImVec2(240.0f, 96.0f);
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

    const auto keyPrefix = interfaceSchema->InterfaceId + "::";
    FGraphNode interfaceNode;
    interfaceNode.Key = keyPrefix + "interface";
    interfaceNode.Title = interfaceSchema->DisplayName;
    interfaceNode.Subtitle = interfaceSchema->InterfaceId;
    interfaceNode.Position = findOrCreatePosition(interfaceNode.Key, ImVec2(460.0f, 180.0f));
    interfaceNode.Size = ImVec2(280.0f, 110.0f);
    interfaceNode.Kind = FGraphNode::EKind::Interface;
    interfaceNode.RefId = interfaceSchema->InterfaceId;
    interfaceNode.bSelected = true;
    nodes.push_back(interfaceNode);

    for (std::size_t i = 0; i < interfaceSchema->Parameters.size(); ++i) {
        const auto &parameter = interfaceSchema->Parameters[i];

        FGraphNode node;
        node.Key = keyPrefix + "param:" + parameter.ParameterId;
        node.Title = parameter.DisplayName;
        node.Subtitle = Slab::Str(parameter.TypeId) + " | " + ToString(parameter.Mutability);
        node.Position = findOrCreatePosition(node.Key, ImVec2(80.0f, 60.0f + static_cast<float>(i) * 130.0f));
        node.Size = ImVec2(290.0f, 104.0f);
        node.Kind = FGraphNode::EKind::Parameter;
        node.RefId = parameter.ParameterId;
        node.bSelected = SelectedSchemeParameterId == parameter.ParameterId;
        nodes.push_back(node);
    }

    for (std::size_t i = 0; i < interfaceSchema->Operations.size(); ++i) {
        const auto &operation = interfaceSchema->Operations[i];

        FGraphNode node;
        node.Key = keyPrefix + "operation:" + operation.OperationId;
        node.Title = operation.DisplayName;
        node.Subtitle = Slab::Str(ToString(operation.Kind)) + " | " + ToString(operation.RunStatePolicy);
        node.Position = findOrCreatePosition(node.Key, ImVec2(860.0f, 60.0f + static_cast<float>(i) * 130.0f));
        node.Size = ImVec2(300.0f, 104.0f);
        node.Kind = FGraphNode::EKind::Operation;
        node.RefId = operation.OperationId;
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
        const float y = nodePos.y + 30.0f + slotRatio * (node.Size.y - 40.0f);
        const float x = bOutput ? nodePos.x + node.Size.x : nodePos.x;
        return ImVec2(x, y);
    };

    auto &root = nodes.front();
    const int parameterCount = static_cast<int>(interfaceSchema->Parameters.size());
    const int operationCount = static_cast<int>(interfaceSchema->Operations.size());
    for (int i = 0; i < parameterCount; ++i) {
        const auto &paramNode = nodes[1 + i];
        const auto source = pinPositionFor(paramNode, true, 0, 1);
        const auto target = pinPositionFor(root, false, i, parameterCount);
        const auto cp1 = ImVec2(source.x + 70.0f, source.y);
        const auto cp2 = ImVec2(target.x - 70.0f, target.y);
        drawList->AddBezierCubic(source, cp1, cp2, target, IM_COL32(104, 168, 224, 190), 2.0f);
    }
    for (int i = 0; i < operationCount; ++i) {
        const auto &operationNode = nodes[1 + parameterCount + i];
        const auto source = pinPositionFor(root, true, i, operationCount);
        const auto target = pinPositionFor(operationNode, false, 0, 1);
        const auto cp1 = ImVec2(source.x + 90.0f, source.y);
        const auto cp2 = ImVec2(target.x - 90.0f, target.y);
        drawList->AddBezierCubic(source, cp1, cp2, target, IM_COL32(120, 214, 146, 200), 2.0f);
    }

    for (auto &node : nodes) {
        const auto nodePos = screenPositionFor(node);
        const auto nodeEnd = ImVec2(nodePos.x + node.Size.x, nodePos.y + node.Size.y);
        if (nodeEnd.x < canvasPos.x || nodePos.x > canvasEnd.x || nodeEnd.y < canvasPos.y || nodePos.y > canvasEnd.y) {
            continue;
        }

        const auto interactionId = "##bpnode_" + node.Key;
        ImGui::SetCursorScreenPos(nodePos);
        ImGui::InvisibleButton(interactionId.c_str(), node.Size, ImGuiButtonFlags_MouseButtonLeft);
        const bool bHovered = ImGui::IsItemHovered();
        const bool bActive = ImGui::IsItemActive();

        if (bActive && ImGui::IsMouseDragging(ImGuiMouseButton_Left, 0.0f)) {
            const auto &io = ImGui::GetIO();
            node.Position->x += io.MouseDelta.x;
            node.Position->y += io.MouseDelta.y;
        }

        if (bHovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
            if (node.Kind == FGraphNode::EKind::Interface) {
                SelectedSchemeInterfaceId = node.RefId;
            } else if (node.Kind == FGraphNode::EKind::Parameter) {
                SelectedSchemeParameterId = node.RefId;
            }
        }

        ImU32 bodyColor = IM_COL32(38, 44, 55, 238);
        ImU32 headerColor = IM_COL32(92, 106, 132, 255);
        if (node.Kind == FGraphNode::EKind::Operation) {
            bodyColor = IM_COL32(34, 48, 46, 238);
            headerColor = IM_COL32(78, 132, 120, 255);
        }
        if (node.Kind == FGraphNode::EKind::Interface) {
            bodyColor = IM_COL32(44, 40, 58, 238);
            headerColor = IM_COL32(112, 94, 154, 255);
        }

        drawList->AddRectFilled(nodePos, nodeEnd, bodyColor, 8.0f);
        drawList->AddRectFilled(nodePos, ImVec2(nodeEnd.x, nodePos.y + 26.0f), headerColor, 8.0f, ImDrawFlags_RoundCornersTop);
        drawList->AddRect(nodePos, nodeEnd, node.bSelected ? IM_COL32(245, 195, 110, 255) : IM_COL32(96, 106, 122, 255), 8.0f, 0, node.bSelected ? 2.0f : 1.0f);
        drawList->AddText(ImVec2(nodePos.x + 9.0f, nodePos.y + 6.0f), IM_COL32(240, 244, 252, 255), node.Title.c_str());
        drawList->AddText(
            ImVec2(nodePos.x + 9.0f, nodePos.y + 36.0f),
            IM_COL32(182, 192, 208, 255),
            TruncateLabel(node.Subtitle, 64).c_str());

        if (node.Kind == FGraphNode::EKind::Parameter) {
            const auto pin = pinPositionFor(node, true, 0, 1);
            drawList->AddCircleFilled(pin, 5.0f, IM_COL32(138, 196, 246, 255));
        } else if (node.Kind == FGraphNode::EKind::Operation) {
            const auto pin = pinPositionFor(node, false, 0, 1);
            drawList->AddCircleFilled(pin, 5.0f, IM_COL32(132, 220, 162, 255));
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
        bShowWindowBlueprintGraph
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
        } else {
            drawToggle("Inspector", &bShowWindowSchemeInspector);
            drawToggle("Blueprint Graph", &bShowWindowBlueprintGraph);
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
    } else {
        const auto dockLeft = ImGui::DockBuilderSplitNode(dockMain, ImGuiDir_Left, 0.35f, nullptr, &dockMain);
        ImGui::DockBuilderDockWindow(WindowTitleSchemesInspector, dockLeft);
        ImGui::DockBuilderDockWindow(WindowTitleBlueprintGraph, dockMain);
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

        if (!bWorkspaceLayoutsBootstrapped) {
            BuildDefaultDockLayout(dockspaceIdSimulations, EWorkspaceTab::Simulations);
            BuildDefaultDockLayout(dockspaceIdSchemes, EWorkspaceTab::Schemes);
            WorkspaceLayoutInitialized[static_cast<std::size_t>(EWorkspaceTab::Simulations)] = true;
            WorkspaceLayoutInitialized[static_cast<std::size_t>(EWorkspaceTab::Schemes)] = true;
            bWorkspaceLayoutsBootstrapped = true;
            RequestViewRetile();
        }

        const auto dockspaceIdFor = [dockspaceIdSimulations, dockspaceIdSchemes]
            (const EWorkspaceTab workspace) -> unsigned int {
                if (workspace == EWorkspaceTab::Schemes) return dockspaceIdSchemes;
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
#endif
    }
    ImGui::End();

    ImGui::PopStyleVar(3);
}

auto FLabV2WindowManager::BuildPanelSurfaceRegistry() -> std::vector<FPanelSurfaceRegistration> {
    std::vector<FPanelSurfaceRegistration> registry;
    registry.reserve(11);

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
