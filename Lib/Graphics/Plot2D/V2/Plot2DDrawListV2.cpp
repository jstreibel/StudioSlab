#include "Plot2DDrawListV2.h"

namespace Slab::Graphics::Plot2D::V2 {

    auto FPlotDrawListV2::AddPolyline(FPolylineCommandV2 command) -> void {
        Commands.emplace_back(std::move(command));
    }

    auto FPlotDrawListV2::AddPointSet(FPointSetCommandV2 command) -> void {
        Commands.emplace_back(std::move(command));
    }

    auto FPlotDrawListV2::AddRectangle(FRectangleCommandV2 command) -> void {
        Commands.emplace_back(std::move(command));
    }

    auto FPlotDrawListV2::AddText(FTextCommandV2 command) -> void {
        Commands.emplace_back(std::move(command));
    }

    auto FPlotDrawListV2::AddLegendEntry(FLegendEntryV2 entry) -> void {
        LegendEntries.emplace_back(std::move(entry));
    }

    auto FPlotDrawListV2::GetCommands() const -> const Vector<FDrawCommandV2> & {
        return Commands;
    }

    auto FPlotDrawListV2::GetLegendEntries() const -> const Vector<FLegendEntryV2> & {
        return LegendEntries;
    }

    auto FPlotDrawListV2::Clear() -> void {
        Commands.clear();
        LegendEntries.clear();
    }

} // namespace Slab::Graphics::Plot2D::V2
