#ifndef STUDIOSLAB_ARTIFACT_TYPES_V2_H
#define STUDIOSLAB_ARTIFACT_TYPES_V2_H

#include "Utils/Arrays.h"
#include "Utils/Optional.h"
#include "Utils/Result.h"
#include "Utils/String.h"
#include "Utils/Types.h"

#include <cstdint>
#include <optional>

namespace Slab::Core::Artifacts::V2 {

enum class EArtifactRunStatusV2 : std::uint8_t { Success, Error, Aborted };

enum class EArtifactRoleV2 : std::uint8_t { State, Observable };

enum class EArtifactEventReasonV2 : std::uint8_t { Initial = 0, Scheduled = 1, Forced = 2, Final = 3, AbortFinal = 4 };

enum class EArtifactRunModeV2 : std::uint8_t { FiniteSteps, FiniteSimulationTime, OpenEnded };

struct FArtifactScalarBindingV2 {
    Str DefinitionId;
    DevFloat Value = 0.0;
};

struct FArtifactInitialStateValueV2 {
    Str DefinitionId;
    DevFloat Value = 0.0;
};

struct FArtifactRunProvenanceV2 {
    Str RuntimeKind = "ode.explicit_first_order";
    Str SolverId = "rk4";
    std::uint32_t ScalarPrecisionBits = sizeof(DevFloat) * 8;
    DevFloat TimeStep = 0.0;
    EArtifactRunModeV2 RunMode = EArtifactRunModeV2::FiniteSteps;
    TOptional<UIntBig> MaxSteps = std::nullopt;
    TOptional<DevFloat> MaxSimulationTime = std::nullopt;
    Str TimeCoordinateDefinitionId;
    DevFloat InitialTime = 0.0;
    bool bCaptureStateHistory = false;
    bool bCaptureObservableHistory = false;
    UIntBig ArtifactSampleIntervalSteps = 1;
    TOptional<UIntBig> MaxArtifactSamples = std::nullopt;
    Vector<FArtifactScalarBindingV2> ScalarBindings;
    Vector<FArtifactInitialStateValueV2> InitialState;
};

struct FArtifactSampleV2 {
    UIntBig Step = 0;
    TOptional<DevFloat> SimulationTime = std::nullopt;
    double WallClockSeconds = 0.0;
    DevFloat Value = 0.0;
    EArtifactEventReasonV2 Reason = EArtifactEventReasonV2::Scheduled;
    UIntBig PublishedVersion = 0;
};

struct FScalarTimeSeriesArtifactV2 {
    EArtifactRoleV2 Role = EArtifactRoleV2::State;
    Str DefinitionId;
    Str DisplayLabel;
    Str CanonicalNotation;
    Vector<FArtifactSampleV2> Samples;
};

struct FArtifactRunV2 {
    Str RunId;
    Str ModelId;
    Str ModelName;
    Str TaskName;
    EArtifactRunStatusV2 Status = EArtifactRunStatusV2::Success;
    std::int64_t CreatedUtcUnixNanoseconds = 0;
    std::int64_t StartedUtcUnixNanoseconds = 0;
    std::int64_t FinishedUtcUnixNanoseconds = 0;
    std::int64_t ExportedUtcUnixNanoseconds = 0;
    FArtifactRunProvenanceV2 Provenance;
    Vector<FScalarTimeSeriesArtifactV2> Artifacts;
};

[[nodiscard]] auto ToString(EArtifactRunStatusV2 status) -> const char*;
[[nodiscard]] auto ToString(EArtifactRoleV2 role) -> const char*;
[[nodiscard]] auto ToString(EArtifactEventReasonV2 reason) -> const char*;
[[nodiscard]] auto ToString(EArtifactRunModeV2 mode) -> const char*;

[[nodiscard]] auto ParseArtifactRunStatusV2(const Str& value) -> TOptional<EArtifactRunStatusV2>;
[[nodiscard]] auto ParseArtifactRoleV2(const Str& value) -> TOptional<EArtifactRoleV2>;
[[nodiscard]] auto ParseArtifactRunModeV2(const Str& value) -> TOptional<EArtifactRunModeV2>;
[[nodiscard]] auto ParseArtifactEventReasonV2(std::uint8_t value) -> TOptional<EArtifactEventReasonV2>;

[[nodiscard]] auto ValidateArtifactRunV2(const FArtifactRunV2& run) -> FResult;

} // namespace Slab::Core::Artifacts::V2

#endif // STUDIOSLAB_ARTIFACT_TYPES_V2_H
