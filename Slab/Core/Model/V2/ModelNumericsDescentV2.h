#ifndef STUDIOSLAB_MODEL_NUMERICS_DESCENT_V2_H
#define STUDIOSLAB_MODEL_NUMERICS_DESCENT_V2_H

#include "ModelRealizationRuntimeV2.h"

#include <algorithm>
#include <cerrno>
#include <cmath>
#include <cstdlib>
#include <map>
#include <optional>
#include <utility>

namespace Slab::Core::Model::V2 {

    struct FScalarRuntimeBindingParseResultV2 {
        bool bProvided = false;
        bool bValid = false;
        DevFloat Value = 0.0;
        Str Message;
    };

    inline auto ParseScalarRuntimeBindingDraftV2(const Str &draft) -> FScalarRuntimeBindingParseResultV2 {
        FScalarRuntimeBindingParseResultV2 result;
        const auto trimmed = TrimAsciiCopyV2(draft);
        result.bProvided = !trimmed.empty();
        if (!result.bProvided) {
            result.Message = "missing";
            return result;
        }

        errno = 0;
        char *end = nullptr;
        const auto parsedValue = std::strtod(trimmed.c_str(), &end);
        if (end == trimmed.c_str() || end == nullptr || *end != '\0' || errno == ERANGE || !std::isfinite(parsedValue)) {
            result.Message = "invalid scalar literal";
            return result;
        }

        result.bValid = true;
        result.Value = static_cast<DevFloat>(parsedValue);
        result.Message = "ready";
        return result;
    }

    inline auto IsODEExplicitFirstOrderSeedLaunchSupportedV2(const FModelV2 &model) -> bool {
        return model.ModelId == "model.harmonic_oscillator" ||
            model.ModelId == "model.damped_harmonic_oscillator";
    }

    inline auto GetDefaultODEExplicitFirstOrderBindingDraftV2(const FModelV2 &model,
                                                              const Str &definitionId) -> Str {
        if (!IsODEExplicitFirstOrderSeedLaunchSupportedV2(model)) return {};
        if (definitionId == "param.m") return "1";
        if (definitionId == "param.k") return "1";
        if (definitionId == "param.x0") return "1";
        if (definitionId == "param.p0") return "0";
        if (definitionId == "param.gamma") return "0.15";
        return {};
    }

    inline auto MakeODEExplicitFirstOrderRuntimeConfigV2(
        const DevFloat timeStep,
        TOptional<UIntBig> maxSteps,
        const UIntBig artifactSampleIntervalSteps,
        TOptional<UIntBig> maxArtifactSamples,
        std::map<Str, DevFloat> scalarBindingsByDefinitionId) -> FODEExplicitFirstOrderRuntimeConfigV2 {
        FODEExplicitFirstOrderRuntimeConfigV2 config;
        config.TimeStep = timeStep;
        config.MaxSteps = std::move(maxSteps);
        config.ArtifactSampleIntervalSteps = std::max<UIntBig>(UIntBig(1), artifactSampleIntervalSteps);
        config.MaxArtifactSamples = maxArtifactSamples.has_value()
            ? std::make_optional(std::max<UIntBig>(UIntBig(1), *maxArtifactSamples))
            : std::nullopt;
        config.ScalarBindingsByDefinitionId = std::move(scalarBindingsByDefinitionId);
        return config;
    }

    inline auto FindODETimeSeriesArtifactByDefinitionIdV2(const Vector<FODETimeSeriesArtifactV2> &artifacts,
                                                          const Str &definitionId) -> const FODETimeSeriesArtifactV2 * {
        const auto it = std::find_if(artifacts.begin(), artifacts.end(), [&](const auto &artifact) {
            return artifact.DefinitionId == definitionId;
        });
        if (it == artifacts.end()) return nullptr;
        return &(*it);
    }

    inline auto FindODETimeSeriesArtifactByDefinitionIdV2(
        const FODEExplicitFirstOrderRuntimeBuildResultV2 &runtime,
        const Str &definitionId) -> const FODETimeSeriesArtifactV2 * {
        if (const auto *artifact = FindODETimeSeriesArtifactByDefinitionIdV2(runtime.ObservableArtifacts, definitionId);
            artifact != nullptr) {
            return artifact;
        }
        return FindODETimeSeriesArtifactByDefinitionIdV2(runtime.StateArtifacts, definitionId);
    }

    inline auto FindFirstODETimeSeriesArtifactDefinitionIdV2(
        const FODEExplicitFirstOrderRuntimeBuildResultV2 &runtime) -> Str {
        if (!runtime.ObservableArtifacts.empty()) return runtime.ObservableArtifacts.front().DefinitionId;
        if (!runtime.StateArtifacts.empty()) return runtime.StateArtifacts.front().DefinitionId;
        return {};
    }

    inline auto FindODETimeSeriesArtifactDisplayLabelV2(
        const FODEExplicitFirstOrderRuntimeBuildResultV2 &runtime,
        const Str &definitionId) -> Str {
        if (const auto *artifact = FindODETimeSeriesArtifactByDefinitionIdV2(runtime, definitionId);
            artifact != nullptr) {
            return artifact->DisplayLabel.empty() ? artifact->DefinitionId : artifact->DisplayLabel;
        }
        return definitionId;
    }

    inline auto IsODEObservableTimeSeriesArtifactV2(
        const FODEExplicitFirstOrderRuntimeBuildResultV2 &runtime,
        const Str &definitionId) -> bool {
        return FindODETimeSeriesArtifactByDefinitionIdV2(runtime.ObservableArtifacts, definitionId) != nullptr;
    }

} // namespace Slab::Core::Model::V2

#endif // STUDIOSLAB_MODEL_NUMERICS_DESCENT_V2_H
