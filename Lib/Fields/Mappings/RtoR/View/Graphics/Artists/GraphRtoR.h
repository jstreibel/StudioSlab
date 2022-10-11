//
// Created by joao on 9/22/22.
//

#ifndef STUDIOSLAB_GRAPH2D_H
#define STUDIOSLAB_GRAPH2D_H


#include "Studios/Graphics/Artists/Graph.h"
#include "Fields/Mappings/FunctionRenderer.h"


class GraphRtoR : public Graph<RtoR::Function> {
public:
    GraphRtoR(double xMin=-1, double xMax=1, double yMin=-1, double yMax=1,
          String title = "", bool filled = false, int samples = 512);

protected:

    void _renderFunction(const RtoR::Function *func, Color color) override;
};


#endif //STUDIOSLAB_GRAPH2D_H
