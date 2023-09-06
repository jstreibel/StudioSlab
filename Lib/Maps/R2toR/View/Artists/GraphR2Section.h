//
// Created by joao on 10/11/22.
//

#ifndef STUDIOSLAB_GRAPHR2SECTION_H
#define STUDIOSLAB_GRAPHR2SECTION_H


#include "Maps/R2toR/Model/R2toRFunction.h"
#include "Math/Graph/FunctionGraph.h"
#include "Maps/RtoR2/ParametricCurve.h"
#include "Maps/RtoR2/StraightLine.h"

class GraphR2Section : public FunctionGraph<R2toR::Function> {
public:
    explicit GraphR2Section(Real xMin=-1, Real xMax=1, Real yMin=-1, Real yMax=1,
                            Str title = "", bool filled = false, int samples = 512);

    void addSection(RtoR2::StraightLine *section, Styles::Color color);

protected:
    void _renderFunction(const R2toR::Function *func, Styles::PlotStyle style) override;
    void __drawLabel();

private:
    std::vector<std::pair<RtoR2::StraightLine*, Styles::Color>> sections;

};


#endif //STUDIOSLAB_GRAPHR2SECTION_H