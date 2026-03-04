#include "AxisArtistV2.h"

#include "Core/Reflection/V2/ReflectionCodecsV2.h"

namespace Slab::Graphics::Plot2D::V2 {

    namespace ReflectionV2 = Slab::Core::Reflection::V2;

    FAxisArtistV2::FAxisArtistV2()
    : FPlotArtistV2("axis", "Axis")
    , AxisStyle(PlotStyle(Grey, Lines, false, Nil, 1.6f))
    , GridStyle(PlotStyle(LightGrey.WithAlpha(0.6f), Lines, false, Nil, 1.0f)) {
        SetAffectGraphRanges(false);
    }

    auto FAxisArtistV2::BuildReflectionParameterBindings() -> Vector<FPlotReflectionParameterBindingV2> {
        auto bindings = FPlotArtistV2::BuildReflectionParameterBindings();

        FPlotReflectionParameterBindingV2 showGridBinding;
        showGridBinding.Schema.ParameterId = "show_grid";
        showGridBinding.Schema.DisplayName = "Show Grid";
        showGridBinding.Schema.Description = "Toggle major grid lines.";
        showGridBinding.Schema.TypeId = ReflectionV2::CTypeIdScalarBool;
        showGridBinding.Schema.Mutability = ReflectionV2::EParameterMutability::RuntimeMutable;
        showGridBinding.Schema.Exposure = ReflectionV2::EParameterExposure::WritableExposed;
        showGridBinding.ReadCurrent = [this] { return ReflectionV2::MakeBoolValue(bShowGrid); };
        showGridBinding.WriteLiveValue = [this](const ReflectionV2::FReflectionValueV2 &value) {
            bool parsed = false;
            if (!ReflectionV2::ParseBoolValue(value.Encoded, parsed)) {
                return ReflectionV2::FOperationResultV2::Error(
                    "plot2d_v2.parameter.parse_bool",
                    "Could not parse bool value for parameter 'show_grid'.");
            }
            bShowGrid = parsed;
            return ReflectionV2::FOperationResultV2::Ok();
        };
        bindings.push_back(std::move(showGridBinding));

        FPlotReflectionParameterBindingV2 showAxesBinding;
        showAxesBinding.Schema.ParameterId = "show_axes";
        showAxesBinding.Schema.DisplayName = "Show Axes";
        showAxesBinding.Schema.Description = "Toggle principal axes (x=0 and y=0).";
        showAxesBinding.Schema.TypeId = ReflectionV2::CTypeIdScalarBool;
        showAxesBinding.Schema.Mutability = ReflectionV2::EParameterMutability::RuntimeMutable;
        showAxesBinding.Schema.Exposure = ReflectionV2::EParameterExposure::WritableExposed;
        showAxesBinding.ReadCurrent = [this] { return ReflectionV2::MakeBoolValue(bShowAxes); };
        showAxesBinding.WriteLiveValue = [this](const ReflectionV2::FReflectionValueV2 &value) {
            bool parsed = false;
            if (!ReflectionV2::ParseBoolValue(value.Encoded, parsed)) {
                return ReflectionV2::FOperationResultV2::Error(
                    "plot2d_v2.parameter.parse_bool",
                    "Could not parse bool value for parameter 'show_axes'.");
            }
            bShowAxes = parsed;
            return ReflectionV2::FOperationResultV2::Ok();
        };
        bindings.push_back(std::move(showAxesBinding));

        FPlotReflectionParameterBindingV2 tickCountBinding;
        tickCountBinding.Schema.ParameterId = "major_tick_count";
        tickCountBinding.Schema.DisplayName = "Major Tick Count";
        tickCountBinding.Schema.Description = "Number of major grid slices per axis.";
        tickCountBinding.Schema.TypeId = ReflectionV2::CTypeIdScalarInt32;
        tickCountBinding.Schema.Mutability = ReflectionV2::EParameterMutability::RuntimeMutable;
        tickCountBinding.Schema.Exposure = ReflectionV2::EParameterExposure::WritableExposed;
        tickCountBinding.ReadCurrent = [this] { return ReflectionV2::MakeIntValue(MajorTickCount); };
        tickCountBinding.WriteLiveValue = [this](const ReflectionV2::FReflectionValueV2 &value) {
            try {
                MajorTickCount = std::max(1, std::stoi(value.Encoded));
            } catch (const std::exception &e) {
                return ReflectionV2::FOperationResultV2::Error(
                    "plot2d_v2.parameter.parse_int",
                    "Could not parse integer value for parameter 'major_tick_count': " + Str(e.what()));
            }

            return ReflectionV2::FOperationResultV2::Ok();
        };
        bindings.push_back(std::move(tickCountBinding));

        return bindings;
    }

