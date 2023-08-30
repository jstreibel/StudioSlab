//
// Created by joao on 29/05/23.
//

#ifndef STUDIOSLAB_GRAPH_H
#define STUDIOSLAB_GRAPH_H


#include "Utils/Utils.h"
#include "Core/Graphics/Utils.h"
#include "Core/Graphics/Window/Window.h"
#include "Core/Graphics/Styles/StylesAndColorSchemes.h"
#include "Core/Graphics/OpenGL/Utils.h"
#include "Core/Graphics/OpenGL/Writer.h"
#include "Mappings/RtoR2/ParametricCurve.h"

#include <memory>


namespace Core::Graphics {

    class Graph2D : public Window {
        static std::map<Str, Graph2D*> graphMap;

        Core::Graphics::Writer writer;

        // ************************ POINT SET **************************************
        struct PointSetMetadata {
            Spaces::PointSet::Ptr data;
            Styles::PlotStyle plotStyle;
            Str name;
            bool affectsGraphRanges=true;
        };

        typedef std::vector<PointSetMetadata> PointSets;
        PointSets mPointSets;

        void renderPointSet(const Spaces::PointSet &pSet, Styles::PlotStyle style) const noexcept;


        // ************************ CURVES *****************************************
        struct CurveMetadata{
            RtoR2::ParametricCurve::Ptr curve;
            Styles::PlotStyle style;
            Str name;
        };

        std::vector<CurveMetadata> curves;

        bool savePopupOn = false;
        bool autoReviewGraphRanges=false;

    protected:
        Real xMin, xMax, yMin, yMax;
        Real yspacing = 1.e-5;
        Real xspacing = 1.e-5;

        Real fontScale = 1.0;

        Str title;
        Resolution samples = 512;


        void drawAxes();
        void drawXAxis();
        void drawYAxis();

        void computeTicksSpacings();

        void nameLabelDraw(int i, int j, const Styles::PlotStyle &style, Str label, const Window *window);

        void drawGUI();
        void drawPointSets();
        void drawCurves();

    public:
        explicit Graph2D(Real xMin=-1, Real xMax=1, Real yMin=-1, Real yMax=1,
                Str title = "no_title", int samples = 512);

        explicit Graph2D(Str title, bool autoReviewGraphLimits=true);

        void draw() override;

        void setupOrtho() const;

        void addPointSet(Spaces::PointSet::Ptr pointSet,
                         Styles::PlotStyle style,
                         Str setName,
                         bool affectsGraphRanges=true);
        void clearPointSets();

        void addCurve(RtoR2::ParametricCurve::Ptr curve, Styles::PlotStyle style, Str name);
        void clearCurves();

        void reviewGraphRanges();

        auto getResolution() const -> Resolution;
        auto setResolution(Resolution samples) -> void;
        auto getLimits() const -> RectR;
        auto setLimits(RectR limits) -> void;
        void set_xMin(Real);
        void set_xMax(Real);
        void set_yMin(Real);
        void set_yMax(Real);
        Real get_xMin() const;
        Real get_xMax() const;
        Real get_yMin() const;
        Real get_yMax() const;

        bool notifyMouseButton(int button, int dir, int x, int y) override;
        bool notifyMouseMotion(int x, int y) override;
        bool notifyMouseWheel(int wheel, int direction, int x, int y) override;

        void notifyReshape(int newWinW, int newWinH) override;
    };

}




#endif //STUDIOSLAB_GRAPH_H