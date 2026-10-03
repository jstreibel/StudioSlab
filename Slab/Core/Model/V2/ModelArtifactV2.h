#ifndef STUDIOSLAB_MODEL_ARTIFACT_V2_H
#define STUDIOSLAB_MODEL_ARTIFACT_V2_H

#include "Core/Artifacts/V2/ArtifactTypesV2.h"
#include "Core/Model/V2/ModelRealizationRuntimeV2.h"
#include "Math/Numerics/V2/Task/NumericTaskV2.h"

namespace Slab::Core::Model::V2 {

namespace ArtifactDetail {

[[nodiscard]] inline auto
ConvertArtifactEventReasonV2(const Math::Numerics::V2::EEventReasonV2 reason) -> Artifacts::V2::EArtifactEventReasonV2 {
    using Source = Math::Numerics::V2::EEventReasonV2;
    using Target = Artifacts::V2::EArtifactEventReasonV2;
    switch (reason) {
    case Source::Initial:
        return Target::Initial;
    case Source::Scheduled:
        return Target::Scheduled;
    case Source::Forced:
        return Target::Forced;
    case Source::Final:
        return Target::Final;
    case Source::AbortFinal:
        return Target::AbortFinal;
    }
    return Target::Scheduled;
}

[[nodiscard]] inline auto
ConvertArtifactRunStatusV2(const Core::ETaskStatus status) -> TOptional<Artifacts::V2::EArtifactRunStatusV2> {
    using Target = Artifacts::V2::EArtifactRunStatusV2;
    switch (status) {
    case Core::TaskSuccess:
        return Target::Success;
    case Core::TaskError:
        return Target::Error;
    case Core::TaskAborted:
        return Target::Aborted;
    case Core::TaskNotInitialized:
    case Core::TaskRunning:
        return std::nullopt;
    }
    return std::nullopt;
}

inline auto AppendArtifactSeriesV2(Artifacts::V2::FArtifactRunV2& run,
                                   const Vector<FODETimeSeriesArtifactV2>& sourceArtifacts,
                                   const Artifacts::V2::EArtifactRoleV2 role) -> void {
    for (const auto& source : sourceArtifacts) {
        if (source.Listener == nullptr)
            continue;

        Artifacts::V2::FScalarTimeSeriesArtifactV2 artifact;
        artifact.Role = role;
        artifact.DefinitionId = source.DefinitionId;
        artifact.DisplayLabel = source.DisplayLabel;
        artifact.CanonicalNotation = source.CanonicalNotation;

        const auto samples = source.Listener->GetSamples();
        artifact.Samples.reserve(samples.size());
        for (const auto& sample : samples) {
            artifact.Samples.push_back({.Step = sample.Cursor.Step,
                                        .SimulationTime = sample.Cursor.SimulationTime,
                                        .WallClockSeconds = sample.Cursor.WallClockSeconds,
                                        .Value = sample.Value,
                                        .Reason = ConvertArtifactEventReasonV2(sample.Reason),
                                        .PublishedVersion = sample.PublishedVersion});
        }
        run.Artifacts.push_back(std::move(artifact));
    }
}

} // namespace ArtifactDetail

[[nodiscard]] inline auto
MaterializeODEArtifactRunV2(Str runId, const FModelV2& model, const FODEExplicitFirstOrderRuntimeBuildResultV2& runtime,
                            const Math::Numerics::V2::FNumericTaskV2_ptr& task,
                            const std::int64_t createdUtcUnixNanoseconds,
                            const std::int64_t exportedUtcUnixNanoseconds) -> TResult<Artifacts::V2::FArtifactRunV2> {
    if (task == nullptr) {
        return TResult<Artifacts::V2::FArtifactRunV2>::Fail("artifact run has no numeric task");
    }
    if (runtime.System == nullptr) {
        return TResult<Artifacts::V2::FArtifactRunV2>::Fail("artifact run has no runtime system");
    }

    const auto status = ArtifactDetail::ConvertArtifactRunStatusV2(task->GetStatus());
    if (!status.has_value()) {
        return TResult<Artifacts::V2::FArtifactRunV2>::Fail(
            "artifact run can only be materialized after its task reaches a terminal state");
    }
    const auto started = task->GetStartedUtcUnixNanoseconds();
    const auto finished = task->GetFinishedUtcUnixNanoseconds();
    if (!started.has_value() || !finished.has_value()) {
        return TResult<Artifacts::V2::FArtifactRunV2>::Fail("artifact task is missing its runtime timestamps");
    }

    Artifacts::V2::FArtifactRunV2 run;
    run.RunId = std::move(runId);
    run.ModelId = model.ModelId;
    run.ModelName = model.Name;
    run.TaskName = task->GetName();
    run.Status = *status;
    run.CreatedUtcUnixNanoseconds = createdUtcUnixNanoseconds;
    run.StartedUtcUnixNanoseconds = *started;
    run.FinishedUtcUnixNanoseconds = *finished;
    run.ExportedUtcUnixNanoseconds = exportedUtcUnixNanoseconds;

    const auto& config = runtime.RuntimeConfig;
    auto& provenance = run.Provenance;
    provenance.RuntimeKind = "ode.explicit_first_order";
    provenance.SolverId = "rk4";
    provenance.ScalarPrecisionBits = sizeof(DevFloat) * 8;
    provenance.TimeStep = config.TimeStep;
    provenance.RunMode = config.MaxSteps.has_value() ? Artifacts::V2::EArtifactRunModeV2::FiniteSteps
                                                     : Artifacts::V2::EArtifactRunModeV2::OpenEnded;
    provenance.MaxSteps = config.MaxSteps;
    provenance.TimeCoordinateDefinitionId = runtime.System->GetTimeCoordinateDefinitionId();
    provenance.InitialTime = runtime.System->GetInitialTime();
    provenance.bCaptureStateHistory = config.bCaptureStateHistory;
    provenance.bCaptureObservableHistory = config.bCaptureObservableHistory;
    provenance.ArtifactSampleIntervalSteps = config.ArtifactSampleIntervalSteps;
    provenance.MaxArtifactSamples = config.MaxArtifactSamples;

    provenance.ScalarBindings.reserve(runtime.System->GetScalarBindings().size());
    for (const auto& [definitionId, value] : runtime.System->GetScalarBindings()) {
        provenance.ScalarBindings.push_back({definitionId, value});
    }

    const auto& stateIds = runtime.System->GetStateDefinitionIds();
    const auto& initialValues = runtime.System->GetInitialStateValues();
    if (stateIds.size() != initialValues.size()) {
        return TResult<Artifacts::V2::FArtifactRunV2>::Fail(
            "artifact runtime state identities do not match initial values");
    }
    provenance.InitialState.reserve(stateIds.size());
    for (std::size_t i = 0; i < stateIds.size(); ++i) {
        provenance.InitialState.push_back({stateIds[i], initialValues[i]});
    }

    ArtifactDetail::AppendArtifactSeriesV2(run, runtime.StateArtifacts, Artifacts::V2::EArtifactRoleV2::State);
    ArtifactDetail::AppendArtifactSeriesV2(run, runtime.ObservableArtifacts,
                                           Artifacts::V2::EArtifactRoleV2::Observable);

    const auto validation = Artifacts::V2::ValidateArtifactRunV2(run);
    if (validation.IsFailure()) {
        return TResult<Artifacts::V2::FArtifactRunV2>::Fail(validation.Errors());
    }
    return TResult<Artifacts::V2::FArtifactRunV2>::Ok(std::move(run));
}

} // namespace Slab::Core::Model::V2

#endif // STUDIOSLAB_MODEL_ARTIFACT_V2_H
