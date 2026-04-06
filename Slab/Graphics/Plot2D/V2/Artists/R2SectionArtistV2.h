#ifndef STUDIOSLAB_R2_SECTION_ARTIST_V2_H
#define STUDIOSLAB_R2_SECTION_ARTIST_V2_H

#include "Graphics/Plot2D/V2/PlotArtistV2.h"

#include "Math/Function/R2toR/Model/R2toRFunction.h"
#include "Math/Function/RtoR2/ParametricCurve.h"

namespace Slab::Graphics::Plot2D::V2 {

    class FR2SectionArtistV2 final : public FPlotArtistV2 {
    public:
        struct FSectionEntryV2 {
            Math::RtoR2::ParametricCurve_ptr Curve = nullptr;
            PlotStyle Style = PlotStyle(White, LineStrip, false, Nil, 1.4f);
            Str Name;
            bool bVisible = true;
        };

    private:
        Math::R2toR::Function_constptr Function2D;
        Vector<FSectionEntryV2> Sections;
        CountType SampleCount = 1024;

    protected:
        [[nodiscard]] auto BuildReflectionParameterBindings() -> Vector<FPlotReflectionParameterBindingV2> override;

    public:
        explicit FR2SectionArtistV2(Math::R2toR::Function_constptr function = nullptr,
                                    CountType sampleCount = 1024);

        auto EmitDrawCommands(const FPlotFrameContextV2 &frame,
                              FPlotDrawListV2 &drawList) const -> void override;

        [[nodiscard]] auto ComputePlotBounds() const -> std::optional<RectR> override;
        [[nodiscard]] auto GetArtistTypeId() const -> Str override;

        auto SetFunction(Math::R2toR::Function_constptr function) -> void;
        [[nodiscard]] auto GetFunction() const -> const Math::R2toR::Function_constptr &;

        auto SetSampleCount(CountType sampleCount) -> void;
        [[nodiscard]] auto GetSampleCount() const -> CountType;

        auto AddSection(const Math::RtoR2::ParametricCurve_ptr &curve,
                        PlotStyle style,
                        Str name,
                        bool bVisible = true) -> void;
        auto ClearSections() -> void;

        [[nodiscard]] auto GetSections() const -> const Vector<FSectionEntryV2> &;
        [[nodiscard]] auto GetSections() -> Vector<FSectionEntryV2> &;
    };

    DefinePointers(FR2SectionArtistV2)

} // namespace Slab::Graphics::Plot2D::V2

#endif // STUDIOSLAB_R2_SECTION_ARTIST_V2_H
