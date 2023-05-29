//
// Created by joao on 9/22/22.
//

#ifndef STUDIOSLAB_GRAPHR2TOR_H
#define STUDIOSLAB_GRAPHR2TOR_H


#include "Fields/Mappings/R2toR/Model/R2toRFunction.h"
#include "Phys/Graph/FunctionGraph.h"
#include "Base/Graphics/ZoomPanRotate.h"


class GraphR2toR : public FunctionGraph<R2toR::Function> {
public:
    GraphR2toR(double xMin=-1, double xMax=1, double yMin=-1, double yMax=1,
               String title = "", bool filled = false, int samples = 512);

    void reshape(int w, int h) override;

protected:
    void _renderFunction(const R2toR::Function *func, Styles::PlotStyle style) override;

private:
    ZoomPanRotate zpr;
};


#endif //STUDIOSLAB_GRAPHR2TOR_H
