#include "PlotArtistV2.h"

#include "Core/Reflection/V2/ReflectionCodecsV2.h"

namespace Slab::Graphics::Plot2D::V2 {

    namespace ReflectionV2 = Slab::Core::Reflection::V2;

    FPlotArtistV2::FPlotArtistV2(Str artistId, Str label)
    : ArtistId(std::move(artistId))
    , Label(std::move(label)) {
    }

    auto FPlotArtistV2::ComputePlotBounds() const -> std::optional<RectR> {
        return std::nullopt;
    }

    auto FPlotArtistV2::HitTest(const FPlotFrameContextV2 &frame,
                                const FPoint2D &plotPosition,
                                const FPoint2D &viewportPosition) const
        -> std::optional<FPlotHitTargetV2> {
        (void) frame;
        (void) plotPosition;
        (void) viewportPosition;
        return std::nullopt;
    }

    auto FPlotArtistV2::HandlePointerEvent(const FPlotFrameContextV2 &frame,
                                           const FPlotPointerEventV2 &event) -> bool {
        (void) frame;
        (void) event;
        return false;
    }

    auto FPlotArtistV2::HandleKeyboardEvent(const FPlotFrameContextV2 &frame,
                                            const FPlotKeyboardEventV2 &event) -> bool {
        (void) frame;
        (void) event;
        return false;
    }

    auto FPlotArtistV2::BuildReflectionParameterBindings() -> Vector<FPlotReflectionParameterBindingV2> {
        Vector<FPlotReflectionParameterBindingV2> bindings;

        FPlotReflectionParameterBindingV2 artistIdBinding;
        artistIdBinding.Schema.ParameterId = "artist_id";
        artistIdBinding.Schema.DisplayName = "Artist Id";
        artistIdBinding.Schema.Description = "Unique artist identifier inside the window.";
        artistIdBinding.Schema.TypeId = ReflectionV2::CTypeIdScalarString;
        artistIdBinding.Schema.Mutability = ReflectionV2::EParameterMutability::Const;
        artistIdBinding.Schema.Exposure = ReflectionV2::EParameterExposure::ReadOnlyExposed;
        artistIdBinding.ReadCurrent = [this] { return ReflectionV2::MakeStringValue(ArtistId); };
        bindings.push_back(std::move(artistIdBinding));

        FPlotReflectionParameterBindingV2 typeBinding;
        typeBinding.Schema.ParameterId = "artist_type";
        typeBinding.Schema.DisplayName = "Artist Type";
        typeBinding.Schema.Description = "Artist implementation type id.";
        typeBinding.Schema.TypeId = ReflectionV2::CTypeIdScalarString;
        typeBinding.Schema.Mutability = ReflectionV2::EParameterMutability::Const;
        typeBinding.Schema.Exposure = ReflectionV2::EParameterExposure::ReadOnlyExposed;
        typeBinding.ReadCurrent = [this] { return ReflectionV2::MakeStringValue(GetArtistTypeId()); };
        bindings.push_back(std::move(typeBinding));

        FPlotReflectionParameterBindingV2 parentWindowBinding;
        parentWindowBinding.Schema.ParameterId = "parent_window_id";
        parentWindowBinding.Schema.DisplayName = "Parent Window Id";
        parentWindowBinding.Schema.Description = "Stable id of the window that owns this artist.";
        parentWindowBinding.Schema.TypeId = ReflectionV2::CTypeIdScalarString;
        parentWindowBinding.Schema.Mutability = ReflectionV2::EParameterMutability::Const;
        parentWindowBinding.Schema.Exposure = ReflectionV2::EParameterExposure::ReadOnlyExposed;
        parentWindowBinding.ReadCurrent = [this] { return ReflectionV2::MakeStringValue(ParentWindowId); };
        bindings.push_back(std::move(parentWindowBinding));

        FPlotReflectionParameterBindingV2 labelBinding;
        labelBinding.Schema.ParameterId = "label";
        labelBinding.Schema.DisplayName = "Label";
        labelBinding.Schema.Description = "Legend/display label for this artist.";
        labelBinding.Schema.TypeId = ReflectionV2::CTypeIdScalarString;
        labelBinding.Schema.Mutability = ReflectionV2::EParameterMutability::RuntimeMutable;
        labelBinding.Schema.Exposure = ReflectionV2::EParameterExposure::WritableExposed;
        labelBinding.ReadCurrent = [this] { return ReflectionV2::MakeStringValue(Label); };
        labelBinding.WriteLiveValue = [this](const ReflectionV2::FReflectionValueV2 &value) {
            Label = value.Encoded;
            return ReflectionV2::FOperationResultV2::Ok();
        };
        bindings.push_back(std::move(labelBinding));

        FPlotReflectionParameterBindingV2 visibleBinding;
        visibleBinding.Schema.ParameterId = "visible";
        visibleBinding.Schema.DisplayName = "Visible";
        visibleBinding.Schema.Description = "Whether this artist is emitted to the draw list.";
        visibleBinding.Schema.TypeId = ReflectionV2::CTypeIdScalarBool;
        visibleBinding.Schema.Mutability = ReflectionV2::EParameterMutability::RuntimeMutable;
        visibleBinding.Schema.Exposure = ReflectionV2::EParameterExposure::WritableExposed;
        visibleBinding.ReadCurrent = [this] { return ReflectionV2::MakeBoolValue(bVisible); };
        visibleBinding.WriteLiveValue = [this](const ReflectionV2::FReflectionValueV2 &value) {
            bool parsed = false;
            if (!ReflectionV2::ParseBoolValue(value.Encoded, parsed)) {
                return ReflectionV2::FOperationResultV2::Error(
                    "plot2d_v2.parameter.parse_bool",
                    "Could not parse bool value for artist parameter 'visible'.");
            }

            bVisible = parsed;
            return ReflectionV2::FOperationResultV2::Ok();
        };
        bindings.push_back(std::move(visibleBinding));

        FPlotReflectionParameterBindingV2 affectRangesBinding;
        affectRangesBinding.Schema.ParameterId = "affect_graph_ranges";
        affectRangesBinding.Schema.DisplayName = "Affect Graph Ranges";
        affectRangesBinding.Schema.Description = "Whether this artist contributes to fit-to-data operations.";
        affectRangesBinding.Schema.TypeId = ReflectionV2::CTypeIdScalarBool;
        affectRangesBinding.Schema.Mutability = ReflectionV2::EParameterMutability::RuntimeMutable;
        affectRangesBinding.Schema.Exposure = ReflectionV2::EParameterExposure::WritableExposed;
        affectRangesBinding.ReadCurrent = [this] { return ReflectionV2::MakeBoolValue(bAffectGraphRanges); };
        affectRangesBinding.WriteLiveValue = [this](const ReflectionV2::FReflectionValueV2 &value) {
            bool parsed = false;
            if (!ReflectionV2::ParseBoolValue(value.Encoded, parsed)) {
                return ReflectionV2::FOperationResultV2::Error(
                    "plot2d_v2.parameter.parse_bool",
                    "Could not parse bool value for artist parameter 'affect_graph_ranges'.");
            }

            bAffectGraphRanges = parsed;
            return ReflectionV2::FOperationResultV2::Ok();
        };
        bindings.push_back(std::move(affectRangesBinding));

        return bindings;
    }

