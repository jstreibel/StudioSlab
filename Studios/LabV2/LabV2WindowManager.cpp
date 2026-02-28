#include "LabV2WindowManager.h"

#include "SimulationManagerV2.h"
#include "StudioConfigV2.h"

#include "imgui.h"
#ifdef IMGUI_HAS_DOCK
#include "imgui_internal.h"
#endif

#include "Core/SlabCore.h"
#include "Core/Backend/Modules/TaskManager/TaskManager.h"
#include "Graphics/Modules/ImGui/ImGuiModule.h"
#include "Graphics/SlabGraphics.h"
#include "Graphics/Plot2D/Plot2DWindow.h"
#include "Graphics/Plot2D/Plotter.h"
#include "Graphics/Plot2D/PlotThemeManager.h"
#include "Graphics/Window/WindowStyles.h"
#include "Math/Data/V2/SessionLiveViewV2.h"
#include "Math/Data/V2/LiveControlTopicsV2.h"
#include "Math/Numerics/NumericTask.h"
#include "Math/Numerics/V2/Task/NumericTaskV2.h"
#include "Math/Function/RtoR/Model/FunctionsCollection/Trigonometric.h"

#include <algorithm>
#include <cctype>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <utility>

namespace {

    auto ToLowerAscii(Slab::Str value) -> Slab::Str {
        std::transform(value.begin(), value.end(), value.begin(), [](const unsigned char c) {
            return static_cast<char>(std::tolower(c));
        });
        return value;
    }

    auto MatchesTopicFilter(const Slab::Str &candidate, const Slab::Str &filter) -> bool {
        if (filter.empty()) return true;
        const auto loweredCandidate = ToLowerAscii(candidate);
        const auto loweredFilter = ToLowerAscii(filter);
        return loweredCandidate.find(loweredFilter) != Slab::Str::npos;
    }

    auto DrawFilterField(const char *label, const char *hint, Slab::Str &value) -> void {
        char buffer[256];
        std::snprintf(buffer, sizeof(buffer), "%s", value.c_str());
        const bool changed = ImGui::InputTextWithHint(label, hint, buffer, sizeof(buffer));
        if (changed) value = buffer;
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
    constexpr auto WindowTitleViews = "Views";
    constexpr auto WindowTitleKG2DControl = "KG2D Control";
    constexpr auto WindowTitleSimulationLauncher = "Simulation Launcher";
    constexpr auto WindowTitleBlueprints = "Blueprints";
    constexpr auto DockspaceHostName = "##LabV2DockspaceHost";
    constexpr auto DockspaceNameSimulations = "##LabV2Dockspace-Simulations";
    constexpr auto DockspaceNameMonitor = "##LabV2Dockspace-Monitor";
    constexpr auto DockspaceNameSchemes = "##LabV2Dockspace-Schemes";
    constexpr auto WorkspaceTabSimulations = "Simulations";
    constexpr auto WorkspaceTabMonitor = "Monitor";
    constexpr auto WorkspaceTabSchemes = "Schemes";

    auto ShowTasksPanel(Slab::Str &nameFilter,
                        bool &bOnlyRunning,
                        bool &bHideSuccess,
                        bool &bOnlyNumeric) -> void {
        const auto taskManager = Slab::Core::GetModule<Slab::Core::FTaskManager>("TaskManager");
        if (taskManager == nullptr) return;

        const auto jobs = taskManager->GetAllJobs();
        ImGui::SeparatorText("Tasks");

        DrawFilterField("##labv2-task-filter", "Filter task name", nameFilter);
        ImGui::SameLine();
        ImGui::Checkbox("Running only", &bOnlyRunning);
        ImGui::SameLine();
        ImGui::Checkbox("Hide success", &bHideSuccess);
        ImGui::SameLine();
        ImGui::Checkbox("Numeric only", &bOnlyNumeric);

        if (jobs.empty()) {
            ImGui::TextDisabled("No tasks.");
            return;
        }

        size_t runningCount = 0;
        size_t successCount = 0;
        size_t errorCount = 0;
        size_t abortedCount = 0;
        size_t notInitCount = 0;

        for (const auto &job : jobs) {
            const auto &task = job.Task;
            if (task == nullptr) continue;
            switch (task->GetStatus()) {
            case Slab::Core::TaskRunning: ++runningCount; break;
            case Slab::Core::TaskSuccess: ++successCount; break;
            case Slab::Core::TaskError: ++errorCount; break;
            case Slab::Core::TaskAborted: ++abortedCount; break;
            case Slab::Core::TaskNotInitialized: ++notInitCount; break;
            default: break;
            }
        }

        ImGui::Text(
            "total=%zu running=%zu success=%zu error=%zu aborted=%zu not-init=%zu",
            jobs.size(),
            runningCount,
            successCount,
            errorCount,
            abortedCount,
            notInitCount);

        if (ImGui::SmallButton("Clear Success")) {
            for (const auto &job : jobs) {
                if (job.Task != nullptr && job.Task->GetStatus() == Slab::Core::TaskSuccess) {
                    (void) taskManager->ClearJob(job);
                }
            }
        }
        ImGui::SameLine();
        if (ImGui::SmallButton("Clear Terminated")) {
            for (const auto &job : jobs) {
                if (job.Task == nullptr) continue;
                const auto status = job.Task->GetStatus();
                if (status == Slab::Core::TaskSuccess ||
                    status == Slab::Core::TaskError ||
                    status == Slab::Core::TaskAborted) {
                    (void) taskManager->ClearJob(job);
                }
            }
        }

        constexpr auto tableFlags =
            ImGuiTableFlags_Borders |
            ImGuiTableFlags_RowBg |
            ImGuiTableFlags_ScrollX |
            ImGuiTableFlags_SizingFixedFit;

        if (!ImGui::BeginTable("LabV2TaskTable", 6, tableFlags)) return;

        ImGui::TableSetupColumn("Task", ImGuiTableColumnFlags_WidthStretch, 1.0f);
        ImGui::TableSetupColumn("Kind");
        ImGui::TableSetupColumn("Status");
        ImGui::TableSetupColumn("Progress");
        ImGui::TableSetupColumn("Cursor");
        ImGui::TableSetupColumn("Action");
        ImGui::TableHeadersRow();

        for (auto &job : jobs) {
            auto &[task, jobThread] = job;
            (void) jobThread;
            if (task == nullptr) continue;

            const auto taskName = task->GetName();
            if (!MatchesTopicFilter(taskName, nameFilter)) continue;

            const auto *numericTaskV2 = dynamic_cast<Slab::Math::Numerics::V2::FNumericTaskV2 *>(task.get());
            const auto *legacyNumericTask = dynamic_cast<Slab::Math::FNumericTask *>(task.get());
            const bool bNumericTask = numericTaskV2 != nullptr || legacyNumericTask != nullptr;

            if (bOnlyNumeric && !bNumericTask) continue;
            if (bOnlyRunning && task->GetStatus() != Slab::Core::TaskRunning) continue;
            if (bHideSuccess && task->GetStatus() == Slab::Core::TaskSuccess) continue;

            ImGui::TableNextRow();
            ImGui::PushID(task.get());

            ImGui::TableSetColumnIndex(0);
            ImGui::TextUnformatted(taskName.c_str());

            ImGui::TableSetColumnIndex(1);
            if (numericTaskV2 != nullptr) ImGui::TextUnformatted("NumericV2");
            else if (legacyNumericTask != nullptr) ImGui::TextUnformatted("NumericLegacy");
            else ImGui::TextUnformatted("Task");

            ImGui::TableSetColumnIndex(2);

            switch (task->GetStatus()) {
            case Slab::Core::TaskRunning:
                ImGui::TextColored(ImVec4(0.2f, 0.6f, 1.0f, 1), "Running");
                break;
            case Slab::Core::TaskAborted:
                ImGui::TextColored(ImVec4(0.9f, 0.5f, 0.1f, 1), "Aborted");
                break;
            case Slab::Core::TaskError:
                ImGui::TextColored(ImVec4(1.0f, 0.2f, 0.7f, 1), "Error");
                break;
            case Slab::Core::TaskSuccess:
                ImGui::TextColored(ImVec4(0.2f, 0.9f, 0.2f, 1), "Success");
                break;
            case Slab::Core::TaskNotInitialized:
                ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.2f, 1), "NotInitialized");
                break;
            default:
                ImGui::TextColored(ImVec4(1, 0, 0, 1), "Unknown");
                break;
            }

            ImGui::TableSetColumnIndex(3);
            if (numericTaskV2 != nullptr) {
                const auto progressOpt = numericTaskV2->GetProgress01();
                if (progressOpt.has_value()) {
                    ImGui::ProgressBar(*progressOpt, ImVec2(110.0f, 0.0f));
                } else {
                    ImGui::TextDisabled("open");
                }
            } else if (legacyNumericTask != nullptr) {
                ImGui::ProgressBar(legacyNumericTask->GetProgress(), ImVec2(110.0f, 0.0f));
            } else {
                ImGui::TextDisabled("-");
            }

            ImGui::TableSetColumnIndex(4);
            if (numericTaskV2 != nullptr) {
                const auto cursor = numericTaskV2->GetCursor();
                if (cursor.SimulationTime.has_value()) {
                    ImGui::Text("s=%llu t=%.4g",
                        static_cast<unsigned long long>(cursor.Step),
                        *cursor.SimulationTime);
                } else {
                    ImGui::Text("s=%llu",
                        static_cast<unsigned long long>(cursor.Step));
                }
            } else {
                ImGui::TextDisabled("-");
            }

            ImGui::TableSetColumnIndex(5);
            if (task->GetStatus() != Slab::Core::TaskRunning) {
                if (ImGui::SmallButton("Clear")) {
                    (void) taskManager->ClearJob(job);
                }
            } else {
                ImGui::TextDisabled("-");
            }

            ImGui::PopID();
        }