    auto FAxisArtistV2::EmitDrawCommands(const FPlotFrameContextV2 &frame, FPlotDrawListV2 &drawList) const -> void {
        const auto &region = frame.PlotRegion;
        const auto slices = std::max(1, MajorTickCount);

        if (bShowGrid) {
            for (int i = 0; i <= slices; ++i) {
                const auto tx = static_cast<DevFloat>(i) / static_cast<DevFloat>(slices);
                const auto x = region.xMin + tx * (region.xMax - region.xMin);

                FPolylineCommandV2 vertical;
                vertical.Style = GridStyle;
                vertical.Points = {
                    FPoint2D{x, region.yMin},
                    FPoint2D{x, region.yMax}
                };
                drawList.AddPolyline(std::move(vertical));
            }

            for (int i = 0; i <= slices; ++i) {
                const auto ty = static_cast<DevFloat>(i) / static_cast<DevFloat>(slices);
                const auto y = region.yMin + ty * (region.yMax - region.yMin);

                FPolylineCommandV2 horizontal;
                horizontal.Style = GridStyle;
                horizontal.Points = {
                    FPoint2D{region.xMin, y},
                    FPoint2D{region.xMax, y}
                };
                drawList.AddPolyline(std::move(horizontal));
            }
        }

        if (bShowAxes) {
            if (region.xMin <= 0.0 && region.xMax >= 0.0) {
                FPolylineCommandV2 yAxis;
                yAxis.Style = AxisStyle;
                yAxis.Points = {
                    FPoint2D{0.0, region.yMin},
                    FPoint2D{0.0, region.yMax}
                };
                drawList.AddPolyline(std::move(yAxis));
            }

            if (region.yMin <= 0.0 && region.yMax >= 0.0) {
                FPolylineCommandV2 xAxis;
                xAxis.Style = AxisStyle;
                xAxis.Points = {
                    FPoint2D{region.xMin, 0.0},
                    FPoint2D{region.xMax, 0.0}
                };
                drawList.AddPolyline(std::move(xAxis));
            }
        }
    }

    auto FAxisArtistV2::GetArtistTypeId() const -> Str {
        return "axis";
    }

    auto FAxisArtistV2::SetAxisStyle(PlotStyle style) -> void {
        AxisStyle = std::move(style);
    }

    auto FAxisArtistV2::SetGridStyle(PlotStyle style) -> void {
        GridStyle = std::move(style);
    }

    auto FAxisArtistV2::SetShowGrid(const bool showGrid) -> void {
        bShowGrid = showGrid;
    }

    auto FAxisArtistV2::SetShowAxes(const bool showAxes) -> void {
        bShowAxes = showAxes;
    }

    auto FAxisArtistV2::SetMajorTickCount(const int tickCount) -> void {
        MajorTickCount = std::max(1, tickCount);
    }

    auto FAxisArtistV2::GetShowGrid() const -> bool {
        return bShowGrid;
    }

    auto FAxisArtistV2::GetShowAxes() const -> bool {
        return bShowAxes;
    }

    auto FAxisArtistV2::GetMajorTickCount() const -> int {
        return MajorTickCount;
    }

} // namespace Slab::Graphics::Plot2D::V2
