//
// Created by joao on 10/13/24.
//

#ifndef STUDIOSLAB_SLABMATH_H
#define STUDIOSLAB_SLABMATH_H

#include "Math/Data/Data.h"

namespace Slab::Math {
    void Startup();
    void Finish();

    Vector<DataName> EnumerateAllData();
}

#endif //STUDIOSLAB_SLABMATH_H