    auto FPlotArtistV2::BuildReflectionOperations() -> Vector<FOperationSchemaV2> {
        return {};
    }

    auto FPlotArtistV2::SetArtistId(Str artistId) -> void {
        ArtistId = std::move(artistId);
    }

    auto FPlotArtistV2::GetArtistId() const -> const Str & {
        return ArtistId;
    }

    auto FPlotArtistV2::SetParentWindowId(Str parentWindowId) -> void {
        ParentWindowId = std::move(parentWindowId);
    }

    auto FPlotArtistV2::GetParentWindowId() const -> const Str & {
        return ParentWindowId;
    }

    auto FPlotArtistV2::SetLabel(Str label) -> void {
        Label = std::move(label);
    }

    auto FPlotArtistV2::GetLabel() const -> const Str & {
        return Label;
    }

    auto FPlotArtistV2::SetVisible(const bool visible) -> void {
        bVisible = visible;
    }

    auto FPlotArtistV2::IsVisible() const -> bool {
        return bVisible;
    }

    auto FPlotArtistV2::SetAffectGraphRanges(const bool affect) -> void {
        bAffectGraphRanges = affect;
    }

    auto FPlotArtistV2::ShouldAffectGraphRanges() const -> bool {
        return bAffectGraphRanges;
    }

    auto FPlotArtistV2::DescribeReflection() -> FPlotEntityReflectionDescriptorV2 {
        FPlotEntityReflectionDescriptorV2 descriptor;
        descriptor.InterfaceId = "v2.plot.artist." + ParentWindowId + "." + ArtistId;
        descriptor.DisplayName = Label.empty() ? ArtistId : Label;
        descriptor.Description = "Plot2D V2 artist reflection surface.";
        descriptor.Tags = {"plot2d", "v2", "artist"};

        descriptor.Parameters = BuildReflectionParameterBindings();
        descriptor.Operations = BuildReflectionOperations();

        return descriptor;
    }

} // namespace Slab::Graphics::Plot2D::V2
