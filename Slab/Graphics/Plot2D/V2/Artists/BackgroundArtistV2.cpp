#include "BackgroundArtistV2.h"

#include "Core/Reflection/V2/ReflectionCodecsV2.h"
#include "Graphics/Plot2D/PlotThemeManager.h"

#include <algorithm>

namespace Slab::Graphics::Plot2D::V2 {

    namespace ReflectionV2 = Slab::Core::Reflection::V2;

    FBackgroundArtistV2::FBackgroundArtistV2(const FColor backgroundColor)
    : FPlotArtistV2("background", "Background")
    , BackgroundColor(backgroundColor) {
        SetAffectGraphRanges(false);
    }

    auto FBackgroundArtistV2::BuildReflectionParameterBindings() -> Vector<FPlotReflectionParameterBindingV2> {
        auto bindings = FPlotArtistV2::BuildReflectionParameterBindings();

        FPlotReflectionParameterBindingV2 useThemeBinding;
        useThemeBinding.Schema.ParameterId = "use_theme_color";
        useThemeBinding.Schema.DisplayName = "Use Theme Color";
        useThemeBinding.Schema.Description = "Use PlotThemeManager graph background color.";
        useThemeBinding.Schema.TypeId = ReflectionV2::CTypeIdScalarBool;
        useThemeBinding.Schema.Mutability = ReflectionV2::EParameterMutability::RuntimeMutable;
        useThemeBinding.Schema.Exposure = ReflectionV2::EParameterExposure::WritableExposed;
        useThemeBinding.ReadCurrent = [this] { return ReflectionV2::MakeBoolValue(bUseThemeColor); };
        useThemeBinding.WriteLiveValue = [this](const ReflectionV2::FReflectionValueV2 &value) {
            bool parsed = false;
            if (!ReflectionV2::ParseBoolValue(value.Encoded, parsed)) {
                return ReflectionV2::FOperationResultV2::Error(
                    "plot2d_v2.parameter.parse_bool",
                    "Could not parse bool value for parameter 'use_theme_color'.");
            }
            bUseThemeColor = parsed;
            return ReflectionV2::FOperationResultV2::Ok();
        };
        bindings.push_back(std::move(useThemeBinding));

        auto addColorBinding = [this, &bindings](const Str &component, float FColor::*member) {
            FPlotReflectionParameterBindingV2 binding;
            binding.Schema.ParameterId = "background_color_" + component;
            binding.Schema.DisplayName = "Background Color " + component;
            binding.Schema.Description = "Background color component in [0, 1].";
            binding.Schema.TypeId = ReflectionV2::CTypeIdScalarFloat64;
            binding.Schema.Mutability = ReflectionV2::EParameterMutability::RuntimeMutable;
            binding.Schema.Exposure = ReflectionV2::EParameterExposure::WritableExposed;
            binding.Schema.Minimum = 0.0;
            binding.Schema.Maximum = 1.0;
            binding.ReadCurrent = [this, member] { return ReflectionV2::MakeFloatValue(BackgroundColor.*member); };
            binding.WriteLiveValue = [this, member, component](const ReflectionV2::FReflectionValueV2 &value) {
                try {
                    const auto parsed = std::clamp<DevFloat>(std::stod(value.Encoded), 0.0, 1.0);
                    BackgroundColor.*member = static_cast<float>(parsed);
                } catch (const std::exception &e) {
                    return ReflectionV2::FOperationResultV2::Error(
                        "plot2d_v2.parameter.parse_float",
                        "Could not parse float for parameter 'background_color_" + component + "': " + Str(e.what()));
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

    auto FBackgroundArtistV2::EmitDrawCommands(const FPlotFrameContextV2 &frame, FPlotDrawListV2 &drawList) const -> void {
        auto color = BackgroundColor;
        if (bUseThemeColor) {
            if (const auto theme = FPlotThemeManager::GetCurrent(); theme != nullptr) {
                color = theme->graphBackground;
            }
        }

        FRectangleCommandV2 command;
        command.Rectangle = frame.PlotRegion;
        command.Color = color;
        command.bFilled = true;
        command.CoordinateSpace = EPlotCoordinateSpaceV2::Plot;
        drawList.AddRectangle(std::move(command));
    }

    auto FBackgroundArtistV2::GetArtistTypeId() const -> Str {
        return "background";
    }

    auto FBackgroundArtistV2::SetBackgroundColor(const FColor color) -> void {
        BackgroundColor = color;
    }

    auto FBackgroundArtistV2::GetBackgroundColor() const -> const FColor & {
        return BackgroundColor;
    }

    auto FBackgroundArtistV2::SetUseThemeColor(const bool useThemeColor) -> void {
        bUseThemeColor = useThemeColor;
    }

    auto FBackgroundArtistV2::GetUseThemeColor() const -> bool {
        return bUseThemeColor;
    }

} // namespace Slab::Graphics::Plot2D::V2
