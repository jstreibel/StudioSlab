#include "ArtifactTypesV2.h"

#include <cmath>
#include <set>
#include <tuple>

namespace Slab::Core::Artifacts::V2 {

auto ToString(const EArtifactRunStatusV2 status) -> const char* {
    switch (status) {
    case EArtifactRunStatusV2::Success:
        return "success";
    case EArtifactRunStatusV2::Error:
        return "error";
    case EArtifactRunStatusV2::Aborted:
        return "aborted";
    }
    return "unknown";
}

auto ToString(const EArtifactRoleV2 role) -> const char* {
    switch (role) {
    case EArtifactRoleV2::State:
        return "state";
    case EArtifactRoleV2::Observable:
        return "observable";
    }
    return "unknown";
}

auto ToString(const EArtifactEventReasonV2 reason) -> const char* {
    switch (reason) {
    case EArtifactEventReasonV2::Initial:
        return "initial";
    case EArtifactEventReasonV2::Scheduled:
        return "scheduled";
    case EArtifactEventReasonV2::Forced:
        return "forced";
    case EArtifactEventReasonV2::Final:
        return "final";
    case EArtifactEventReasonV2::AbortFinal:
        return "abort_final";
    }
    return "unknown";
}

auto ToString(const EArtifactRunModeV2 mode) -> const char* {
    switch (mode) {
    case EArtifactRunModeV2::FiniteSteps:
        return "finite_steps";
    case EArtifactRunModeV2::FiniteSimulationTime:
        return "finite_simulation_time";
    case EArtifactRunModeV2::OpenEnded:
        return "open_ended";
    }
    return "unknown";
}

auto ParseArtifactRunStatusV2(const Str& value) -> TOptional<EArtifactRunStatusV2> {
    if (value == "success")
        return EArtifactRunStatusV2::Success;
    if (value == "error")
        return EArtifactRunStatusV2::Error;
    if (value == "aborted")
        return EArtifactRunStatusV2::Aborted;
    return std::nullopt;
}

auto ParseArtifactRoleV2(const Str& value) -> TOptional<EArtifactRoleV2> {
    if (value == "state")
        return EArtifactRoleV2::State;
    if (value == "observable")
        return EArtifactRoleV2::Observable;
    return std::nullopt;
}

auto ParseArtifactRunModeV2(const Str& value) -> TOptional<EArtifactRunModeV2> {
    if (value == "finite_steps")
        return EArtifactRunModeV2::FiniteSteps;
    if (value == "finite_simulation_time")
        return EArtifactRunModeV2::FiniteSimulationTime;
    if (value == "open_ended")
        return EArtifactRunModeV2::OpenEnded;
    return std::nullopt;
}

auto ParseArtifactEventReasonV2(const std::uint8_t value) -> TOptional<EArtifactEventReasonV2> {
    switch (value) {
    case 0:
        return EArtifactEventReasonV2::Initial;
    case 1:
        return EArtifactEventReasonV2::Scheduled;
    case 2:
        return EArtifactEventReasonV2::Forced;
    case 3:
        return EArtifactEventReasonV2::Final;
    case 4:
        return EArtifactEventReasonV2::AbortFinal;
    default:
        return std::nullopt;
    }
}

auto ValidateArtifactRunV2(const FArtifactRunV2& run) -> FResult {
    StrVector errors;
    const auto append = [&](const Str& message) { errors.push_back(message); };

    if (run.RunId.empty())
        append("artifact run id is empty");
    if (run.ModelId.empty())
        append("artifact model id is empty");
    if (run.CreatedUtcUnixNanoseconds < 0)
        append("artifact creation timestamp is negative");
    if (run.StartedUtcUnixNanoseconds < 0)
        append("artifact start timestamp is negative");
    if (run.FinishedUtcUnixNanoseconds < 0)
        append("artifact finish timestamp is negative");
    if (run.ExportedUtcUnixNanoseconds < 0)
        append("artifact export timestamp is negative");
    if (run.StartedUtcUnixNanoseconds > 0 && run.FinishedUtcUnixNanoseconds > 0 &&
        run.FinishedUtcUnixNanoseconds < run.StartedUtcUnixNanoseconds) {
        append("artifact finish timestamp precedes its start timestamp");
    }

    const auto& provenance = run.Provenance;
    if (provenance.RuntimeKind.empty())
        append("artifact runtime kind is empty");
    if (provenance.SolverId.empty())
        append("artifact solver id is empty");
    if (provenance.ScalarPrecisionBits != 32 && provenance.ScalarPrecisionBits != 64) {
        append("artifact scalar precision must be 32 or 64 bits");
    }
    if (!std::isfinite(static_cast<double>(provenance.TimeStep)) || provenance.TimeStep <= 0.0) {
        append("artifact time step must be finite and positive");
    }
    if (!std::isfinite(static_cast<double>(provenance.InitialTime))) {
        append("artifact initial time must be finite");
    }
    if (provenance.ArtifactSampleIntervalSteps == 0) {
        append("artifact sample interval must be positive");
    }
    if (provenance.MaxArtifactSamples.has_value() && *provenance.MaxArtifactSamples == 0) {
        append("artifact maximum sample count must be positive when present");
    }
    if (provenance.RunMode == EArtifactRunModeV2::FiniteSteps && !provenance.MaxSteps.has_value()) {
        append("finite-step artifact provenance is missing max steps");
    }
    if (provenance.RunMode == EArtifactRunModeV2::FiniteSimulationTime && !provenance.MaxSimulationTime.has_value()) {
        append("finite-time artifact provenance is missing max simulation time");
    }

    std::set<Str> bindingIds;
    for (const auto& binding : provenance.ScalarBindings) {
        if (binding.DefinitionId.empty())
            append("artifact scalar binding has an empty definition id");
        if (!bindingIds.insert(binding.DefinitionId).second) {
            append("artifact scalar binding id is duplicated: " + binding.DefinitionId);
        }
        if (!std::isfinite(static_cast<double>(binding.Value))) {
            append("artifact scalar binding is not finite: " + binding.DefinitionId);
        }
    }

    std::set<Str> initialStateIds;
    for (const auto& initialValue : provenance.InitialState) {
        if (initialValue.DefinitionId.empty())
            append("artifact initial-state value has an empty definition id");
        if (!initialStateIds.insert(initialValue.DefinitionId).second) {
            append("artifact initial-state id is duplicated: " + initialValue.DefinitionId);
        }
        if (!std::isfinite(static_cast<double>(initialValue.Value))) {
            append("artifact initial-state value is not finite: " + initialValue.DefinitionId);
        }
    }

    std::set<std::pair<EArtifactRoleV2, Str>> artifactIds;
    for (const auto& artifact : run.Artifacts) {
        if (artifact.DefinitionId.empty())
            append("scalar time-series artifact has an empty definition id");
        if (!artifactIds.emplace(artifact.Role, artifact.DefinitionId).second) {
            append("scalar time-series artifact is duplicated: " + Str(ToString(artifact.Role)) + "/" +
                   artifact.DefinitionId);
        }
        for (const auto& sample : artifact.Samples) {
            if (sample.SimulationTime.has_value() && !std::isfinite(static_cast<double>(*sample.SimulationTime))) {
                append("artifact simulation time is not finite: " + artifact.DefinitionId);
                break;
            }
            if (!std::isfinite(sample.WallClockSeconds) || sample.WallClockSeconds < 0.0) {
                append("artifact wall-clock time must be finite and non-negative: " + artifact.DefinitionId);
                break;
            }
        }
    }

    return errors.empty() ? FResult::Ok() : FResult::Fail(errors);
}

} // namespace Slab::Core::Artifacts::V2
