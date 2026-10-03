#include "HDF5ArtifactStoreV2.h"

#include <H5Cpp.h>
#include <hdf5.h>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <filesystem>
#include <iomanip>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <vector>

namespace Slab::Core::Artifacts::V2 {

namespace {

constexpr auto FormatId = "studioslab.artifact-run";
constexpr std::uint32_t SchemaMajor = 1;
constexpr std::uint32_t SchemaMinor = 0;
constexpr hsize_t CompressionThreshold = 1024;
constexpr hsize_t MaximumChunkElements = 4096;

[[nodiscard]] auto IndexedName(const std::uint64_t index) -> Str {
    std::ostringstream stream;
    stream << std::setw(6) << std::setfill('0') << index;
    return stream.str();
}

[[nodiscard]] auto HasAttribute(const H5::Group& group, const Str& name) -> bool {
    return H5Aexists(group.getId(), name.c_str()) > 0;
}

[[nodiscard]] auto HasLink(const H5::Group& group, const Str& name) -> bool {
    return H5Lexists(group.getId(), name.c_str(), H5P_DEFAULT) > 0;
}

auto RequireAttribute(const H5::Group& group, const Str& name) -> void {
    if (!HasAttribute(group, name))
        throw std::runtime_error("missing required attribute '" + name + "'");
}

auto RequireLink(const H5::Group& group, const Str& name) -> void {
    if (!HasLink(group, name))
        throw std::runtime_error("missing required object '" + name + "'");
}

[[nodiscard]] auto MakeUtf8StringType() -> H5::StrType {
    H5::StrType type(H5::PredType::C_S1, H5T_VARIABLE);
    type.setCset(H5T_CSET_UTF8);
    type.setStrpad(H5T_STR_NULLTERM);
    return type;
}

auto WriteStringAttribute(H5::Group& group, const Str& name, const Str& value) -> void {
    const H5::DataSpace scalar(H5S_SCALAR);
    auto type = MakeUtf8StringType();
    auto attribute = group.createAttribute(name, type, scalar);
    attribute.write(type, value);
}

[[nodiscard]] auto ReadStringAttribute(const H5::Group& group, const Str& name) -> Str {
    RequireAttribute(group, name);
    auto attribute = group.openAttribute(name);
    auto type = attribute.getStrType();
    Str value;
    attribute.read(type, value);
    return value;
}

template <typename TValue>
auto WriteNumericAttribute(H5::Group& group, const Str& name, const TValue value, const H5::DataType& fileType,
                           const H5::DataType& memoryType) -> void {
    const H5::DataSpace scalar(H5S_SCALAR);
    auto attribute = group.createAttribute(name, fileType, scalar);
    attribute.write(memoryType, &value);
}

template <typename TValue>
[[nodiscard]] auto ReadNumericAttribute(const H5::Group& group, const Str& name,
                                        const H5::DataType& memoryType) -> TValue {
    RequireAttribute(group, name);
    auto attribute = group.openAttribute(name);
    TValue value{};
    attribute.read(memoryType, &value);
    return value;
}

auto WriteU8(H5::Group& group, const Str& name, const std::uint8_t value) -> void {
    WriteNumericAttribute(group, name, value, H5::PredType::STD_U8LE, H5::PredType::NATIVE_UINT8);
}

auto WriteU32(H5::Group& group, const Str& name, const std::uint32_t value) -> void {
    WriteNumericAttribute(group, name, value, H5::PredType::STD_U32LE, H5::PredType::NATIVE_UINT32);
}

auto WriteU64(H5::Group& group, const Str& name, const std::uint64_t value) -> void {
    WriteNumericAttribute(group, name, value, H5::PredType::STD_U64LE, H5::PredType::NATIVE_UINT64);
}

auto WriteI64(H5::Group& group, const Str& name, const std::int64_t value) -> void {
    WriteNumericAttribute(group, name, value, H5::PredType::STD_I64LE, H5::PredType::NATIVE_INT64);
}

auto WriteF64(H5::Group& group, const Str& name, const double value) -> void {
    WriteNumericAttribute(group, name, value, H5::PredType::IEEE_F64LE, H5::PredType::NATIVE_DOUBLE);
}

[[nodiscard]] auto ReadU8(const H5::Group& group, const Str& name) -> std::uint8_t {
    return ReadNumericAttribute<std::uint8_t>(group, name, H5::PredType::NATIVE_UINT8);
}

[[nodiscard]] auto ReadU32(const H5::Group& group, const Str& name) -> std::uint32_t {
    return ReadNumericAttribute<std::uint32_t>(group, name, H5::PredType::NATIVE_UINT32);
}

[[nodiscard]] auto ReadU64(const H5::Group& group, const Str& name) -> std::uint64_t {
    return ReadNumericAttribute<std::uint64_t>(group, name, H5::PredType::NATIVE_UINT64);
}

[[nodiscard]] auto ReadI64(const H5::Group& group, const Str& name) -> std::int64_t {
    return ReadNumericAttribute<std::int64_t>(group, name, H5::PredType::NATIVE_INT64);
}

[[nodiscard]] auto ReadF64(const H5::Group& group, const Str& name) -> double {
    return ReadNumericAttribute<double>(group, name, H5::PredType::NATIVE_DOUBLE);
}

[[nodiscard]] auto DeflateEncodingAvailable() -> bool {
    if (H5Zfilter_avail(H5Z_FILTER_DEFLATE) <= 0)
        return false;
    unsigned int filterInfo = 0;
    return H5Zget_filter_info(H5Z_FILTER_DEFLATE, &filterInfo) >= 0 &&
           (filterInfo & H5Z_FILTER_CONFIG_ENCODE_ENABLED) != 0;
}

template <typename TValue>
auto WriteVectorDataset(H5::Group& group, const Str& name, const std::vector<TValue>& values,
                        const H5::DataType& fileType, const H5::DataType& memoryType) -> void {
    const hsize_t dimensions[] = {static_cast<hsize_t>(values.size())};
    const H5::DataSpace space(1, dimensions);

    H5::DataSet dataset;
    if (dimensions[0] >= CompressionThreshold) {
        H5::DSetCreatPropList properties;
        const hsize_t chunk[] = {std::min(dimensions[0], MaximumChunkElements)};
        properties.setChunk(1, chunk);
        properties.setShuffle();
        if (DeflateEncodingAvailable())
            properties.setDeflate(4);
        dataset = group.createDataSet(name, fileType, space, properties);
    } else {
        dataset = group.createDataSet(name, fileType, space);
    }

    if (!values.empty())
        dataset.write(values.data(), memoryType);
}

template <typename TValue>
[[nodiscard]] auto ReadVectorDataset(const H5::Group& group, const Str& name,
                                     const H5::DataType& memoryType) -> std::vector<TValue> {
    RequireLink(group, name);
    auto dataset = group.openDataSet(name);
    auto space = dataset.getSpace();
    if (space.getSimpleExtentNdims() != 1) {
        throw std::runtime_error("dataset '" + name + "' must have rank one");
    }

    hsize_t dimensions[1] = {0};
    space.getSimpleExtentDims(dimensions);
    if (dimensions[0] > static_cast<hsize_t>(std::numeric_limits<std::size_t>::max())) {
        throw std::runtime_error("dataset '" + name + "' is too large for this build");
    }

    std::vector<TValue> values(static_cast<std::size_t>(dimensions[0]));
    if (!values.empty())
        dataset.read(values.data(), memoryType);
    return values;
}

[[nodiscard]] auto CheckedIndexedCount(const H5::Group& group, const std::uint64_t count,
                                       const Str& label) -> std::size_t {
    if (count > static_cast<std::uint64_t>(std::numeric_limits<std::size_t>::max()) ||
        count > static_cast<std::uint64_t>(group.getNumObjs())) {
        throw std::runtime_error(label + " count exceeds its indexed children");
    }
    return static_cast<std::size_t>(count);
}

auto RequireSameSize(const Str& artifactId, const std::size_t expected, const Str& column,
                     const std::size_t actual) -> void {
    if (expected != actual) {
        throw std::runtime_error("artifact '" + artifactId + "' column '" + column + "' has a mismatched length");
    }
}

auto WriteScalarEntries(H5::Group& runtimeGroup, const FArtifactRunProvenanceV2& provenance) -> void {
    auto bindingsGroup = runtimeGroup.createGroup("scalar_bindings");
    WriteU64(bindingsGroup, "count", provenance.ScalarBindings.size());
    for (std::size_t i = 0; i < provenance.ScalarBindings.size(); ++i) {
        auto entry = bindingsGroup.createGroup(IndexedName(i));
        WriteStringAttribute(entry, "definition_id", provenance.ScalarBindings[i].DefinitionId);
        WriteF64(entry, "value", static_cast<double>(provenance.ScalarBindings[i].Value));
    }

    auto initialStateGroup = runtimeGroup.createGroup("initial_state");
    WriteU64(initialStateGroup, "count", provenance.InitialState.size());
    for (std::size_t i = 0; i < provenance.InitialState.size(); ++i) {
        auto entry = initialStateGroup.createGroup(IndexedName(i));
        WriteStringAttribute(entry, "definition_id", provenance.InitialState[i].DefinitionId);
        WriteF64(entry, "value", static_cast<double>(provenance.InitialState[i].Value));
    }
}

auto ReadScalarEntries(const H5::Group& runtimeGroup, FArtifactRunProvenanceV2& provenance) -> void {
    RequireLink(runtimeGroup, "scalar_bindings");
    auto bindingsGroup = runtimeGroup.openGroup("scalar_bindings");
    const auto bindingCount = ReadU64(bindingsGroup, "count");
    provenance.ScalarBindings.reserve(CheckedIndexedCount(bindingsGroup, bindingCount, "scalar binding"));
    for (std::uint64_t i = 0; i < bindingCount; ++i) {
        const auto entryName = IndexedName(i);
        RequireLink(bindingsGroup, entryName);
        auto entry = bindingsGroup.openGroup(entryName);
        provenance.ScalarBindings.push_back({.DefinitionId = ReadStringAttribute(entry, "definition_id"),
                                             .Value = static_cast<DevFloat>(ReadF64(entry, "value"))});
    }

    RequireLink(runtimeGroup, "initial_state");
    auto initialStateGroup = runtimeGroup.openGroup("initial_state");
    const auto initialStateCount = ReadU64(initialStateGroup, "count");
    provenance.InitialState.reserve(CheckedIndexedCount(initialStateGroup, initialStateCount, "initial state"));
    for (std::uint64_t i = 0; i < initialStateCount; ++i) {
        const auto entryName = IndexedName(i);
        RequireLink(initialStateGroup, entryName);
        auto entry = initialStateGroup.openGroup(entryName);
        provenance.InitialState.push_back({.DefinitionId = ReadStringAttribute(entry, "definition_id"),
                                           .Value = static_cast<DevFloat>(ReadF64(entry, "value"))});
    }
}

auto WriteArtifact(H5::Group& roleGroup, const std::uint64_t index,
                   const FScalarTimeSeriesArtifactV2& artifact) -> void {
    auto artifactGroup = roleGroup.createGroup(IndexedName(index));
    WriteStringAttribute(artifactGroup, "artifact_type", "scalar_time_series");
    WriteStringAttribute(artifactGroup, "role", ToString(artifact.Role));
    WriteStringAttribute(artifactGroup, "definition_id", artifact.DefinitionId);
    WriteStringAttribute(artifactGroup, "display_label", artifact.DisplayLabel);
    WriteStringAttribute(artifactGroup, "canonical_notation", artifact.CanonicalNotation);

    std::vector<std::uint64_t> steps;
    std::vector<double> simulationTimes;
    std::vector<std::uint8_t> simulationTimeValidity;
    std::vector<double> wallClockSeconds;
    std::vector<double> values;
    std::vector<std::uint8_t> reasons;
    std::vector<std::uint64_t> publishedVersions;
    const auto count = artifact.Samples.size();
    steps.reserve(count);
    simulationTimes.reserve(count);
    simulationTimeValidity.reserve(count);
    wallClockSeconds.reserve(count);
    values.reserve(count);
    reasons.reserve(count);
    publishedVersions.reserve(count);

    for (const auto& sample : artifact.Samples) {
        steps.push_back(sample.Step);
        simulationTimes.push_back(sample.SimulationTime.has_value() ? static_cast<double>(*sample.SimulationTime)
                                                                    : 0.0);
        simulationTimeValidity.push_back(sample.SimulationTime.has_value() ? 1 : 0);
        wallClockSeconds.push_back(sample.WallClockSeconds);
        values.push_back(static_cast<double>(sample.Value));
        reasons.push_back(static_cast<std::uint8_t>(sample.Reason));
        publishedVersions.push_back(sample.PublishedVersion);
    }

    WriteVectorDataset(artifactGroup, "step", steps, H5::PredType::STD_U64LE, H5::PredType::NATIVE_UINT64);
    WriteVectorDataset(artifactGroup, "simulation_time", simulationTimes, H5::PredType::IEEE_F64LE,
                       H5::PredType::NATIVE_DOUBLE);
    WriteVectorDataset(artifactGroup, "simulation_time_valid", simulationTimeValidity, H5::PredType::STD_U8LE,
                       H5::PredType::NATIVE_UINT8);
    WriteVectorDataset(artifactGroup, "wall_clock_seconds", wallClockSeconds, H5::PredType::IEEE_F64LE,
                       H5::PredType::NATIVE_DOUBLE);
    WriteVectorDataset(artifactGroup, "value", values, H5::PredType::IEEE_F64LE, H5::PredType::NATIVE_DOUBLE);
    WriteVectorDataset(artifactGroup, "event_reason", reasons, H5::PredType::STD_U8LE, H5::PredType::NATIVE_UINT8);
    WriteVectorDataset(artifactGroup, "published_version", publishedVersions, H5::PredType::STD_U64LE,
                       H5::PredType::NATIVE_UINT64);
}

[[nodiscard]] auto ReadArtifact(const H5::Group& roleGroup, const std::uint64_t index,
                                const EArtifactRoleV2 expectedRole) -> FScalarTimeSeriesArtifactV2 {
    const auto entryName = IndexedName(index);
    RequireLink(roleGroup, entryName);
    auto artifactGroup = roleGroup.openGroup(entryName);
    if (ReadStringAttribute(artifactGroup, "artifact_type") != "scalar_time_series") {
        throw std::runtime_error("unsupported artifact type in '" + entryName + "'");
    }

    const auto parsedRole = ParseArtifactRoleV2(ReadStringAttribute(artifactGroup, "role"));
    if (!parsedRole.has_value() || *parsedRole != expectedRole) {
        throw std::runtime_error("artifact role does not match its containing group");
    }

    FScalarTimeSeriesArtifactV2 artifact;
    artifact.Role = *parsedRole;
    artifact.DefinitionId = ReadStringAttribute(artifactGroup, "definition_id");
    artifact.DisplayLabel = ReadStringAttribute(artifactGroup, "display_label");
    artifact.CanonicalNotation = ReadStringAttribute(artifactGroup, "canonical_notation");

    const auto steps = ReadVectorDataset<std::uint64_t>(artifactGroup, "step", H5::PredType::NATIVE_UINT64);
    const auto simulationTimes =
        ReadVectorDataset<double>(artifactGroup, "simulation_time", H5::PredType::NATIVE_DOUBLE);
    const auto simulationTimeValidity =
        ReadVectorDataset<std::uint8_t>(artifactGroup, "simulation_time_valid", H5::PredType::NATIVE_UINT8);
    const auto wallClockSeconds =
        ReadVectorDataset<double>(artifactGroup, "wall_clock_seconds", H5::PredType::NATIVE_DOUBLE);
    const auto values = ReadVectorDataset<double>(artifactGroup, "value", H5::PredType::NATIVE_DOUBLE);
    const auto reasons = ReadVectorDataset<std::uint8_t>(artifactGroup, "event_reason", H5::PredType::NATIVE_UINT8);
    const auto publishedVersions =
        ReadVectorDataset<std::uint64_t>(artifactGroup, "published_version", H5::PredType::NATIVE_UINT64);

    const auto count = steps.size();
    RequireSameSize(artifact.DefinitionId, count, "simulation_time", simulationTimes.size());
    RequireSameSize(artifact.DefinitionId, count, "simulation_time_valid", simulationTimeValidity.size());
    RequireSameSize(artifact.DefinitionId, count, "wall_clock_seconds", wallClockSeconds.size());
    RequireSameSize(artifact.DefinitionId, count, "value", values.size());
    RequireSameSize(artifact.DefinitionId, count, "event_reason", reasons.size());
    RequireSameSize(artifact.DefinitionId, count, "published_version", publishedVersions.size());

    artifact.Samples.reserve(count);
    for (std::size_t i = 0; i < count; ++i) {
        if (simulationTimeValidity[i] > 1) {
            throw std::runtime_error("artifact simulation_time_valid contains a value other than 0 or 1");
        }
        const auto reason = ParseArtifactEventReasonV2(reasons[i]);
        if (!reason.has_value())
            throw std::runtime_error("artifact event_reason contains an unknown value");

        artifact.Samples.push_back(
            {.Step = steps[i],
             .SimulationTime = simulationTimeValidity[i] != 0
                                   ? std::make_optional(static_cast<DevFloat>(simulationTimes[i]))
                                   : std::nullopt,
             .WallClockSeconds = wallClockSeconds[i],
             .Value = static_cast<DevFloat>(values[i]),
             .Reason = *reason,
             .PublishedVersion = publishedVersions[i]});
    }
    return artifact;
}

auto WriteArtifactRole(H5::Group& artifactsGroup, const EArtifactRoleV2 role,
                       const Vector<FScalarTimeSeriesArtifactV2>& artifacts) -> void {
    auto roleGroup = artifactsGroup.createGroup(ToString(role));
    const auto count = static_cast<std::uint64_t>(std::count_if(
        artifacts.begin(), artifacts.end(), [role](const auto& artifact) { return artifact.Role == role; }));
    WriteU64(roleGroup, "count", count);
    std::uint64_t roleIndex = 0;
    for (const auto& artifact : artifacts) {
        if (artifact.Role != role)
            continue;
        WriteArtifact(roleGroup, roleIndex++, artifact);
    }
}

auto ReadArtifactRole(const H5::Group& artifactsGroup, const EArtifactRoleV2 role,
                      Vector<FScalarTimeSeriesArtifactV2>& artifacts) -> void {
    const Str roleName = ToString(role);
    RequireLink(artifactsGroup, roleName);
    auto roleGroup = artifactsGroup.openGroup(roleName);
    const auto count = ReadU64(roleGroup, "count");
    artifacts.reserve(artifacts.size() + CheckedIndexedCount(roleGroup, count, "artifact"));
    for (std::uint64_t i = 0; i < count; ++i) {
        artifacts.push_back(ReadArtifact(roleGroup, i, role));
    }
}

[[nodiscard]] auto MakeTemporaryPath(const std::filesystem::path& destination) -> std::filesystem::path {
    static std::atomic<std::uint64_t> counter = 0;
    const auto now = std::chrono::time_point_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now())
                         .time_since_epoch()
                         .count();
    return destination.string() + ".tmp." + std::to_string(now) + "." +
           std::to_string(counter.fetch_add(1, std::memory_order_relaxed));
}

[[nodiscard]] auto HDF5ErrorMessage(const H5::Exception& exception) -> Str {
    const auto detail = exception.getDetailMsg();
    return detail.empty() ? Str("unknown HDF5 error") : detail;
}

} // namespace

