#include "ReflectionServiceV2.h"

#include "Core/Reflection/V2/LegacyInterfaceAdapterV2.h"

namespace Slab::Core::Composition::V2 {

    namespace {
        constexpr auto CLegacyReflectionSourceIdV2 = "legacy.interfaces";
    }

    FLegacyReflectionServiceV2::FLegacyReflectionServiceV2() {
        Registry.RegisterSource(Slab::Core::Reflection::V2::FReflectionCatalogSourceBindingV2{
            .SourceId = CLegacyReflectionSourceIdV2,
            .DisplayName = "Legacy Interfaces",
            .Description = "Legacy interface manager projected through Reflection V2.",
            .Refresh = [this]() {
                LegacyAdapter.RefreshFromLegacyInterfaces();
            },
            .GetCatalog = [this]() -> const Slab::Core::Reflection::V2::FReflectionCatalogV2 & {
                return LegacyAdapter.GetCatalog();
            },
            .Invoke = [this](const Str &interfaceId,
                             const Str &operationId,
                             const Slab::Core::Reflection::V2::FValueMapV2 &inputs,
                             const Slab::Core::Reflection::V2::FInvocationContextV2 &context) {
                return LegacyAdapter.Invoke(interfaceId, operationId, inputs, context);
            },
            .EncodeCurrentParameterValue = [this](const Str &interfaceId, const Str &parameterId) {
                return LegacyAdapter.EncodeCurrentParameterValue(interfaceId, parameterId);
            }
        });
    }

    auto FLegacyReflectionServiceV2::Refresh() -> void {
        Registry.RefreshAll();
    }

    auto FLegacyReflectionServiceV2::ListSources() const
        -> Vector<Slab::Core::Reflection::V2::FReflectionCatalogSourceSummaryV2> {
        return Registry.ListSources();
    }

    auto FLegacyReflectionServiceV2::GetMergedCatalog(const bool annotateSources) const
        -> Slab::Core::Reflection::V2::FReflectionCatalogV2 {
        return Registry.BuildMergedCatalog(annotateSources);
    }

    auto FLegacyReflectionServiceV2::FindSourceForInterface(const Str &interfaceId) const
        -> std::optional<Slab::Core::Reflection::V2::FReflectionCatalogSourceSummaryV2> {
        return Registry.FindSourceForInterface(interfaceId);
    }

    auto FLegacyReflectionServiceV2::Invoke(
        const Str &interfaceId,
        const Str &operationId,
        const Slab::Core::Reflection::V2::FValueMapV2 &inputs,
        const Slab::Core::Reflection::V2::FInvocationContextV2 &context) const
        -> Slab::Core::Reflection::V2::FOperationResultV2 {
        return Registry.Invoke(interfaceId, operationId, inputs, context);
    }

    auto FLegacyReflectionServiceV2::EncodeCurrentParameterValue(const Str &interfaceId, const Str &parameterId) const
        -> std::optional<Slab::Core::Reflection::V2::FReflectionValueV2> {
        return Registry.EncodeCurrentParameterValue(interfaceId, parameterId);
    }

} // namespace Slab::Core::Composition::V2
