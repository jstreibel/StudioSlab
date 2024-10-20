//
// Created by joao on 29/05/23.
//

#ifndef STUDIOSLAB_PLOT2DWINDOW_H
#define STUDIOSLAB_PLOT2DWINDOW_H

#include <memory>
#include <list>

#include "Utils/Utils.h"

#include "Graphics/Plot2D/Artists/LabelsArtist.h"

#include "PlotStyle.h"

#include "Graphics/Utils.h"
#include "Graphics/Window/SlabWindow.h"
#include "Graphics/OpenGL/Utils.h"
#include "Graphics/Utils/Writer.h"

#include "Artists/AxisArtist.h"
#include "Artists/XHairArtist.h"
#include "Graphics/Plot2D/Util/PlottingRegion2D.h"
#include "Graphics/Plot2D/Artists/BackgroundArtist.h"

namespace Slab::Graphics {

    class Plot2DWindow : public SlabWindow {
        static Count WindowCount;
        Count id;
        static std::map<Str, Plot2DWindow*> graphMap;

    protected: typedef Int zOrder_t;
    public:    typedef std::multimap<zOrder_t, Artist_ptr> ContentMap;

    private:
        ContentMap content;

        bool showInterface = false;

        bool popupOn = false;
        bool autoReviewGraphRanges=false;

        PlottingRegion2D region;

        Real animationTimeSeconds = 0.2;

        Math::PointSet XHair;

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

    public:
        explicit Plot2DWindow(Real xMin=-1, Real xMax=1, Real yMin=-1, Real yMax=1,
                              Str title = "no_title");

        explicit Plot2DWindow(Str title, bool autoReviewGraphLimits=false);

        void draw() override;

        void addArtist(const Artist_ptr& pArtist, zOrder_t zOrder=0);
        bool removeArtist(const Artist_ptr& pArtist);
        void removeArtists(const Vector<Artist_ptr> &artists);

        AxisArtist &getAxisArtist();

        auto getLastXHairPosition() const -> Point2D;
        virtual Str getXHairLabel(const Point2D &coords) const;

        void setupOrtho() const;

        void setAutoReviewGraphRanges(bool);
        void reviewGraphRanges();

        auto getRegion() const -> const PlottingRegion2D&;
        auto getRegion() -> PlottingRegion2D&;

        void tieRegion_xMaxMin(const Plot2DWindow&);
        void tieRegion_yMaxMin(const Plot2DWindow&);

        void requireLabelOverlay(const Str& label, const Pointer<PlotStyle>& style) const;

        void setAnimationTime(Real value);
        Real getAnimationTime() const;

        bool notifyMouseButton(MouseButton button, KeyState state, ModKeys keys) override;
        bool notifyMouseWheel(double dx, double dy) override;
        bool notifyMouseMotion(int x, int y) override;
        bool notifyKeyboard(KeyMap key, KeyState state, ModKeys modKeys) override;

        void notifyReshape(int newWinW, int newWinH) override;

    };

    DefinePointers(Plot2DWindow)

}




#endif //STUDIOSLAB_PLOT2DWINDOW_H
