//
// Created by joao on 11/4/21.
//

#include "AAHyperCube.h"


AAHyperCube::AAHyperCube(PosInt dim, Real L)
    : Space(dim), L(L), measure(new Measure(std::vector<Real>(dim, L)))
{   }

AAHyperCube::~AAHyperCube() {
    delete measure;
}

const Measure AAHyperCube::getMeasure() const {
    return *measure;
}

auto AAHyperCube::getSide() const -> const Real {
    return L;
}

