#ifndef STUDIOSLAB_REFLECTION_SERVICE_V2_H
#define STUDIOSLAB_REFLECTION_SERVICE_V2_H

#include "Core/Reflection/V2/LegacyInterfaceAdapterV2.h"
#include "Core/Reflection/V2/ReflectionCatalogRegistryV2.h"
#include "Utils/Pointer.h"

namespace Slab::Core::Composition::V2 {

    class IReflectionServiceV2 {
    public:
        virtual ~IReflectionServiceV2() = default;

        virtual auto Refresh() -> void = 0;

        [[nodiscard]] virtual auto ListSources() const
            -> Vector<Slab::Core::Reflection::V2::FReflectionCatalogSourceSummaryV2> = 0;
        [[nodiscard]] virtual auto GetMergedCatalog(bool annotateSources = true) const
            -> Slab::Core::Reflection::V2::FReflectionCatalogV2 = 0;
        [[nodiscard]] virtual auto FindSourceForInterface(const Str &interfaceId) const
            -> std::optional<Slab::Core::Reflection::V2::FReflectionCatalogSourceSummaryV2> = 0;

        virtual auto Invoke(const Str &interfaceId,
                            const Str &operationId,
                            const Slab::Core::Reflection::V2::FValueMapV2 &inputs,
                            const Slab::Core::Reflection::V2::FInvocationContextV2 &context) const
            -> Slab::Core::Reflection::V2::FOperationResultV2 = 0;

        [[nodiscard]] virtual auto EncodeCurrentParameterValue(const Str &interfaceId, const Str &parameterId) const
            -> std::optional<Slab::Core::Reflection::V2::FReflectionValueV2> = 0;
    };

    DefinePointers(IReflectionServiceV2)

    class FLegacyReflectionServiceV2 final : public IReflectionServiceV2 {
    public:
        FLegacyReflectionServiceV2();

        auto Refresh() -> void override;

        [[nodiscard]] auto ListSources() const
            -> Vector<Slab::Core::Reflection::V2::FReflectionCatalogSourceSummaryV2> override;
        [[nodiscard]] auto GetMergedCatalog(bool annotateSources = true) const
            -> Slab::Core::Reflection::V2::FReflectionCatalogV2 override;
        [[nodiscard]] auto FindSourceForInterface(const Str &interfaceId) const
            -> std::optional<Slab::Core::Reflection::V2::FReflectionCatalogSourceSummaryV2> override;

        auto Invoke(const Str &interfaceId,
                    const Str &operationId,
                    const Slab::Core::Reflection::V2::FValueMapV2 &inputs,
                    const Slab::Core::Reflection::V2::FInvocationContextV2 &context) const
            -> Slab::Core::Reflection::V2::FOperationResultV2 override;

        [[nodiscard]] auto EncodeCurrentParameterValue(const Str &interfaceId, const Str &parameterId) const
            -> std::optional<Slab::Core::Reflection::V2::FReflectionValueV2> override;

    private:
        Slab::Core::Reflection::V2::FReflectionCatalogRegistryV2 Registry;
        Slab::Core::Reflection::V2::FLegacyReflectionCatalogAdapterV2 LegacyAdapter;
    };

    DefinePointers(FLegacyReflectionServiceV2)

} // namespace Slab::Core::Composition::V2

#endif // STUDIOSLAB_REFLECTION_SERVICE_V2_H
