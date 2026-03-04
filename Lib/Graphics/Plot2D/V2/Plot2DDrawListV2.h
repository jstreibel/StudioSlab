#ifndef STUDIOSLAB_PLOT2D_DRAWLIST_V2_H
#define STUDIOSLAB_PLOT2D_DRAWLIST_V2_H

#include "Graphics/Plot2D/PlotStyle.h"
#include "Graphics/Types2D.h"
#include "Graphics/Styles/Colors.h"
#include "Utils/Types.h"

#include <variant>

namespace Slab::Graphics::Plot2D::V2 {

    enum class EPlotCoordinateSpaceV2 : unsigned char {
        Plot,
        Screen
    };

    struct FPolylineCommandV2 {
        Vector<FPoint2D> Points;
        PlotStyle Style = PlotStyle(White);
        bool bClosed = false;
        EPlotCoordinateSpaceV2 CoordinateSpace = EPlotCoordinateSpaceV2::Plot;
    };

    struct FPointSetCommandV2 {
        Vector<FPoint2D> Points;
        PlotStyle Style = PlotStyle(White, Slab::Graphics::Points);
        EPlotCoordinateSpaceV2 CoordinateSpace = EPlotCoordinateSpaceV2::Plot;
    };

    struct FRectangleCommandV2 {
        RectR Rectangle{};
        FColor Color = White;
        bool bFilled = false;
        EPlotCoordinateSpaceV2 CoordinateSpace = EPlotCoordinateSpaceV2::Plot;
    };

    struct FTextCommandV2 {
        Str Text;
        FPoint2D Location;
        FColor Color = White;
        DevFloat FontScale = 1.0;
        bool bVertical = false;
        EPlotCoordinateSpaceV2 CoordinateSpace = EPlotCoordinateSpaceV2::Screen;
    };

    struct FLegendEntryV2 {
        Str Label;
        PlotStyle Style = PlotStyle(White);
    };

    using FDrawCommandV2 = std::variant<FPolylineCommandV2,
                                        FPointSetCommandV2,
                                        FRectangleCommandV2,
                                        FTextCommandV2>;

    class FPlotDrawListV2 {
        Vector<FDrawCommandV2> Commands;
        Vector<FLegendEntryV2> LegendEntries;

    public:
        auto AddPolyline(FPolylineCommandV2 command) -> void;
        auto AddPointSet(FPointSetCommandV2 command) -> void;
        auto AddRectangle(FRectangleCommandV2 command) -> void;
        auto AddText(FTextCommandV2 command) -> void;

        auto AddLegendEntry(FLegendEntryV2 entry) -> void;

        [[nodiscard]] auto GetCommands() const -> const Vector<FDrawCommandV2> &;
        [[nodiscard]] auto GetLegendEntries() const -> const Vector<FLegendEntryV2> &;

        auto Clear() -> void;
    };

} // namespace Slab::Graphics::Plot2D::V2

#endif // STUDIOSLAB_PLOT2D_DRAWLIST_V2_H
