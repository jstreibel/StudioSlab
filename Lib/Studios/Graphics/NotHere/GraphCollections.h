//
// Created by joao on 28/05/2021.
//

#ifndef V_SHAPE_GRAPHCOLLECTIONS_H
#define V_SHAPE_GRAPHCOLLECTIONS_H

#include "Studios/Graphics/graph.h"

#include <Studios/Util/Typedefs.h>

#include <Studios/CoreMath/Maps/RtoR2/StraightLine.h>
#include <Studios/CoreMath/Maps/R2toR/View/R2toROutputOpenGLGeneric.h>




namespace Base {
    /*! This typedef exists because we usually store pairs of graphs, one for the field and the other for it's time
     * derivative. */
    typedef std::pair<Graph, Graph> GraphPair;
    typedef std::pair<RtoR2::StraightLine, Color> LineAndColor;
    typedef std::pair<GraphPair, LineAndColor> SectionPair;

    Graph& getPhiGraph(SectionPair &sectionPair);
    Graph& getDtPhiGraph(SectionPair &sectionPair);
    RtoR2::StraightLine& getLine(SectionPair &sectionPair);
    Color& getColor(SectionPair &sectionPair);

    extern std::vector<Color> colors;
}


#endif //V_SHAPE_GRAPHCOLLECTIONS_H
