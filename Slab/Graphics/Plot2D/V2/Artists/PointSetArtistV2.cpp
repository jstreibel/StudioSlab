#include "PointSetArtistV2.h"

#include "Core/Reflection/V2/ReflectionCodecsV2.h"

#include <algorithm>

namespace Slab::Graphics::Plot2D::V2 {

    namespace ReflectionV2 = Slab::Core::Reflection::V2;

    FPointSetArtistV2::FPointSetArtistV2(Vector<FPoint2D> points, PlotStyle style)
    : FPlotArtistV2("point_set", "Point Set")
    , Points(std::move(points))
    , Style(std::move(style)) {
    }

    auto FPointSetArtistV2::BuildReflectionParameterBindings() -> Vector<FPlotReflectionParameterBindingV2> {
        auto bindings = FPlotArtistV2::BuildReflectionParameterBindings();

        FPlotReflectionParameterBindingV2 countBinding;
        countBinding.Schema.ParameterId = "point_count";
        countBinding.Schema.DisplayName = "Point Count";
        countBinding.Schema.Description = "Number of points currently held by this artist.";
        countBinding.Schema.TypeId = ReflectionV2::CTypeIdScalarInt32;
        countBinding.Schema.Mutability = ReflectionV2::EParameterMutability::Const;
        countBinding.Schema.Exposure = ReflectionV2::EParameterExposure::ReadOnlyExposed;
        countBinding.ReadCurrent = [this] { return ReflectionV2::MakeIntValue(static_cast<int>(Points.size())); };
        bindings.push_back(std::move(countBinding));

        FPlotReflectionParameterBindingV2 thicknessBinding;
        thicknessBinding.Schema.ParameterId = "line_thickness";
        thicknessBinding.Schema.DisplayName = "Line Thickness";
        thicknessBinding.Schema.Description = "Line thickness used for polyline/point rendering.";
        thicknessBinding.Schema.TypeId = ReflectionV2::CTypeIdScalarFloat64;
        thicknessBinding.Schema.Mutability = ReflectionV2::EParameterMutability::RuntimeMutable;
        thicknessBinding.Schema.Exposure = ReflectionV2::EParameterExposure::WritableExposed;
        thicknessBinding.ReadCurrent = [this] { return ReflectionV2::MakeFloatValue(Style.thickness); };
        thicknessBinding.WriteLiveValue = [this](const ReflectionV2::FReflectionValueV2 &value) {
            try {
                Style.thickness = std::max<DevFloat>(0.1, std::stod(value.Encoded));
            } catch (const std::exception &e) {
                return ReflectionV2::FOperationResultV2::Error(
                    "plot2d_v2.parameter.parse_float",
                    "Could not parse float for parameter 'line_thickness': " + Str(e.what()));
            }

            return ReflectionV2::FOperationResultV2::Ok();
        };
        bindings.push_back(std::move(thicknessBinding));

        auto addColorBinding = [this, &bindings](const Str &component, float FColor::*member) {
            FPlotReflectionParameterBindingV2 binding;
            binding.Schema.ParameterId = "line_color_" + component;
            binding.Schema.DisplayName = "Line Color " + component;
            binding.Schema.Description = "Line color component in [0, 1].";
            binding.Schema.TypeId = ReflectionV2::CTypeIdScalarFloat64;
            binding.Schema.Mutability = ReflectionV2::EParameterMutability::RuntimeMutable;
            binding.Schema.Exposure = ReflectionV2::EParameterExposure::WritableExposed;
            binding.Schema.Minimum = 0.0;
            binding.Schema.Maximum = 1.0;
            binding.ReadCurrent = [this, member] {
                return ReflectionV2::MakeFloatValue(Style.lineColor.*member);
            };
            binding.WriteLiveValue = [this, member, component](const ReflectionV2::FReflectionValueV2 &value) {
                try {
                    const auto parsed = std::clamp<DevFloat>(std::stod(value.Encoded), 0.0, 1.0);
                    Style.lineColor.*member = static_cast<float>(parsed);
                } catch (const std::exception &e) {
                    return ReflectionV2::FOperationResultV2::Error(
                        "plot2d_v2.parameter.parse_float",
                        "Could not parse float for parameter 'line_color_" + component + "': " + Str(e.what()));
                }

                return ReflectionV2::FOperationResultV2::Ok();
            };
            bindings.push_back(std::move(binding));
        };

        addColorBinding("r", &FColor::r);
        addColorBinding("g", &FColor::g);
        addColorBinding("b", &FColor::b);
        addColorBinding("a", &FColor::a);

        return bindings;
    }

    auto FPointSetArtistV2::EmitDrawCommands(const FPlotFrameContextV2 &frame,
                                              FPlotDrawListV2 &drawList) const -> void {
        (void) frame;
        if (Points.empty()) return;

        if (!GetLabel().empty()) {
            drawList.AddLegendEntry({GetLabel(), Style});
        }

        const auto primitive = Style.getPrimitive();

        if (primitive == Slab::Graphics::Points) {
            FPointSetCommandV2 pointsCommand;
            pointsCommand.Style = Style;
            pointsCommand.Points = Points;
            drawList.AddPointSet(std::move(pointsCommand));
            return;
        }

        if (primitive == Slab::Graphics::PlottingVerticalLinesWithCircles) {
            FPointSetCommandV2 pointsCommand;
            pointsCommand.Style = Style;
            pointsCommand.Points = Points;
            drawList.AddPointSet(std::move(pointsCommand));

            for (const auto &point : Points) {
                FPolylineCommandV2 segment;
                segment.Style = Style;
                segment.Points = {
                    FPoint2D{point.x, frame.PlotRegion.yMin},
                    point
                };
                drawList.AddPolyline(std::move(segment));
            }
            return;
        }

        FPolylineCommandV2 polyline;
        polyline.Style = Style;
        polyline.Points = Points;
        polyline.bClosed = primitive == Slab::Graphics::LineLoop;
        drawList.AddPolyline(std::move(polyline));
    }

    auto FPointSetArtistV2::ComputePlotBounds() const -> std::optional<RectR> {
        if (Points.empty()) return std::nullopt;

        RectR bounds{Points.front().x, Points.front().x, Points.front().y, Points.front().y};

        for (const auto &point : Points) {
            bounds.xMin = std::min(bounds.xMin, point.x);
            bounds.xMax = std::max(bounds.xMax, point.x);
            bounds.yMin = std::min(bounds.yMin, point.y);
            bounds.yMax = std::max(bounds.yMax, point.y);
        }

        return bounds;
    }

    auto FPointSetArtistV2::GetArtistTypeId() const -> Str {
        return "point_set";
    }

    auto FPointSetArtistV2::SetPoints(Vector<FPoint2D> points) -> void {
        Points = std::move(points);
    }

    auto FPointSetArtistV2::SetStyle(PlotStyle style) -> void {
        Style = std::move(style);
    }

    auto FPointSetArtistV2::GetPoints() const -> const Vector<FPoint2D> & {
        return Points;
    }

    auto FPointSetArtistV2::GetStyle() const -> const PlotStyle & {
        return Style;
    }

} // namespace Slab::Graphics::Plot2D::V2
