//
// Created by joao on 11/25/25.
//

#ifndef STUDIOSLAB_POLYGONMASSPROPERTIES_H
#define STUDIOSLAB_POLYGONMASSPROPERTIES_H

#include <vector>
#include <cassert>
#include <cmath>

#include "Math/Formalism/Categories.h"
#include "Math/VectorSpace/Impl/PointSet.h"
#include "Utils/Numbers.h"

using namespace Slab;

struct PolygonMassProps {
    Real64 Mass;             // total mass
    Math::Real2D Centroid;   // COM in same coords as input vertices
    Real64 InertiaOrigin;    // I_z about origin
    Real64 InertiaCentroid;  // I_z about centroid
};

// vertices: polygon vertices in order (CW or CCW), non-self-intersecting
// density: mass per unit area
PolygonMassProps ComputePolygonMassProperties(const Math::FPointSet& Polygon, Real64 Density);


#endif //STUDIOSLAB_POLYGONMASSPROPERTIES_H