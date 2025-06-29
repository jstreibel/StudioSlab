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
#include "Graphics/Plot2D/Util/PlottingRegion2D.h"
#include "Graphics/Plot2D/Artists/BackgroundArtist.h"
#include "Graphics/Modules/ImGui/ImGuiContext.h"

#define POPUP_ON_MOUSE_CALL false

namespace Slab::Graphics {

    class Plot2DWindow final : public FSlabWindow {
        static CountType WindowCount;
        CountType id;
        static std::map<Str, Plot2DWindow*> graphMap;

    protected: typedef Int zOrder_t;
    public:    typedef std::multimap<zOrder_t, Artist_ptr> ContentMap;

    private:
        ContentMap content;

        bool showInterface = false;

        bool popupOn = false;
        bool autoReviewGraphRanges=false;

        PlottingRegion2D region;

        DevFloat animationTimeSeconds = 0.2;

        Math::PointSet XHair;

        bool gui_context_is_local;
        Pointer<SlabImGuiContext> gui_context;

    protected:
        Str title;

        void artistsDraw();
        void toggleShowInterface();
        virtual void drawGUI();

        friend class Artist;

        AxisArtist axisArtist;
        XHairArtist artistXHair;
        LabelsArtist labelsArtist;
        BackgroundArtist bgArtist;

        Plot2DWindow(DevFloat xMin, DevFloat xMax, DevFloat yMin, DevFloat yMax, Str title = "no_title");

    public:

        explicit Plot2DWindow(Str title);

        void Draw() override;

        void addArtist(const Artist_ptr& pArtist, zOrder_t zOrder=0);
        bool removeArtist(const Artist_ptr& pArtist);
        void removeArtists(const Vector<Artist_ptr> &artists);

        AxisArtist &GetAxisArtist();

        auto getLastXHairPosition() const -> Point2D;
        virtual Str getXHairLabel(const Point2D &coords) const;

        void setupOrtho() const;

        void SetAutoReviewGraphRanges(bool);
        void reviewGraphRanges();

        auto getRegion() const -> const PlottingRegion2D&;
        auto getRegion() -> PlottingRegion2D&;

        void tieRegion_xMaxMin(const Plot2DWindow&);
        void tieRegion_yMaxMin(const Plot2DWindow&);

        void requireLabelOverlay(const Str& label, const Pointer<PlotStyle>& style) const;

        void setAnimationTime(DevFloat value);
        DevFloat getAnimationTime() const;

        bool NotifyMouseButton(MouseButton button, KeyState state, ModKeys keys) override;
        bool notifyMouseWheel(double dx, double dy) override;
        bool notifyMouseMotion(int x, int y, int dx, int dy) override;
        bool NotifyKeyboard(KeyMap key, KeyState state, ModKeys modKeys) override;

    };

    DefinePointers(Plot2DWindow)

}




#endif //STUDIOSLAB_PLOT2DWINDOW_H
