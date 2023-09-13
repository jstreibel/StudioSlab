//
// Created by joao on 11/4/21.
//

#ifndef STUDIOSLAB_AAHYPERCUBE_H
#define STUDIOSLAB_AAHYPERCUBE_H


#include "Math/Space/Space.h"


class AAHyperCube : Space {
    Real L;
    Measure *measure;

public:
    AAHyperCube(UInt dim, Real L);
    ~AAHyperCube();

    [[nodiscard]] auto getMeasure() const -> const Measure override;
    auto getSide() const -> const Real;

};


#endif //STUDIOSLAB_AAHYPERCUBE_H