auto FHDF5ArtifactStoreV2::SaveRun(const std::filesystem::path& path, const FArtifactRunV2& run) const -> FResult {
    H5::Exception::dontPrint();
    const auto validation = ValidateArtifactRunV2(run);
    if (validation.IsFailure())
        return FResult::Fail(validation.Errors());
    if (path.empty())
        return FResult::Fail("artifact destination path is empty");

    std::filesystem::path temporaryPath;
    try {
        if (std::filesystem::exists(path)) {
            return FResult::Fail("artifact destination already exists: " + path.string());
        }
        if (path.has_parent_path())
            std::filesystem::create_directories(path.parent_path());

        temporaryPath = MakeTemporaryPath(path);
        if (std::filesystem::exists(temporaryPath)) {
            return FResult::Fail("artifact temporary path unexpectedly exists: " + temporaryPath.string());
        }

        {
            H5::H5File file(temporaryPath.string(), H5F_ACC_EXCL);
            auto root = file.openGroup("/");
            WriteStringAttribute(root, "studioslab_format", FormatId);
            WriteU32(root, "schema_major", SchemaMajor);
            WriteU32(root, "schema_minor", SchemaMinor);
            WriteStringAttribute(root, "producer", "StudioSlab");

            auto runGroup = root.createGroup("run");
            WriteStringAttribute(runGroup, "run_id", run.RunId);
            WriteStringAttribute(runGroup, "model_id", run.ModelId);
            WriteStringAttribute(runGroup, "model_name", run.ModelName);
            WriteStringAttribute(runGroup, "task_name", run.TaskName);
            WriteStringAttribute(runGroup, "status", ToString(run.Status));
            WriteI64(runGroup, "created_utc_unix_ns", run.CreatedUtcUnixNanoseconds);
            WriteI64(runGroup, "started_utc_unix_ns", run.StartedUtcUnixNanoseconds);
            WriteI64(runGroup, "finished_utc_unix_ns", run.FinishedUtcUnixNanoseconds);
            WriteI64(runGroup, "exported_utc_unix_ns", run.ExportedUtcUnixNanoseconds);

            auto runtimeGroup = runGroup.createGroup("runtime");
            const auto& provenance = run.Provenance;
            WriteStringAttribute(runtimeGroup, "runtime_kind", provenance.RuntimeKind);
            WriteStringAttribute(runtimeGroup, "solver_id", provenance.SolverId);
            WriteU32(runtimeGroup, "scalar_precision_bits", provenance.ScalarPrecisionBits);
            WriteF64(runtimeGroup, "time_step", static_cast<double>(provenance.TimeStep));
            WriteStringAttribute(runtimeGroup, "run_mode", ToString(provenance.RunMode));
            WriteU8(runtimeGroup, "has_max_steps", provenance.MaxSteps.has_value() ? 1 : 0);
            if (provenance.MaxSteps.has_value())
                WriteU64(runtimeGroup, "max_steps", *provenance.MaxSteps);
            WriteU8(runtimeGroup, "has_max_simulation_time", provenance.MaxSimulationTime.has_value() ? 1 : 0);
            if (provenance.MaxSimulationTime.has_value()) {
                WriteF64(runtimeGroup, "max_simulation_time", static_cast<double>(*provenance.MaxSimulationTime));
            }
            WriteStringAttribute(runtimeGroup, "time_coordinate_definition_id", provenance.TimeCoordinateDefinitionId);
            WriteF64(runtimeGroup, "initial_time", static_cast<double>(provenance.InitialTime));
            WriteU8(runtimeGroup, "capture_state_history", provenance.bCaptureStateHistory ? 1 : 0);
            WriteU8(runtimeGroup, "capture_observable_history", provenance.bCaptureObservableHistory ? 1 : 0);
            WriteU64(runtimeGroup, "artifact_sample_interval_steps", provenance.ArtifactSampleIntervalSteps);
            WriteU8(runtimeGroup, "has_max_artifact_samples", provenance.MaxArtifactSamples.has_value() ? 1 : 0);
            if (provenance.MaxArtifactSamples.has_value()) {
                WriteU64(runtimeGroup, "max_artifact_samples", *provenance.MaxArtifactSamples);
            }
            WriteScalarEntries(runtimeGroup, provenance);

            auto artifactsGroup = root.createGroup("artifacts");
            WriteArtifactRole(artifactsGroup, EArtifactRoleV2::State, run.Artifacts);
            WriteArtifactRole(artifactsGroup, EArtifactRoleV2::Observable, run.Artifacts);
            file.flush(H5F_SCOPE_GLOBAL);
        }

        std::filesystem::rename(temporaryPath, path);
        return FResult::Ok();
    } catch (const H5::Exception& exception) {
        std::error_code ignored;
        if (!temporaryPath.empty())
            std::filesystem::remove(temporaryPath, ignored);
        return FResult::Fail("failed to save HDF5 artifact run: " + HDF5ErrorMessage(exception));
    } catch (const std::exception& exception) {
        std::error_code ignored;
        if (!temporaryPath.empty())
            std::filesystem::remove(temporaryPath, ignored);
        return FResult::Fail("failed to save HDF5 artifact run: " + Str(exception.what()));
    }
}