        ImGui::EndTable();
    }

    auto ToStatusString(const Slab::Math::LiveData::V2::ESessionRunStateV2 runState) -> Slab::Str {
        using EState = Slab::Math::LiveData::V2::ESessionRunStateV2;
        switch (runState) {
        case EState::Running: return "Running";
        case EState::Finished: return "Finished";
        case EState::Aborted: return "Aborted";
        }
        return "Unknown";
    }

    auto ToReasonString(const Slab::Math::Numerics::V2::EEventReasonV2 reason) -> Slab::Str {
        using EReason = Slab::Math::Numerics::V2::EEventReasonV2;
        switch (reason) {
        case EReason::Initial: return "Initial";
        case EReason::Scheduled: return "Scheduled";
        case EReason::Forced: return "Forced";
        case EReason::Final: return "Final";
        case EReason::AbortFinal: return "AbortFinal";
        }
        return "Unknown";
    }

    auto ShowLiveDataV2Panel(const Slab::TPointer<Slab::Math::LiveData::V2::FLiveDataHubV2> &hub,
                             Slab::Str &topicFilter,
                             bool &bOnlyBound,
                             Slab::Str &selectedTopic) -> void {
        ImGui::SeparatorText("Live Data V2");

        if (hub == nullptr) {
            ImGui::TextColored(ImVec4(1, 0, 0, 1), "LiveDataHubV2 unavailable.");
            return;
        }

        DrawFilterField("##labv2-live-data-filter", "Filter topics", topicFilter);
        ImGui::SameLine();
        ImGui::Checkbox("Only bound", &bOnlyBound);

        const auto names = hub->ListSessionLiveViewTopics();
        if (names.empty()) {
            selectedTopic.clear();
            ImGui::TextDisabled("No session streams registered.");
            return;
        }

        Slab::Vector<Slab::Str> filteredNames;
        filteredNames.reserve(names.size());
        for (const auto &name : names) {
            if (!MatchesTopicFilter(name, topicFilter)) continue;
            const auto view = hub->FindSessionLiveView(name);
            const bool bBound = view != nullptr && view->HasBoundSession();
            if (bOnlyBound && !bBound) continue;
            filteredNames.emplace_back(name);
        }

        if (filteredNames.empty()) {
            selectedTopic.clear();
            ImGui::TextDisabled("No topic matched current filter.");
            return;
        }

        constexpr auto tableFlags =
            ImGuiTableFlags_Borders |
            ImGuiTableFlags_RowBg |
            ImGuiTableFlags_ScrollX |
            ImGuiTableFlags_SizingFixedFit;
        if (ImGui::BeginTable("LabV2LiveDataTopics", 6, tableFlags)) {
            ImGui::TableSetupColumn("Topic", ImGuiTableColumnFlags_WidthStretch, 1.0f);
            ImGui::TableSetupColumn("Run");
            ImGui::TableSetupColumn("Step");
            ImGui::TableSetupColumn("t");
            ImGui::TableSetupColumn("Ver");
            ImGui::TableSetupColumn("Bnd");
            ImGui::TableHeadersRow();

            for (const auto &name : filteredNames) {
                const auto view = hub->FindSessionLiveView(name);
                const auto telemetry = view != nullptr ? view->TryGetTelemetry() : std::nullopt;
                const auto status = view != nullptr ? view->TryGetStatus() : std::nullopt;
                const bool bBound = view != nullptr && view->HasBoundSession();

                ImGui::TableNextRow();

                ImGui::TableSetColumnIndex(0);
                ImGui::PushID(name.c_str());
                const bool bSelected = selectedTopic == name;
                if (ImGui::Selectable("##row", bSelected, ImGuiSelectableFlags_SpanAllColumns)) {
                    selectedTopic = name;
                }
                ImGui::SameLine();
                ImGui::TextUnformatted(name.c_str());
                ImGui::PopID();

                ImGui::TableSetColumnIndex(1);
                if (status.has_value()) {
                    const auto runString = ToStatusString(status->RunState);
                    const auto color =
                        status->RunState == Slab::Math::LiveData::V2::ESessionRunStateV2::Running
                            ? ImVec4(0.2f, 0.9f, 0.2f, 1.0f)
                        : status->RunState == Slab::Math::LiveData::V2::ESessionRunStateV2::Finished
                            ? ImVec4(0.5f, 0.9f, 1.0f, 1.0f)
                            : ImVec4(1.0f, 0.6f, 0.2f, 1.0f);
                    ImGui::TextColored(color, "%s", runString.c_str());
                }
                else ImGui::TextDisabled("n/a");

                ImGui::TableSetColumnIndex(2);
                if (telemetry.has_value()) ImGui::Text("%llu", static_cast<unsigned long long>(telemetry->Cursor.Step));
                else ImGui::TextDisabled("-");

                ImGui::TableSetColumnIndex(3);
                if (telemetry.has_value() && telemetry->Cursor.SimulationTime.has_value()) {
                    ImGui::Text("%.6g", *telemetry->Cursor.SimulationTime);
                } else {
                    ImGui::TextDisabled("-");
                }

                ImGui::TableSetColumnIndex(4);
                if (telemetry.has_value()) ImGui::Text("%llu", static_cast<unsigned long long>(telemetry->PublishedVersion));
                else ImGui::TextDisabled("-");

                ImGui::TableSetColumnIndex(5);
                ImGui::Text("%s", bBound ? "yes" : "no");

                if (telemetry.has_value() && ImGui::IsItemHovered()) {
                    if (ImGui::BeginTooltip()) {
                        ImGui::Text("Reason: %s", ToReasonString(telemetry->LastReason).c_str());
                        ImGui::Text("State present: %s", telemetry->bHasState ? "yes" : "no");
                        ImGui::EndTooltip();
                    }
                }
            }

            ImGui::EndTable();
        }

        if (!selectedTopic.empty()) {
            const auto selectedView = hub->FindSessionLiveView(selectedTopic);
            if (selectedView == nullptr) {
                selectedTopic.clear();
                return;
            }

            ImGui::SeparatorText("Selected Session Topic");
            ImGui::TextUnformatted(selectedTopic.c_str());
            ImGui::Text("Bound session: %s", selectedView->HasBoundSession() ? "yes" : "no");

            if (const auto status = selectedView->TryGetStatus(); status.has_value()) {
                ImGui::Text("Run: %s", ToStatusString(status->RunState).c_str());
                ImGui::Text("Last reason: %s", ToReasonString(status->LastReason).c_str());
            } else {
                ImGui::TextDisabled("Status: n/a");
            }

            if (const auto telemetry = selectedView->TryGetTelemetry(); telemetry.has_value()) {
                ImGui::Text("Step: %llu", static_cast<unsigned long long>(telemetry->Cursor.Step));
                if (telemetry->Cursor.SimulationTime.has_value()) {
                    ImGui::Text("t: %.6g", *telemetry->Cursor.SimulationTime);
                } else {
                    ImGui::TextDisabled("t: n/a");
                }
                ImGui::Text("Version: %llu", static_cast<unsigned long long>(telemetry->PublishedVersion));
            } else {
                ImGui::TextDisabled("Telemetry: n/a");
            }
        }
    }

    auto ToControlSemanticString(const Slab::Math::LiveControl::V2::EControlSemanticV2 semantic) -> Slab::Str {
        using ESemantic = Slab::Math::LiveControl::V2::EControlSemanticV2;
        switch (semantic) {
        case ESemantic::Level: return "Level";
        case ESemantic::Event: return "Event";
        }
        return "Unknown";
    }

    auto ToControlTimeDomainString(const Slab::Math::LiveControl::V2::EControlTimeDomainV2 domain) -> Slab::Str {
        using EDomain = Slab::Math::LiveControl::V2::EControlTimeDomainV2;
        switch (domain) {
        case EDomain::WallClockTime: return "WallClock";
        case EDomain::SimulationTime: return "Simulation";
        case EDomain::StepIndex: return "Step";
        }
        return "Unknown";
    }

    auto ToControlValueTypeString(const Slab::Math::LiveControl::V2::FControlValueV2 &value) -> Slab::Str {
        if (std::holds_alternative<bool>(value)) return "bool";
        if (std::holds_alternative<Slab::DevFloat>(value)) return "scalar";
        if (std::holds_alternative<Slab::Math::Real2D>(value)) return "point2d";
        return "unknown";
    }

    auto ToControlValueString(const Slab::Math::LiveControl::V2::FControlValueV2 &value) -> Slab::Str {
        if (std::holds_alternative<bool>(value)) {
            return std::get<bool>(value) ? "true" : "false";
        }

        if (std::holds_alternative<Slab::DevFloat>(value)) {
            return Slab::ToStr(std::get<Slab::DevFloat>(value), 6, true);
        }

        if (std::holds_alternative<Slab::Math::Real2D>(value)) {
            const auto point = std::get<Slab::Math::Real2D>(value);
            return "(" + Slab::ToStr(point.x, 4, true) + ", " + Slab::ToStr(point.y, 4, true) + ")";
        }

        return "<unknown>";
    }

    auto ToControlTimeString(const Slab::Math::LiveControl::V2::FControlTimestampV2 &timestamp) -> Slab::Str {
        using EDomain = Slab::Math::LiveControl::V2::EControlTimeDomainV2;

        switch (timestamp.Domain) {
        case EDomain::WallClockTime:
            if (timestamp.WallClockSeconds.has_value()) return Slab::ToStr(*timestamp.WallClockSeconds, 6, true);
            return "-";
        case EDomain::SimulationTime:
            if (timestamp.SimulationSeconds.has_value()) return Slab::ToStr(*timestamp.SimulationSeconds, 6, true);
            return "-";
        case EDomain::StepIndex:
            if (timestamp.Step.has_value()) return Slab::ToStr(*timestamp.Step);
            return "-";
        }

        return "-";
    }

    auto ShowLiveControlV2Panel(const Slab::TPointer<Slab::Math::LiveControl::V2::FLiveControlHubV2> &hub,
                                Slab::Str &topicFilter,
                                bool &bLevelsOnly,
                                Slab::Str &selectedTopic) -> void {
        ImGui::SeparatorText("Live Control V2");

        if (hub == nullptr) {
            ImGui::TextColored(ImVec4(1, 0, 0, 1), "LiveControlHubV2 unavailable.");
            return;
        }

        DrawFilterField("##labv2-live-control-filter", "Filter topics", topicFilter);
        ImGui::SameLine();
        ImGui::Checkbox("Levels only", &bLevelsOnly);

        const auto topicNames = hub->ListTopics();
        if (topicNames.empty()) {
            selectedTopic.clear();
            ImGui::TextDisabled("No control streams registered.");
            return;
        }

        Slab::Vector<Slab::Str> filteredTopics;
        filteredTopics.reserve(topicNames.size());
        for (const auto &topicName : topicNames) {
            if (!MatchesTopicFilter(topicName, topicFilter)) continue;
            const auto topic = hub->FindTopic(topicName);
            const auto sample = topic != nullptr ? topic->TryGetLatestSample() : std::nullopt;
            if (bLevelsOnly && sample.has_value() &&
                sample->Semantic != Slab::Math::LiveControl::V2::EControlSemanticV2::Level) {
                continue;
            }
            filteredTopics.emplace_back(topicName);
        }

        if (filteredTopics.empty()) {
            selectedTopic.clear();
            ImGui::TextDisabled("No control topic matched current filter.");
            return;
        }

        constexpr auto tableFlags =
            ImGuiTableFlags_Borders |
            ImGuiTableFlags_RowBg |
            ImGuiTableFlags_ScrollX |
            ImGuiTableFlags_SizingFixedFit;
        if (ImGui::BeginTable("LabV2LiveControlTopics", 7, tableFlags)) {
            ImGui::TableSetupColumn("Topic", ImGuiTableColumnFlags_WidthStretch, 1.0f);
            ImGui::TableSetupColumn("Semantic");
            ImGui::TableSetupColumn("Type");
            ImGui::TableSetupColumn("Value");
            ImGui::TableSetupColumn("Domain");
            ImGui::TableSetupColumn("Time");
            ImGui::TableSetupColumn("Ver");
            ImGui::TableHeadersRow();

            for (const auto &topicName : filteredTopics) {
                const auto topic = hub->FindTopic(topicName);
                const auto sample = topic != nullptr ? topic->TryGetLatestSample() : std::nullopt;

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::PushID(topicName.c_str());
                const bool bSelected = selectedTopic == topicName;
                if (ImGui::Selectable("##row", bSelected, ImGuiSelectableFlags_SpanAllColumns)) {
                    selectedTopic = topicName;
                }
                ImGui::SameLine();
                ImGui::TextUnformatted(topicName.c_str());
                ImGui::PopID();

                ImGui::TableSetColumnIndex(1);
                if (sample.has_value()) ImGui::Text("%s", ToControlSemanticString(sample->Semantic).c_str());
                else ImGui::TextDisabled("-");

                ImGui::TableSetColumnIndex(2);
                if (sample.has_value()) ImGui::Text("%s", ToControlValueTypeString(sample->Value).c_str());
                else ImGui::TextDisabled("-");

                ImGui::TableSetColumnIndex(3);
                if (sample.has_value()) ImGui::Text("%s", ToControlValueString(sample->Value).c_str());
                else ImGui::TextDisabled("-");

                ImGui::TableSetColumnIndex(4);
                if (sample.has_value()) ImGui::Text("%s", ToControlTimeDomainString(sample->Timestamp.Domain).c_str());
                else ImGui::TextDisabled("-");

                ImGui::TableSetColumnIndex(5);
                if (sample.has_value()) ImGui::Text("%s", ToControlTimeString(sample->Timestamp).c_str());
                else ImGui::TextDisabled("-");

                ImGui::TableSetColumnIndex(6);
                if (sample.has_value()) ImGui::Text("%llu", static_cast<unsigned long long>(sample->PublishedVersion));
                else ImGui::TextDisabled("-");
            }

            ImGui::EndTable();
        }

        if (!selectedTopic.empty()) {
            const auto selected = hub->FindTopic(selectedTopic);
            if (selected == nullptr) {
                selectedTopic.clear();
                return;
            }

            ImGui::SeparatorText("Selected Control Topic");
            ImGui::TextUnformatted(selectedTopic.c_str());
            if (const auto sample = selected->TryGetLatestSample(); sample.has_value()) {
                ImGui::Text("Semantic: %s", ToControlSemanticString(sample->Semantic).c_str());
                ImGui::Text("Type: %s", ToControlValueTypeString(sample->Value).c_str());
                ImGui::Text("Value: %s", ToControlValueString(sample->Value).c_str());
                ImGui::Text("Time domain: %s", ToControlTimeDomainString(sample->Timestamp.Domain).c_str());
                ImGui::Text("Time: %s", ToControlTimeString(sample->Timestamp).c_str());
                ImGui::Text("Version: %llu", static_cast<unsigned long long>(sample->PublishedVersion));
            } else {
                ImGui::TextDisabled("No sample published yet.");
            }
        }
    }

    auto ShowKG2DControlSourcePanelAndPublish(
        const Slab::TPointer<Slab::Math::LiveControl::V2::FLiveControlHubV2> &hub,
        bool &bPublish,
        Slab::DevFloat &xCenter,
        Slab::DevFloat &yCenter,
        Slab::DevFloat &width,
        Slab::DevFloat &amplitude,
        bool &bEnabled,
        const Slab::Str &topicPrefix) -> void {
        using namespace Slab::Math::LiveControl::V2;

        ImGui::SeparatorText("KG2D Control Source");
        ImGui::Checkbox("Publish KG2D forcing controls", &bPublish);
        ImGui::TextDisabled("Topic prefix: %s", topicPrefix.c_str());

        ImGui::DragScalar("KG2D ctrl x", ImGuiDataType_Double, &xCenter, 0.01, nullptr, nullptr, "%.6g");
        ImGui::DragScalar("KG2D ctrl y", ImGuiDataType_Double, &yCenter, 0.01, nullptr, nullptr, "%.6g");
        ImGui::DragScalar("KG2D ctrl width", ImGuiDataType_Double, &width, 0.001, nullptr, nullptr, "%.6g");
        ImGui::DragScalar("KG2D ctrl amplitude", ImGuiDataType_Double, &amplitude, 0.01, nullptr, nullptr, "%.6g");
        ImGui::Checkbox("KG2D ctrl enabled", &bEnabled);

        if (!bPublish || hub == nullptr) return;

        const auto now = std::chrono::duration<Slab::DevFloat>(
            std::chrono::steady_clock::now().time_since_epoch()).count();

        FControlTimestampV2 stamp;
        stamp.Domain = EControlTimeDomainV2::WallClockTime;
        stamp.WallClockSeconds = now;

        FControlSampleV2 centerSample;
        centerSample.Value = Slab::Math::Real2D{xCenter, yCenter};
        centerSample.Semantic = EControlSemanticV2::Level;
        centerSample.Timestamp = stamp;
        hub->GetOrCreateTopic(topicPrefix + "/forcing/center")->Publish(centerSample);

        FControlSampleV2 amplitudeSample;
        amplitudeSample.Value = amplitude;
        amplitudeSample.Semantic = EControlSemanticV2::Level;
        amplitudeSample.Timestamp = stamp;
        hub->GetOrCreateTopic(topicPrefix + "/forcing/amplitude")->Publish(amplitudeSample);

        FControlSampleV2 widthSample;
        widthSample.Value = std::max<Slab::DevFloat>(width, 1e-9);
        widthSample.Semantic = EControlSemanticV2::Level;
        widthSample.Timestamp = stamp;
        hub->GetOrCreateTopic(topicPrefix + "/forcing/width")->Publish(widthSample);

        FControlSampleV2 enabledSample;
        enabledSample.Value = bEnabled;
        enabledSample.Semantic = EControlSemanticV2::Level;
        enabledSample.Timestamp = stamp;
        hub->GetOrCreateTopic(topicPrefix + "/forcing/enabled")->Publish(enabledSample);
    }

} // namespace

