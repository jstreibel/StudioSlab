//
// Created by joao on 29/05/23.
//

#ifndef STUDIOSLAB_PLOT2DWINDOW_H
#define STUDIOSLAB_PLOT2DWINDOW_H

#include <memory>

#include "Graphics/Plot2D/Artists/LabelsArtist.h"

#include "PlotStyle.h"

#include "Graphics/Utils.h"
#include "Graphics/Window/SlabWindow.h"
#include "Graphics/Utils/Writer.h"

#include "Artists/AxisArtist.h"
#include "Artists/XHairArtist.h"
#include "Graphics/ImGui/ImGui-SingleSlabWindow-Wrapper.h"
#include "Graphics/Plot2D/Util/PlottingRegion2D.h"
#include "Graphics/Plot2D/Artists/BackgroundArtist.h"
#include "Graphics/Modules/ImGui/ImGuiContext.h"
#include "Graphics/OpenGL/WriterOpenGL.h"

#define POPUP_ON_MOUSE_CALL false

namespace Slab::Graphics {

    class FPlot2DWindow final : public FSlabWindow {
        static CountType WindowCount;
        CountType Id;
        static std::map<Str, FPlot2DWindow*> GraphMap;
        bool b_DrawGUI = true;

    protected:
        typedef Int zOrder_t;

    public:
        typedef std::multimap<zOrder_t, FArtist_ptr> ContentMap;

        explicit FPlot2DWindow(Str Title);

        void ImmediateDraw(const FPlatformWindow&) override;
        auto RegisterDeferredDrawCalls(const FPlatformWindow& PlatformWindow) -> void override;
        void SetNoGUI();

        void AddArtist(const FArtist_ptr& pArtist, zOrder_t zOrder=0);
        bool RemoveArtist(const FArtist_ptr& pArtist);
        void RemoveArtists(const Vector<FArtist_ptr> &artists);

        FAxisArtist &GetAxisArtist();

        auto GetLastXHairPosition() const -> Point2D;
        Str GetXHairLabel(const Point2D &coords) const;

        void SetAutoReviewGraphRanges(bool);
        void ReviewGraphRanges();

        auto GetRegion() const -> const PlottingRegion2D&;
        auto GetRegion() -> PlottingRegion2D&;
        void SetRegion(const PlottingRegion2D&);

        void TieRegion_xMaxMin(const FPlot2DWindow&);
        void TieRegion_yMaxMin(const FPlot2DWindow&);

        void RequireLabelOverlay(const Str& label, const TPointer<PlotStyle>& style) const;

        void SetAnimationTime(float value);
        [[nodiscard]] float GetAnimationTime() const;

        bool NotifyMouseButton(EMouseButton button, EKeyState state, EModKeys keys) override;
        bool NotifyMouseWheel(double dx, double dy) override;
        bool NotifyMouseMotion(int x, int y, int dx, int dy) override;
        bool NotifyKeyboard(EKeyMap key, EKeyState state, EModKeys modKeys) override;

    protected:
        Str Title;

        void ArtistsDraw();
        void toggleShowInterface();
        void RegisterGUIDraws();

        friend class FArtist;

        FAxisArtist AxisArtist;
        XHairArtist ArtistXHair;
        FLabelsArtist LabelsArtist;
        FBackgroundArtist bgArtist;

        FPlot2DWindow(
            DevFloat xMin,
            DevFloat xMax,
            DevFloat yMin,
            DevFloat yMax,
            Str title);

    private:
        ContentMap Content;

        bool ShowInterface = false;

        bool PopupOn = false;
        bool AutoReviewGraphRanges=false;

        PlottingRegion2D Region;

        float AnimationTimeSeconds = 0.2f;

        // FImGuiWindowContext WindowContext;

        void SetupOrtho() const;


    };

    DefinePointers(FPlot2DWindow)

}




#endif //STUDIOSLAB_PLOT2DWINDOW_H
