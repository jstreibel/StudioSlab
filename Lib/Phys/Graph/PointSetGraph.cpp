//
// Created by joao on 29/05/23.
//

#include "PointSetGraph.h"

namespace My = Fields::Graphing;

My::PointSetGraph::PointSetGraph(const Str &title, bool autoReviewGraphRanges)
    : Graph2D(0, 0.1, 0, 0.1, title, true, 10), autoReviewGraphRanges(autoReviewGraphRanges) {

}

void My::PointSetGraph::addPointSet(Spaces::PointSet::Ptr pointSet,
                                    Styles::PlotStyle style, Str setName) {
    auto triple = PointSetTriple{pointSet, style, setName};
    mPointSets.emplace_back(triple);
}

void My::PointSetGraph::_renderPointSet(const Spaces::PointSet &pSet,
                                        Styles::PlotStyle style) const noexcept {
    auto pts = pSet.getPoints();

    if(style.filled)
    {
        const auto color = style.fillColor;

        glColor4f(color.r, color.g, color.b, color.a);
        glBegin(GL_QUADS);
        {
            int iMax = pts.size()-1;
            for(auto i=0; i<iMax; ++i){
                auto pLeft = pts[i];
                auto pRite = pts[i + 1];

                const Real xmin = pLeft.x;
                const Real xmax = pRite.x;

                const Real ymin = 0,
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
            for(auto p : pts)
                glVertex2d(p.x, p.y);
        }
        glEnd();

    }


}

void Fields::Graphing::PointSetGraph::reviewGraphRanges() {
    if(mPointSets.size() != 0)
    {

        auto referencePointSet = GetPointSet(mPointSets[0]);

        xMax = referencePointSet->getMax().x;
        xMin = referencePointSet->getMin().x;
        yMax = referencePointSet->getMax().y;
        yMin = referencePointSet->getMin().y;
    }

    for (auto &set: mPointSets) {
        auto max = GetPointSet(set)->getMax();
        auto min = GetPointSet(set)->getMin();

        if (max.x > xMax) xMax = max.x;
        if (min.x < xMin) xMin = min.x;
        if (max.y > yMax) yMax = max.y;
        if (min.y < yMin) yMin = min.y;
    }

    // Give an extra 100*dMin% room each side.
    if(true)
    {
        const auto dMin = .025;

        auto dx = dMin * (xMax - xMin);
        auto dy = dMin * (yMax - yMin);

        if(dx == 0) dx = dMin;
        if(dy == 0) dy = dMin;

        xMax += dx;
        xMin -= dx;
        yMax += dy;
        yMin -= dy;
    }
}

void Fields::Graphing::PointSetGraph::draw()
{
    if(autoReviewGraphRanges) reviewGraphRanges();

    Graph2D::draw();

    int i=0;
    for(auto &triple : mPointSets){
        auto &func = *GetPointSet(triple);
        auto style = GetStyle(triple);
        auto label = GetName(triple);

        if(label != "") _nameLabelDraw(i++, 0, style, label, this);

        this->_renderPointSet(func, style);
    }
}


