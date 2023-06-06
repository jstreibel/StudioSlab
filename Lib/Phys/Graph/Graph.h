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
            typedef std::tuple<RtoR2::ParametricCurve::Ptr, Styles::PlotStyle, String> CurveTriple;
            static auto GetCurve    ( CurveTriple triple ) { return std::get<0>(triple); };
            static auto GetStyle    ( CurveTriple triple ) { return std::get<1>(triple); };
            static auto GetName     ( CurveTriple triple ) { return std::get<2>(triple); };

            std::vector<Label*> labels;
            std::vector<CurveTriple> curves;

            Graph2D() = default;

            bool savePopupOn = false;

        protected:
            double xMin, xMax, yMin, yMax;
            double yspacing = 1.e-5;
            double xspacing = 1.e-5;

            String title = "";
            bool filled = false;
            int samples = 512;


            void _drawAxes();
            void __drawXAxis();
            void __drawYAxis();

            void __computeSpacings();

            void _nameLabelDraw(int i, const Styles::PlotStyle &style, String label, const Window *window);

            void _drawCurves();

        public:
            Graph2D(double xMin=-1, double xMax=1, double yMin=-1, double yMax=1,
                  String title = "no_title", bool filled = false, int samples = 512);

            void draw() override;

            void setupOrtho();

            void addLabel(Label *label);
            void addCurve(RtoR2::ParametricCurve::Ptr curve, Styles::PlotStyle style, String name);

            void clearCurves();

            Real get_yMin() const;
            Real get_yMax() const;

            bool notifyMouseButton(int button, int dir, int x, int y) override;

        };

    }

}


#endif //STUDIOSLAB_GRAPH_H
