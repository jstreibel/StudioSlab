#include "R2SectionArtistV2.h"

#include "Core/Reflection/V2/ReflectionCodecsV2.h"

#include <algorithm>
#include <cmath>

namespace Slab::Graphics::Plot2D::V2 {

    namespace ReflectionV2 = Slab::Core::Reflection::V2;

    namespace {

        auto BuildSectionDisplayName(const std::size_t index, const Str &name, const Str &field) -> Str {
            const auto base = name.empty()
                ? Str("Section ") + ToStr(static_cast<int>(index + 1))
                : name;
            return base + " " + field;
        }

        auto BuildSectionDescription(const std::size_t index,
                                     const Str &name,
                                     const Str &description) -> Str {
            const auto base = name.empty()
                ? Str("Section ") + ToStr(static_cast<int>(index + 1))
                : name;
            return base + ": " + description;
        }

        auto TryAppendSectionPoint(const Math::R2toR::Function &function,
                                   const Math::RtoR2::ParametricCurve &curve,
                                   const DevFloat s,
                                   Vector<FPoint2D> &points,
                                   RectR *bounds) -> void {
            try {
                const auto samplePoint = curve(s);
                const auto value = function(samplePoint);
                if (!std::isfinite(samplePoint.x) || !std::isfinite(samplePoint.y) || !std::isfinite(value)) return;

                const FPoint2D point{s, value};
                points.push_back(point);

                if (bounds == nullptr) return;
                if (points.size() == 1) {
                    *bounds = {point.x, point.x, point.y, point.y};
                    return;
                }

                bounds->xMin = std::min(bounds->xMin, point.x);
                bounds->xMax = std::max(bounds->xMax, point.x);
                bounds->yMin = std::min(bounds->yMin, point.y);
                bounds->yMax = std::max(bounds->yMax, point.y);
            } catch (...) {
            }
        }

    } // namespace

    FR2SectionArtistV2::FR2SectionArtistV2(Math::R2toR::Function_constptr function,
                                           const CountType sampleCount)
    : FPlotArtistV2("r2_section", "R2 Section")
    , Function2D(std::move(function))
    , SampleCount(std::max<CountType>(2, sampleCount)) {
    }

