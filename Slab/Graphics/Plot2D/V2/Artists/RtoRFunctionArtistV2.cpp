#include "RtoRFunctionArtistV2.h"

#include "Core/Reflection/V2/ReflectionCodecsV2.h"

#include <algorithm>

namespace Slab::Graphics::Plot2D::V2 {

    namespace ReflectionV2 = Slab::Core::Reflection::V2;

    FRtoRFunctionArtistV2::FRtoRFunctionArtistV2(Math::RtoR::Function_ptr function,
                                                 PlotStyle style,
                                                 const CountType sampleCount,
                                                 const DevFloat domainXMin,
                                                 const DevFloat domainXMax)
    : FPlotArtistV2("r_to_r_function", "f(x)")
    , Function(std::move(function))
    , Style(std::move(style))
    , SampleCount(std::max<CountType>(2, sampleCount))
    , DomainXMin(std::min(domainXMin, domainXMax))
    , DomainXMax(std::max(domainXMin, domainXMax)) {
    }

    auto FRtoRFunctionArtistV2::BuildReflectionParameterBindings() -> Vector<FPlotReflectionParameterBindingV2> {
        auto bindings = FPlotArtistV2::BuildReflectionParameterBindings();

        FPlotReflectionParameterBindingV2 sampleBinding;
        sampleBinding.Schema.ParameterId = "sample_count";
        sampleBinding.Schema.DisplayName = "Sample Count";
        sampleBinding.Schema.Description = "Number of samples used to discretize f(x).";
        sampleBinding.Schema.TypeId = ReflectionV2::CTypeIdScalarInt32;
        sampleBinding.Schema.Mutability = ReflectionV2::EParameterMutability::RuntimeMutable;
        sampleBinding.Schema.Exposure = ReflectionV2::EParameterExposure::WritableExposed;
        sampleBinding.ReadCurrent = [this] { return ReflectionV2::MakeIntValue(static_cast<int>(SampleCount)); };
        sampleBinding.WriteLiveValue = [this](const ReflectionV2::FReflectionValueV2 &value) {
            try {
                const auto parsed = std::max(2, std::stoi(value.Encoded));
                SampleCount = static_cast<CountType>(parsed);
            } catch (const std::exception &e) {
                return ReflectionV2::FOperationResultV2::Error(
                    "plot2d_v2.parameter.parse_int",
                    "Could not parse integer for parameter 'sample_count': " + Str(e.what()));
            }
            return ReflectionV2::FOperationResultV2::Ok();
        };
        bindings.push_back(std::move(sampleBinding));

        auto makeDomainBinding = [this](const Str &parameterId,
                                        const Str &displayName,
                                        DevFloat *target,
                                        bool bMin) {
            FPlotReflectionParameterBindingV2 binding;
            binding.Schema.ParameterId = parameterId;
            binding.Schema.DisplayName = displayName;
            binding.Schema.Description = "Function sampling domain boundary.";
            binding.Schema.TypeId = ReflectionV2::CTypeIdScalarFloat64;
            binding.Schema.Mutability = ReflectionV2::EParameterMutability::RuntimeMutable;
            binding.Schema.Exposure = ReflectionV2::EParameterExposure::WritableExposed;
            binding.ReadCurrent = [target] { return ReflectionV2::MakeFloatValue(*target); };
            binding.WriteLiveValue = [this, target, bMin, parameterId](const ReflectionV2::FReflectionValueV2 &value) {
                DevFloat parsed = 0.0;
                try {
                    parsed = std::stod(value.Encoded);
                } catch (const std::exception &e) {
                    return ReflectionV2::FOperationResultV2::Error(
                        "plot2d_v2.parameter.parse_float",
                        "Could not parse float for parameter '" + parameterId + "': " + Str(e.what()));
                }

                *target = parsed;
                if (bMin && DomainXMin > DomainXMax) std::swap(DomainXMin, DomainXMax);
                if (!bMin && DomainXMin > DomainXMax) std::swap(DomainXMin, DomainXMax);
                return ReflectionV2::FOperationResultV2::Ok();
            };
            return binding;
        };

        bindings.push_back(makeDomainBinding("domain_x_min", "Domain X Min", &DomainXMin, true));
        bindings.push_back(makeDomainBinding("domain_x_max", "Domain X Max", &DomainXMax, false));

        FPlotReflectionParameterBindingV2 hasFunctionBinding;
        hasFunctionBinding.Schema.ParameterId = "has_function";
        hasFunctionBinding.Schema.DisplayName = "Has Function";
        hasFunctionBinding.Schema.Description = "Whether this artist has a function bound.";
        hasFunctionBinding.Schema.TypeId = ReflectionV2::CTypeIdScalarBool;
        hasFunctionBinding.Schema.Mutability = ReflectionV2::EParameterMutability::Const;
        hasFunctionBinding.Schema.Exposure = ReflectionV2::EParameterExposure::ReadOnlyExposed;
        hasFunctionBinding.ReadCurrent = [this] { return ReflectionV2::MakeBoolValue(Function != nullptr); };
        bindings.push_back(std::move(hasFunctionBinding));

        return bindings;
    }

