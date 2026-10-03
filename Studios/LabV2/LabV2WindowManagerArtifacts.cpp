#include "LabV2WindowManager.h"

#include "Core/Model/V2/ModelArtifactV2.h"
#include "imgui.h"

#include <algorithm>
#include <array>
#include <cfloat>
#include <chrono>
#include <cstdio>
#include <filesystem>
#include <vector>

namespace {

namespace ArtifactV2 = Slab::Core::Artifacts::V2;
namespace ModelV2 = Slab::Core::Model::V2;
namespace NumericsV2 = Slab::Math::Numerics::V2;

struct FArtifactSeriesView {
    ArtifactV2::EArtifactRoleV2 Role = ArtifactV2::EArtifactRoleV2::State;
    Slab::Str DefinitionId;
    Slab::Str DisplayLabel;
    Slab::Str CanonicalNotation;
    Slab::Vector<ArtifactV2::FArtifactSampleV2> Samples;
};

[[nodiscard]] auto UtcNowUnixNanoseconds() -> std::int64_t {
    return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch())
        .count();
}

[[nodiscard]] auto IsTerminalTaskStatus(const Slab::Core::ETaskStatus status) -> bool {
    return status == Slab::Core::TaskSuccess || status == Slab::Core::TaskError || status == Slab::Core::TaskAborted;
}

[[nodiscard]] auto TaskStatusLabel(const Slab::Core::ETaskStatus status) -> const char* {
    switch (status) {
    case Slab::Core::TaskRunning:
        return "Running";
    case Slab::Core::TaskSuccess:
        return "Success";
    case Slab::Core::TaskError:
        return "Error";
    case Slab::Core::TaskAborted:
        return "Aborted";
    case Slab::Core::TaskNotInitialized:
        return "NotInitialized";
    }
    return "Unknown";
}

[[nodiscard]] auto TaskStatusColor(const Slab::Core::ETaskStatus status) -> ImVec4 {
    switch (status) {
    case Slab::Core::TaskSuccess:
        return ImVec4(0.45f, 0.85f, 0.54f, 1.0f);
    case Slab::Core::TaskError:
        return ImVec4(0.95f, 0.42f, 0.38f, 1.0f);
    case Slab::Core::TaskAborted:
        return ImVec4(0.95f, 0.70f, 0.34f, 1.0f);
    case Slab::Core::TaskRunning:
        return ImVec4(0.42f, 0.72f, 0.95f, 1.0f);
    case Slab::Core::TaskNotInitialized:
        return ImVec4(0.65f, 0.65f, 0.65f, 1.0f);
    }
    return ImVec4(0.65f, 0.65f, 0.65f, 1.0f);
}

[[nodiscard]] auto RunStatusLabel(const ArtifactV2::EArtifactRunStatusV2 status) -> const char* {
    switch (status) {
    case ArtifactV2::EArtifactRunStatusV2::Success:
        return "Success";
    case ArtifactV2::EArtifactRunStatusV2::Error:
        return "Error";
    case ArtifactV2::EArtifactRunStatusV2::Aborted:
        return "Aborted";
    }
    return "Unknown";
}

[[nodiscard]] auto RunStatusColor(const ArtifactV2::EArtifactRunStatusV2 status) -> ImVec4 {
    switch (status) {
    case ArtifactV2::EArtifactRunStatusV2::Success:
        return ImVec4(0.45f, 0.85f, 0.54f, 1.0f);
    case ArtifactV2::EArtifactRunStatusV2::Error:
        return ImVec4(0.95f, 0.42f, 0.38f, 1.0f);
    case ArtifactV2::EArtifactRunStatusV2::Aborted:
        return ImVec4(0.95f, 0.70f, 0.34f, 1.0f);
    }
    return ImVec4(0.65f, 0.65f, 0.65f, 1.0f);
}

