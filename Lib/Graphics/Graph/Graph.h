//
// Created by joao on 29/05/23.
//

#ifndef STUDIOSLAB_GRAPH_H
#define STUDIOSLAB_GRAPH_H

#include "LabelingHelper.h"

#include "PlotStyle.h"

#include "Graphics/Utils.h"
#include "Graphics/Window/Window.h"
#include "Graphics/OpenGL/Utils.h"
#include "Graphics/OpenGL/Writer.h"

#include "Utils/Utils.h"


#include "Math/Function/RtoR2/ParametricCurve.h"
#include "Artists/AxisArtist.h"
#include "Artists/XHairArtist.h"


#include <memory>
#include <list>


namespace Graphics {

    class Graph2D : public Window {
    protected:
        typedef Int zOrder_t;

    private:
        static std::map<Str, Graph2D*> graphMap;

        typedef std::multimap<zOrder_t, Artist::Ptr> ContentMap;
        ContentMap content;

        // ************************ POINT SET **************************************
        struct PointSetMetadata {
            Spaces::PointSet::Ptr data;
            PlotStyle plotStyle;
            Str name;
            bool affectsGraphRanges=true;
        };
        typedef std::list<PointSetMetadata> PointSets;


        // ************************ CURVES *****************************************
        struct CurveMetadata{
            RtoR2::ParametricCurve::Ptr curve;
            PlotStyle style;
            Str name;
        };

        PointSets mPointSets;
        std::vector<CurveMetadata> curves;

        bool savePopupOn = false;
        bool autoReviewGraphRanges=false;

        RectR region;

        Real animationTimeSeconds = 0.2;

        Spaces::PointSet XHair;


    protected:
        Str title;
        Resolution samples = 512;

        Math::Graphics::LabelingHelper labelingHelper;

        virtual auto countDisplayItems() const -> Count;
        void nameLabelDraw(const PlotStyle &style, const Str& label);

        void artistsDraw();
        void drawPointSets();
        void drawCurves();
        virtual void drawGUI();

        friend class Artist;;

        AxisArtist axisArtist;
        XHairArtist artistXHair;

    public:
        explicit Graph2D(Real xMin=-1, Real xMax=1, Real yMin=-1, Real yMax=1,
                Str title = "no_title", int samples = 512);

        explicit Graph2D(Str title, bool autoReviewGraphLimits=true);

        void addArtist(const Artist::Ptr& pArtist, zOrder_t zOrder=0);
        bool removeArtist(const Artist::Ptr& pArtist);

        void draw() override;


        void setupOrtho() const;

        void addPointSet(Spaces::PointSet::Ptr pointSet,
                         PlotStyle style,
                         Str setName="",
                         bool affectsGraphRanges=true);
        void removePointSet(const Spaces::PointSet::Ptr& pointSet);
        void removePointSet(const Str& name);
        static void renderPointSet(const Spaces::PointSet &pSet, PlotStyle style) noexcept;
        void clearPointSets();

        void addCurve(RtoR2::ParametricCurve::Ptr curve, PlotStyle style, Str name);
        void clearCurves();

        void reviewGraphRanges();

        auto getLastXHairPosition() const -> Point2D;

        virtual Str getXHairLabel(const Point2D &coords) const;

        auto getResolution() const -> Resolution;
        auto setResolution(Resolution samples) -> void;
        auto getRegion() const -> const RectR&;
        auto setLimits(RectR limits) -> void;
        void set_xMin(Real);
        void set_xMax(Real);
        void set_yMin(Real);
        void set_yMax(Real);
        Real get_xMin() const;
        Real get_xMax() const;
        Real get_yMin() const;
        Real get_yMax() const;

        AxisArtist &getAxisArtist();

        void setAnimationTime(Real value);
        Real getAnimationTime() const;

        bool notifyMouseButton(Core::MouseButton button, Core::KeyState state, Core::ModKeys keys) override;

        bool notifyMouseWheel(double dx, double dy) override;

        bool notifyMouseMotion(int x, int y) override;


        void notifyReshape(int newWinW, int newWinH) override;
    };

}




#endif //STUDIOSLAB_GRAPH_H
