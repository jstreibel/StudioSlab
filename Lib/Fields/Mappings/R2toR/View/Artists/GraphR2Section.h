//
// Created by joao on 10/11/22.
//

#ifndef STUDIOSLAB_GRAPHR2SECTION_H
#define STUDIOSLAB_GRAPHR2SECTION_H


#include "Fields/Mappings/R2toR/Model/R2toRFunction.h"
#include "Studios/Graphics/Artists/Graph.h"
#include "Fields/Mappings/RtoR2/ParametricCurve.h"
#include "Fields/Mappings/RtoR2/StraightLine.h"

class GraphR2Section : public Graph<R2toR::Function> {
public:
    explicit GraphR2Section(double xMin=-1, double xMax=1, double yMin=-1, double yMax=1,
                            String title = "", bool filled = false, int samples = 512);

    void addSection(RtoR2::StraightLine *section);

protected:
    void _renderFunction(const R2toR::Function *func, Color color) override;

private:
    std::vector<RtoR2::StraightLine*> sections;
};


#endif //STUDIOSLAB_GRAPHR2SECTION_H
