#include "RtoRFunctionArtistV2.h"

#include "Core/Reflection/V2/ReflectionCodecsV2.h"
#include "Math/Function/RtoR/Model/FunctionsCollection/NativeFunction.h"
#include "Math/Function/RtoR/Model/FunctionsCollection/NullFunction.h"
#include "Math/Function/RtoR/Model/FunctionsCollection/Trigonometric.h"

#include <algorithm>
#include <cctype>
#include <limits>

namespace Slab::Graphics::Plot2D::V2 {

    namespace ReflectionV2 = Slab::Core::Reflection::V2;

    namespace {

        auto ToLowerAscii(Slab::Str value) -> Slab::Str {
            for (auto &ch : value) ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
            return value;
        }

        auto RemoveAsciiSpaces(Slab::Str value) -> Slab::Str {
            value.erase(std::remove_if(value.begin(), value.end(), [](const char ch) {
                return std::isspace(static_cast<unsigned char>(ch)) != 0;
            }), value.end());
            return value;
        }

        auto TryParseDomainInput(const Slab::Str &encoded,
                                 Slab::DevFloat &outXMin,
                                 Slab::DevFloat &outXMax,
                                 Slab::Str &outError) -> bool {
            const auto commaPos = encoded.find(',');
            if (commaPos != Slab::Str::npos) {
                try {
                    const auto left = encoded.substr(0, commaPos);
                    const auto right = encoded.substr(commaPos + 1);
                    outXMin = std::stod(left);
                    outXMax = std::stod(right);
                    return true;
                } catch (const std::exception &e) {
                    outError = Slab::Str("Failed to parse domain as 'x_min,x_max': ") + e.what();
                    return false;
                }
            }

            ReflectionV2::FFunctionDescriptorV2 descriptor;
            if (!ReflectionV2::DecodeFunctionDescriptorCLI(encoded, descriptor, &outError)) return false;
            outXMin = descriptor.DomainMin;
            outXMax = descriptor.DomainMax;
            return true;
        }

        auto BuildFunctionFromDescriptor(const ReflectionV2::FFunctionDescriptorV2 &descriptor,
                                         Slab::Str &outError) -> Slab::Math::RtoR::Function_ptr {
            const auto family = ToLowerAscii(descriptor.Family);
            const auto expr = RemoveAsciiSpaces(ToLowerAscii(descriptor.Expression));

            if (expr == "sin(x)" || expr == "sin" || family == "sine") {
                return Slab::New<Slab::Math::RtoR::FSine>(1.0, 1.0);
            }
            if (expr == "cos(x)" || expr == "cos" || family == "cosine") {
                return Slab::New<Slab::Math::RtoR::FCosine>(1.0, 1.0);
            }
            if (expr == "x" || family == "identity") {
                return Slab::New<Slab::Math::RtoR::NativeFunction>([](const Slab::DevFloat x) -> Slab::DevFloat {
                    return x;
                });
            }
            if (expr == "x^2" || expr == "x*x") {
                return Slab::New<Slab::Math::RtoR::NativeFunction>([](const Slab::DevFloat x) -> Slab::DevFloat {
                    return x * x;
                });
            }
            if (expr == "0" || expr == "zero" || expr == "null" || family == "null") {
                return Slab::New<Slab::Math::RtoR::NullFunction>();
            }

            outError =
                "Unsupported function descriptor. Supported expressions: sin(x), cos(x), x, x^2, 0.";
            return nullptr;
        }

    } // namespace

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

