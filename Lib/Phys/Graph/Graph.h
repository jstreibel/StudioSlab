//
// Created by joao on 29/05/23.
//

#ifndef STUDIOSLAB_GRAPH_H
#define STUDIOSLAB_GRAPH_H


#include "Common/Utils.h"

#include "Base/Graphics/Utils.h"
#include "Base/Graphics/Artists/Label.h"

#include "Base/Graphics/Window/Window.h"

#include "Base/Graphics/Styles/StylesAndColorSchemes.h"
#include "Base/Graphics/OpenGL/Utils.h"

#include "Mappings/RtoR2/ParametricCurve.h"

#include <memory>


namespace Base {

    namespace Graphics {

        class Graph2D : public Window {
            typedef std::tuple<RtoR2::ParametricCurve::Ptr, Styles::PlotStyle, Str> CurveTriple;
            static auto GetCurve    ( CurveTriple triple ) { return std::get<0>(triple); };
            static auto GetStyle    ( CurveTriple triple ) { return std::get<1>(triple); };
            static auto GetName     ( CurveTriple triple ) { return std::get<2>(triple); };

            std::vector<Label*> labels;
            std::vector<CurveTriple> curves;

            bool savePopupOn = false;

        protected:
            Real xMin, xMax, yMin, yMax;
            Real yspacing = 1.e-5;
            Real xspacing = 1.e-5;

            Real fontScale = 1.0;

            Str title;
            bool filled = false;
            int samples = 512;


            void _drawAxes();
            void __drawXAxis();
            void __drawYAxis();

            void __computeSpacings();

            void _nameLabelDraw(int i, int j, const Styles::PlotStyle &style, Str label, const Window *window);

            void _drawCurves();

        public:
            Graph2D(Real xMin=-1, Real xMax=1, Real yMin=-1, Real yMax=1,
                    Str title = "no_title", bool filled = false, int samples = 512);

            void draw() override;

            void setupOrtho();

            void addLabel(Label *label);
            void addCurve(RtoR2::ParametricCurve::Ptr curve, Styles::PlotStyle style, Str name);

            void clearCurves();

            auto getResolution() const -> Resolution;
            auto setResolution(Resolution samples) -> void;
            auto getLimits() const -> Rect;
            auto setLimits(Rect limits) -> void;
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
        };

    }

}


#endif //STUDIOSLAB_GRAPH_H
