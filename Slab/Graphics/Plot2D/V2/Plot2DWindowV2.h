#ifndef STUDIOSLAB_PLOT2D_WINDOW_V2_H
#define STUDIOSLAB_PLOT2D_WINDOW_V2_H

#include "Plot2DRenderBackendV2.h"
#include "PlotArtistV2.h"

#include <map>
#include <mutex>

namespace Slab::Graphics::Plot2D::V2 {

    class FPlot2DWindowV2 : public IPlotReflectableEntityV2 {
    public:
        struct FArtistSlotV2 {
            int ZOrder = 0;
            FPlotArtistV2_ptr Artist = nullptr;
        };

    private:
        static std::mutex RegistryMutex;
        static std::map<Str, FPlot2DWindowV2 *> Registry;
        static UIntBig WindowCounter;

        Str WindowId;
        Str Title;
        RectR Region{};
        RectI Viewport{};
        bool bAutoFitRanges = false;

        Vector<FArtistSlotV2> Artists;

        static auto NormalizeToken(Str raw) -> Str;
        static auto MakeUniqueWindowId(const Str &titleToken) -> Str;
        auto MakeUniqueArtistId(Str desiredId) const -> Str;

        auto BuildReflectionParameterBindings() -> Vector<FPlotReflectionParameterBindingV2>;
        auto BuildReflectionOperations() -> Vector<FOperationSchemaV2>;

    public:
        explicit FPlot2DWindowV2(Str title = "Plot2D V2",
                                 RectR region = {-1.0, 1.0, -1.0, 1.0},
                                 RectI viewport = {0, 1024, 0, 768});

        ~FPlot2DWindowV2() override;

        [[nodiscard]] static auto GetLiveWindows() -> Vector<FPlot2DWindowV2 *>;

        auto AddArtist(FPlotArtistV2_ptr artist, int zOrder = 0) -> void;
        auto RemoveArtist(const FPlotArtistV2_ptr &artist) -> bool;
        auto ClearArtists() -> void;

        [[nodiscard]] auto GetArtists() const -> const Vector<FArtistSlotV2> &;
        [[nodiscard]] auto GetArtistsInDrawOrder() const -> Vector<FArtistSlotV2>;
        [[nodiscard]] auto FindArtistById(const Str &artistId) const -> FPlotArtistV2_ptr;

        auto FitRegionToArtists(DevFloat paddingFraction = 0.05) -> bool;

        auto BuildDrawList() const -> FPlotDrawListV2;
        auto Render(IPlotRenderBackendV2 &backend) const -> bool;

        auto SetWindowId(Str windowId) -> void;
        [[nodiscard]] auto GetWindowId() const -> const Str &;

        auto SetTitle(Str title) -> void;
        [[nodiscard]] auto GetTitle() const -> const Str &;

        auto SetRegion(RectR region) -> void;
        [[nodiscard]] auto GetRegion() const -> const RectR &;

        auto SetViewport(RectI viewport) -> void;
        [[nodiscard]] auto GetViewport() const -> const RectI &;

        auto SetAutoFitRanges(bool autoFitRanges) -> void;
        [[nodiscard]] auto GetAutoFitRanges() const -> bool;

        [[nodiscard]] auto DescribeReflection() -> FPlotEntityReflectionDescriptorV2 override;
    };

    DefinePointers(FPlot2DWindowV2)

} // namespace Slab::Graphics::Plot2D::V2

#endif // STUDIOSLAB_PLOT2D_WINDOW_V2_H