    auto FR2SectionArtistV2::BuildReflectionParameterBindings() -> Vector<FPlotReflectionParameterBindingV2> {
        auto bindings = FPlotArtistV2::BuildReflectionParameterBindings();

        FPlotReflectionParameterBindingV2 sampleBinding;
        sampleBinding.Schema.ParameterId = "sample_count";
        sampleBinding.Schema.DisplayName = "Sample Count";
        sampleBinding.Schema.Description = "Number of samples used to project each section.";
        sampleBinding.Schema.TypeId = ReflectionV2::CTypeIdScalarInt32;
        sampleBinding.Schema.Mutability = ReflectionV2::EParameterMutability::RuntimeMutable;
        sampleBinding.Schema.Exposure = ReflectionV2::EParameterExposure::WritableExposed;
        sampleBinding.Schema.Minimum = 2.0;
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

        FPlotReflectionParameterBindingV2 sectionCountBinding;
        sectionCountBinding.Schema.ParameterId = "section_count";
        sectionCountBinding.Schema.DisplayName = "Section Count";
        sectionCountBinding.Schema.Description = "Number of configured section curves.";
        sectionCountBinding.Schema.TypeId = ReflectionV2::CTypeIdScalarInt32;
        sectionCountBinding.Schema.Mutability = ReflectionV2::EParameterMutability::Const;
        sectionCountBinding.Schema.Exposure = ReflectionV2::EParameterExposure::ReadOnlyExposed;
        sectionCountBinding.ReadCurrent = [this] { return ReflectionV2::MakeIntValue(static_cast<int>(Sections.size())); };
        bindings.push_back(std::move(sectionCountBinding));

        FPlotReflectionParameterBindingV2 hasFunctionBinding;
        hasFunctionBinding.Schema.ParameterId = "has_function";
        hasFunctionBinding.Schema.DisplayName = "Has Function";
        hasFunctionBinding.Schema.Description = "Whether this section artist currently has a source function.";
        hasFunctionBinding.Schema.TypeId = ReflectionV2::CTypeIdScalarBool;
        hasFunctionBinding.Schema.Mutability = ReflectionV2::EParameterMutability::Const;
        hasFunctionBinding.Schema.Exposure = ReflectionV2::EParameterExposure::ReadOnlyExposed;
        hasFunctionBinding.ReadCurrent = [this] { return ReflectionV2::MakeBoolValue(Function2D != nullptr); };
        bindings.push_back(std::move(hasFunctionBinding));

        for (std::size_t index = 0; index < Sections.size(); ++index) {
            const auto indexToken = ToStr(static_cast<int>(index));

            FPlotReflectionParameterBindingV2 visibleBinding;
            visibleBinding.Schema.ParameterId = "section_" + indexToken + "_visible";
            visibleBinding.Schema.DisplayName = BuildSectionDisplayName(index, Sections[index].Name, "Visible");
            visibleBinding.Schema.Description = BuildSectionDescription(index, Sections[index].Name, "Toggle section visibility.");
            visibleBinding.Schema.TypeId = ReflectionV2::CTypeIdScalarBool;
            visibleBinding.Schema.Mutability = ReflectionV2::EParameterMutability::RuntimeMutable;
            visibleBinding.Schema.Exposure = ReflectionV2::EParameterExposure::WritableExposed;
            visibleBinding.ReadCurrent = [this, index] {
                return ReflectionV2::MakeBoolValue(index < Sections.size() ? Sections[index].bVisible : false);
            };
            visibleBinding.WriteLiveValue = [this, index](const ReflectionV2::FReflectionValueV2 &value) {
                if (index >= Sections.size()) {
                    return ReflectionV2::FOperationResultV2::Error(
                        "plot2d_v2.parameter.missing_section",
                        "Requested section index is out of range.");
                }

                bool parsed = false;
                if (!ReflectionV2::ParseBoolValue(value.Encoded, parsed)) {
                    return ReflectionV2::FOperationResultV2::Error(
                        "plot2d_v2.parameter.parse_bool",
                        "Could not parse bool value for section visibility.");
                }

                Sections[index].bVisible = parsed;
                return ReflectionV2::FOperationResultV2::Ok();
            };
            bindings.push_back(std::move(visibleBinding));

            FPlotReflectionParameterBindingV2 thicknessBinding;
            thicknessBinding.Schema.ParameterId = "section_" + indexToken + "_line_thickness";
            thicknessBinding.Schema.DisplayName = BuildSectionDisplayName(index, Sections[index].Name, "Line Thickness");
            thicknessBinding.Schema.Description = BuildSectionDescription(index, Sections[index].Name, "Adjust line thickness.");
            thicknessBinding.Schema.TypeId = ReflectionV2::CTypeIdScalarFloat64;
            thicknessBinding.Schema.Mutability = ReflectionV2::EParameterMutability::RuntimeMutable;
            thicknessBinding.Schema.Exposure = ReflectionV2::EParameterExposure::WritableExposed;
            thicknessBinding.Schema.Minimum = 0.1;
            thicknessBinding.ReadCurrent = [this, index] {
                return ReflectionV2::MakeFloatValue(index < Sections.size() ? Sections[index].Style.thickness : 0.0);
            };
            thicknessBinding.WriteLiveValue = [this, index](const ReflectionV2::FReflectionValueV2 &value) {
                if (index >= Sections.size()) {
                    return ReflectionV2::FOperationResultV2::Error(
                        "plot2d_v2.parameter.missing_section",
                        "Requested section index is out of range.");
                }

                try {
                    Sections[index].Style.thickness = std::max<DevFloat>(0.1, std::stod(value.Encoded));
                } catch (const std::exception &e) {
                    return ReflectionV2::FOperationResultV2::Error(
                        "plot2d_v2.parameter.parse_float",
                        "Could not parse float value for section thickness: " + Str(e.what()));
                }

                return ReflectionV2::FOperationResultV2::Ok();
            };
            bindings.push_back(std::move(thicknessBinding));

            auto addColorBinding = [this, index, &bindings](const Str &component, float FColor::*member) {
                FPlotReflectionParameterBindingV2 colorBinding;
                colorBinding.Schema.ParameterId = "section_" + ToStr(static_cast<int>(index)) + "_line_color_" + component;
                colorBinding.Schema.DisplayName = BuildSectionDisplayName(index, Sections[index].Name, "Color " + component);
                colorBinding.Schema.Description = BuildSectionDescription(index, Sections[index].Name, "Adjust line color component.");
                colorBinding.Schema.TypeId = ReflectionV2::CTypeIdScalarFloat64;
                colorBinding.Schema.Mutability = ReflectionV2::EParameterMutability::RuntimeMutable;
                colorBinding.Schema.Exposure = ReflectionV2::EParameterExposure::WritableExposed;
                colorBinding.Schema.Minimum = 0.0;
                colorBinding.Schema.Maximum = 1.0;
                colorBinding.ReadCurrent = [this, index, member] {
                    return ReflectionV2::MakeFloatValue(
                        index < Sections.size() ? Sections[index].Style.lineColor.*member : 0.0);
                };
                colorBinding.WriteLiveValue = [this, index, member, component](const ReflectionV2::FReflectionValueV2 &value) {
                    if (index >= Sections.size()) {
                        return ReflectionV2::FOperationResultV2::Error(
                            "plot2d_v2.parameter.missing_section",
                            "Requested section index is out of range.");
                    }

                    try {
                        const auto parsed = std::clamp<DevFloat>(std::stod(value.Encoded), 0.0, 1.0);
                        Sections[index].Style.lineColor.*member = static_cast<float>(parsed);
                    } catch (const std::exception &e) {
                        return ReflectionV2::FOperationResultV2::Error(
                            "plot2d_v2.parameter.parse_float",
                            "Could not parse float value for section color '" + component + "': " + Str(e.what()));
                    }

                    return ReflectionV2::FOperationResultV2::Ok();
                };
                bindings.push_back(std::move(colorBinding));
            };

            addColorBinding("r", &FColor::r);
            addColorBinding("g", &FColor::g);
            addColorBinding("b", &FColor::b);
            addColorBinding("a", &FColor::a);
        }

        return bindings;
    }

