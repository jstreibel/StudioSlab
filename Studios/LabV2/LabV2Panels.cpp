#include "LabV2Panels.h"

#include "imgui.h"

#include "Core/Backend/Modules/TaskManager/TaskManager.h"

#include "Math/Data/V2/LiveControlTopicsV2.h"
#include "Math/Data/V2/SessionLiveViewV2.h"
#include "Math/Numerics/NumericTask.h"
#include "Math/Numerics/V2/Task/NumericTaskV2.h"

#include "Studios/Common/Simulations/V2/KGR2toRControlTopicsV2.h"

#include <algorithm>
#include <cctype>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <variant>

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
        if (ImGui::InputTextWithHint(label, hint, buffer, sizeof(buffer))) {
            value = buffer;
        }
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
            return timestamp.WallClockSeconds.has_value() ? Slab::ToStr(*timestamp.WallClockSeconds, 6, true) : "-";
        case EDomain::SimulationTime:
            return timestamp.SimulationSeconds.has_value() ? Slab::ToStr(*timestamp.SimulationSeconds, 6, true) : "-";
        case EDomain::StepIndex:
            return timestamp.Step.has_value() ? Slab::ToStr(*timestamp.Step) : "-";
        }
        return "-";
    }

    enum class ETaskKind {
        Generic,
        NumericLegacy,
        NumericV2
    };

    struct FTaskInspection {
        const Slab::Core::FTask *Task = nullptr;
        const Slab::Math::FNumericTask *LegacyNumeric = nullptr;
        const Slab::Math::Numerics::V2::FNumericTaskV2 *NumericV2 = nullptr;

        [[nodiscard]] auto GetKind() const -> ETaskKind {
            if (NumericV2 != nullptr) return ETaskKind::NumericV2;
            if (LegacyNumeric != nullptr) return ETaskKind::NumericLegacy;
            return ETaskKind::Generic;
        }

        [[nodiscard]] auto IsNumeric() const -> bool {
            return GetKind() != ETaskKind::Generic;
        }
    };

    [[nodiscard]] auto InspectTask(const Slab::Core::FTask *task) -> FTaskInspection {
        FTaskInspection inspection;
        inspection.Task = task;
        if (task == nullptr) return inspection;

        // Keep legacy/V2 detection in one place so the panel logic stays type-agnostic.
        inspection.NumericV2 = dynamic_cast<const Slab::Math::Numerics::V2::FNumericTaskV2 *>(task);
        if (inspection.NumericV2 == nullptr) {
            inspection.LegacyNumeric = dynamic_cast<const Slab::Math::FNumericTask *>(task);
        }
        return inspection;
    }

    auto ToTaskKindLabel(const ETaskKind taskKind) -> const char * {
        switch (taskKind) {
        case ETaskKind::NumericV2: return "NumericV2";
        case ETaskKind::NumericLegacy: return "NumericLegacy";
        case ETaskKind::Generic: return "Task";
        }
        return "Task";
    }

} // namespace

namespace Slab::Studios::LabV2::Panels {

