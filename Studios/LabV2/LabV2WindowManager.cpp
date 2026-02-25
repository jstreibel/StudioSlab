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
#include "Math/Numerics/NumericTask.h"
#include "Math/Numerics/V2/Task/NumericTaskV2.h"

#include <cmath>
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

    auto ShowLiveDataV2Panel(const Slab::TPointer<Slab::Math::LiveData::V2::FLiveDataHubV2> &hub) -> void {
        ImGui::SeparatorText("Live Data V2");

        if (hub == nullptr) {
            ImGui::TextColored(ImVec4(1, 0, 0, 1), "LiveDataHubV2 unavailable.");
            return;
        }

        const auto names = hub->ListSessionLiveViewTopics();
        if (names.empty()) {
            ImGui::TextDisabled("No session streams registered.");
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

            for (const auto &name : names) {
                const auto view = hub->FindSessionLiveView(name);
                const auto telemetry = view != nullptr ? view->TryGetTelemetry() : std::nullopt;
                const auto status = view != nullptr ? view->TryGetStatus() : std::nullopt;
                const bool bBound = view != nullptr && view->HasBoundSession();

                ImGui::TableNextRow();

                ImGui::TableSetColumnIndex(0);
                ImGui::Text("%s", name.c_str());

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
    }

} // namespace

FLabV2WindowManager::FLabV2WindowManager()
: SidePaneWidth(FStudioConfigV2::SidePaneWidth) {
    const auto imGuiModule = Slab::Core::GetModule<Slab::Graphics::FImGuiModule>("ImGui");
    const auto platformWindow = Slab::Graphics::GetGraphicsBackend()->GetMainSystemWindow();

    ImGuiContext = Slab::DynamicPointerCast<Slab::Graphics::FImGuiContext>(imGuiModule->CreateContext(platformWindow));
    ImGuiContext->SetManualRender(true);

    LiveDataHub = Slab::New<Slab::Math::LiveData::V2::FLiveDataHubV2>();
    SimulationManager = Slab::New<FSimulationManagerV2>(
        ImGuiContext,
        LiveDataHub,
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
        ShowLiveDataV2Panel(LiveDataHub);

        if (const auto windowWidth = static_cast<int>(ImGui::GetWindowWidth()); SidePaneWidth != windowWidth) {
            SidePaneWidth = windowWidth;
            NotifySystemWindowReshape(WidthSysWin, HeightSysWin);
        }
    }
    ImGui::End();

    AddExitMenuEntry(platformWindow, *ImGuiContext);

    for (const auto &window : SlabWindows) {
        if (window != nullptr && window->WantsClose()) {
            RemoveResponder(window);
            std::erase(SlabWindows, window);
        }
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