auto FHDF5ArtifactStoreV2::LoadRun(const std::filesystem::path& path) const -> TResult<FArtifactRunV2> {
    H5::Exception::dontPrint();
    if (path.empty())
        return TResult<FArtifactRunV2>::Fail("artifact source path is empty");

    try {
        if (!std::filesystem::exists(path)) {
            return TResult<FArtifactRunV2>::Fail("artifact source does not exist: " + path.string());
        }
        if (H5Fis_hdf5(path.string().c_str()) <= 0) {
            return TResult<FArtifactRunV2>::Fail("artifact source is not an HDF5 file: " + path.string());
        }

        H5::H5File file(path.string(), H5F_ACC_RDONLY);
        auto root = file.openGroup("/");
        if (ReadStringAttribute(root, "studioslab_format") != FormatId) {
            return TResult<FArtifactRunV2>::Fail("HDF5 file is not a StudioSlab artifact run");
        }
        const auto schemaMajor = ReadU32(root, "schema_major");
        const auto schemaMinor = ReadU32(root, "schema_minor");
        if (schemaMajor != SchemaMajor) {
            return TResult<FArtifactRunV2>::Fail("unsupported StudioSlab artifact schema major version: " +
                                                 std::to_string(schemaMajor));
        }

        RequireLink(root, "run");
        auto runGroup = root.openGroup("run");
        FArtifactRunV2 run;
        run.RunId = ReadStringAttribute(runGroup, "run_id");
        run.ModelId = ReadStringAttribute(runGroup, "model_id");
        run.ModelName = ReadStringAttribute(runGroup, "model_name");
        run.TaskName = ReadStringAttribute(runGroup, "task_name");
        const auto status = ParseArtifactRunStatusV2(ReadStringAttribute(runGroup, "status"));
        if (!status.has_value())
            throw std::runtime_error("artifact run has an unknown terminal status");
        run.Status = *status;
        run.CreatedUtcUnixNanoseconds = ReadI64(runGroup, "created_utc_unix_ns");
        run.StartedUtcUnixNanoseconds = ReadI64(runGroup, "started_utc_unix_ns");
        run.FinishedUtcUnixNanoseconds = ReadI64(runGroup, "finished_utc_unix_ns");
        run.ExportedUtcUnixNanoseconds = ReadI64(runGroup, "exported_utc_unix_ns");

        RequireLink(runGroup, "runtime");
        auto runtimeGroup = runGroup.openGroup("runtime");
        auto& provenance = run.Provenance;
        provenance.RuntimeKind = ReadStringAttribute(runtimeGroup, "runtime_kind");
        provenance.SolverId = ReadStringAttribute(runtimeGroup, "solver_id");
        provenance.ScalarPrecisionBits = ReadU32(runtimeGroup, "scalar_precision_bits");
        provenance.TimeStep = static_cast<DevFloat>(ReadF64(runtimeGroup, "time_step"));
        const auto runMode = ParseArtifactRunModeV2(ReadStringAttribute(runtimeGroup, "run_mode"));
        if (!runMode.has_value())
            throw std::runtime_error("artifact run has an unknown run mode");
        provenance.RunMode = *runMode;

        const auto hasMaxSteps = ReadU8(runtimeGroup, "has_max_steps");
        if (hasMaxSteps > 1)
            throw std::runtime_error("has_max_steps must be 0 or 1");
        if (hasMaxSteps != 0)
            provenance.MaxSteps = ReadU64(runtimeGroup, "max_steps");
        const auto hasMaxSimulationTime = ReadU8(runtimeGroup, "has_max_simulation_time");
        if (hasMaxSimulationTime > 1)
            throw std::runtime_error("has_max_simulation_time must be 0 or 1");
        if (hasMaxSimulationTime != 0) {
            provenance.MaxSimulationTime = static_cast<DevFloat>(ReadF64(runtimeGroup, "max_simulation_time"));
        }

        provenance.TimeCoordinateDefinitionId = ReadStringAttribute(runtimeGroup, "time_coordinate_definition_id");
        provenance.InitialTime = static_cast<DevFloat>(ReadF64(runtimeGroup, "initial_time"));
        const auto captureState = ReadU8(runtimeGroup, "capture_state_history");
        const auto captureObservable = ReadU8(runtimeGroup, "capture_observable_history");
        if (captureState > 1 || captureObservable > 1) {
            throw std::runtime_error("artifact capture flags must be 0 or 1");
        }
        provenance.bCaptureStateHistory = captureState != 0;
        provenance.bCaptureObservableHistory = captureObservable != 0;
        provenance.ArtifactSampleIntervalSteps = ReadU64(runtimeGroup, "artifact_sample_interval_steps");
        const auto hasMaxArtifactSamples = ReadU8(runtimeGroup, "has_max_artifact_samples");
        if (hasMaxArtifactSamples > 1)
            throw std::runtime_error("has_max_artifact_samples must be 0 or 1");
        if (hasMaxArtifactSamples != 0) {
            provenance.MaxArtifactSamples = ReadU64(runtimeGroup, "max_artifact_samples");
        }
        ReadScalarEntries(runtimeGroup, provenance);

        RequireLink(root, "artifacts");
        auto artifactsGroup = root.openGroup("artifacts");
        ReadArtifactRole(artifactsGroup, EArtifactRoleV2::State, run.Artifacts);
        ReadArtifactRole(artifactsGroup, EArtifactRoleV2::Observable, run.Artifacts);

        const auto validation = ValidateArtifactRunV2(run);
        if (validation.IsFailure())
            return TResult<FArtifactRunV2>::Fail(validation.Errors());

        auto result = TResult<FArtifactRunV2>::Ok(std::move(run));
        if (schemaMinor > SchemaMinor) {
            result.WithMessage("loaded newer compatible artifact schema minor version " + std::to_string(schemaMinor));
        }
        return result;
    } catch (const H5::Exception& exception) {
        return TResult<FArtifactRunV2>::Fail("failed to load HDF5 artifact run: " + HDF5ErrorMessage(exception));
    } catch (const std::exception& exception) {
        return TResult<FArtifactRunV2>::Fail("failed to load HDF5 artifact run: " + Str(exception.what()));
    }
}

} // namespace Slab::Core::Artifacts::V2
