#ifndef STUDIOSLAB_PLOT_ARTIST_V2_H
#define STUDIOSLAB_PLOT_ARTIST_V2_H

#include "Graphics/Backend/Events/KeyMap.h"
#include "Plot2DDrawListV2.h"
#include "Plot2DRenderBackendV2.h"
#include "PlotReflectionSchemaV2.h"

namespace Slab::Graphics::Plot2D::V2 {

    struct FPlotHitTargetV2 {
        Str TargetId;
        Str DisplayLabel;
        Str Detail;
    };

    enum class EPlotPointerEventKindV2 : unsigned char {
        Move,
        Button,
        Wheel,
        Leave
    };

    struct FPlotPointerEventV2 {
        EPlotPointerEventKindV2 Kind = EPlotPointerEventKindV2::Move;
        FPoint2D PlotPosition{};
        FPoint2D ViewportPosition{};
        EMouseButton Button = MouseButton_LEFT;
        EKeyState ButtonState = Release;
        EModKeys ModKeys{};
        double WheelDx = 0.0;
        double WheelDy = 0.0;
    };

    struct FPlotKeyboardEventV2 {
        EKeyMap Key = Key_UNKNOWN;
        EKeyState State = Release;
        EModKeys ModKeys{};
    };

    class FPlotArtistV2 : public IPlotReflectableEntityV2 {
        Str ArtistId;
        Str Label;
        Str ParentWindowId;
        bool bVisible = true;
        bool bAffectGraphRanges = true;

    protected:
        [[nodiscard]] virtual auto BuildReflectionParameterBindings() -> Vector<FPlotReflectionParameterBindingV2>;
        [[nodiscard]] virtual auto BuildReflectionOperations() -> Vector<FOperationSchemaV2>;

    public:
        explicit FPlotArtistV2(Str artistId, Str label = "");
        ~FPlotArtistV2() override = default;

        virtual auto EmitDrawCommands(const FPlotFrameContextV2 &frame,
                                      FPlotDrawListV2 &drawList) const -> void = 0;

        [[nodiscard]] virtual auto ComputePlotBounds() const -> std::optional<RectR>;
        [[nodiscard]] virtual auto HitTest(const FPlotFrameContextV2 &frame,
                                           const FPoint2D &plotPosition,
                                           const FPoint2D &viewportPosition) const
            -> std::optional<FPlotHitTargetV2>;
        virtual auto HandlePointerEvent(const FPlotFrameContextV2 &frame,
                                        const FPlotPointerEventV2 &event) -> bool;
        virtual auto HandleKeyboardEvent(const FPlotFrameContextV2 &frame,
                                         const FPlotKeyboardEventV2 &event) -> bool;
        [[nodiscard]] virtual auto GetArtistTypeId() const -> Str = 0;

        auto SetArtistId(Str artistId) -> void;
        [[nodiscard]] auto GetArtistId() const -> const Str &;

        auto SetParentWindowId(Str parentWindowId) -> void;
        [[nodiscard]] auto GetParentWindowId() const -> const Str &;

        auto SetLabel(Str label) -> void;
        [[nodiscard]] auto GetLabel() const -> const Str &;

        auto SetVisible(bool visible) -> void;
        [[nodiscard]] auto IsVisible() const -> bool;

        auto SetAffectGraphRanges(bool affect) -> void;
        [[nodiscard]] auto ShouldAffectGraphRanges() const -> bool;

        [[nodiscard]] auto DescribeReflection() -> FPlotEntityReflectionDescriptorV2 override;
    };

    DefinePointers(FPlotArtistV2)

} // namespace Slab::Graphics::Plot2D::V2

#endif // STUDIOSLAB_PLOT_ARTIST_V2_H
