//
// Created by joao on 29/05/23.
//

#include "PointSetGraph.h"

namespace My = Phys::Graphing;

My::PointSetGraph::PointSetGraph(const String &title)
    : Graph2D(0, 1, 0, 1, title, true, 10) {

}

void My::PointSetGraph::draw(const Window *window) {
    Graph2D::draw(window);

    int i=0;
    for(auto &triple : mPointSets){
        auto &func = *GetPointSet(triple);
        auto style = GetStyle(triple);
        auto label = GetName(triple);

        if(label != "") _labelDraw(i++, style, label, window);

        this->_renderPointSet(func, style);
    }
}

void My::PointSetGraph::addPointSet(PointSetGraph::PointSetPtr pointSet, Styles::PlotStyle style, String setName) {
    auto triple = PointSetTriple{pointSet, style, setName};
    mPointSets.emplace_back(triple);
}

void My::PointSetGraph::_renderPointSet(const Spaces::PointSet &pSet, Styles::PlotStyle style) const noexcept {
    auto pts = pSet.getPoints();

    if(style.filled)
    {
        const auto color = style.fillColor;

        glColor4f(color.r, color.g, color.b, color.a);
        glBegin(GL_QUADS);
        {

            for(auto i=0; i<pts.size()-1; ++i){
                auto pLeft = pts[i];
                auto pRite = pts[i + 1];

                const double xmin = pLeft.x;
                const double xmax = pRite.x;

                const double ymin = 0,
                        ymax1 = pLeft.y,
                        ymax2 = pRite.y;

                glVertex2d(xmin, ymin);
                glVertex2d(xmin, ymax1);
                glVertex2d(xmax, ymax2);
                glVertex2d(xmax, ymin);
            }
        }
        glEnd();
    }

    {
        glLineWidth(style.lineWidth);

        auto color = style.lineColor;
        glColor4f(color.r, color.g, color.b, color.a);

        if(style.trace != Styles::Solid){
            glDisable(GL_LINE_SMOOTH);
            glEnable(GL_LINE_STIPPLE);
            glLineStipple(style.stippleFactor, style.stipplePattern);
        } else glEnable(GL_LINE_SMOOTH);

        glBegin(GL_LINE_STRIP);
        {
            for(auto p : pts) glVertex2d(p.x, p.y);
        }
        glEnd();

    }


}


