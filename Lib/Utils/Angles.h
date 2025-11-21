//
// Created by joao on 11/20/25.
//

#ifndef STUDIOSLAB_ANGLES_H
#define STUDIOSLAB_ANGLES_H

namespace Slab {

constexpr double Pi = 3.14159265358979323846;

inline double DegToRad(const double ang) { return ang * Pi / 180.0;}
inline double RadToDeg(const double ang) { return ang * 180.0 / Pi;}
}

#endif //STUDIOSLAB_ANGLES_H