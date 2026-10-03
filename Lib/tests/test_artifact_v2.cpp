#include <catch2/catch_all.hpp>

#include "Core/Artifacts/V2/ArtifactTypesV2.h"
#include "Core/Composition/V2/Modules/ArtifactModuleV2.h"
#include "Core/Composition/V2/RuntimeContextV2.h"

#include <limits>

namespace {

namespace ArtifactV2 = Slab::Core::Artifacts::V2;
namespace CompositionV2 = Slab::Core::Composition::V2;

[[nodiscard]] auto MakeValidArtifactRun() -> ArtifactV2::FArtifactRunV2 {
    ArtifactV2::FArtifactRunV2 run;
    run.RunId = "run.validation";
    run.ModelId = "model.oscillator";
    run.ModelName = "Oscillator";
    run.TaskName = "Numeric Integration";
    run.Status = ArtifactV2::EArtifactRunStatusV2::Success;
    run.CreatedUtcUnixNanoseconds = 10;
    run.StartedUtcUnixNanoseconds = 20;
    run.FinishedUtcUnixNanoseconds = 30;
    run.ExportedUtcUnixNanoseconds = 40;
    run.Provenance.TimeStep = 0.01;
    run.Provenance.MaxSteps = 100;
    run.Provenance.TimeCoordinateDefinitionId = "time.t";
    run.Provenance.bCaptureStateHistory = true;
    run.Provenance.ArtifactSampleIntervalSteps = 2;
    run.Provenance.MaxArtifactSamples = 50;
    run.Provenance.ScalarBindings.push_back({"parameter.omega", 2.0});
    run.Provenance.InitialState.push_back({"state.q", 1.0});
    run.Artifacts.push_back({.Role = ArtifactV2::EArtifactRoleV2::State,
                             .DefinitionId = "state.q",
                             .DisplayLabel = "q",
                             .CanonicalNotation = "q(t)",
                             .Samples = {{.Step = 0,
                                          .SimulationTime = 0.0,
                                          .WallClockSeconds = 0.0,
                                          .Value = 1.0,
                                          .Reason = ArtifactV2::EArtifactEventReasonV2::Initial,
                                          .PublishedVersion = 1}}});
    return run;
}

} // namespace

TEST_CASE("Artifact V2 validates portable completed-run records", "[ArtifactV2]") {
    auto run = MakeValidArtifactRun();
    REQUIRE(ArtifactV2::ValidateArtifactRunV2(run).IsSuccess());

    SECTION("sample values may preserve non-finite numeric output") {
        run.Artifacts.front().Samples.front().Value = std::numeric_limits<Slab::DevFloat>::quiet_NaN();
        REQUIRE(ArtifactV2::ValidateArtifactRunV2(run).IsSuccess());
    }

    SECTION("runtime seed values must remain finite") {
        run.Provenance.InitialState.front().Value = std::numeric_limits<Slab::DevFloat>::infinity();
        REQUIRE(ArtifactV2::ValidateArtifactRunV2(run).IsFailure());
    }

    SECTION("finite-step provenance requires a limit") {
        run.Provenance.MaxSteps = std::nullopt;
        REQUIRE(ArtifactV2::ValidateArtifactRunV2(run).IsFailure());
    }

    SECTION("artifact identities are unique within a role") {
        run.Artifacts.push_back(run.Artifacts.front());
        REQUIRE(ArtifactV2::ValidateArtifactRunV2(run).IsFailure());
    }
}

TEST_CASE("Artifact V2 enum encodings are stable and reversible", "[ArtifactV2]") {
    REQUIRE(ArtifactV2::ParseArtifactRunStatusV2("success") == ArtifactV2::EArtifactRunStatusV2::Success);
    REQUIRE(ArtifactV2::ParseArtifactRoleV2("observable") == ArtifactV2::EArtifactRoleV2::Observable);
    REQUIRE(ArtifactV2::ParseArtifactRunModeV2("open_ended") == ArtifactV2::EArtifactRunModeV2::OpenEnded);
    REQUIRE(ArtifactV2::ParseArtifactEventReasonV2(2) == ArtifactV2::EArtifactEventReasonV2::Forced);
    REQUIRE_FALSE(ArtifactV2::ParseArtifactEventReasonV2(255).has_value());
}

TEST_CASE("Artifact module V2 registers only an available store", "[ArtifactV2][CompositionV2]") {
    CompositionV2::FRuntimeContextV2 context;
    context.InstallModule(CompositionV2::MakeArtifactModuleV2(nullptr));

    REQUIRE_FALSE(context.GetServices().Has<ArtifactV2::IArtifactStoreV2>());
    REQUIRE(context.GetInstalledModules().empty());
}