[[nodiscard]] auto EventReasonLabel(const ArtifactV2::EArtifactEventReasonV2 reason) -> const char* {
    switch (reason) {
    case ArtifactV2::EArtifactEventReasonV2::Initial:
        return "Initial";
    case ArtifactV2::EArtifactEventReasonV2::Scheduled:
        return "Scheduled";
    case ArtifactV2::EArtifactEventReasonV2::Forced:
        return "Forced";
    case ArtifactV2::EArtifactEventReasonV2::Final:
        return "Final";
    case ArtifactV2::EArtifactEventReasonV2::AbortFinal:
        return "AbortFinal";
    }
    return "Unknown";
}

[[nodiscard]] auto ConvertEventReason(const NumericsV2::EEventReasonV2 reason) -> ArtifactV2::EArtifactEventReasonV2 {
    switch (reason) {
    case NumericsV2::EEventReasonV2::Initial:
        return ArtifactV2::EArtifactEventReasonV2::Initial;
    case NumericsV2::EEventReasonV2::Scheduled:
        return ArtifactV2::EArtifactEventReasonV2::Scheduled;
    case NumericsV2::EEventReasonV2::Forced:
        return ArtifactV2::EArtifactEventReasonV2::Forced;
    case NumericsV2::EEventReasonV2::Final:
        return ArtifactV2::EArtifactEventReasonV2::Final;
    case NumericsV2::EEventReasonV2::AbortFinal:
        return ArtifactV2::EArtifactEventReasonV2::AbortFinal;
    }
    return ArtifactV2::EArtifactEventReasonV2::Scheduled;
}

[[nodiscard]] auto FormatScalar(const Slab::DevFloat value) -> Slab::Str {
    char buffer[64];
    std::snprintf(buffer, sizeof(buffer), "%.6g", value);
    return buffer;
}

[[nodiscard]] auto NormalizeArtifactPath(const std::filesystem::path& path) -> Slab::Str {
    std::error_code error;
    auto absolutePath = std::filesystem::absolute(path, error);
    if (error)
        absolutePath = path;
    return absolutePath.lexically_normal().string();
}

auto AppendLiveSeries(Slab::Vector<FArtifactSeriesView>& views,
                      const Slab::Vector<ModelV2::FODETimeSeriesArtifactV2>& artifacts,
                      const ArtifactV2::EArtifactRoleV2 role) -> void {
    for (const auto& artifact : artifacts) {
        FArtifactSeriesView view;
        view.Role = role;
        view.DefinitionId = artifact.DefinitionId;
        view.DisplayLabel = artifact.DisplayLabel;
        view.CanonicalNotation = artifact.CanonicalNotation;
        if (artifact.Listener != nullptr) {
            const auto samples = artifact.Listener->GetSamples();
            view.Samples.reserve(samples.size());
            for (const auto& sample : samples) {
                view.Samples.push_back({.Step = sample.Cursor.Step,
                                        .SimulationTime = sample.Cursor.SimulationTime,
                                        .WallClockSeconds = sample.Cursor.WallClockSeconds,
                                        .Value = sample.Value,
                                        .Reason = ConvertEventReason(sample.Reason),
                                        .PublishedVersion = sample.PublishedVersion});
            }
        }
        views.push_back(std::move(view));
    }
}

[[nodiscard]] auto BuildSeriesViews(const ModelV2::FODEExplicitFirstOrderRuntimeBuildResultV2& runtime,
                                    const Slab::TOptional<ArtifactV2::FArtifactRunV2>& importedRecord)
    -> Slab::Vector<FArtifactSeriesView> {
    Slab::Vector<FArtifactSeriesView> views;
    if (importedRecord.has_value()) {
        views.reserve(importedRecord->Artifacts.size());
        for (const auto& artifact : importedRecord->Artifacts) {
            views.push_back({.Role = artifact.Role,
                             .DefinitionId = artifact.DefinitionId,
                             .DisplayLabel = artifact.DisplayLabel,
                             .CanonicalNotation = artifact.CanonicalNotation,
                             .Samples = artifact.Samples});
        }
        return views;
    }

    views.reserve(runtime.ObservableArtifacts.size() + runtime.StateArtifacts.size());
    AppendLiveSeries(views, runtime.ObservableArtifacts, ArtifactV2::EArtifactRoleV2::Observable);
    AppendLiveSeries(views, runtime.StateArtifacts, ArtifactV2::EArtifactRoleV2::State);
    return views;
}

} // namespace