    auto FR2SectionArtistV2::EmitDrawCommands(const FPlotFrameContextV2 &frame,
                                              FPlotDrawListV2 &drawList) const -> void {
        (void) frame;
        if (Function2D == nullptr || Sections.empty() || SampleCount < 2) return;

        const auto sampleDenominator = static_cast<DevFloat>(SampleCount - 1);

        for (const auto &section : Sections) {
            if (!section.bVisible || section.Curve == nullptr) continue;

            Vector<FPoint2D> points;
            points.reserve(SampleCount);

            const auto sMin = section.Curve->get_sMin();
            const auto sMax = section.Curve->get_sMax();

            for (CountType i = 0; i < SampleCount; ++i) {
                const auto t = static_cast<DevFloat>(i) / sampleDenominator;
                const auto s = sMin + t * (sMax - sMin);
                TryAppendSectionPoint(*Function2D, *section.Curve, s, points, nullptr);
            }

            if (points.empty()) continue;

            if (!section.Name.empty()) {
                drawList.AddLegendEntry({section.Name, section.Style});
            }

            if (section.Style.getPrimitive() == Slab::Graphics::Points) {
                FPointSetCommandV2 pointSet;
                pointSet.Style = section.Style;
                pointSet.Points = std::move(points);
                drawList.AddPointSet(std::move(pointSet));
                continue;
            }

            FPolylineCommandV2 polyline;
            polyline.Style = section.Style;
            polyline.Points = std::move(points);
            polyline.bClosed = section.Style.getPrimitive() == Slab::Graphics::LineLoop;
            drawList.AddPolyline(std::move(polyline));
        }
    }

    auto FR2SectionArtistV2::ComputePlotBounds() const -> std::optional<RectR> {
        if (Function2D == nullptr || Sections.empty() || SampleCount < 2) return std::nullopt;

        bool bHasBounds = false;
        RectR bounds{};
        const auto sampleDenominator = static_cast<DevFloat>(SampleCount - 1);

        for (const auto &section : Sections) {
            if (section.Curve == nullptr) continue;

            RectR sectionBounds{};
            Vector<FPoint2D> sampledPoints;
            sampledPoints.reserve(SampleCount);

            const auto sMin = section.Curve->get_sMin();
            const auto sMax = section.Curve->get_sMax();
            for (CountType i = 0; i < SampleCount; ++i) {
                const auto t = static_cast<DevFloat>(i) / sampleDenominator;
                const auto s = sMin + t * (sMax - sMin);
                TryAppendSectionPoint(*Function2D, *section.Curve, s, sampledPoints, &sectionBounds);
            }

            if (sampledPoints.empty()) continue;

            if (!bHasBounds) {
                bounds = sectionBounds;
                bHasBounds = true;
                continue;
            }

            bounds.xMin = std::min(bounds.xMin, sectionBounds.xMin);
            bounds.xMax = std::max(bounds.xMax, sectionBounds.xMax);
            bounds.yMin = std::min(bounds.yMin, sectionBounds.yMin);
            bounds.yMax = std::max(bounds.yMax, sectionBounds.yMax);
        }

        if (!bHasBounds) return std::nullopt;
        return bounds;
    }

    auto FR2SectionArtistV2::GetArtistTypeId() const -> Str {
        return "r2_section";
    }

    auto FR2SectionArtistV2::SetFunction(Math::R2toR::Function_constptr function) -> void {
        Function2D = std::move(function);
    }

    auto FR2SectionArtistV2::GetFunction() const -> const Math::R2toR::Function_constptr & {
        return Function2D;
    }

    auto FR2SectionArtistV2::SetSampleCount(const CountType sampleCount) -> void {
        SampleCount = std::max<CountType>(2, sampleCount);
    }

    auto FR2SectionArtistV2::GetSampleCount() const -> CountType {
        return SampleCount;
    }

    auto FR2SectionArtistV2::AddSection(const Math::RtoR2::ParametricCurve_ptr &curve,
                                        PlotStyle style,
                                        Str name,
                                        const bool bVisible) -> void {
        Sections.push_back({
            .Curve = curve,
            .Style = std::move(style),
            .Name = std::move(name),
            .bVisible = bVisible
        });
    }

    auto FR2SectionArtistV2::ClearSections() -> void {
        Sections.clear();
    }

    auto FR2SectionArtistV2::GetSections() const -> const Vector<FSectionEntryV2> & {
        return Sections;
    }

    auto FR2SectionArtistV2::GetSections() -> Vector<FSectionEntryV2> & {
        return Sections;
    }

} // namespace Slab::Graphics::Plot2D::V2
