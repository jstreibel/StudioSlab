#include <catch2/catch_all.hpp>

#include "Core/Artifacts/V2/ArtifactStoreV2.h"

#include <array>
#include <chrono>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <limits>

namespace {

namespace ArtifactV2 = Slab::Core::Artifacts::V2;

class FTemporaryArtifactDirectory {
  public:
    FTemporaryArtifactDirectory() {
        const auto token = std::chrono::steady_clock::now().time_since_epoch().count();
        Path = std::filesystem::temp_directory_path() / ("studioslab-artifact-test-" + std::to_string(token));
        std::filesystem::create_directories(Path);
    }

    ~FTemporaryArtifactDirectory() {
        std::error_code ignored;
        std::filesystem::remove_all(Path, ignored);
    }

    std::filesystem::path Path;
};

[[nodiscard]] auto MakeRoundTripRun() -> ArtifactV2::FArtifactRunV2 {
    ArtifactV2::FArtifactRunV2 run;
    run.RunId = "run.hdf5.roundtrip";
    run.ModelId = "model.oscillator";
    run.ModelName = "Oscilador harmônico — μ";
    run.TaskName = "Numeric Integration";
    run.Status = ArtifactV2::EArtifactRunStatusV2::Aborted;
    run.CreatedUtcUnixNanoseconds = 1'000;
    run.StartedUtcUnixNanoseconds = 2'000;
    run.FinishedUtcUnixNanoseconds = 3'000;
    run.ExportedUtcUnixNanoseconds = 4'000;

    auto& provenance = run.Provenance;
    provenance.RuntimeKind = "ode.explicit_first_order";
    provenance.SolverId = "rk4";
    provenance.ScalarPrecisionBits = 64;
    provenance.TimeStep = 0.125;
    provenance.RunMode = ArtifactV2::EArtifactRunModeV2::FiniteSteps;
    provenance.MaxSteps = 4;
    provenance.TimeCoordinateDefinitionId = "time.t";
    provenance.InitialTime = 0.0;
    provenance.bCaptureStateHistory = true;
    provenance.bCaptureObservableHistory = true;
    provenance.ArtifactSampleIntervalSteps = 1;
    provenance.MaxArtifactSamples = std::nullopt;
    provenance.ScalarBindings = {{"parameter.omega", 2.5}};
    provenance.InitialState = {{"state.q", 1.0}, {"state.p", 0.0}};

    ArtifactV2::FScalarTimeSeriesArtifactV2 state;
    state.Role = ArtifactV2::EArtifactRoleV2::State;
    state.DefinitionId = "state.q";
    state.DisplayLabel = "posição q";
    state.CanonicalNotation = "q(t)";
    const auto reasons =
        std::array{ArtifactV2::EArtifactEventReasonV2::Initial, ArtifactV2::EArtifactEventReasonV2::Scheduled,
                   ArtifactV2::EArtifactEventReasonV2::Forced, ArtifactV2::EArtifactEventReasonV2::Final,
                   ArtifactV2::EArtifactEventReasonV2::AbortFinal};
    for (std::size_t i = 0; i < 1100; ++i) {
        const auto reason = i < reasons.size() ? reasons[i] : ArtifactV2::EArtifactEventReasonV2::Scheduled;
        state.Samples.push_back(
            {.Step = i,
             .SimulationTime = i == 2 ? std::nullopt : std::make_optional(0.125 * i),
             .WallClockSeconds = 0.01 * i,
             .Value = i == 3 ? std::numeric_limits<Slab::DevFloat>::infinity() : static_cast<Slab::DevFloat>(i),
             .Reason = reason,
             .PublishedVersion = i + 10});
    }
    run.Artifacts.push_back(std::move(state));
    run.Artifacts.push_back({.Role = ArtifactV2::EArtifactRoleV2::Observable,
                             .DefinitionId = "observable.energy",
                             .DisplayLabel = "Energy",
                             .CanonicalNotation = "E(q,p)",
                             .Samples = {}});
    return run;
}

} // namespace

TEST_CASE("HDF5 artifact store round-trips an open scientific run file", "[ArtifactV2][HDF5]") {
    const auto store = ArtifactV2::MakeDefaultArtifactStoreV2();
    REQUIRE(store != nullptr);

    FTemporaryArtifactDirectory temporary;
    const auto path = temporary.Path / "run.h5";
    const auto expected = MakeRoundTripRun();

    REQUIRE(store->SaveRun(path, expected).IsSuccess());
    REQUIRE(std::filesystem::is_regular_file(path));

    const auto loadedResult = store->LoadRun(path);
    REQUIRE(loadedResult.IsSuccess());
    const auto& loaded = loadedResult.Value();
    REQUIRE(loaded.RunId == expected.RunId);
    REQUIRE(loaded.ModelName == expected.ModelName);
    REQUIRE(loaded.Status == expected.Status);
    REQUIRE(loaded.Provenance.MaxSteps == expected.Provenance.MaxSteps);
    REQUIRE_FALSE(loaded.Provenance.MaxArtifactSamples.has_value());
    REQUIRE(loaded.Provenance.ScalarBindings.front().DefinitionId == "parameter.omega");
    REQUIRE(loaded.Artifacts.size() == 2);
    REQUIRE(loaded.Artifacts.front().Samples.size() == 1100);
    REQUIRE_FALSE(loaded.Artifacts.front().Samples[2].SimulationTime.has_value());
    REQUIRE(std::isinf(loaded.Artifacts.front().Samples[3].Value));
    REQUIRE(loaded.Artifacts.front().Samples[4].Reason == ArtifactV2::EArtifactEventReasonV2::AbortFinal);
    REQUIRE(loaded.Artifacts.back().Samples.empty());

    REQUIRE(store->SaveRun(path, expected).IsFailure());
}

TEST_CASE("HDF5 artifact store rejects non-HDF5 input", "[ArtifactV2][HDF5]") {
    const auto store = ArtifactV2::MakeDefaultArtifactStoreV2();
    REQUIRE(store != nullptr);

    FTemporaryArtifactDirectory temporary;
    const auto path = temporary.Path / "not-hdf5.json";
    {
        std::ofstream output(path);
        output << "{\"not\":\"hdf5\"}";
    }

    const auto loaded = store->LoadRun(path);
    REQUIRE(loaded.IsFailure());
}
