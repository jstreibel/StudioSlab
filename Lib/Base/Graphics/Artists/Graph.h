//
// Created by joao on 29/05/23.
//

#ifndef STUDIOSLAB_GRAPH_H
#define STUDIOSLAB_GRAPH_H


#include "Base/Graphics/PlottingUtils.h"
#include "Base/Graphics/Artists/Artist.h"

#include "Common/Workaround/StringStream.h"
#include "Common/BinaryToInt.h"

#include "Base/Graphics/WindowManagement/Window.h"

#include "Base/Graphics/Artists/StylesAndColorSchemes.h"
#include "Base/Graphics/OpenGL/Utils.h"

#include <memory>


namespace Base {

    namespace Graphics {

        class Graph2D : public Artist {
            std::vector<Label*> labels;

            Graph2D() = default;

        protected:
            double xMin, xMax, yMin, yMax;
            double yspacing = 1.e-5;

            String title = "";
            bool filled = false;
            int samples = 512;


            void _drawAxes(const Window *win);
            void __drawXAxis(const Window *win);
            void __drawYAxis(const Window *win);

            void _labelDraw(int i, const Styles::PlotStyle &style, String label, const Window *window);

        public:
            Graph2D(double xMin=-1, double xMax=1, double yMin=-1, double yMax=1,
                  String title = "no_title", bool filled = false, int samples = 512);

            void draw(const Window *window) override;

            void setupOrtho();

            void addLabel(Label *label);

            Real get_yMin() const;
            Real get_yMax() const;

        };

    }

}


#endif //STUDIOSLAB_GRAPH_H
