//
// Created by joao on 9/22/22.
//

#ifndef STUDIOSLAB_GRAPH2D_H
#define STUDIOSLAB_GRAPH2D_H


#include "Phys/Graph/FunctionGraph.h"
#include "Mappings/FunctionRenderer.h"


class GraphRtoR : public FunctionGraph<RtoR::Function> {
public:
    GraphRtoR(Real xMin=-1, Real xMax=1, Real yMin=-1, Real yMax=1,
              Str title = "", bool filled = false, int samples = 512);

protected:

    void _renderFunction(const RtoR::Function *func, Styles::PlotStyle style) override;
};


#endif //STUDIOSLAB_GRAPH2D_H
