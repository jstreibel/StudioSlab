//
// Created by joao on 28/05/2021.
//

#ifndef V_SHAPE_GRAPH_DEPRCOLLECTIONS_H
#define V_SHAPE_GRAPH_DEPRCOLLECTIONS_H


#include <R2toR/Output/R2toROutputOpenGLGeneric.h>
#include "Model/MathTypes.h"
#include "RtoR2/StraightLine.h"
#include "Graph_depr.h"


namespace Base {
    /*! This typedef exists because we usually store pairs of graphs, one for the field and the other for it's time
     * derivative. */
    typedef std::pair<Graph_depr, Graph_depr> GraphPair;
    typedef std::pair<RtoR2::StraightLine, Color> LineAndColor;
    typedef std::pair<GraphPair, LineAndColor> SectionPair;

    Graph_depr& getPhiGraph(SectionPair &sectionPair);
    Graph_depr& getDtPhiGraph(SectionPair &sectionPair);
    RtoR2::StraightLine& getLine(SectionPair &sectionPair);
    Color& getColor(SectionPair &sectionPair);

    extern std::vector<Color> colors;
}


#endif //V_SHAPE_GRAPH_DEPRCOLLECTIONS_H