    auto ShowTasksPanel(Str &nameFilter,
                        bool &bOnlyRunning,
                        bool &bHideSuccess,
                        bool &bOnlyNumeric) -> void {
        const auto taskManager = Core::GetModule<Core::FTaskManager>("TaskManager");
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
            case Core::TaskRunning: ++runningCount; break;
            case Core::TaskSuccess: ++successCount; break;
            case Core::TaskError: ++errorCount; break;
            case Core::TaskAborted: ++abortedCount; break;
            case Core::TaskNotInitialized: ++notInitCount; break;
            default: break;
            }
        }

        ImGui::Text("total=%zu running=%zu success=%zu error=%zu aborted=%zu not-init=%zu",
            jobs.size(), runningCount, successCount, errorCount, abortedCount, notInitCount);

        if (ImGui::SmallButton("Clear Success")) {
            for (const auto &job : jobs) {
                if (job.Task != nullptr && job.Task->GetStatus() == Core::TaskSuccess) {
                    (void) taskManager->ClearJob(job);
                }
            }
        }
        ImGui::SameLine();
        if (ImGui::SmallButton("Clear Terminated")) {
            for (const auto &job : jobs) {
                if (job.Task == nullptr) continue;
                const auto status = job.Task->GetStatus();
                if (status == Core::TaskSuccess || status == Core::TaskError || status == Core::TaskAborted) {
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

            const auto inspection = InspectTask(task.get());
            if (bOnlyNumeric && !inspection.IsNumeric()) continue;
            if (bOnlyRunning && task->GetStatus() != Core::TaskRunning) continue;
            if (bHideSuccess && task->GetStatus() == Core::TaskSuccess) continue;

            ImGui::TableNextRow();
            ImGui::PushID(task.get());

            ImGui::TableSetColumnIndex(0);
            ImGui::TextUnformatted(taskName.c_str());

            ImGui::TableSetColumnIndex(1);
            ImGui::TextUnformatted(ToTaskKindLabel(inspection.GetKind()));

            ImGui::TableSetColumnIndex(2);
            switch (task->GetStatus()) {
            case Core::TaskRunning:
                ImGui::TextColored(ImVec4(0.2f, 0.6f, 1.0f, 1), "Running");
                break;
            case Core::TaskAborted:
                ImGui::TextColored(ImVec4(0.9f, 0.5f, 0.1f, 1), "Aborted");
                break;
            case Core::TaskError:
                ImGui::TextColored(ImVec4(1.0f, 0.2f, 0.7f, 1), "Error");
                break;
            case Core::TaskSuccess:
                ImGui::TextColored(ImVec4(0.2f, 0.9f, 0.2f, 1), "Success");
                break;
            case Core::TaskNotInitialized:
                ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.2f, 1), "NotInitialized");
                break;
            default:
                ImGui::TextColored(ImVec4(1, 0, 0, 1), "Unknown");
                break;
            }

            ImGui::TableSetColumnIndex(3);
            if (inspection.NumericV2 != nullptr) {
                const auto progressOpt = inspection.NumericV2->GetProgress01();
                if (progressOpt.has_value()) {
                    ImGui::ProgressBar(*progressOpt, ImVec2(110.0f, 0.0f));
                } else {
                    ImGui::TextDisabled("open");
                }
            } else if (inspection.LegacyNumeric != nullptr) {
                ImGui::ProgressBar(inspection.LegacyNumeric->GetProgress(), ImVec2(110.0f, 0.0f));
            } else {
                ImGui::TextDisabled("-");
            }

            ImGui::TableSetColumnIndex(4);
            if (inspection.NumericV2 != nullptr) {
                const auto cursor = inspection.NumericV2->GetCursor();
                if (cursor.SimulationTime.has_value()) {
                    ImGui::Text("s=%llu t=%.4g",
                        static_cast<unsigned long long>(cursor.Step),
                        *cursor.SimulationTime);
                } else {
                    ImGui::Text("s=%llu", static_cast<unsigned long long>(cursor.Step));
                }
            } else {
                ImGui::TextDisabled("-");
            }

            ImGui::TableSetColumnIndex(5);
            if (task->GetStatus() != Core::TaskRunning) {
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

    auto ShowLiveDataV2Panel(const TPointer<Math::LiveData::V2::FLiveDataHubV2> &hub,
                             Str &topicFilter,
                             bool &bOnlyBound,
                             Str &selectedTopic) -> void {
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

        Vector<Str> filteredNames;
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
                        status->RunState == Math::LiveData::V2::ESessionRunStateV2::Running
                        ? ImVec4(0.2f, 0.9f, 0.2f, 1.0f)
                        : status->RunState == Math::LiveData::V2::ESessionRunStateV2::Finished
                            ? ImVec4(0.5f, 0.9f, 1.0f, 1.0f)
                            : ImVec4(1.0f, 0.6f, 0.2f, 1.0f);
                    ImGui::TextColored(color, "%s", runString.c_str());
                } else {
                    ImGui::TextDisabled("n/a");
                }

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

    auto ShowLiveControlV2Panel(const TPointer<Math::LiveControl::V2::FLiveControlHubV2> &hub,
                                Str &topicFilter,
                                bool &bLevelsOnly,
                                Str &selectedTopic) -> void {
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

        Vector<Str> filteredTopics;
        filteredTopics.reserve(topicNames.size());
        for (const auto &topicName : topicNames) {
            if (!MatchesTopicFilter(topicName, topicFilter)) continue;
            const auto topic = hub->FindTopic(topicName);
            const auto sample = topic != nullptr ? topic->TryGetLatestSample() : std::nullopt;
            if (bLevelsOnly && sample.has_value() && sample->Semantic != Math::LiveControl::V2::EControlSemanticV2::Level) {
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
        const TPointer<Math::LiveControl::V2::FLiveControlHubV2> &hub,
        bool &bPublish,
        DevFloat &xCenter,
        DevFloat &yCenter,
        DevFloat &width,
        DevFloat &amplitude,
        bool &bEnabled,
        const Str &topicPrefix) -> void {
        using namespace Math::LiveControl::V2;
        using namespace Slab::Studios::Common::Simulations::V2;

        ImGui::SeparatorText("KG2D Control Source");
        ImGui::Checkbox("Publish KG2D forcing controls", &bPublish);
        ImGui::TextDisabled("Topic prefix: %s", topicPrefix.c_str());

        ImGui::DragScalar("KG2D ctrl x", ImGuiDataType_Double, &xCenter, 0.01, nullptr, nullptr, "%.6g");
        ImGui::DragScalar("KG2D ctrl y", ImGuiDataType_Double, &yCenter, 0.01, nullptr, nullptr, "%.6g");
        ImGui::DragScalar("KG2D ctrl width", ImGuiDataType_Double, &width, 0.001, nullptr, nullptr, "%.6g");
        ImGui::DragScalar("KG2D ctrl amplitude", ImGuiDataType_Double, &amplitude, 0.01, nullptr, nullptr, "%.6g");
        ImGui::Checkbox("KG2D ctrl enabled", &bEnabled);

        if (!bPublish || hub == nullptr) return;

        const auto now = std::chrono::duration<DevFloat>(
            std::chrono::steady_clock::now().time_since_epoch()).count();

        FControlTimestampV2 stamp;
        stamp.Domain = EControlTimeDomainV2::WallClockTime;
        stamp.WallClockSeconds = now;

        const auto topics = BuildKG2DForcingTopicNamesV2(topicPrefix);

        FControlSampleV2 centerSample;
        centerSample.Value = Math::Real2D{xCenter, yCenter};
        centerSample.Semantic = EControlSemanticV2::Level;
        centerSample.Timestamp = stamp;
        hub->GetOrCreateTopic(topics.Center)->Publish(centerSample);

        FControlSampleV2 amplitudeSample;
        amplitudeSample.Value = amplitude;
        amplitudeSample.Semantic = EControlSemanticV2::Level;
        amplitudeSample.Timestamp = stamp;
        hub->GetOrCreateTopic(topics.Amplitude)->Publish(amplitudeSample);

        FControlSampleV2 widthSample;
        widthSample.Value = std::max<DevFloat>(width, 1e-9);
        widthSample.Semantic = EControlSemanticV2::Level;
        widthSample.Timestamp = stamp;
        hub->GetOrCreateTopic(topics.Width)->Publish(widthSample);

        FControlSampleV2 enabledSample;
        enabledSample.Value = bEnabled;
        enabledSample.Semantic = EControlSemanticV2::Level;
        enabledSample.Timestamp = stamp;
        hub->GetOrCreateTopic(topics.Enabled)->Publish(enabledSample);
    }

} // namespace Slab::Studios::LabV2::Panels
