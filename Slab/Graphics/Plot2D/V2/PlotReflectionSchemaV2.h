#ifndef STUDIOSLAB_PLOT_REFLECTION_SCHEMA_V2_H
#define STUDIOSLAB_PLOT_REFLECTION_SCHEMA_V2_H

#include "Core/Reflection/V2/ReflectionTypesV2.h"

namespace Slab::Graphics::Plot2D::V2 {

    using Slab::Core::Reflection::V2::FInvocationContextV2;
    using Slab::Core::Reflection::V2::FInterfaceSchemaV2;
    using Slab::Core::Reflection::V2::FReflectionCatalogV2;
    using Slab::Core::Reflection::V2::FOperationResultV2;
    using Slab::Core::Reflection::V2::FOperationSchemaV2;
    using Slab::Core::Reflection::V2::FParameterSchemaV2;
    using Slab::Core::Reflection::V2::FReflectionValueV2;
    using Slab::Core::Reflection::V2::FValueMapV2;

    inline constexpr auto CPlotOperationIdQueryListParametersV2 = "query.parameters";
    inline constexpr auto CPlotOperationIdQueryGetParameterV2 = "query.parameter.get";
    inline constexpr auto CPlotOperationIdCommandSetParameterV2 = "command.parameter.set";
    inline constexpr auto CPlotOperationIdCommandApplyPendingV2 = "command.parameter.apply_pending";
    inline constexpr auto CPlotOperationIdCommandArtistDoubleSampleCountV2 = "command.artist.double_sample_count";

    struct FPlotReflectionParameterBindingV2 {
        FParameterSchemaV2 Schema;
        std::function<FReflectionValueV2()> ReadCurrent;
        std::function<FOperationResultV2(const FReflectionValueV2 &)> WriteLiveValue;
    };

    struct FPlotEntityReflectionDescriptorV2 {
        Str InterfaceId;
        Str DisplayName;
        Str Description;
        int Version = 1;

        Vector<FPlotReflectionParameterBindingV2> Parameters;
        Vector<FOperationSchemaV2> Operations;
        Vector<Str> Children;
        Vector<Str> Tags;
    };

    class IPlotReflectableEntityV2 {
    public:
        virtual ~IPlotReflectableEntityV2() = default;

        virtual auto DescribeReflection() -> FPlotEntityReflectionDescriptorV2 = 0;
    };

} // namespace Slab::Graphics::Plot2D::V2

#endif // STUDIOSLAB_PLOT_REFLECTION_SCHEMA_V2_H
