#ifndef STUDIOSLAB_LEGACY_PLOT_REFLECTION_ADAPTER_V2_H
#define STUDIOSLAB_LEGACY_PLOT_REFLECTION_ADAPTER_V2_H

#include "Graphics/Plot2D/Plot2DWindow.h"

#include "PlotReflectionSchemaV2.h"

namespace Slab::Graphics::Plot2D::V2 {

    class FLegacyPlotReflectionAdapterV2 {
    public:
        [[nodiscard]] static auto BuildWindowInterfaceId(const Slab::Graphics::FPlot2DWindow &window) -> Str;
        [[nodiscard]] static auto BuildArtistInterfaceId(const Slab::Graphics::FPlot2DWindow &window,
                                                         const Slab::Graphics::FArtist &artist) -> Str;

        [[nodiscard]] static auto DescribeWindow(Slab::Graphics::FPlot2DWindow &window) -> FPlotEntityReflectionDescriptorV2;
        [[nodiscard]] static auto DescribeArtist(Slab::Graphics::FPlot2DWindow &window,
                                                 Slab::Graphics::FPlot2DWindow::zOrder_t zOrder,
                                                 const Slab::Graphics::FArtist_ptr &artist) -> FPlotEntityReflectionDescriptorV2;
    };

} // namespace Slab::Graphics::Plot2D::V2

#endif // STUDIOSLAB_LEGACY_PLOT_REFLECTION_ADAPTER_V2_H
