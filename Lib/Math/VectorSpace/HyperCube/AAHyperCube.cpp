//
// Created by joao on 11/4/21.
//

#include "AAHyperCube.h"


namespace Slab::Math {

    AAHyperCube::AAHyperCube(UInt dim, DevFloat L)
            : Space(dim), L(L), measure(new Measure(Vector<DevFloat>(dim, L))) {}

    AAHyperCube::~AAHyperCube() {
        delete measure;
    }

    const Measure AAHyperCube::getMeasure() const {
        return *measure;
    }

    auto AAHyperCube::getSide() const -> const DevFloat {
        return L;
    }

}