    auto FRtoRFunctionArtistV2::BuildReflectionOperations() -> Vector<FOperationSchemaV2> {
        auto operations = FPlotArtistV2::BuildReflectionOperations();

        FOperationSchemaV2 doubleSampleCount;
        doubleSampleCount.OperationId = CPlotOperationIdCommandArtistDoubleSampleCountV2;
        doubleSampleCount.DisplayName = "Double Sample Count";
        doubleSampleCount.Description = "Double function sample_count while preserving minimum bounds.";
        doubleSampleCount.Kind = ReflectionV2::EOperationKind::Command;
        doubleSampleCount.ThreadAffinity = ReflectionV2::EThreadAffinity::Any;
        doubleSampleCount.RunStatePolicy = ReflectionV2::ERunStatePolicy::Any;
        doubleSampleCount.SideEffectClass = ReflectionV2::ESideEffectClass::LocalState;
        doubleSampleCount.InvokeHandler = [this](const ReflectionV2::FValueMapV2 &inputs,
                                                 const ReflectionV2::FInvocationContextV2 &) {
            (void) inputs;

            const auto maxCount = std::numeric_limits<CountType>::max();
            const auto current = GetSampleCount();
            const auto next = current > (maxCount / 2) ? maxCount : (current * 2);
            SetSampleCount(next);

            ReflectionV2::FValueMapV2 output;
            output["artist_id"] = ReflectionV2::MakeStringValue(GetArtistId());
            output["sample_count"] = ReflectionV2::MakeStringValue(ToStr(GetSampleCount()));
            return ReflectionV2::FOperationResultV2::Ok(std::move(output));
        };
        operations.push_back(std::move(doubleSampleCount));

        FOperationSchemaV2 setDomain;
        setDomain.OperationId = CPlotOperationIdCommandArtistSetDomainV2;
        setDomain.DisplayName = "Set Domain";
        setDomain.Description =
            "Set x-domain bounds. Input 'value' accepts 'x_min,x_max' or descriptor fields "
            "'domain_min=...;domain_max=...'.";
        setDomain.Kind = ReflectionV2::EOperationKind::Command;
        setDomain.ThreadAffinity = ReflectionV2::EThreadAffinity::Any;
        setDomain.RunStatePolicy = ReflectionV2::ERunStatePolicy::Any;
        setDomain.SideEffectClass = ReflectionV2::ESideEffectClass::LocalState;
        setDomain.Inputs = {
            ReflectionV2::FOperationFieldSchemaV2{
                "value",
                "Domain Value",
                "Domain payload encoded as 'x_min,x_max' or descriptor fields with domain_min/domain_max.",
                ReflectionV2::CTypeIdScalarString,
                true
            }
        };
        setDomain.InvokeHandler = [this](const ReflectionV2::FValueMapV2 &inputs,
                                         const ReflectionV2::FInvocationContextV2 &) {
            const auto valueIt = inputs.find("value");
            if (valueIt == inputs.end()) {
                return ReflectionV2::FOperationResultV2::Error(
                    "reflection.operation.missing_input",
                    "Missing required input 'value'.");
            }

            DevFloat xMin = 0.0;
            DevFloat xMax = 0.0;
            Str parseError;
            if (!TryParseDomainInput(valueIt->second.Encoded, xMin, xMax, parseError)) {
                return ReflectionV2::FOperationResultV2::Error(
                    "plot2d_v2.operation.parse_domain",
                    "Could not parse domain payload: " + parseError);
            }

            SetDomain(xMin, xMax);

            ReflectionV2::FValueMapV2 output;
            output["artist_id"] = ReflectionV2::MakeStringValue(GetArtistId());
            output["domain_x_min"] = ReflectionV2::MakeFloatValue(GetDomainXMin());
            output["domain_x_max"] = ReflectionV2::MakeFloatValue(GetDomainXMax());
            return ReflectionV2::FOperationResultV2::Ok(std::move(output));
        };
        operations.push_back(std::move(setDomain));

        FOperationSchemaV2 setFunction;
        setFunction.OperationId = CPlotOperationIdCommandArtistSetFunctionV2;
        setFunction.DisplayName = "Set Function";
        setFunction.Description =
            "Set function from descriptor in input 'value' (family/expr/domain). "
            "Also applies descriptor domain bounds.";
        setFunction.Kind = ReflectionV2::EOperationKind::Command;
        setFunction.ThreadAffinity = ReflectionV2::EThreadAffinity::Any;
        setFunction.RunStatePolicy = ReflectionV2::ERunStatePolicy::Any;
        setFunction.SideEffectClass = ReflectionV2::ESideEffectClass::LocalState;
        setFunction.Inputs = {
            ReflectionV2::FOperationFieldSchemaV2{
                "value",
                "Function Descriptor",
                "Encoded function descriptor. Example: family=analytic;expr=sin(x);domain_min=-3.14;domain_max=3.14",
                ReflectionV2::CTypeIdFunctionRtoR,
                true
            }
        };
        setFunction.InvokeHandler = [this](const ReflectionV2::FValueMapV2 &inputs,
                                           const ReflectionV2::FInvocationContextV2 &) {
            const auto valueIt = inputs.find("value");
            if (valueIt == inputs.end()) {
                return ReflectionV2::FOperationResultV2::Error(
                    "reflection.operation.missing_input",
                    "Missing required input 'value'.");
            }

            const auto &value = valueIt->second;
            if (!value.TypeId.empty() &&
                value.TypeId != ReflectionV2::CTypeIdFunctionRtoR &&
                value.TypeId != ReflectionV2::CTypeIdScalarString) {
                return ReflectionV2::FOperationResultV2::Error(
                    "plot2d_v2.operation.type_mismatch",
                    "Set Function expects type '" + Str(ReflectionV2::CTypeIdFunctionRtoR) + "' or scalar string.");
            }

            ReflectionV2::FFunctionDescriptorV2 descriptor;
            Str decodeError;
            if (!ReflectionV2::DecodeFunctionDescriptorCLI(value.Encoded, descriptor, &decodeError)) {
                return ReflectionV2::FOperationResultV2::Error(
                    "plot2d_v2.operation.decode_function_descriptor",
                    "Could not decode function descriptor: " + decodeError);
            }

            Str functionError;
            auto function = BuildFunctionFromDescriptor(descriptor, functionError);
            if (function == nullptr) {
                return ReflectionV2::FOperationResultV2::Error(
                    "plot2d_v2.operation.unsupported_function",
                    functionError);
            }

            SetFunction(std::move(function));
            SetDomain(descriptor.DomainMin, descriptor.DomainMax);

            ReflectionV2::FValueMapV2 output;
            output["artist_id"] = ReflectionV2::MakeStringValue(GetArtistId());
            output["function_family"] = ReflectionV2::MakeStringValue(descriptor.Family);
            output["function_expr"] = ReflectionV2::MakeStringValue(descriptor.Expression);
            output["domain_x_min"] = ReflectionV2::MakeFloatValue(GetDomainXMin());
            output["domain_x_max"] = ReflectionV2::MakeFloatValue(GetDomainXMax());
            return ReflectionV2::FOperationResultV2::Ok(std::move(output));
        };
        operations.push_back(std::move(setFunction));

        return operations;
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