    auto FRtoRFunctionArtistV2::EmitDrawCommands(const FPlotFrameContextV2 &frame,
                                                  FPlotDrawListV2 &drawList) const -> void {
        if (Function == nullptr || SampleCount < 2) return;

        Vector<FPoint2D> points;
        points.reserve(SampleCount);

        const auto xMin = DomainXMin;
        const auto xMax = DomainXMax;
        const auto n = static_cast<DevFloat>(SampleCount - 1);

        for (CountType i = 0; i < SampleCount; ++i) {
            const auto tx = static_cast<DevFloat>(i) / n;
            const auto x = xMin + tx * (xMax - xMin);

            DevFloat y = 0.0;
            try {
                y = (*Function)(x);
            } catch (...) {
                y = 0.0;
            }

            points.push_back({x, y});
        }

        if (!GetLabel().empty()) {
            drawList.AddLegendEntry({GetLabel(), Style});
        }

        if (Style.getPrimitive() == Slab::Graphics::Points) {
            FPointSetCommandV2 pointSet;
            pointSet.Style = Style;
            pointSet.Points = points;
            drawList.AddPointSet(std::move(pointSet));
        }

        FPolylineCommandV2 polyline;
        polyline.Style = Style;
        polyline.Points = std::move(points);
        polyline.bClosed = Style.getPrimitive() == Slab::Graphics::LineLoop;
        drawList.AddPolyline(std::move(polyline));

        (void) frame;
    }

    auto FRtoRFunctionArtistV2::ComputePlotBounds() const -> std::optional<RectR> {
        if (Function == nullptr || SampleCount < 2) return std::nullopt;

        RectR bounds{DomainXMin, DomainXMax, 0.0, 0.0};
        bool bFirst = true;

        const auto n = static_cast<DevFloat>(SampleCount - 1);
        for (CountType i = 0; i < SampleCount; ++i) {
            const auto tx = static_cast<DevFloat>(i) / n;
            const auto x = DomainXMin + tx * (DomainXMax - DomainXMin);

            DevFloat y = 0.0;
            try {
                y = (*Function)(x);
            } catch (...) {
                continue;
            }

            if (bFirst) {
                bounds.yMin = y;
                bounds.yMax = y;
                bFirst = false;
            } else {
                bounds.yMin = std::min(bounds.yMin, y);
                bounds.yMax = std::max(bounds.yMax, y);
            }
        }

        if (bFirst) return std::nullopt;
        return bounds;
    }

    auto FRtoRFunctionArtistV2::GetArtistTypeId() const -> Str {
        return "r_to_r_function";
    }

    auto FRtoRFunctionArtistV2::SetFunction(Math::RtoR::Function_ptr function) -> void {
        Function = std::move(function);
    }

    auto FRtoRFunctionArtistV2::SetStyle(PlotStyle style) -> void {
        Style = std::move(style);
    }

    auto FRtoRFunctionArtistV2::SetSampleCount(const CountType sampleCount) -> void {
        SampleCount = std::max<CountType>(2, sampleCount);
    }

    auto FRtoRFunctionArtistV2::SetDomain(const DevFloat xMin, const DevFloat xMax) -> void {
        DomainXMin = std::min(xMin, xMax);
        DomainXMax = std::max(xMin, xMax);
    }

    auto FRtoRFunctionArtistV2::GetFunction() const -> const Math::RtoR::Function_ptr & {
        return Function;
    }

    auto FRtoRFunctionArtistV2::GetStyle() const -> const PlotStyle & {
        return Style;
    }

    auto FRtoRFunctionArtistV2::GetSampleCount() const -> CountType {
        return SampleCount;
    }

    auto FRtoRFunctionArtistV2::GetDomainXMin() const -> DevFloat {
        return DomainXMin;
    }

    auto FRtoRFunctionArtistV2::GetDomainXMax() const -> DevFloat {
        return DomainXMax;
    }

} // namespace Slab::Graphics::Plot2D::V2