auto FLabV2WindowManager::SaveSelectedArtifactRunToFile() -> bool {
    if (ArtifactStore == nullptr) {
        ArtifactPersistenceStatus = "HDF5 artifact support is unavailable in this build.";
        return false;
    }
    if (ModelArtifactRuns.empty() || SelectedArtifactRunIndex < 0 ||
        SelectedArtifactRunIndex >= static_cast<int>(ModelArtifactRuns.size())) {
        ArtifactPersistenceStatus = "Select a live artifact run to save.";
        return false;
    }

    auto& source = ModelArtifactRuns[static_cast<std::size_t>(SelectedArtifactRunIndex)];
    if (source.ImportedRecord.has_value()) {
        ArtifactPersistenceStatus = "Imported artifact runs are read-only.";
        return false;
    }
    if (source.Task == nullptr || !IsTerminalTaskStatus(source.Task->GetStatus())) {
        ArtifactPersistenceStatus = "A run can be saved only after its task finishes.";
        return false;
    }
    if (ArtifactPersistenceFilePath.empty()) {
        ArtifactPersistenceStatus = "Choose an HDF5 file path.";
        return false;
    }

    ModelV2::FModelV2 modelIdentity;
    modelIdentity.ModelId = source.ModelId;
    modelIdentity.Name = source.ModelName;
    const auto materialized =
        ModelV2::MaterializeODEArtifactRunV2(source.RunId, modelIdentity, source.Runtime, source.Task,
                                             source.CreatedUtcUnixNanoseconds, UtcNowUnixNanoseconds());
    if (materialized.IsFailure()) {
        ArtifactPersistenceStatus = "Save failed: " + materialized.ToString();
        return false;
    }

    const std::filesystem::path path(ArtifactPersistenceFilePath);
    const auto saveResult = ArtifactStore->SaveRun(path, materialized.Value());
    if (saveResult.IsFailure()) {
        ArtifactPersistenceStatus = "Save failed: " + saveResult.ToString();
        return false;
    }

    source.SourcePath = NormalizeArtifactPath(path);
    ArtifactPersistenceFilePath = source.SourcePath;
    ArtifactPersistenceStatus = "Saved one completed run to " + source.SourcePath;
    return true;
}

auto FLabV2WindowManager::LoadArtifactRunFromFile() -> bool {
    if (ArtifactStore == nullptr) {
        ArtifactPersistenceStatus = "HDF5 artifact support is unavailable in this build.";
        return false;
    }
    if (ArtifactPersistenceFilePath.empty()) {
        ArtifactPersistenceStatus = "Choose an HDF5 file path.";
        return false;
    }

    const std::filesystem::path path(ArtifactPersistenceFilePath);
    const auto loadResult = ArtifactStore->LoadRun(path);
    if (loadResult.IsFailure()) {
        ArtifactPersistenceStatus = "Load failed: " + loadResult.ToString();
        return false;
    }

    const auto normalizedPath = NormalizeArtifactPath(path);
    FModelArtifactRunState imported;
    imported.RunId = loadResult.Value().RunId;
    imported.ModelId = loadResult.Value().ModelId;
    imported.ModelName = loadResult.Value().ModelName;
    imported.TaskName = loadResult.Value().TaskName;
    imported.ImportedRecord = loadResult.Value();
    imported.SourcePath = normalizedPath;
    imported.CreatedUtcUnixNanoseconds = loadResult.Value().CreatedUtcUnixNanoseconds;

    auto existing = std::find_if(ModelArtifactRuns.begin(), ModelArtifactRuns.end(), [&](const auto& run) {
        return run.ImportedRecord.has_value() && run.SourcePath == normalizedPath;
    });
    if (existing != ModelArtifactRuns.end()) {
        *existing = std::move(imported);
        SelectedArtifactRunIndex = static_cast<int>(std::distance(ModelArtifactRuns.begin(), existing));
    } else {
        ModelArtifactRuns.push_back(std::move(imported));
        SelectedArtifactRunIndex = static_cast<int>(ModelArtifactRuns.size()) - 1;
    }

    auto& selected = ModelArtifactRuns[static_cast<std::size_t>(SelectedArtifactRunIndex)];
    SelectedArtifactDefinitionId = selected.ImportedRecord->Artifacts.empty()
                                       ? Slab::Str{}
                                       : selected.ImportedRecord->Artifacts.front().DefinitionId;
    ArtifactPersistenceFilePath = normalizedPath;
    ArtifactPersistenceStatus = "Loaded one artifact run from " + normalizedPath;
    return true;
}

