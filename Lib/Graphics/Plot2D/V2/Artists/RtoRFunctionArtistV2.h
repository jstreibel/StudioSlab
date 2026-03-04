#ifndef STUDIOSLAB_RTOR_FUNCTION_ARTIST_V2_H
#define STUDIOSLAB_RTOR_FUNCTION_ARTIST_V2_H

#include "Graphics/Plot2D/V2/PlotArtistV2.h"

#include "Math/Function/RtoR/Model/RtoRFunction.h"

namespace Slab::Graphics::Plot2D::V2 {

    class FRtoRFunctionArtistV2 final : public FPlotArtistV2 {
        Math::RtoR::Function_ptr Function;
        PlotStyle Style;
        CountType SampleCount = 1024;
        DevFloat DomainXMin = -1.0;
        DevFloat DomainXMax = 1.0;

    protected:
        [[nodiscard]] auto BuildReflectionParameterBindings() -> Vector<FPlotReflectionParameterBindingV2> override;

    public:
        explicit FRtoRFunctionArtistV2(Math::RtoR::Function_ptr function = nullptr,
                                       PlotStyle style = PlotStyle(White, LineStrip, false, Nil, 1.4f),
                                       CountType sampleCount = 1024,
                                       DevFloat domainXMin = -1.0,
                                       DevFloat domainXMax = 1.0);

        auto EmitDrawCommands(const FPlotFrameContextV2 &frame,
                              FPlotDrawListV2 &drawList) const -> void override;

        [[nodiscard]] auto ComputePlotBounds() const -> std::optional<RectR> override;
        [[nodiscard]] auto GetArtistTypeId() const -> Str override;

        auto SetFunction(Math::RtoR::Function_ptr function) -> void;
        auto SetStyle(PlotStyle style) -> void;
        auto SetSampleCount(CountType sampleCount) -> void;
        auto SetDomain(DevFloat xMin, DevFloat xMax) -> void;

        [[nodiscard]] auto GetFunction() const -> const Math::RtoR::Function_ptr &;
        [[nodiscard]] auto GetStyle() const -> const PlotStyle &;
        [[nodiscard]] auto GetSampleCount() const -> CountType;
        [[nodiscard]] auto GetDomainXMin() const -> DevFloat;
        [[nodiscard]] auto GetDomainXMax() const -> DevFloat;
    };

    DefinePointers(FRtoRFunctionArtistV2)

} // namespace Slab::Graphics::Plot2D::V2

#endif // STUDIOSLAB_RTOR_FUNCTION_ARTIST_V2_H
