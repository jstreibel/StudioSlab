//
// Created by joao on 11/4/21.
//

#ifndef STUDIOSLAB_AAHYPERCUBE_H
#define STUDIOSLAB_AAHYPERCUBE_H


#include "Math/VectorSpace/Space.h"


namespace Slab::Math {

    class AAHyperCube : Space {
        DevFloat L;
        Measure *measure;

    public:
        AAHyperCube(UInt dim, DevFloat L);

        ~AAHyperCube();

        [[nodiscard]] auto GetMeasure() const -> const Measure override;

        auto getSide() const -> const DevFloat;

    };


}

#endif //STUDIOSLAB_AAHYPERCUBE_H
