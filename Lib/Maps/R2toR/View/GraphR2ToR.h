//
// Created by joao on 9/22/22.
//

#ifndef STUDIOSLAB_GRAPHR2TOR_H
#define STUDIOSLAB_GRAPHR2TOR_H

#include "Graphics/ZoomPanRotate.h"
#include "Maps/R2toR/Model/R2toRFunction.h"
#include "Graphics/Graph/FunctionGraph.h"


class GraphR2toR : public FunctionGraph<R2toR::Function> {
public:
    GraphR2toR(Real xMin=-1, Real xMax=1, Real yMin=-1, Real yMax=1,
               Str title = "", bool filled = false, int samples = 512);

    void notifyReshape(int newWinW, int newWinH) override;

protected:
    void _renderFunction(const R2toR::Function *func, Styles::PlotStyle style) override;

private:
    ZoomPanRotate zpr;
};


#endif //STUDIOSLAB_GRAPHR2TOR_H
