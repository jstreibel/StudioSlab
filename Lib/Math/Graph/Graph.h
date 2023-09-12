//
// Created by joao on 29/05/23.
//

#ifndef STUDIOSLAB_GRAPH_H
#define STUDIOSLAB_GRAPH_H

#include "LabelingHelper.h"

#include "Styles.h"

#include "Utils/Utils.h"
#include "Core/Graphics/Utils.h"
#include "Core/Graphics/Window/Window.h"
#include "Core/Graphics/OpenGL/Utils.h"
#include "Core/Graphics/OpenGL/Writer.h"
#include "Maps/RtoR2/ParametricCurve.h"

#include <memory>


namespace Core::Graphics {

    class Graph2D : public Window {
        static std::map<Str, Graph2D*> graphMap;

        // ************************ POINT SET **************************************
        struct PointSetMetadata {
            Spaces::PointSet::Ptr data;
            Styles::PlotStyle plotStyle;
            Str name;
            bool affectsGraphRanges=true;
        };
        typedef std::vector<PointSetMetadata> PointSets;
        static void renderPointSet(const Spaces::PointSet &pSet, Styles::PlotStyle style) noexcept;


        // ************************ CURVES *****************************************
        struct CurveMetadata{
            RtoR2::ParametricCurve::Ptr curve;
            Styles::PlotStyle style;
            Str name;
        };

        PointSets mPointSets;
        std::vector<CurveMetadata> curves;

        bool savePopupOn = false;
        bool autoReviewGraphRanges=false;

        RectR region;

        Real animationTimeSeconds = 0.2;
    protected:

        Real yspacing = 1.e-5;
        Real xspacing = 1.e-5;

        Str title;
        Resolution samples = 512;

        Math::Graphics::LabelingHelper labelingHelper;


        void drawAxes();
        void drawXAxis();
        void drawYAxis();

        void computeTicksSpacings();

        virtual auto countDisplayItems() const -> Count;
        void nameLabelDraw(const Styles::PlotStyle &style, const Str& label);

        void drawPointSets();
        void drawCurves();
        void drawGUI();
        void drawXHair();

    public:
        explicit Graph2D(Real xMin=-1, Real xMax=1, Real yMin=-1, Real yMax=1,
                Str title = "no_title", int samples = 512);

        explicit Graph2D(Str title, bool autoReviewGraphLimits=true);

        void draw() override;

        void setupOrtho() const;

        void addPointSet(Spaces::PointSet::Ptr pointSet,
                         Styles::PlotStyle style,
                         Str setName="",
                         bool affectsGraphRanges=true);
        void clearPointSets();

        void addCurve(RtoR2::ParametricCurve::Ptr curve, Styles::PlotStyle style, Str name);
        void clearCurves();

        void reviewGraphRanges();

        auto getResolution() const -> Resolution;
        auto setResolution(Resolution samples) -> void;
        auto getLimits() const -> const RectR&;
        auto setLimits(RectR limits) -> void;
        void set_xMin(Real);
        void set_xMax(Real);
        void set_yMin(Real);
        void set_yMax(Real);
        Real get_xMin() const;
        Real get_xMax() const;
        Real get_yMin() const;
        Real get_yMax() const;

        void setAnimationTime(Real value);
        Real getAnimationTime() const;

        bool notifyMouseButton(int button, int dir, int x, int y) override;
        bool notifyMouseMotion(int x, int y) override;
        bool notifyMouseWheel(int wheel, int direction, int x, int y) override;

        void notifyReshape(int newWinW, int newWinH) override;
    };

}




#endif //STUDIOSLAB_GRAPH_H
