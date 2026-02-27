#include "LabV2WindowManager.h"

#include "SimulationManagerV2.h"
#include "StudioConfigV2.h"

#include "imgui.h"

#include "Core/SlabCore.h"
#include "Core/Backend/Modules/TaskManager/TaskManager.h"
#include "Graphics/Modules/ImGui/ImGuiModule.h"
#include "Graphics/SlabGraphics.h"
#include "Graphics/Window/WindowStyles.h"
#include "Math/Data/V2/SessionLiveViewV2.h"
#include "Math/Data/V2/LiveControlTopicsV2.h"
#include "Math/Numerics/NumericTask.h"
#include "Math/Numerics/V2/Task/NumericTaskV2.h"

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

    auto ShowTasksPanel() -> void {
        const auto taskManager = Slab::Core::GetModule<Slab::Core::FTaskManager>("TaskManager");
        if (taskManager == nullptr) return;

        const auto jobs = taskManager->GetAllJobs();
        if (jobs.empty()) {
            ImGui::TextDisabled("No tasks.");
            return;
        }

        ImGui::SeparatorText("Tasks");

        for (auto &job : jobs) {
            auto &[task, jobThread] = job;
            (void) jobThread;

            ImGui::Text("%s", task->GetName().c_str());
            ImGui::SameLine();

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
                ImGui::SameLine();
                if (ImGui::SmallButton(("Clear##" + task->GetName()).c_str())) {
                    taskManager->ClearJob(job);
                }
                break;
            case Slab::Core::TaskNotInitialized:
                ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.2f, 1), "NotInitialized");
                break;
            default:
                ImGui::TextColored(ImVec4(1, 0, 0, 1), "Unknown");
                break;
            }

            if (task->GetStatus() == Slab::Core::TaskRunning) {
                if (const auto *numericTaskV2 =
                        dynamic_cast<Slab::Math::Numerics::V2::FNumericTaskV2 *>(task.get())) {
                    const auto progressOpt = numericTaskV2->GetProgress01();
                    if (progressOpt.has_value()) {
                        ImGui::ProgressBar(*progressOpt, ImVec2(-1.0f, 0.0f));
                    } else {
                        const auto cursor = numericTaskV2->GetCursor();
                        ImGui::TextDisabled("open-ended | step=%llu",
                                            static_cast<unsigned long long>(cursor.Step));
                    }
                } else if (const auto *legacyNumericTask =
                               dynamic_cast<Slab::Math::FNumericTask *>(task.get())) {
                    ImGui::ProgressBar(legacyNumericTask->GetProgress(), ImVec2(-1.0f, 0.0f));
                }
            }
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
        ArrangeTopLevelSlabWindows();
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

    const int xWorkspace = SidePaneWidth + gap;
    const int yWorkspace = menuHeight + gap;
    const int wWorkspace = std::max(0, WidthSysWin - xWorkspace - gap);
    const int hWorkspace = std::max(0, HeightSysWin - yWorkspace - gap);

    if (wWorkspace <= 0 || hWorkspace <= 0) return;

    const int nWindows = static_cast<int>(SlabWindows.size());

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

bool FLabV2WindowManager::NotifyRender(const Slab::Graphics::FPlatformWindow &platformWindow) {
    FlushPendingSlabWindows();

    platformWindow.Clear(Slab::Graphics::WindowStyle::PlatformWindow_BackgroundColor);

    ImGuiContext->NewFrame();
    ImGuiContext->SetupOptionalMenuItems();

    const auto menuHeight = Slab::Graphics::WindowStyle::GlobalMenuHeight;
    ImGui::SetNextWindowPos(ImVec2(0, static_cast<float>(menuHeight)));
    ImGui::SetNextWindowSize(
        ImVec2(static_cast<float>(SidePaneWidth), static_cast<float>(HeightSysWin - menuHeight)),
        ImGuiCond_Appearing);

    constexpr auto flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar;
    if (ImGui::Begin(FStudioConfigV2::SidePaneId, nullptr, flags)) {
        ImGui::SeparatorText("Lab V2");
        ImGui::TextDisabled("V2 observability + launcher shell");

        ShowTasksPanel();
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

    AddExitMenuEntry(platformWindow, *ImGuiContext);

    if (PruneClosedSlabWindows()) {
        ArrangeTopLevelSlabWindows();
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
    ArrangeTopLevelSlabWindows();
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
