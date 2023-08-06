#ifndef DEFS_H
#define DEFS_H

#include <assert.h>

typedef double Real;


#include <vector>
#include <iostream>
#include <cmath>


union DoubleAccess {
    double val;
    char bytes[8];

    inline bool isPositive() const {
        return !((bytes[7] & 0x80) >> 7);

    }
};


struct Point2D {
    Point2D() {}
    Point2D(Real x, Real y) : x(x), y(y) {}
    Point2D(const Point2D& p) : x(p.x), y(p.y) {}

    //Point2D operator * (const double a) const {
    //    return Point2D(x*a, y*a);
    //}
    //
    //Point2D operator - () const { return Point2D(-x,-y); }

    inline Real lengthSqr() const { return x * x + y * y;}
    inline Real length( ) const { return sqrt(lengthSqr());}

    Real x, y;
};
Point2D operator + (const Point2D &a, const Point2D &b);
Point2D operator - (const Point2D &a, const Point2D &b);
Point2D operator * (const Real &a, const Point2D &p);

typedef std::vector<Point2D> PointContainer;

typedef std::pair<PointContainer, PointContainer> VerletMoleculeContainer;

// Usado para o RK4
struct Molecule {
    Molecule(Point2D q, Point2D p) : m_q(q), m_p(p) {}
    Molecule() = default;

    Point2D m_q, m_p;
};

Molecule operator*(const Real &a, const Molecule &m);
Molecule operator+(const Molecule &a, const Molecule &b);

typedef std::vector<Molecule> MoleculeContainer;


#endif // DEFS_H
