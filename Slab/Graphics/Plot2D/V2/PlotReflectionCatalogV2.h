#ifndef STUDIOSLAB_PLOT_REFLECTION_CATALOG_V2_H
#define STUDIOSLAB_PLOT_REFLECTION_CATALOG_V2_H

#include "Plot2DWindowV2.h"

#include "Core/Reflection/V2/ReflectionCodecsV2.h"
#include "Core/Reflection/V2/ReflectionInvokeV2.h"

namespace Slab::Graphics::Plot2D::V2 {

    class FPlotReflectionCatalogV2 {
        struct FBoundParameter {
            FParameterSchemaV2 Schema;
            std::optional<FReflectionValueV2> PendingRestartValue;
            std::function<FReflectionValueV2()> ReadCurrent;
            std::function<FOperationResultV2(const FReflectionValueV2 &)> WriteLiveValue;
        };

        struct FBoundInterface {
            FInterfaceSchemaV2 Schema;
            std::map<Str, FBoundParameter> ParametersById;
        };

        FReflectionCatalogV2 Catalog;
        std::map<Str, FBoundInterface> InterfacesById;

        [[nodiscard]] auto CapturePendingValues() const -> std::map<Str, FReflectionValueV2>;
        static auto MakePendingKey(const Str &interfaceId, const Str &parameterId) -> Str;

        auto RebuildCatalogVector() -> void;
        auto AddEntityDescriptor(FPlotEntityReflectionDescriptorV2 descriptor,
                                 const std::map<Str, FReflectionValueV2> &previousPending) -> void;

        auto BuildDefaultOperations(const Str &interfaceId) -> Vector<FOperationSchemaV2>;

        auto HandleQueryListParameters(const Str &interfaceId, const FValueMapV2 &inputs) -> FOperationResultV2;
        auto HandleQueryGetParameter(const Str &interfaceId, const FValueMapV2 &inputs) -> FOperationResultV2;
        auto HandleCommandSetParameter(const Str &interfaceId, const FValueMapV2 &inputs) -> FOperationResultV2;
        auto HandleCommandApplyPending(const Str &interfaceId, const FValueMapV2 &inputs) -> FOperationResultV2;

        auto FindInterfaceBinding(const Str &interfaceId) -> FBoundInterface *;
        auto FindInterfaceBinding(const Str &interfaceId) const -> const FBoundInterface *;

        auto FindParameterBinding(const Str &interfaceId, const Str &parameterId) -> FBoundParameter *;
        auto FindParameterBinding(const Str &interfaceId, const Str &parameterId) const -> const FBoundParameter *;

        static auto ReadStringInput(const FValueMapV2 &inputs, const Str &key) -> std::optional<Str>;
        static auto ReadValueInput(const FValueMapV2 &inputs, const Str &key) -> std::optional<FReflectionValueV2>;

    public:
        auto RefreshFromLiveWindows() -> void;

        [[nodiscard]] auto GetCatalog() const -> const FReflectionCatalogV2 &;
        [[nodiscard]] auto GetInterface(const Str &interfaceId) const -> const FInterfaceSchemaV2 *;
        [[nodiscard]] auto GetParameter(const Str &interfaceId, const Str &parameterId) const -> const FParameterSchemaV2 *;

        auto Invoke(const Str &interfaceId,
                    const Str &operationId,
                    const FValueMapV2 &inputs,
                    const FInvocationContextV2 &context) -> FOperationResultV2;

        [[nodiscard]] auto EncodeCurrentParameterValue(const Str &interfaceId,
                                                       const Str &parameterId) const -> std::optional<FReflectionValueV2>;
    };

} // namespace Slab::Graphics::Plot2D::V2

#endif // STUDIOSLAB_PLOT_REFLECTION_CATALOG_V2_H