auto FLabV2WindowManager::DrawArtifactsPanel() -> void {
    ImGui::SeparatorText("HDF5 Run File");

    std::array<char, 2048> pathBuffer{};
    std::snprintf(pathBuffer.data(), pathBuffer.size(), "%s", ArtifactPersistenceFilePath.c_str());
    ImGui::SetNextItemWidth(-1.0f);
    if (ImGui::InputText("##ArtifactFilePath", pathBuffer.data(), pathBuffer.size())) {
        ArtifactPersistenceFilePath = pathBuffer.data();
    }

    const bool bStoreAvailable = ArtifactStore != nullptr;
    if (!bStoreAvailable)
        ImGui::BeginDisabled();
    if (ImGui::SmallButton("Load Run"))
        LoadArtifactRunFromFile();
    if (!bStoreAvailable)
        ImGui::EndDisabled();

    const bool bHasSelection = !ModelArtifactRuns.empty() && SelectedArtifactRunIndex >= 0 &&
                               SelectedArtifactRunIndex < static_cast<int>(ModelArtifactRuns.size());
    bool bCanSave = false;
    if (bStoreAvailable && bHasSelection) {
        const auto& run = ModelArtifactRuns[static_cast<std::size_t>(SelectedArtifactRunIndex)];
        bCanSave =
            !run.ImportedRecord.has_value() && run.Task != nullptr && IsTerminalTaskStatus(run.Task->GetStatus());
    }
    ImGui::SameLine();
    if (!bCanSave)
        ImGui::BeginDisabled();
    if (ImGui::SmallButton("Save Selected Run"))
        SaveSelectedArtifactRunToFile();
    if (!bCanSave)
        ImGui::EndDisabled();

    if (!ArtifactPersistenceStatus.empty()) {
        ImGui::TextWrapped("%s", ArtifactPersistenceStatus.c_str());
    } else if (!bStoreAvailable) {
        ImGui::TextDisabled("Configure with STUDIOSLAB_HDF5_SUPPORT=ON to save or load run files.");
    }

    ImGui::SeparatorText("Model Artifact Runs");
    if (ModelArtifactRuns.empty()) {
        ImGui::TextDisabled("No artifact runs in this session.");
        ImGui::TextDisabled("Launch an ODE runtime or load an HDF5 run file.");
        return;
    }

    SelectedArtifactRunIndex = std::clamp(SelectedArtifactRunIndex, 0, static_cast<int>(ModelArtifactRuns.size()) - 1);

    if (ImGui::SmallButton("Remove Selected")) {
        ModelArtifactRuns.erase(ModelArtifactRuns.begin() + SelectedArtifactRunIndex);
        SelectedArtifactRunIndex =
            std::clamp(SelectedArtifactRunIndex, 0, std::max(0, static_cast<int>(ModelArtifactRuns.size()) - 1));
        SelectedArtifactDefinitionId.clear();
    }
    ImGui::SameLine();
    if (ImGui::SmallButton("Clear All")) {
        ModelArtifactRuns.clear();
        SelectedArtifactRunIndex = 0;
        SelectedArtifactDefinitionId.clear();
    }
    if (ModelArtifactRuns.empty())
        return;

    if (ImGui::BeginChild("ArtifactsRunList", ImVec2(ImGui::GetFontSize() * 20.0f, 0.0f), true)) {
        for (std::size_t i = 0; i < ModelArtifactRuns.size(); ++i) {
            const auto& run = ModelArtifactRuns[i];
            const auto labelPrefix = run.ImportedRecord.has_value() ? "file: " : "live: ";
            const auto label = labelPrefix + run.ModelName + " [" + run.RunId + "]";
            if (ImGui::Selectable(label.c_str(), SelectedArtifactRunIndex == static_cast<int>(i))) {
                SelectedArtifactRunIndex = static_cast<int>(i);
                const auto views = BuildSeriesViews(run.Runtime, run.ImportedRecord);
                SelectedArtifactDefinitionId = views.empty() ? Slab::Str{} : views.front().DefinitionId;
                ArtifactPersistenceFilePath =
                    run.SourcePath.empty() ? "Build/artifacts/" + run.RunId + ".h5" : run.SourcePath;
            }
        }
    }
    ImGui::EndChild();

    ImGui::SameLine();
    if (!ImGui::BeginChild("ArtifactsRunDetails", ImVec2(0.0f, 0.0f), false)) {
        ImGui::EndChild();
        return;
    }

    auto& selectedRun = ModelArtifactRuns[static_cast<std::size_t>(SelectedArtifactRunIndex)];
    auto seriesViews = BuildSeriesViews(selectedRun.Runtime, selectedRun.ImportedRecord);
    const auto selectedStillExists = std::ranges::any_of(
        seriesViews, [&](const auto& series) { return series.DefinitionId == SelectedArtifactDefinitionId; });
    if (!selectedStillExists) {
        SelectedArtifactDefinitionId = seriesViews.empty() ? Slab::Str{} : seriesViews.front().DefinitionId;
    }

    ImGui::Text("Model: %s", selectedRun.ModelName.c_str());
    ImGui::TextDisabled("%s | %s", selectedRun.ModelId.c_str(), selectedRun.RunId.c_str());
    if (selectedRun.ImportedRecord.has_value()) {
        const auto& record = *selectedRun.ImportedRecord;
        ImGui::TextColored(RunStatusColor(record.Status), "Run: %s", RunStatusLabel(record.Status));
        ImGui::Text("Runtime: %s | Solver: %s | dt=%s", record.Provenance.RuntimeKind.c_str(),
                    record.Provenance.SolverId.c_str(), FormatScalar(record.Provenance.TimeStep).c_str());
        ImGui::TextWrapped("Source: %s", selectedRun.SourcePath.c_str());
    } else {
        const auto taskStatus =
            selectedRun.Task != nullptr ? selectedRun.Task->GetStatus() : Slab::Core::TaskNotInitialized;
        ImGui::TextColored(TaskStatusColor(taskStatus), "Task: %s", TaskStatusLabel(taskStatus));
        if (selectedRun.Task != nullptr) {
            const auto cursor = selectedRun.Task->GetCursor();
            if (cursor.SimulationTime.has_value()) {
                ImGui::Text("Cursor: s=%llu  t=%s", static_cast<unsigned long long>(cursor.Step),
                            Slab::ToStr(*cursor.SimulationTime, 6, true).c_str());
            } else {
                ImGui::Text("Cursor: s=%llu", static_cast<unsigned long long>(cursor.Step));
            }
        }
        if (!selectedRun.SourcePath.empty()) {
            ImGui::TextWrapped("Saved as: %s", selectedRun.SourcePath.c_str());
        }
    }

    ImGui::SeparatorText("Artifacts");
    for (const auto& series : seriesViews) {
        const auto roleLabel = series.Role == ArtifactV2::EArtifactRoleV2::Observable ? "obs: " : "state: ";
        const auto label =
            Slab::Str(roleLabel) + (series.DisplayLabel.empty() ? series.DefinitionId : series.DisplayLabel);
        if (ImGui::Selectable(label.c_str(), SelectedArtifactDefinitionId == series.DefinitionId)) {
            SelectedArtifactDefinitionId = series.DefinitionId;
        }
        if (!series.CanonicalNotation.empty() && ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort)) {
            ImGui::SetTooltip("%s", series.CanonicalNotation.c_str());
        }
    }

    const auto selectedSeries = std::ranges::find_if(
        seriesViews, [&](const auto& series) { return series.DefinitionId == SelectedArtifactDefinitionId; });
    ImGui::SeparatorText("Series");
    if (selectedSeries == seriesViews.end()) {
        ImGui::TextDisabled("Select an artifact stream.");
        ImGui::EndChild();
        return;
    }

    ImGui::Text(
        "%s (%s)",
        (selectedSeries->DisplayLabel.empty() ? selectedSeries->DefinitionId : selectedSeries->DisplayLabel).c_str(),
        selectedSeries->Role == ArtifactV2::EArtifactRoleV2::Observable ? "observable" : "state");
    ImGui::TextDisabled("%s", selectedSeries->DefinitionId.c_str());
    if (selectedSeries->Samples.empty()) {
        ImGui::TextDisabled("No samples captured.");
        ImGui::EndChild();
        return;
    }

    const auto& latest = selectedSeries->Samples.back();
    if (latest.SimulationTime.has_value()) {
        ImGui::Text("Latest: step=%llu  t=%s  value=%s", static_cast<unsigned long long>(latest.Step),
                    Slab::ToStr(*latest.SimulationTime, 6, true).c_str(), FormatScalar(latest.Value).c_str());
    } else {
        ImGui::Text("Latest: step=%llu  value=%s", static_cast<unsigned long long>(latest.Step),
                    FormatScalar(latest.Value).c_str());
    }

    std::vector<float> plotValues;
    plotValues.reserve(selectedSeries->Samples.size());
    for (const auto& sample : selectedSeries->Samples) {
        plotValues.push_back(static_cast<float>(sample.Value));
    }
    ImGui::PlotLines("##ArtifactSeriesPlot", plotValues.data(), static_cast<int>(plotValues.size()), 0, nullptr,
                     FLT_MAX, FLT_MAX, ImVec2(0.0f, 220.0f));
    ImGui::TextDisabled("Samples: %d", static_cast<int>(selectedSeries->Samples.size()));

    constexpr auto tableFlags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingFixedFit;
    if (ImGui::BeginTable("ArtifactSamplesTable", 4, tableFlags)) {
        ImGui::TableSetupColumn("Step");
        ImGui::TableSetupColumn("Time");
        ImGui::TableSetupColumn("Value");
        ImGui::TableSetupColumn("Reason");
        ImGui::TableHeadersRow();

        const auto firstRow = selectedSeries->Samples.size() > 10 ? selectedSeries->Samples.size() - 10 : 0;
        for (std::size_t i = firstRow; i < selectedSeries->Samples.size(); ++i) {
            const auto& sample = selectedSeries->Samples[i];
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("%llu", static_cast<unsigned long long>(sample.Step));
            ImGui::TableSetColumnIndex(1);
            if (sample.SimulationTime.has_value()) {
                ImGui::Text("%s", Slab::ToStr(*sample.SimulationTime, 6, true).c_str());
            } else {
                ImGui::TextDisabled("-");
            }
            ImGui::TableSetColumnIndex(2);
            ImGui::Text("%s", FormatScalar(sample.Value).c_str());
            ImGui::TableSetColumnIndex(3);
            ImGui::TextUnformatted(EventReasonLabel(sample.Reason));
        }
        ImGui::EndTable();
    }

    ImGui::EndChild();
}