FLabV2WindowManager::FLabV2WindowManager()
: SidePaneWidth(FStudioConfigV2::SidePaneWidth) {
    const auto imGuiModule = Slab::Core::GetModule<Slab::Graphics::FImGuiModule>("ImGui");
    const auto platformWindow = Slab::Graphics::GetGraphicsBackend()->GetMainSystemWindow();

    ImGuiContext = Slab::DynamicPointerCast<Slab::Graphics::FImGuiContext>(imGuiModule->CreateContext(platformWindow));
    ImGuiContext->SetManualRender(true);

    LiveDataHub = Slab::New<Slab::Math::LiveData::V2::FLiveDataHubV2>();
    LiveControlHub = Slab::New<Slab::Math::LiveControl::V2::FLiveControlHubV2>();
    SimulationManager = Slab::New<FSimulationManagerV2>(
        ImGuiContext,
        LiveDataHub,
        LiveControlHub,
        [this](const Slab::TPointer<Slab::Graphics::FSlabWindow> &window) { QueueSlabWindow(window); });

    AddResponder(ImGuiContext);
    AddResponder(SimulationManager);
    LoadWorkspacePanelVisibility(ActiveWorkspace);

    {
        const auto plotWindow = Slab::New<Slab::Graphics::FPlot2DWindow>("Monitor Plot");
        plotWindow->SetAutoReviewGraphRanges(true);
        if (const auto theme = Slab::Graphics::FPlotThemeManager::GetCurrent();
            theme != nullptr && !theme->FuncPlotStyles.empty()) {
            const auto sine = Slab::New<Slab::Math::RtoR::FSine>(1.0, 2.0);
            Slab::Graphics::FPlotter::AddRtoRFunction(
                plotWindow,
                sine,
                theme->FuncPlotStyles.front(),
                "sin(2x)",
                2048);
        }
        AddSlabWindow(plotWindow, false);
    }
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
    bPendingViewRetile = true;
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

auto FLabV2WindowManager::DrawViewManagerPanel() -> void {
    ImGui::SeparatorText("Views");
    if (SlabWindows.empty()) {
        ImGui::TextDisabled("No active view windows.");
        return;
    }

    if (ImGui::Button("Retile Views")) {
        bPendingViewRetile = true;
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

    constexpr auto tableFlags =
        ImGuiTableFlags_Borders |
        ImGuiTableFlags_RowBg |
        ImGuiTableFlags_ScrollX |
        ImGuiTableFlags_SizingFixedFit;

    if (ImGui::BeginTable("LabV2ViewTable", 5, tableFlags)) {
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

auto FLabV2WindowManager::ArrangeTopLevelSlabWindows() -> void {
    if (SlabWindows.empty()) return;

    constexpr int MinTileWidth = 420;

    const int gap = Slab::Graphics::WindowStyle::TilingGapSize;
    const int menuHeight = Slab::Graphics::WindowStyle::GlobalMenuHeight;
    const bool bDockingMode = IsDockingEnabled();
    const int sidePaneInset = bDockingMode ? 0 : SidePaneWidth;
    const int tabsHeight = bDockingMode ? static_cast<int>(std::ceil(WorkspaceTabsHeight)) : 0;

    int xWorkspace = sidePaneInset + gap;
    int yWorkspace = menuHeight + tabsHeight + gap;
    int wWorkspace = std::max(0, WidthSysWin - xWorkspace - gap);
    int hWorkspace = std::max(0, HeightSysWin - yWorkspace - gap);

#ifdef IMGUI_HAS_DOCK
    if (bDockingMode && DockspaceId != 0) {
        if (const auto *centralNode = ImGui::DockBuilderGetCentralNode(static_cast<ImGuiID>(DockspaceId));
            centralNode != nullptr) {
            xWorkspace = static_cast<int>(std::floor(centralNode->Pos.x)) + gap;
            yWorkspace = static_cast<int>(std::floor(centralNode->Pos.y)) + gap;
            wWorkspace = std::max(0, static_cast<int>(std::ceil(centralNode->Size.x)) - 2 * gap);
            hWorkspace = std::max(0, static_cast<int>(std::ceil(centralNode->Size.y)) - 2 * gap);
        }
    }
#endif

    if (wWorkspace <= 0 || hWorkspace <= 0) return;

    const int nWindows = static_cast<int>(SlabWindows.size());

    if (bDockingMode && ActiveWorkspace == EWorkspaceTab::Monitor && nWindows == 1) {
        if (const auto &window = SlabWindows.front(); window != nullptr) {
            window->Set_x(xWorkspace);
            window->Set_y(yWorkspace);
            window->NotifyReshape(wWorkspace, hWorkspace);
        }
        return;
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
}

auto FLabV2WindowManager::IsDockingEnabled() const -> bool {
    if (!bUseDockspaceLayout) return false;
    if (ImGui::GetCurrentContext() == nullptr) return false;
    const auto &io = ImGui::GetIO();
    return (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) != 0;
}

auto FLabV2WindowManager::SaveWorkspacePanelVisibility(const EWorkspaceTab workspace) -> void {
    const auto index = static_cast<std::size_t>(workspace);
    WorkspacePanels[index] = FWorkspacePanelVisibility{
        bShowWindowLab,
        bShowWindowTasks,
        bShowWindowLiveData,
        bShowWindowLiveControl,
        bShowWindowViews,
        bShowWindowKG2DControl,
        bShowWindowBlueprints
    };
}

auto FLabV2WindowManager::LoadWorkspacePanelVisibility(const EWorkspaceTab workspace) -> void {
    const auto index = static_cast<std::size_t>(workspace);
    const auto &cfg = WorkspacePanels[index];
    bShowWindowLab = cfg.bShowWindowLab;
    bShowWindowTasks = cfg.bShowWindowTasks;
    bShowWindowLiveData = cfg.bShowWindowLiveData;
    bShowWindowLiveControl = cfg.bShowWindowLiveControl;
    bShowWindowViews = cfg.bShowWindowViews;
    bShowWindowKG2DControl = cfg.bShowWindowKG2DControl;
    bShowWindowBlueprints = cfg.bShowWindowBlueprints;
}

auto FLabV2WindowManager::SetActiveWorkspace(const EWorkspaceTab workspace) -> void {
    if (ActiveWorkspace == workspace) return;
    SaveWorkspacePanelVisibility(ActiveWorkspace);
    ActiveWorkspace = workspace;
    LoadWorkspacePanelVisibility(ActiveWorkspace);
    bPendingViewRetile = true;
}

auto FLabV2WindowManager::DrawWorkspaceTabs() -> void {
    if (!IsDockingEnabled()) return;

    const auto *viewport = ImGui::GetMainViewport();
    if (viewport == nullptr) return;

    const auto menuHeight = static_cast<float>(Slab::Graphics::WindowStyle::GlobalMenuHeight);
    const float estimatedHeight = ImGui::GetFrameHeightWithSpacing() + 8.0f;

    ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x, viewport->Pos.y + menuHeight), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(viewport->Size.x, estimatedHeight), ImGuiCond_Always);
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

    const auto previousHeight = WorkspaceTabsHeight;
    WorkspaceTabsHeight = estimatedHeight;
    if (ImGui::Begin("##LabV2WorkspaceTabs", nullptr, flags)) {
        auto selected = ActiveWorkspace;
        if (ImGui::BeginTabBar("##WorkspaceTabBar",
            ImGuiTabBarFlags_FittingPolicyResizeDown | ImGuiTabBarFlags_NoCloseWithMiddleMouseButton)) {

            const auto drawTab = [&selected](const EWorkspaceTab workspace, const char *label) {
                const auto tabFlags =
                    selected == workspace ? ImGuiTabItemFlags_SetSelected : ImGuiTabItemFlags_None;
                if (ImGui::BeginTabItem(label, nullptr, tabFlags)) {
                    selected = workspace;
                    ImGui::EndTabItem();
                }
            };

            drawTab(EWorkspaceTab::Simulations, WorkspaceTabSimulations);
            drawTab(EWorkspaceTab::Monitor, WorkspaceTabMonitor);
            drawTab(EWorkspaceTab::Schemes, WorkspaceTabSchemes);

            ImGui::EndTabBar();
        }

        if (selected != ActiveWorkspace) SetActiveWorkspace(selected);
    }
    ImGui::End();
    ImGui::PopStyleVar(3);

    if (std::abs(previousHeight - WorkspaceTabsHeight) > 0.5f) {
        bPendingViewRetile = true;
    }
}

auto FLabV2WindowManager::BuildDefaultDockLayout(const unsigned int dockspaceId,
                                                 const EWorkspaceTab workspace) -> void {
#ifdef IMGUI_HAS_DOCK
    const auto dockId = static_cast<ImGuiID>(dockspaceId);
    if (dockId == 0) return;

    const auto *viewport = ImGui::GetMainViewport();
    if (viewport == nullptr) return;

    const auto menuHeight = static_cast<float>(Slab::Graphics::WindowStyle::GlobalMenuHeight);
    const auto dockSize = ImVec2(
        viewport->Size.x,
        std::max(0.0f, viewport->Size.y - menuHeight - WorkspaceTabsHeight));

    ImGui::DockBuilderRemoveNode(dockId);
    ImGui::DockBuilderAddNode(dockId, ImGuiDockNodeFlags_DockSpace | ImGuiDockNodeFlags_PassthruCentralNode);
    ImGui::DockBuilderSetNodeSize(dockId, dockSize);

    ImGuiID dockMain = dockId;
    if (workspace == EWorkspaceTab::Simulations) {
        const auto dockLeft = ImGui::DockBuilderSplitNode(dockMain, ImGuiDir_Left, 0.24f, nullptr, &dockMain);
        const auto dockRight = ImGui::DockBuilderSplitNode(dockMain, ImGuiDir_Right, 0.30f, nullptr, &dockMain);
        const auto dockLeftBottom = ImGui::DockBuilderSplitNode(dockLeft, ImGuiDir_Down, 0.42f, nullptr, nullptr);

        ImGui::DockBuilderDockWindow(WindowTitleSimulationLauncher, dockLeft);
        ImGui::DockBuilderDockWindow(WindowTitleTasks, dockLeftBottom);
        ImGui::DockBuilderDockWindow(WindowTitleLiveData, dockRight);
        ImGui::DockBuilderDockWindow(WindowTitleLiveControl, dockRight);
        ImGui::DockBuilderDockWindow(WindowTitleKG2DControl, dockRight);
        ImGui::DockBuilderDockWindow(WindowTitleLab, dockRight);
        ImGui::DockBuilderDockWindow(WindowTitleViews, dockMain);
    } else if (workspace == EWorkspaceTab::Monitor) {
        const auto dockLeft = ImGui::DockBuilderSplitNode(dockMain, ImGuiDir_Left, 0.22f, nullptr, &dockMain);
        const auto dockRight = ImGui::DockBuilderSplitNode(dockMain, ImGuiDir_Right, 0.26f, nullptr, &dockMain);
        const auto dockBottom = ImGui::DockBuilderSplitNode(dockMain, ImGuiDir_Down, 0.28f, nullptr, &dockMain);

        ImGui::DockBuilderDockWindow(WindowTitleViews, dockLeft);
        ImGui::DockBuilderDockWindow(WindowTitleLiveData, dockBottom);
        ImGui::DockBuilderDockWindow(WindowTitleLiveControl, dockBottom);
        ImGui::DockBuilderDockWindow(WindowTitleKG2DControl, dockRight);
        ImGui::DockBuilderDockWindow(WindowTitleLab, dockRight);
    } else {
        ImGui::DockBuilderDockWindow(WindowTitleBlueprints, dockMain);
    }

    ImGui::DockBuilderFinish(dockId);
#else
    (void) dockspaceId;
    (void) workspace;
#endif
}

auto FLabV2WindowManager::DrawDockspaceHost() -> void {
    if (!IsDockingEnabled()) return;

    const auto menuHeight = static_cast<float>(Slab::Graphics::WindowStyle::GlobalMenuHeight);
    const auto *viewport = ImGui::GetMainViewport();
    if (viewport == nullptr) return;

    ImGui::SetNextWindowPos(
        ImVec2(viewport->Pos.x, viewport->Pos.y + menuHeight + WorkspaceTabsHeight),
        ImGuiCond_Always);
    ImGui::SetNextWindowSize(
        ImVec2(viewport->Size.x, std::max(0.0f, viewport->Size.y - menuHeight - WorkspaceTabsHeight)),
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
        const char *dockspaceName = DockspaceNameSimulations;
        if (ActiveWorkspace == EWorkspaceTab::Monitor) dockspaceName = DockspaceNameMonitor;
        else if (ActiveWorkspace == EWorkspaceTab::Schemes) dockspaceName = DockspaceNameSchemes;

        DockspaceId = static_cast<unsigned int>(ImGui::GetID(dockspaceName));
        const auto workspaceIndex = static_cast<std::size_t>(ActiveWorkspace);
        if (!WorkspaceLayoutInitialized[workspaceIndex] || bResetDockLayoutRequested) {
            BuildDefaultDockLayout(DockspaceId, ActiveWorkspace);
            WorkspaceLayoutInitialized[workspaceIndex] = true;
            bResetDockLayoutRequested = false;
            bPendingViewRetile = true;
        }
#ifdef IMGUI_HAS_DOCK
        ImGui::DockSpace(
            static_cast<ImGuiID>(DockspaceId),
            ImVec2(0.0f, 0.0f),
            ImGuiDockNodeFlags_PassthruCentralNode);
#endif
    }
    ImGui::End();

    ImGui::PopStyleVar(3);
}

auto FLabV2WindowManager::DrawDockedToolWindows() -> void {
    if (ActiveWorkspace == EWorkspaceTab::Schemes) {
        if (ImGui::Begin(WindowTitleBlueprints)) {
            ImGui::TextDisabled("Blueprints demo placeholder");
            ImGui::Separator();
            ImGui::TextWrapped(
                "This workspace is reserved for node/scheme authoring. "
                "A full Blueprint renderer can be attached here incrementally.");
        }
        ImGui::End();
        SaveWorkspacePanelVisibility(ActiveWorkspace);
        return;
    }

    if (bShowWindowLab) {
        if (ImGui::Begin(WindowTitleLab, &bShowWindowLab)) {
            ImGui::TextDisabled("V2 observability + launcher shell");
            ImGui::SeparatorText("Workspace");
            ImGui::Checkbox("Enable dockspace layout", &bUseDockspaceLayout);
            if (ImGui::Button("Reset Dock Layout")) {
                bResetDockLayoutRequested = true;
            }
            ImGui::SameLine();
            if (ImGui::Button("Retile Views")) {
                bPendingViewRetile = true;
            }

            ImGui::SeparatorText("Panels");
            ImGui::Checkbox("Tasks", &bShowWindowTasks);
            ImGui::Checkbox("Live Data", &bShowWindowLiveData);
            ImGui::Checkbox("Live Control", &bShowWindowLiveControl);
            ImGui::Checkbox("Views", &bShowWindowViews);
            ImGui::Checkbox("KG2D Control", &bShowWindowKG2DControl);
            ImGui::Checkbox("Blueprints", &bShowWindowBlueprints);
        }
        ImGui::End();
    }

    if (bShowWindowTasks) {
        if (ImGui::Begin(WindowTitleTasks, &bShowWindowTasks)) {
            ShowTasksPanel(TaskNameFilter, bTaskOnlyRunning, bTaskHideSuccess, bTaskOnlyNumeric);
        }
        ImGui::End();
    }

    if (bShowWindowLiveData) {
        if (ImGui::Begin(WindowTitleLiveData, &bShowWindowLiveData)) {
            ShowLiveDataV2Panel(LiveDataHub, LiveDataTopicFilter, bLiveDataOnlyBound, SelectedLiveDataTopic);
        }
        ImGui::End();
    }

    if (bShowWindowLiveControl) {
        if (ImGui::Begin(WindowTitleLiveControl, &bShowWindowLiveControl)) {
            ShowLiveControlV2Panel(
                LiveControlHub,
                LiveControlTopicFilter,
                bLiveControlLevelsOnly,
                SelectedLiveControlTopic);
        }
        ImGui::End();
    }

    if (bShowWindowViews) {
        if (ImGui::Begin(WindowTitleViews, &bShowWindowViews)) {
            DrawViewManagerPanel();
        }
        ImGui::End();
    }

    if (bShowWindowKG2DControl) {
        if (ImGui::Begin(WindowTitleKG2DControl, &bShowWindowKG2DControl)) {
            ShowKG2DControlSourcePanelAndPublish(
                LiveControlHub,
                bPublishKG2DControlSource,
                KG2DControlX,
                KG2DControlY,
                KG2DControlWidth,
                KG2DControlAmplitude,
                bKG2DControlEnabled,
                KG2DControlTopicPrefix);
        }
        ImGui::End();
    }

    if (bShowWindowBlueprints) {
        if (ImGui::Begin(WindowTitleBlueprints, &bShowWindowBlueprints)) {
            ImGui::TextDisabled("Blueprints demo placeholder");
            ImGui::Separator();
            ImGui::TextWrapped(
                "This workspace is reserved for node/scheme authoring. "
                "A full Blueprint renderer can be attached here incrementally.");
        }
        ImGui::End();
    }

    SaveWorkspacePanelVisibility(ActiveWorkspace);
}

auto FLabV2WindowManager::DrawLegacySidePane() -> void {
    const auto menuHeight = Slab::Graphics::WindowStyle::GlobalMenuHeight;
    ImGui::SetNextWindowPos(ImVec2(0, static_cast<float>(menuHeight)));
    ImGui::SetNextWindowSize(
        ImVec2(static_cast<float>(SidePaneWidth), static_cast<float>(HeightSysWin - menuHeight)),
        ImGuiCond_Appearing);

    constexpr auto flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar;
    if (ImGui::Begin(FStudioConfigV2::SidePaneId, nullptr, flags)) {
        ImGui::SeparatorText("Lab V2");
        ImGui::TextDisabled("V2 observability + launcher shell");

        ShowTasksPanel(TaskNameFilter, bTaskOnlyRunning, bTaskHideSuccess, bTaskOnlyNumeric);
        ShowLiveDataV2Panel(LiveDataHub, LiveDataTopicFilter, bLiveDataOnlyBound, SelectedLiveDataTopic);
        ShowLiveControlV2Panel(LiveControlHub, LiveControlTopicFilter, bLiveControlLevelsOnly, SelectedLiveControlTopic);
        DrawViewManagerPanel();
        ShowKG2DControlSourcePanelAndPublish(
            LiveControlHub,
            bPublishKG2DControlSource,
            KG2DControlX,
            KG2DControlY,
            KG2DControlWidth,
            KG2DControlAmplitude,
            bKG2DControlEnabled,
            KG2DControlTopicPrefix);

        if (const auto windowWidth = static_cast<int>(ImGui::GetWindowWidth()); SidePaneWidth != windowWidth) {
            SidePaneWidth = windowWidth;
            NotifySystemWindowReshape(WidthSysWin, HeightSysWin);
        }
    }
    ImGui::End();
}

bool FLabV2WindowManager::NotifyRender(const Slab::Graphics::FPlatformWindow &platformWindow) {
    FlushPendingSlabWindows();

    platformWindow.Clear(Slab::Graphics::WindowStyle::PlatformWindow_BackgroundColor);

    ImGuiContext->NewFrame();
    ImGuiContext->SetupOptionalMenuItems();

    if (IsDockingEnabled()) {
        DrawWorkspaceTabs();
        DrawDockspaceHost();
        DrawDockedToolWindows();
    } else {
        WorkspaceTabsHeight = 0.0f;
        DrawLegacySidePane();
    }

    AddExitMenuEntry(platformWindow, *ImGuiContext);

    if (PruneClosedSlabWindows()) {
        bPendingViewRetile = true;
    }

    if (bPendingViewRetile) {
        ArrangeTopLevelSlabWindows();
        bPendingViewRetile = false;
    }

    FWindowManager::NotifyRender(platformWindow);
    FlushPendingSlabWindows();
    ImGuiContext->Render();
    return true;
}

bool FLabV2WindowManager::NotifySystemWindowReshape(const int w, const int h) {
    WidthSysWin = w;
    HeightSysWin = h;
    const auto responded = FWindowManager::NotifySystemWindowReshape(w, h);
    bPendingViewRetile = true;
    return responded;
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
